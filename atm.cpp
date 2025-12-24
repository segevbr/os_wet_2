#include "atm.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

extern void write_to_log(const string &msg);

void *run_atm(void *arg) { // todo why void*
  ATM *atm = (ATM *)arg;
  if (!atm) return NULL;
  
  atm->bank_ptr->add_atm(atm); // atm asks bank to register it

  atm->input_file.open(atm->input_file_path);
  if (!atm->input_file.is_open())
    return NULL;

  string line;

  while (atm->is_running) {
    Command cmd;

    if (!getline(atm->input_file, line)) {
      atm->is_running = false; // atm finished
      break;
    }
    cmd = atm->parse_command(line);
    
    if (cmd.vip_priority > 0) {
      // todo add vip command in bank list
      continue;
    }
    atm->run_command(cmd);
  }

  if (atm->input_file.is_open()) {
    atm->input_file.close();
  }
  // write to log

  return NULL;
}

Command ATM::parse_command(const string &line) {
  Command cmd;
  stringstream ss(line);
  char cmd_type_char;

  ss >> cmd_type_char;

  switch (cmd_type_char) {
  case 'O':
    cmd.type = CMD_OPEN;
    break;
  case 'D':
    cmd.type = CMD_DEPOSIT;
    break;
  case 'W':
    cmd.type = CMD_WITHDRAW;
    break;
  case 'B':
    cmd.type = CMD_BALANCE;
    break;
  case 'Q':
    cmd.type = CMD_CLOSE;
    break;
  case 'T':
    cmd.type = CMD_TRANSFER;
    break;
  case 'C':
    cmd.type = CMD_CLOSE_ATM;
    break;
  case 'R':
    cmd.type = CMD_ROLLBACK;
    break;
  case 'X':
    cmd.type = CMD_EXCHANGE;
    break;
  case 'I':
    cmd.type = CMD_INVEST;
    break;
  case 'S':
    cmd.type = CMD_SLEEP;
    break;
  default:
    cmd.type = CMD_OPEN;
    break; // error handling
  }

  // get command string without type
  if (line.length() > 1)
    cmd.cmd_string = line.substr(1);
  else
    cmd.cmd_string = "";

  cmd.vip_priority = 0; // vip is between (1,100)

  string arg;
  while (ss >> arg) {
    // Check for VIP
    if (arg.find("VIP=") == 0) {
      cmd.vip_priority = stoi(arg.substr(4));
    }
  }

  return cmd;
}

bool ATM::run_command(const Command &cmd) {
  int status;

  // cout << "Running command..." << endl;
  // cout << "Persitent: " << cmd.is_persistent << "\tVIP: " << cmd.vip_priority
  // << endl;
  switch (cmd.type) {
  case (CMD_OPEN):
    status = open_account(cmd.cmd_string);
    break;
  case (CMD_DEPOSIT):
    status = deposit(cmd.cmd_string);
    break;
  case (CMD_WITHDRAW):
    status = withdraw(cmd.cmd_string);
    break;
  case (CMD_BALANCE):
    status = balance(cmd.cmd_string);
    break;
  case (CMD_CLOSE):
    status = close_account(cmd.cmd_string);
    break;
  case (CMD_TRANSFER):
    status = transfer(cmd.cmd_string);
    break;
  case (CMD_CLOSE_ATM):
    status = close_atm(cmd.cmd_string);
    break;
  case (CMD_ROLLBACK):
    status = rollback(cmd.cmd_string);
    break;
  case (CMD_EXCHANGE):
    status = exchange(cmd.cmd_string);
    break;
  case (CMD_INVEST):
    status = invest(cmd.cmd_string);
    break;
  case (CMD_SLEEP):
    status = sleep_func(cmd.cmd_string);
    break;
  default:
    status = COMMAND_FAILED;
  }

  return status;
}

// Wrapper implementations
// Wrappers parse arguments and call the actual function 
int ATM::open_account(const string& args){
    stringstream ss(args);
    int account;
    string password;
    int amount_ils;
    int amount_usd;
    // cout << "Command: " << 'O' << args << endl;
    
    ss >> account >> password >> amount_ils >> amount_usd;
    // Parse Check
    // cout << "Account: " << account <<  "\tPassword: " << password 
    //     << "\tAmount ILS: " << amount_ils << "\tAmount USD" << amount_usd << endl;
    
    // Check if account already exists
    if (this->get_bank_ptr()->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(this->get_id()) + 
        ": Your transaction failed - account with the same id exists\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }

    return func_open_account(account, password, amount_ils, amount_usd);
}

int ATM::deposit(const string& args) {
    stringstream ss(args);
    int account;
    string password;
    int amount;
    string currency;
    // cout << "Command: " << 'D' << args << endl;

    ss >> account >> password >> amount >> currency;

    // Parse Check
    // cout << "Account: " << account <<  "\tPassword: " << password 
    //     << "\tAmount: " << amount << "\tCurrency: " << currency << endl;
    
    // Check if account doesn't exist
    if (!this->get_bank_ptr()->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(this->get_id()) + 
        ": Your transaction failed - account id " + to_string(account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }

    return func_deposit(account, password, amount, currency);
}

int ATM::withdraw(const string& args) {
    stringstream ss(args);
    int account;
    string password;
    int amount;
    string currency;
    // cout << "Command: " << 'W' << args << endl;

    ss >> account >> password >> amount >> currency;

    // Parse Check
    // cout << "Account: " << account <<  "\tPassword: " << password 
    //     << "\tAmount: " << amount << "\tCurrency: " << currency << endl;

    // Check if account doesn't exist
    if (!this->get_bank_ptr()->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(this->get_id()) + 
        ": Your transaction failed - account id " + to_string(account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }
    return func_withdraw(account, password, amount, currency);
}

int ATM::balance(const string& args){
    stringstream ss(args);
    int account;
    string password;

    // cout << "Command: " << 'B' << args << endl;

    ss >> account >> password;
    
    // cout << "Account: " << account << "\tPassword: " << password << endl;
    // Check if account doesn't exist
    if (!this->get_bank_ptr()->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(this->get_id()) + 
        ": Your transaction failed - account id " + to_string(account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }
    return func_balance(account, password); 
}

int ATM::close_account(const string& args){
    stringstream ss(args);
    int account;
    string password;

    // cout << "Command: " << 'Q' << args << endl;

    ss >> account >> password;

    // cout << "Account: " << account << "\tPassword: " << password << endl;
    // Check if account doesn't exist
    if (!this->get_bank_ptr()->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(this->get_id()) + 
        ": Your transaction failed - account id " + to_string(account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }
    return func_close_account(account, password);
}

int ATM::transfer(const string& args) {
    stringstream ss(args);
    int source_account;
    string password;
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
    if (!this->get_bank_ptr()->account_exists(source_account)) {
        // unlock bank?
        string msg = "Error " + to_string(this->get_id()) + 
        ": Your transaction failed - account id " + to_string(source_account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    } else if (!this->get_bank_ptr()->account_exists(target_account)) {
        // unlock bank?
        string msg = "Error " + to_string(this->get_id()) + 
        ": Your transaction failed - account id " + to_string(target_account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }

    return func_transfer(source_account, password, target_account, amount, currency);
}

int ATM::close_atm(const string& args){
    stringstream ss(args);
    int target_atm;
    
    // cout << "Command: " << "C" << endl;

    ss >> target_atm;
    
    // cout << "Target ATM: " << target_atm << endl;

    // check if atm id is valid
    return func_close_atm(target_atm);
}

int ATM::rollback(const string& args) {
    stringstream ss(args);
    int iterations;

    // cout << "Command: " << "R" << endl;

    ss >> iterations;

    // cout << "Iterations: " << iterations << endl;
    return func_rollback(iterations);
}

int ATM::exchange(const string& args){
    stringstream ss(args);
    int account;
    string password;
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
    
    if (!this->get_bank_ptr()->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(this->get_id()) + 
        ": Your transaction failed - account id " + to_string(account) +
        " does not exist\n";
        // write msg to log
        cout << msg << endl;
        return COMMAND_FAILED;
    }

    return func_exchange(account, password, source_currency, target_currency,
                                                                     source_amount);
}

int ATM::invest(const string& args) {
    stringstream ss(args);
    int account;
    string password;
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
    if (!this->get_bank_ptr()->account_exists(account)) {
        // unlock bank?
        string msg = "Error " + to_string(this->get_id()) + 
        ": Your transaction failed - account id " + to_string(account) +
        " does not exist\n";
        // write msg to log 
        cout << msg << endl;
        return COMMAND_FAILED;
    }

    return func_invest(account, password, amount, currency, time);
}

int ATM::sleep_func(const string& args){
    return COMMAND_SUCCESSFULL;
}

// ----- Actual functions -----
// If reached one of these functions - account exists

int ATM::func_open_account(int acc, string pswd, int ils, int usd) {
    // If account already exists, return error
    
    // Create new account and add to bank
    Account* new_account = new Account(acc, pswd, ils, usd);
    this->get_bank_ptr()->add_account(new_account);

    string msg = to_string(this->get_id()) + ": New account id is " + to_string(acc) + 
    " with password "+ pswd + " and initial balance " + to_string(ils) 
    + " ILS and " + to_string(usd) + " USD\n";
    // write msg to log
    cout << msg << endl;
    return COMMAND_SUCCESSFULL; 
}

int ATM::func_deposit(int acc, string password, int amount, string curr) {
    // check password
    if (!is_password_correct(acc, password)) return COMMAND_FAILED;
    Account* account = this->get_bank_ptr()->get_account(acc);
    
    if (curr == "ILS"){
        // deposit ILS
        account->set_ils_balance(amount);
    } else {
        // deposit USD
        account->set_usd_balance(amount);
    }

    string msg = to_string(this->get_id()) + ": Account " + to_string(acc) +
    " new balance is " + to_string(account->get_ils_balance()) + " ILS and " +
    to_string(account->get_usd_balance()) + " USD after " + 
    to_string(amount) + " " + curr + " deposited\n";
    // write msg to log
    cout << msg << endl;
    return COMMAND_SUCCESSFULL; 
}

int ATM::func_withdraw(int acc, string pswd, int amount, string curr) {
    // check password
    if (!is_password_correct(acc, pswd)) return COMMAND_FAILED;
    Account* account = this->get_bank_ptr()->get_account(acc);

    // If not enough relavent balance return error 
    if ((curr == "ILS" && account->get_ils_balance() < amount) || 
            (curr == "USD" && account->get_usd_balance() < amount)) {

        string msg = "Error " + to_string(this->get_id()) + 
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

    string msg = to_string(this->get_id()) + ": Account " + to_string(acc) +
    " new balance is " + to_string(account->get_ils_balance()) + " ILS and " +
    to_string(account->get_usd_balance()) + " USD after " + to_string(amount) +
    " " + curr + " was withdrawn\n";
    // write msg to log
    cout << msg << endl;
    return COMMAND_SUCCESSFULL;
}

int ATM::func_balance(int acc, string pswd) {
    if (!is_password_correct(acc, pswd)) return COMMAND_FAILED;

    Account* account = this->get_bank_ptr()->get_account(acc);

    string msg = to_string(this->get_id()) + ": Account " + to_string(acc) +
    " balance is " + to_string(account->get_ils_balance()) + " ILS and " +
    to_string(account->get_usd_balance()) + " USD\n";
    // write msg to log
    cout << msg << endl;
    return COMMAND_SUCCESSFULL;
}

int ATM::func_close_account(int acc, string pswd) {
    if (!is_password_correct(acc, pswd)) return COMMAND_FAILED;
    // get account pointer
    Account* account = this->get_bank_ptr()->get_account(acc);
    // get final balance
    int final_ils = account->get_ils_balance();
    int final_usd = account->get_usd_balance();

    // remove account from bank
    this->get_bank_ptr()->remove_account(acc);

    string msg = to_string(this->get_id()) + ": Account " + to_string(acc) +
    " is now closed. Balance was " + to_string(final_ils) + " ILS and " +
    to_string(final_usd) + " USD\n";
    // write msg to log
    cout << msg << endl;
    return COMMAND_SUCCESSFULL; 
}

int ATM::func_transfer(int s_acc, string pswd, int t_acc, int amount, string curr) {
    // Check password of source account
    if (!is_password_correct(s_acc, pswd)) return COMMAND_FAILED;
    Account* source_account = this->get_bank_ptr()->get_account(s_acc);
    Account* target_account = this->get_bank_ptr()->get_account(t_acc);

    // If not enough relavent balance return error
    if ((curr == "ILS" && source_account->get_ils_balance() < amount) || 
            (curr == "USD" && source_account->get_usd_balance() < amount)) {

        string msg = "Error " + to_string(this->get_id()) + 
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

    string msg = to_string(this->get_id()) + ": Transfer " + to_string(amount) + " " + 
    curr + " from account " + to_string(s_acc) + " to account " + to_string(t_acc) +
    " new account balance is " + to_string(source_account->get_ils_balance()) +
    " ILS and " + to_string(source_account->get_usd_balance()) + " USD " +
    "new target account balance is " + to_string(target_account->get_ils_balance()) +
    " ILS and " + to_string(target_account->get_usd_balance()) + " USD\n";
    // write msg to log
    cout << msg << endl;

    return COMMAND_SUCCESSFULL; 
}
int ATM::func_close_atm(int t_atm_id) {
    // flag to bank to close t_atm_id ATM
    // bank should write to log about atm closure
    this->get_bank_ptr()->close_atm(t_atm_id);

    return COMMAND_SUCCESSFULL; // for checks
}
int ATM::func_rollback(int it) {
    this->get_bank_ptr()->rollback_bank(it);
    string msg = to_string(this->get_id()) + ": Rollback to " + to_string(it) +
    " bank iterations ago was completed successfully\n";
    // write msg to log
    cout << msg << endl;
    return COMMAND_SUCCESSFULL; // for checks
}
int ATM::func_exchange(int acc, string pswd, string s_curr, string t_curr, int s_amount) {
    // check password
    if (!is_password_correct(acc, pswd)) return COMMAND_FAILED;
    // Account* account = atm->bank_ptr->get_account(acc);
    // check if enough balance after conversion
    // int rate = 5; // 1 USD = 5 ILS

    // Todo : finish implementation
    return COMMAND_SUCCESSFULL; // for checks
}
int ATM::func_invest(int acc, string pswd, int amount, string curr, int time) {
    return COMMAND_SUCCESSFULL; // for checks
}
bool ATM::is_password_correct(int acc_id, string password){
    if (this->get_bank_ptr()->get_account(acc_id)->get_password() != password){
        string msg = "Error " + to_string(this->get_id()) + 
        ": Your transaction failed - password for account id " + to_string(acc_id) +
        " is incorrect\n";
        // write msg to log 
        cout << msg << endl;
        return false;
    }
    
    return true;
}
// Helpers
int ATM::get_id() { return atm_id; }

Bank* ATM::get_bank_ptr() { return bank_ptr; }