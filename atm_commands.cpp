#include "atm_commands.h"
#include "command.h"
#include "bank.h"
#include <sstream>

// Wrappers parse arguments and call the actual function 
int open_account(ATM* atm, const string& args){
    stringstream ss(args);
    int account;
    int password;
    int amount_ils;
    int amount_usd;
    // cout << "Command: " << 'O' << args << endl;
    
    ss >> account >> password >> amount_ils >> amount_usd;
    // Parse Check
    // cout << "Account: " << account <<  "\tPassword: " << password 
    //     << "\tAmount ILS: " << amount_ils << "\tAmount USD" << amount_usd << endl;
    
    // Check if account already exists
    if (atm->bank_ptr->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(atm->get_id()) + 
        ": Your transaction failed - account with the same id exists\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }

    return func_open_account(atm, account, password, amount_ils, amount_usd);
}

int deposit(ATM* atm, const string& args) {
    stringstream ss(args);
    int account;
    int password;
    int amount;
    string currency;
    // cout << "Command: " << 'D' << args << endl;

    ss >> account >> password >> amount >> currency;

    // Parse Check
    // cout << "Account: " << account <<  "\tPassword: " << password 
    //     << "\tAmount: " << amount << "\tCurrency: " << currency << endl;
    
    // Check if account doesn't exist
    if (!atm->bank_ptr->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(atm->get_id()) + 
        ": Your transaction failed - account id " + to_string(account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }

    return func_deposit(atm, account, password, amount, currency);
}

int withdraw(ATM* atm, const string& args) {
    stringstream ss(args);
    int account;
    int password;
    int amount;
    string currency;
    // cout << "Command: " << 'W' << args << endl;

    ss >> account >> password >> amount >> currency;

    // Parse Check
    // cout << "Account: " << account <<  "\tPassword: " << password 
    //     << "\tAmount: " << amount << "\tCurrency: " << currency << endl;

    // Check if account doesn't exist
    if (!atm->bank_ptr->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(atm->get_id()) + 
        ": Your transaction failed - account id " + to_string(account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }
    return func_withdraw(atm, account, password, amount, currency);
}

int balance(ATM* atm, const string& args){
    stringstream ss(args);
    int account;
    int password;

    // cout << "Command: " << 'B' << args << endl;

    ss >> account >> password;
    
    // cout << "Account: " << account << "\tPassword: " << password << endl;
    // Check if account doesn't exist
    if (!atm->bank_ptr->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(atm->get_id()) + 
        ": Your transaction failed - account id " + to_string(account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }
    return func_balance(atm, account, password); 
}

int close_account(ATM* atm, const string& args){
    stringstream ss(args);
    int account;
    int password;

    // cout << "Command: " << 'Q' << args << endl;

    ss >> account >> password;

    // cout << "Account: " << account << "\tPassword: " << password << endl;
    // Check if account doesn't exist
    if (!atm->bank_ptr->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(atm->get_id()) + 
        ": Your transaction failed - account id " + to_string(account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }
    return func_close_account(atm, account, password);
}

int transfer(ATM* atm, const string& args) {
    stringstream ss(args);
    int source_account;
    int password;
    int target_account;
    int amount;
    string currency;

    // cout << "Command: " << "T" << endl;

    ss >> source_account >> password >> target_account >> amount >> currency;

    // Parse Check
    // cout << "Source account: " << source_account <<  "\tPassword: " << password 
    //     << "\tTarget Account: " << target_account << "\tAmount: " << amount <<
    //      "\tCurrency: " << currency << endl;
    // Check if account doesn't exist
    if (!atm->bank_ptr->account_exists(source_account)) {
        // unlock bank?
        string msg = "Error " + to_string(atm->get_id()) + 
        ": Your transaction failed - account id " + to_string(source_account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    } else if (!atm->bank_ptr->account_exists(target_account)) {
        // unlock bank?
        string msg = "Error " + to_string(atm->get_id()) + 
        ": Your transaction failed - account id " + to_string(target_account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }

    return func_transfer(atm, source_account, password, target_account, amount, currency);
}

int close_atm(ATM* atm, const string& args){
    stringstream ss(args);
    int target_atm;
    
    // cout << "Command: " << "C" << endl;

    ss >> target_atm;
    
    // cout << "Target ATM: " << target_atm << endl;

    // check if atm id is valid
    return func_close_atm(atm, target_atm);
}

int rollback(ATM* atm, const string& args) {
    stringstream ss(args);
    int iterations;

    // cout << "Command: " << "R" << endl;

    ss >> iterations;

    // cout << "Iterations: " << iterations << endl;
    return func_rollback(atm, iterations);
}

int exchange(ATM* atm, const string& args){
    stringstream ss(args);
    int account;
    int password;
    string source_currency;
    string target_currency;
    int source_amount;

    // cout << "Command: " << "X" << endl;

    ss >> account >> password >> source_currency >> target_currency >> source_amount;

    // Parse Check
    // cout << "Account: " << account <<  "\tPassword: " << password 
    //     << "\tSource Currency " << source_currency << "\tTarget Currency: " 
    //     << target_currency << "\tSource Amount: " << source_amount << endl;
    // Check if account doesn't exist
    
    if (!atm->bank_ptr->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(atm->get_id()) + 
        ": Your transaction failed - account id " + to_string(account) +
        " does not exist\n";
        // write msg to log
        cout << msg << endl;
        return COMMAND_FAILED;
    }

    return func_exchange(atm, account, password, source_currency, target_currency,
                                                                     source_amount);
}

int invest(ATM* atm, const string& args) {
    stringstream ss(args);
    int account;
    int password;
    int amount;
    string currency; 
    int time;

    // cout << "Command: " << "I" << endl;

    ss >> account >> password >> amount >> currency >> time;

    // Parse Check
    // cout << "Account: " << account <<  "\tPassword: " << password 
    //     << "\tAmount " << amount << "\tCurrency: " 
    //     << currency << "\tTime: " << time << endl;
    
    // Check if account doesn't exist
    if (!atm->bank_ptr->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(atm->get_id()) + 
        ": Your transaction failed - account id " + to_string(account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }

    return func_invest(atm, account, password, amount, currency, time);
}

int sleep_func(ATM* atm, const string& args){
    return COMMAND_SUCCESSFULL;
}

// ----- Actual functions -----
// If reached one of these functions - account exists

int func_open_account(ATM* atm, int acc, int pswd, int ils, int usd) {
    // If account already exists, return error
    
    // Create new account and add to bank
    Account* new_account = new Account(acc, pswd, ils, usd);
    atm->bank_ptr->add_account(new_account);

    string msg = to_string(atm->get_id()) + ": New account id is " + to_string(acc) + 
    " with password "+ to_string(pswd) + " and initial balance " + to_string(ils) 
    + " ILS and " + to_string(usd) + " USD\n";
    // write msg to log
    cout << msg << endl;
    return COMMAND_SUCCESSFULL; 
}

int func_deposit(ATM* atm, int acc, int pswd, int amount, string curr) {
    // check password
    if (!is_password_correct(atm, acc, pswd)) return COMMAND_FAILED;
    Account* account = atm->bank_ptr->get_account(acc);
    
    if (curr == "ILS"){
        // deposit ILS
        account->set_ils_balance(amount);
    } else {
        // deposit USD
        account->set_usd_balance(amount);
    }

    string msg = to_string(atm->get_id()) + ": Account " + to_string(acc) +
    " new balance is " + to_string(account->get_ils_balance()) + " ILS and " +
    to_string(account->get_usd_balance()) + " USD after " + 
    to_string(amount) + " " + curr + " deposited\n";
    // write msg to log
    cout << msg << endl;
    return COMMAND_SUCCESSFULL; 
}

int func_withdraw(ATM* atm, int acc, int pswd, int amount, string curr) {
    // check password
    if (!is_password_correct(atm, acc, pswd)) return COMMAND_FAILED;
    Account* account = atm->bank_ptr->get_account(acc);

    // If not enough relavent balance return error 
    if ((curr == "ILS" && account->get_ils_balance() < amount) || 
            (curr == "USD" && account->get_usd_balance() < amount)) {

        string msg = "Error " + to_string(atm->get_id()) + 
        ": Your transaction failed - account id " + to_string(acc) +
        " balance is " + to_string(account->get_ils_balance()) + " ILS and " +
        to_string(account->get_usd_balance()) + " USD is lower than " + 
        to_string(amount) + " " + curr + "\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }
    
    // Otherwise withdraw accordingly
    if (curr == "ILS"){
        // withdraw ILS
        account->set_ils_balance(-amount);
    } else {
        // withdraw USD
        account->set_usd_balance(-amount);
    }

    string msg = to_string(atm->get_id()) + ": Account " + to_string(acc) +
    " new balance is " + to_string(account->get_ils_balance()) + " ILS and " +
    to_string(account->get_usd_balance()) + " USD after " + to_string(amount) +
    " " + curr + " was withdrawn\n";
    // write msg to log
    cout << msg << endl;
    return COMMAND_SUCCESSFULL;
}

int func_balance(ATM* atm, int acc, int pswd) {
    if (!is_password_correct(atm, acc, pswd)) return COMMAND_FAILED;

    Account* account = atm->bank_ptr->get_account(acc);

    string msg = to_string(atm->get_id()) + ": Account " + to_string(acc) +
    " balance is " + to_string(account->get_ils_balance()) + " ILS and " +
    to_string(account->get_usd_balance()) + " USD\n";
    // write msg to log
    cout << msg << endl;
    return COMMAND_SUCCESSFULL;
}

int func_close_account(ATM* atm, int acc, int pswd) {
    if (!is_password_correct(atm, acc, pswd)) return COMMAND_FAILED;
    // get account pointer
    Account* account = atm->bank_ptr->get_account(acc);
    // get final balance
    int final_ils = account->get_ils_balance();
    int final_usd = account->get_usd_balance();

    // remove account from bank
    atm->bank_ptr->remove_account(acc);

    string msg = to_string(atm->get_id()) + ": Account " + to_string(acc) +
    " is now closed. Balance was " + to_string(final_ils) + " ILS and " +
    to_string(final_usd) + " USD\n";
    // write msg to log
    cout << msg << endl;
    return COMMAND_SUCCESSFULL; 
}

int func_transfer(ATM* atm, int s_acc, int pswd, int t_acc, int amount, string curr) {
    // Check password of source account
    if (!is_password_correct(atm, s_acc, pswd)) return COMMAND_FAILED;
    Account* source_account = atm->bank_ptr->get_account(s_acc);
    Account* target_account = atm->bank_ptr->get_account(t_acc);

    // If not enough relavent balance return error
    if ((curr == "ILS" && source_account->get_ils_balance() < amount) || 
            (curr == "USD" && source_account->get_usd_balance() < amount)) {

        string msg = "Error " + to_string(atm->get_id()) + 
        ": Your transaction failed - balance of account id " + to_string(s_acc) +
        " is lower than " + to_string(amount) + " " + curr + "\n"; 
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }

    // Otherwise transfer accordingly
    if (curr == "ILS"){
        // transfer ILS
        source_account->set_ils_balance(-amount);
        target_account->set_ils_balance(amount);
    } else {
        // transfer USD
        source_account->set_usd_balance(-amount);
        target_account->set_usd_balance(amount);
    }

    string msg = to_string(atm->get_id()) + ": Transfer " + to_string(amount) + " " + 
    curr + " from account " + to_string(s_acc) + " to account " + to_string(t_acc) +
    " new account balance is " + to_string(source_account->get_ils_balance()) +
    " ILS and " + to_string(source_account->get_usd_balance()) + " USD " +
    "new target account balance is " + to_string(target_account->get_ils_balance()) +
    " ILS and " + to_string(target_account->get_usd_balance()) + " USD\n";
    // write msg to log
    cout << msg << endl;

    return COMMAND_SUCCESSFULL; 
}
int func_close_atm(ATM* atm, int t_atm_id) {
    // flag to bank to close t_atm_id ATM
    // Todo: implement with Lior
    return COMMAND_SUCCESSFULL; // for checks
}
int func_rollback(ATM* atm, int it) {
    // Todo: implement with Lior
    return COMMAND_SUCCESSFULL; // for checks
}
int func_exchange(ATM* atm, int acc, int pswd, string s_curr, string t_curr, int s_amount) {
    // check password
    if (!is_password_correct(atm, acc, pswd)) return COMMAND_FAILED;
    // Account* account = atm->bank_ptr->get_account(acc);
    // check if enough balance after conversion
    // int rate = 5; // 1 USD = 5 ILS

    // Todo : finish implementation
    return COMMAND_SUCCESSFULL; // for checks
}
int func_invest(ATM* atm, int acc, int pswd, int amount, string curr, int time) {
    return COMMAND_SUCCESSFULL; // for checks
}

bool is_password_correct(ATM* atm, int acc_id, int password){
    if (atm->bank_ptr->get_account(acc_id)->get_password() != password){
        string msg = "Error " + to_string(atm->get_id()) + 
        ": Your transaction failed - password for account id " + to_string(acc_id) +
        " is incorrect\n";
        // write msg to log 
        cout << msg << endl;
        return false;
    }
    
    return true;
}