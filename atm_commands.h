#ifndef ATM_COMMANDS_H
#define ATM_COMMANDS_H

#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "atm.h"

using namespace std;
// Wrappers
int open_account(ATM* atm, const string& args);
int deposit(ATM* atm, const string& args);
int withdraw(ATM* atm, const string& args);
int balance(ATM* atm, const string& args);
int close_account(ATM* atm, const string& args);
int transfer(ATM* atm, const string& args);
int close_atm(ATM* atm, const string& args);
int rollback(ATM* atm, const string& args);
int exchange(ATM* atm, const string& args);
int invest(ATM* atm, const string& args);
int sleep_func(ATM* atm, const string& args);

// actual functions
int func_open_account(ATM* atm, int acc, int pswd, int ils, int usd);
int func_deposit(ATM* atm, int acc, int pswd, int amount, string curr);
int func_withdraw(ATM* atm, int acc, int pswd, int amount, string curr);
int func_balance(ATM* atm, int acc, int pswd);
int func_close_account(ATM* atm, int acc, int pswd);
int func_transfer(ATM* atm, int s_acc, int pswd, int t_acc, int amount, string curr);
int func_close_atm(ATM* atm, int t_atm_id);
int func_rollback(ATM* atm, int it);
int func_exchange(ATM* atm, int acc, int pswd, string s_curr, string t_curr, int s_amount);
int func_invest(ATM* atm, int acc, int pswd, int amount, string curr, int time);

// Helpers
enum Currencies {
    ILS = 0,
    USD = 1
};

bool is_password_correct(ATM* atm, int acc_id, int password);


#endif