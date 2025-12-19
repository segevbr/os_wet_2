#include "atm_commands.h"

// Wrappers
int open_account(const string& args){
    stringstream ss(args);
    int account;
    int password;
    int amount_ils;
    int amount_usd;

    ss >> account >> password >> amount_ils >> amount_usd;
    
}
int deposit(const string& args);
int withdraw(const string& args);
int balance(const string& args);
int close_account(const string& args);
int transfer(const string& args);
int close_atm(const string& args);
int rollback(const string& args);
int exchange(const string& args);
int invest(const string& args);

// actual functions
int func_open_account(int acc, int pswd, int ils, int usd);
int func_deposit(int acc, int pswd, int amount, int curr);
int func_withdraw(int acc, int pswd, int amount, int curr);
int func_balance(int acc, int pswd);
int func_close_account(int acc, int pswd);
int func_transfer(int s_acc, int pswd, int t_acc, int amount, int curr);
int func_close_atm(int t_atm_id);
int func_rollback(int it);
int func_exchange(int acc, int pswd, int s_curr, int t_curr, int s_amount);
int func_invest(int acc, int pswd, int amount, int curr, int time);
