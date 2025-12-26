#ifndef ATM_H
#define ATM_H

#include <string>
#include <fstream>
#include <iostream>
#include "bank.h"
#include "command.h"

using namespace std;

class ATM{
    public:
        int atm_id;
        string input_file_path; 
        ifstream input_file;
        Bank* bank_ptr;        
        bool is_running;
        int num_atms;
        
        ATM(int id, string& file_path, Bank* bank, int num_atms) : atm_id(id),
        input_file_path(file_path), bank_ptr(bank), is_running(true), num_atms(num_atms){};

        
        Command parse_command(const string& line);
        bool run_command(const Command& cmd);
        
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
        int sleep_func(const string& args);
        
        // actual functions
        int func_open_account(int acc, string pswd, int ils, int usd);
        int func_deposit(int acc, string pswd, int amount, string curr);
        int func_withdraw(int acc, string pswd, int amount, string curr);
        int func_balance(int acc, string pswd);
        int func_close_account(int acc, string pswd);
        int func_transfer(int s_acc, string pswd, int t_acc, int amount, string curr);
        int func_close_atm(int t_atm_id);
        int func_rollback(int it);
        int func_exchange(int acc, string pswd, string s_curr, string t_curr, int s_amount);
        int func_invest(int acc, string pswd, int amount, string curr, int time);

        // Helpers
        enum Currencies {
            ILS = 0,
            USD = 1
        };
        int get_id();
        Bank* get_bank_ptr();
        bool is_password_correct(int acc_id, string password);
    };
    
void* run_atm(void* arg);


#endif