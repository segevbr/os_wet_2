#include "bank.h"
#include "atm.h"
#include "log.h"

// TODO: initialize bank state, mutexes, etc.
Bank::Bank(int num_atms) {
  history.reserve(120);                 // preallocate space for history
  pthread_mutex_init(&vip_lock, NULL);
  pthread_cond_init(&vip_cond, NULL);
  is_bank_running_vip = true;
  atm_connected.resize(num_atms, true); // all atms open to business at start
}

Bank::~Bank() {
  // free accounts
  for (auto const &pair : accounts) {
    delete pair.second;
  }
  accounts.clear();
  atms.clear(); // clear the map
  
  pthread_mutex_destroy(&vip_lock);
  pthread_cond_destroy(&vip_cond);
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
void Bank::add_atm(ATM *atm_ptr) { 
  bank_lock.writeLock();
  atms[atm_ptr->get_id() - 1] = atm_ptr;
  atm_connected[atm_ptr->get_id() - 1] = true; 
  bank_lock.writeUnlock();
}

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
  bank_lock.readLock();
  Status current_status;

  for (auto const &pair : accounts) { // pair refers to <id, Account*>
    Account *acc = pair.second;
    
    acc->lock.readLock(); // lock account for reading

    AccountData *acc_data = new AccountData(); // allocate memory for copy

    acc_data->id = acc->get_id();
    acc_data->password = acc->get_password();
    acc_data->ils_blc = acc->get_ils_balance();
    acc_data->usd_blc = acc->get_usd_balance();

    acc->lock.readUnlock(); // unlock account after reading

    current_status.accounts_data[acc->get_id()] = acc_data;
  }

  if ((int)history.size() >= 100) {
    Status& old_status = history.front();
    for (auto& pair : old_status.accounts_data) {
        delete pair.second;  // free allocated memory for AccountData
    }
    history.erase(history.begin()); // remove oldest entry
  }
  history.push_back(current_status);

  bank_lock.readUnlock();
}

void Bank::rollback_bank(int iterations) {
  bank_lock.writeLock();
  if (iterations > (int)history.size()) {
    // error - not enough history
    bank_lock.writeUnlock();
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
  
  bank_lock.writeUnlock();
}

void Bank::print_status() {
  bank_lock.readLock();

  printf("\033[2J");   // clear the console
  printf("\033[1;1H"); // move cursor to top-left corner

  cout << "Current Bank Status" << endl;

  // for (auto const &[id, account] : accounts) {
  for (auto const &pair : accounts) {
    Account *account = pair.second;
    account->lock.readLock();
    cout << "Account " << account->get_id() << ": Password "
         << account->get_password() << " Balance " << account->get_ils_balance()
         << " [ILS] " << account->get_usd_balance() << " [USD]" << endl;
    account->lock.readUnlock();
  }

  bank_lock.readUnlock();
}

// VIP functions
void Bank::add_vip_command(Command cmd) {
  pthread_mutex_lock(&vip_lock);

  bool inserted = false;
  for (auto it = vip_queue.begin(); it != vip_queue.end(); ++it) {
    if (cmd.vip_priority > it->vip_priority) { // higer priority inserted before
      vip_queue.insert(it, cmd);
      inserted = true;
      break;
    }
  }

  // if queue was empty or command has loweset priority
  if (!inserted) {
    vip_queue.push_back(cmd);
  }

  pthread_cond_signal(&vip_cond); // signal waiting threads for available command
  pthread_mutex_unlock(&vip_lock);
}

bool Bank::get_next_vip_command(Command &cmd) {
  pthread_mutex_lock(&vip_lock);

  while (vip_queue.empty() && is_bank_running_vip) {
    pthread_cond_wait(&vip_cond, &vip_lock);
  }
  
  // if we wake up because bank stopped and queue is empty
  if (vip_queue.empty()) {
    pthread_mutex_unlock(&vip_lock);
    return false;
  }

  // get command from queue
  cmd = vip_queue.front();
  vip_queue.erase(vip_queue.begin());

  pthread_mutex_unlock(&vip_lock);
  return true;
}

void Bank::stop_vip_thread() {
  pthread_mutex_lock(&vip_lock);
  is_bank_running_vip = false;
  pthread_cond_broadcast(&vip_cond);
  pthread_mutex_unlock(&vip_lock);
}

ATM* Bank::get_atm(int atm_id) {
  bank_lock.readLock();
  auto it = atms.find(atm_id - 1);
  ATM* atm_ptr = nullptr;
  if (it != atms.end()) {
    atm_ptr = it->second;
  }
  bank_lock.readUnlock();
  return atm_ptr;
}

bool Bank::atm_exists(int atm_id) {
  bank_lock.readLock();
  bool exists = (atms.find(atm_id - 1) != atms.end());
  bank_lock.readUnlock();
  return exists;
}