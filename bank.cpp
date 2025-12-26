#include "bank.h"
#include "atm.h"
#include "log.h"

// TODO: initialize bank state, mutexes, etc.
Bank::Bank(int num_atms) {
  history.reserve(120);                 // preallocate space for history
  pthread_mutex_init(&vip_lock, NULL);  // todo is needed?
  atm_connected.resize(num_atms, true); // all atms open to business at start
}

Bank::~Bank() {
  // free accounts
  for (auto const &pair : accounts) {
    delete pair.second;
  }
  accounts.clear();
  atms.clear(); // clear the map
}

// Account management functions
bool Bank::add_account(Account *new_account) {

  bank_lock.writeLock();

  // If account already exists, return error
  if (accounts.find(new_account->get_id()) != accounts.end()) {
    bank_lock.writeUnlock();
    return false; // account with same id exists
  }

  accounts[new_account->get_id()] = new_account;
  bank_lock.writeUnlock();
  return true;
}

bool Bank::remove_account(int account_id) {
  bank_lock.writeLock();

  auto it = accounts.find(account_id);
  if (it == accounts.end()) {
    bank_lock.writeUnlock();
    return false;
  } else {
    Account *acc = it->second;

    // make sure no other thread is using the account
    acc->lock.writeLock();
    acc->lock.writeUnlock();

    delete acc; // free account memory
    accounts.erase(it);
  }

  bank_lock.writeUnlock();
  return true;
}

Account *Bank::get_account(int account_id) {
  // lock list?
  auto it = accounts.find(account_id);
  if (it != accounts.end()) {
    // unlock list?
    return it->second;
  } else {
    // unlock list?
    return nullptr;
  }
}

// ATM management functions
void Bank::add_atm(ATM *atm_ptr) { atms[atm_ptr->get_id()] = atm_ptr; }

// This function set the atm flag as closed.
bool Bank::close_atm(int atm_id, int source_atm_id) {
  bank_lock.writeLock();

  if (!atm_connected[atm_id - 1]) {
    bank_lock.writeUnlock();
    return false; // already closed
  }

  atm_connected[atm_id - 1] = false;

  string msg = "Bank: ATM " + to_string(source_atm_id) + " closed " +
               to_string(atm_id) + " successfully";
  Log::getInstance().write(msg);

  bank_lock.writeUnlock();
  return true;
}

// helper in case of closed atm
bool Bank::is_atm_connected(int atm_id) {
  bank_lock.readLock();
  bool status = atm_connected[atm_id - 1];
  bank_lock.readUnlock();
  return status;
}

// Rollback functions
void Bank::make_snapshot() {
  Status current_status;

  for (auto const &pair : accounts) { // pair refers to <id, Account*>
    Account *acc = pair.second;
    AccountData *acc_data = new AccountData(); // allocate memory for copy

    acc_data->id = acc->get_id();
    acc_data->password = acc->get_password();
    acc_data->ils_blc = acc->get_ils_balance();
    acc_data->usd_blc = acc->get_usd_balance();

    current_status.accounts_data[acc->get_id()] = acc_data;
  }

  if ((int)history.size() >= 100) {
    history.erase(history.begin()); // remove oldest entry
  }
  history.push_back(current_status);
}

void Bank::rollback_bank(int iterations) {
  if (iterations > (int)history.size()) {
    // error - not enough history
    return;
  }
  // get target status - we can assume itetations is valid (> 0 and <= 100)
  int target_index = history.size() - iterations - 1;
  Status target_status = history[target_index]; // save target status

  // Wipe current accounts
  for (auto const &pair : accounts) {
    delete pair.second; // for each account, free account memory
  }
  accounts.clear(); // clear the map

  // build accounts from snapshot
  for (auto const &pair : target_status.accounts_data) {
    AccountData *acc_data = pair.second;
    Account *new_account = new Account(acc_data->id, acc_data->password,
                                       acc_data->ils_blc, acc_data->usd_blc);
    accounts[acc_data->id] = new_account;
  }
  history.resize(target_index + 1); // remove future history
}

void Bank::print_status() {
  bank_lock.readLock();

  printf("\033[2J");   // clear the console
  printf("\033[1;1H"); // move cursor to top-left corner

  cout << "Current Bank Status" << endl;

  for (auto const &[id, account] : accounts) {

    account->lock.readLock();
    cout << "Account " << account->get_id() << ": Password "
         << account->get_password() << " Balance " << account->get_ils_balance()
         << " [ILS] " << account->get_usd_balance() << " [USD]" << endl;
    account->lock.readUnlock();
  }

  bank_lock.readUnlock();
}
