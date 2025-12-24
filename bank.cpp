#include "bank.h"
#include "atm.h"
// TODO: initialize bank state, mutexes, etc.
Bank::Bank() {
    history.reserve(120); // reserve space to avid reallocations
}

Bank::~Bank() {
    // free accounts
    for (auto const& pair : accounts) {
        delete pair.second;
    }
    accounts.clear();
    atms.clear(); // clear the map
}

// Account management functions
void Bank::add_account(Account *new_account) {
    // lock list?
    accounts[new_account->get_id()] = new_account;
    // unlock list?
}

void Bank::remove_account(int account_id) {
    // lock list?
    auto it = accounts.find(account_id);
    if (it != accounts.end()) {
        delete it->second; // free account memory
        accounts.erase(it);
    }
    // unlock list?
}

Account* Bank::get_account(int account_id) {
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

bool Bank::account_exists(int account_id) {
    // read lock?
    bool exists = accounts.find(account_id) != accounts.end();
    // read unlock?
    return exists;
}

// ATM management functions
void Bank::add_atm(ATM* atm_ptr) {
    atms[atm_ptr->get_id()] = atm_ptr;
}

// This function set the atm flag as closed.
void Bank::close_atm(int atm_id) {
    auto it = atms.find(atm_id);
    if (it != atms.end()) {
        it->second->is_running = false; // stop ATM
    }
    // if ATM doens't exist - handle
    // TODO
}

// Rollback functions
void Bank::make_snapshot(){
    Status current_status;

    for (auto const& pair : accounts) { // pair refers to <id, Account*>
        Account* acc = pair.second;
        AccountData* acc_data = new AccountData(); // allocate memory for copy

        acc_data->id = acc->get_id();
        acc_data->password = acc->get_password();
        acc_data->ils_blc =  acc->get_ils_balance();
        acc_data->usd_blc = acc->get_usd_balance();
        
        current_status.accounts_data[acc->get_id()] = acc_data;
    }

    if ((int)history.size() >= 100){
        history.erase(history.begin()); // remove oldest entry
    }
    history.push_back(current_status);
}

void Bank::rollback_bank(int iterations){
    if (iterations > (int)history.size()){
        // error - not enough history
        return;
    }
    // get target status - we can assume itetations is valid (> 0 and <= 100)
    int target_index = history.size() - iterations - 1;
    Status target_status = history[target_index]; // save target status

    // Wipe current accounts
    for (auto const& pair : accounts) {
        delete pair.second; // for each account, free account memory
    }
    accounts.clear(); // clear the map

    // build accounts from snapshot
    for (auto const& pair : target_status.accounts_data) {
        AccountData* acc_data = pair.second;
        Account* new_account = new Account(acc_data->id, acc_data->password,
                                            acc_data->ils_blc, acc_data->usd_blc);
        accounts[acc_data->id] = new_account;
    }
    history.resize(target_index + 1); // remove future history
}
