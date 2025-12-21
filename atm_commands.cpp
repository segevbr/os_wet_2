#include "atm_commands.h"
#include "command.h"

// Wrappers parse arguments and call the actual function 
int open_account(const string& args){
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

    return func_open_account(account, password, amount_ils, amount_usd);
}

int deposit(const string& args) {
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

    return func_deposit(account, password, amount, currency);
}

int withdraw(const string& args) {
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

    return func_deposit(account, password, amount, currency);
}

int balance(const string& args){
    stringstream ss(args);
    int account;
    int password;

    // cout << "Command: " << 'B' << args << endl;

    ss >> account >> password;
    
    // cout << "Account: " << account << "\tPassword: " << password << endl;
    
    return func_balance(account, password); 
}

int close_account(const string& args){
    stringstream ss(args);
    int account;
    int password;

    // cout << "Command: " << 'Q' << args << endl;

    ss >> account >> password;

    // cout << "Account: " << account << "\tPassword: " << password << endl;

    return func_close_account(account, password);
}

int transfer(const string& args) {
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

    return func_transfer(source_account, password, target_account, amount, currency);
}

int close_atm(const string& args){
    stringstream ss(args);
    int target_atm;
    
    // cout << "Command: " << "C" << endl;

    ss >> target_atm;
    
    // cout << "Target ATM: " << target_atm << endl;

    return func_close_atm(target_atm);
}

int rollback(const string& args) {
    stringstream ss(args);
    int iterations;

    // cout << "Command: " << "R" << endl;

    ss >> iterations;

    // cout << "Iterations: " << iterations << endl;

    return func_rollback(iterations);
}

int exchange(const string& args){
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


    return func_exchange(account, password, source_currency, target_currency,
                                                                     source_amount);
}

int invest(const string& args) {
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
        
    return func_invest(account, password, amount, currency, time);
}

int sleep_func(const string& args){
    return COMMAND_SUCCESSFULL;
}

// actual functions
int func_open_account(int acc, int pswd, int ils, int usd) {
    return COMMAND_SUCCESSFULL; // for checks
}
int func_deposit(int acc, int pswd, int amount, string curr) {
    return COMMAND_SUCCESSFULL; // for checks
}
int func_withdraw(int acc, int pswd, int amount, string curr) {
    return COMMAND_SUCCESSFULL; // for checks
}
int func_balance(int acc, int pswd) {
    return COMMAND_SUCCESSFULL; // for checks
}
int func_close_account(int acc, int pswd) {
    return COMMAND_SUCCESSFULL; // for checks
}
int func_transfer(int s_acc, int pswd, int t_acc, int amount, string curr) {
    return COMMAND_SUCCESSFULL; // for checks
}
int func_close_atm(int t_atm_id) {
    return COMMAND_SUCCESSFULL; // for checks
}
int func_rollback(int it) {
    return COMMAND_SUCCESSFULL; // for checks
}
int func_exchange(int acc, int pswd, string s_curr, string t_curr, int s_amount) {
    return COMMAND_SUCCESSFULL; // for checks
}
int func_invest(int acc, int pswd, int amount, string curr, int time) {
    return COMMAND_SUCCESSFULL; // for checks
}
