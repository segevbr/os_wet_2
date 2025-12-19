#ifndef ATM_COMMANDS_H
#define ATM_COMMANDS_H

#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;
// Wrappers
int open_account(const string& args);
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
int func_deposit(int acc, int pswd, int amount, string curr);
int func_withdraw(int acc, int pswd, int amount, string curr);
int func_balance(int acc, int pswd);
int func_close_account(int acc, int pswd);
int func_transfer(int s_acc, int pswd, int t_acc, int amount, string curr);
int func_close_atm(int t_atm_id);
int func_rollback(int it);
int func_exchange(int acc, int pswd, string s_curr, string t_curr, int s_amount);
int func_invest(int acc, int pswd, int amount, string curr, int time);

// Helpers
enum Currencies {
    ILS = 0,
    USD = 1
};


#endif