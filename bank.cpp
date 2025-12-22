#include "bank.h"

// TODO: initialize bank state, mutexes, etc.
Bank::Bank() {
}

Bank::~Bank() {
    // free accounts
    for (auto const& pair : accounts) {
        delete pair.second;
    }
    accounts.clear();
}

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

