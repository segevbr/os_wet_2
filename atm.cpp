#include "atm.h"
#include "log.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <cmath>


#define RATE 5 // 1 USD = 5 ILS

void *run_atm(void *arg) { 
  ATM *atm = (ATM *)arg;
  if (!atm)
    return NULL;

  atm->bank_ptr->add_atm(atm); // atm asks bank to register it

  atm->input_file.open(atm->input_file_path);
  if (!atm->input_file.is_open())
    return NULL;

  string line;

  while (atm->is_running) {
    if (!atm->bank_ptr->is_atm_connected(atm->get_id())) {
      atm->is_running = false; //atm closed
      break;
    }

    Command cmd;

    if (!getline(atm->input_file, line)) {
      atm->is_running = false; // atm finished
      break;
    }
    cmd = atm->parse_command(line);
    cmd.atm_id = atm->get_id(); // used so the vip thread knows which atm to run the command on

    if (cmd.vip_priority > 0) {
      atm->bank_ptr->add_vip_command(cmd);
      continue;
    }
    atm->run_command(cmd);
    // usleep(1000000); // sleep for 1 second between commands (debugging print_status)
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
    status = sleep(cmd.cmd_string);
    break;
  default:
    status = COMMAND_FAILED;
  }

  return status;
}

// Wrapper implementations
// Wrappers parse arguments and call the actual function
int ATM::open_account(const string &args) {
  stringstream ss(args);
  int account;
  string password;
  int amount_ils;
  int amount_usd;
  ss >> account >> password >> amount_ils >> amount_usd;

  return func_open_account(account, password, amount_ils, amount_usd);
}

int ATM::deposit(const string &args) {
  stringstream ss(args);
  int account;
  string password;
  int amount;
  string currency;

  ss >> account >> password >> amount >> currency;

  return func_deposit(account, password, amount, currency);
}

int ATM::withdraw(const string &args) {
  stringstream ss(args);
  int account;
  string password;
  int amount;
  string currency;

  ss >> account >> password >> amount >> currency;

  return func_withdraw(account, password, amount, currency);
}

int ATM::balance(const string &args) {
  stringstream ss(args);
  int account;
  string password;

  ss >> account >> password;

  return func_balance(account, password);
}

int ATM::close_account(const string &args) {
  stringstream ss(args);
  int account;
  string password;

  ss >> account >> password;

  return func_close_account(account, password);
}

int ATM::transfer(const string &args) {
  stringstream ss(args);
  int source_account;
  string password;
  int target_account;
  int amount;
  string currency;

  ss >> source_account >> password >> target_account >> amount >> currency;

  return func_transfer(source_account, password, target_account, amount,
                       currency);
}

int ATM::close_atm(const string &args) {
  stringstream ss(args);
  int target_atm;

  ss >> target_atm;

  if (target_atm > this->num_atms || target_atm <= 0) {
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - ATM ID " + to_string(target_atm) +
                 " does not exist";
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }

  // check if atm id is valid
  return func_close_atm(target_atm);
}

int ATM::rollback(const string &args) {
  stringstream ss(args);
  int iterations;

  ss >> iterations;

  return func_rollback(iterations);
}

int ATM::exchange(const string &args) {
  stringstream ss(args);
  int account;
  string password;
  string source_currency;
  string target_currency;
  string to_word;
  int source_amount;

  ss >> account >> password >> source_currency >> to_word >> target_currency >>
      source_amount;

  return func_exchange(account, password, source_currency, target_currency,
                       source_amount);
}

int ATM::invest(const string &args) {
  stringstream ss(args);
  int account;
  string password;
  int amount;
  string currency;
  int time;

  ss >> account >> password >> amount >> currency >> time;


  return func_invest(account, password, amount, currency, time);
}

int ATM::sleep(const string &args) { 
  stringstream ss(args);
  int sleep_time_in_ms;
  ss >> sleep_time_in_ms;

  return sleep_func(sleep_time_in_ms);
}

// ----- Actual functions -----

int ATM::func_open_account(int acc, string pswd, int ils, int usd) {
  
  Account *new_account = new Account(acc, pswd, ils, usd);
  bool success_adding_account = this->get_bank_ptr()->add_account(new_account);

  if (!success_adding_account) {
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - account with the same id exists";
    Log::getInstance().write(msg);
    delete new_account; // free allocated memory
    return COMMAND_FAILED;
  }

  string msg = to_string(this->get_id()) + ": New account id is " +
               to_string(acc) + " with password " + pswd +
               " and initial balance " + to_string(ils) + " ILS and " +
               to_string(usd) + " USD";
  Log::getInstance().write(msg);
  return COMMAND_SUCCESSFULL;
}

int ATM::func_deposit(int acc, string password, int amount, string curr) {
  bank_ptr->lock_bank_read();

  Account *account = this->get_bank_ptr()->get_account(acc);

  // Check if account doesn't exist
  if (account == nullptr) {
    bank_ptr->unlock_bank_read();
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - account id " + to_string(acc) +
                 " does not exist";
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }

  // check password
  if (!is_password_correct(acc, password)) {
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - password for account id " +
                 to_string(acc) + " is incorrect";
    Log::getInstance().write(msg);
    bank_ptr->unlock_bank_read();
    return COMMAND_FAILED;
  }

  account->lock.writeLock();

  if (curr == "ILS") { // deposit ILS
    account->set_ils_balance(amount);
  } else { // deposit USD
    account->set_usd_balance(amount);
  }

  int account_ils = account->get_ils_balance();
  int account_usd = account->get_usd_balance();

  account->lock.writeUnlock();
  bank_ptr->unlock_bank_read();

  string msg = to_string(this->get_id()) + ": Account " + to_string(acc) +
               " new balance is " + to_string(account_ils) + " ILS and " +
               to_string(account_usd) + " USD after " + to_string(amount) +
               " " + curr + " was deposited";
  Log::getInstance().write(msg);
  return COMMAND_SUCCESSFULL;
}

int ATM::func_withdraw(int acc, string pswd, int amount, string curr) {

  bank_ptr->lock_bank_read();

  Account *account = this->get_bank_ptr()->get_account(acc);

  // Check if account doesn't exist
  if (account == nullptr) {
    bank_ptr->unlock_bank_read();
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - account id " + to_string(acc) +
                 " does not exist";
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }

  // check password
  if (!is_password_correct(acc, pswd)) {
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - password for account id " +
                 to_string(acc) + " is incorrect";
    Log::getInstance().write(msg);
    bank_ptr->unlock_bank_read();
    return COMMAND_FAILED;
  }

  account->lock.writeLock();

  int account_ils = account->get_ils_balance();
  int account_usd = account->get_usd_balance();

  // If not enough relavent balance return error
  if ((curr == "ILS" && account_ils < amount) ||
      (curr == "USD" && account_usd < amount)) {

    account->lock.writeUnlock();
    bank_ptr->unlock_bank_read();

    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - account id " + to_string(acc) +
                 " balance is " + to_string(account_ils) + " ILS and " +
                 to_string(account_usd) + " USD is lower than " +
                 to_string(amount) + " " + curr;
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }

  // Otherwise withdraw accordingly
  if (curr == "ILS") { // withdraw ILS
    account->set_ils_balance(-amount);
  } else { // withdraw USD
    account->set_usd_balance(-amount);
  }

  account_ils = account->get_ils_balance();
  account_usd = account->get_usd_balance();

  account->lock.writeUnlock();
  bank_ptr->unlock_bank_read();

  string msg = to_string(this->get_id()) + ": Account " + to_string(acc) +
               " new balance is " + to_string(account_ils) + " ILS and " +
               to_string(account_usd) + " USD after " + to_string(amount) +
               " " + curr + " was withdrawn";
  Log::getInstance().write(msg);
  return COMMAND_SUCCESSFULL;
}

int ATM::func_balance(int acc, string pswd) {
  bank_ptr->lock_bank_read();

  Account *account = this->get_bank_ptr()->get_account(acc);

  // Check if account doesn't exist
  if (account == nullptr) {
    bank_ptr->unlock_bank_read();
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - account id " + to_string(acc) +
                 " does not exist";
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }

  // check password
  if (!is_password_correct(acc, pswd)) {
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - password for account id " +
                 to_string(acc) + " is incorrect";
    Log::getInstance().write(msg);
    bank_ptr->unlock_bank_read();
    return COMMAND_FAILED;
  }

  account->lock.readLock();

  int account_ils = account->get_ils_balance();
  int account_usd = account->get_usd_balance();

  account->lock.readUnlock();
  bank_ptr->unlock_bank_read();

  string msg = to_string(this->get_id()) + ": Account " + to_string(acc) +
               " balance is " + to_string(account_ils) + " ILS and " +
               to_string(account_usd) + " USD";
  Log::getInstance().write(msg);
  return COMMAND_SUCCESSFULL;
}

int ATM::func_close_account(int acc, string pswd) {
  bank_ptr->lock_bank_read();

  Account *account = this->get_bank_ptr()->get_account(acc);

  // Check if account doesn't exist
  if (account == nullptr) {
    bank_ptr->unlock_bank_read();
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - account id " + to_string(acc) +
                 " does not exist";
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }

  // check password
  if (!is_password_correct(acc, pswd)) {
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - password for account id " +
                 to_string(acc) + " is incorrect";
    Log::getInstance().write(msg);
    bank_ptr->unlock_bank_read();
    return COMMAND_FAILED;
  }

  account->lock.readLock();
  // get final balance
  int final_ils = account->get_ils_balance();
  int final_usd = account->get_usd_balance();

  account->lock.readUnlock();
  bank_ptr->unlock_bank_read();

  // remove account from bank
  bool success = this->get_bank_ptr()->remove_account(acc);
  if (!success) {
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - account id " + to_string(acc) +
                 " does not exist";
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }

  string msg = to_string(this->get_id()) + ": Account " + to_string(acc) +
               " is now closed. Balance was " + to_string(final_ils) +
               " ILS and " + to_string(final_usd) + " USD";
  Log::getInstance().write(msg);
  return COMMAND_SUCCESSFULL;
}

int ATM::func_transfer(int s_acc, string pswd, int t_acc, int amount,
                       string curr) {
  bank_ptr->lock_bank_read();

  Account *source_account = this->get_bank_ptr()->get_account(s_acc);
  Account *target_account = this->get_bank_ptr()->get_account(t_acc);

  // Check if account doesn't exist
  if (source_account == nullptr) {
    bank_ptr->unlock_bank_read();
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - account id " + to_string(s_acc) +
                 " does not exist";
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }
  if (target_account == nullptr) {
    bank_ptr->unlock_bank_read();
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - account id " + to_string(t_acc) +
                 " does not exist";
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }

  // check password
  if (!is_password_correct(s_acc, pswd)) {
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - password for account id " +
                 to_string(s_acc) + " is incorrect";
    Log::getInstance().write(msg);
    bank_ptr->unlock_bank_read();
    return COMMAND_FAILED;
  }

  Account *first_lock = (s_acc < t_acc) ? source_account : target_account;
  Account *second_lock = (s_acc < t_acc) ? target_account : source_account;

  first_lock->lock.writeLock();
  second_lock->lock.writeLock();

  // If not enough relevant balance return error

  int source_account_ils = source_account->get_ils_balance();
  int source_account_usd = source_account->get_usd_balance();

  if ((curr == "ILS" && source_account_ils < amount) ||
      (curr == "USD" && source_account_usd < amount)) {

    second_lock->lock.writeUnlock();
    first_lock->lock.writeUnlock();
    bank_ptr->unlock_bank_read();

    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - balance of account id " +
                 to_string(s_acc) + " is lower than " + to_string(amount) +
                 " " + curr;
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }

  // Otherwise transfer accordingly
  if (curr == "ILS") { // transfer ILS
    source_account->set_ils_balance(-amount);
    target_account->set_ils_balance(amount);
  } else { // transfer USD
    source_account->set_usd_balance(-amount);
    target_account->set_usd_balance(amount);
  }

  source_account_ils = source_account->get_ils_balance();
  source_account_usd = source_account->get_usd_balance();
  int target_account_ils = target_account->get_ils_balance();
  int target_account_usd = target_account->get_usd_balance();

  first_lock->lock.writeUnlock();
  second_lock->lock.writeUnlock();
  bank_ptr->unlock_bank_read();

  string msg = to_string(this->get_id()) + ": Transfer " + to_string(amount) +
               " " + curr + " from account " + to_string(s_acc) +
               " to account " + to_string(t_acc) + " new account balance is " +
               to_string(source_account_ils) + " ILS and " +
               to_string(source_account_usd) + " USD " +
               "new target account balance is " +
               to_string(target_account_ils) + " ILS and " +
               to_string(target_account_usd) + " USD";
  Log::getInstance().write(msg);

  return COMMAND_SUCCESSFULL;
}

int ATM::func_close_atm(int t_atm_id) {
  bool success = this->get_bank_ptr()->close_atm(t_atm_id, this->get_id());

  if (!success) {
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your close operation failed - ATM ID " +
                 to_string(t_atm_id) + " is already in a closed state";
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }
  return COMMAND_SUCCESSFULL; // for checks
}

int ATM::func_rollback(int it) {
  this->get_bank_ptr()->rollback_bank(it);
  string msg = to_string(this->get_id()) + ": Rollback to " + to_string(it) +
               " bank iterations ago was completed successfully";
  Log::getInstance().write(msg);
  return COMMAND_SUCCESSFULL; // for checks
}

int ATM::func_exchange(int acc, string pswd, string s_curr,
                       string t_curr, int s_amount) {

  bank_ptr->lock_bank_read();

  Account *account = this->get_bank_ptr()->get_account(acc);

  // Check if account doesn't exist
  if (account == nullptr) {
    bank_ptr->unlock_bank_read();
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - account id " + to_string(acc) +
                 " does not exist";
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }

  // check password
  if (!is_password_correct(acc, pswd)) {
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - password for account id " +
                 to_string(acc) + " is incorrect";
    Log::getInstance().write(msg);
    bank_ptr->unlock_bank_read();
    return COMMAND_FAILED;
  }

  account->lock.writeLock();

  // Account* account = atm->bank_ptr->get_account(acc);
  // check if account have enough balance in source currency before exchange
  if ((s_curr == "ILS" && account->get_ils_balance() < s_amount) ||
      (s_curr == "USD" && account->get_usd_balance() < s_amount)) {

    account->lock.writeUnlock();
    bank_ptr->unlock_bank_read();

    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - account id " + to_string(acc) +
                 " balance is " + to_string(account->get_ils_balance()) +
                 " ILS and " + to_string(account->get_usd_balance()) + 
                 " USD is lower than " + to_string(s_amount) + " " + s_curr;
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }

  // perform exchange 
  if (s_curr == "ILS" && t_curr == "USD") {
    account->set_ils_balance(-s_amount);
    int t_amount = s_amount / RATE; // integer division
    account->set_usd_balance(t_amount);
  } else if (s_curr == "USD" && t_curr == "ILS") {
    account->set_usd_balance(-s_amount);
    int t_amount = s_amount * RATE;
    account->set_ils_balance(t_amount);
  }
  int src_ils = account->get_ils_balance();
  int src_usd =  account->get_usd_balance();

  account->lock.writeUnlock();
  bank_ptr->unlock_bank_read();
  
  string msg = to_string(this->get_id()) + ": Account " + to_string(acc) +
               " new balance is " + to_string(src_ils) + " ILS and " +
               to_string(src_usd) + " USD after " + to_string(s_amount) +
               " " + s_curr + " was exchanged";
  Log::getInstance().write(msg);
  
  return COMMAND_SUCCESSFULL; // for checks
}
int ATM::func_invest(int acc, string pswd, int amount, string curr, int time) {
  bank_ptr->lock_bank_read();
  Account *account = this->get_bank_ptr()->get_account(acc);

  if (account == nullptr) {
    bank_ptr->unlock_bank_read();
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - account id " + to_string(acc) +
                 " does not exist";
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }

  if (!is_password_correct(acc, pswd)) {
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - password for account id " +
                 to_string(acc) + " is incorrect";
    Log::getInstance().write(msg);
    bank_ptr->unlock_bank_read();
    return COMMAND_FAILED;
  }

  account->lock.writeLock();

  int current_balance = (curr == "ILS") ? account->get_ils_balance() : account->get_usd_balance();

  if (current_balance < amount) {
    account->lock.writeUnlock();
    bank_ptr->unlock_bank_read();
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - account id " + to_string(acc) +
                 " balance is " + to_string(current_balance) + " " + curr + 
                 " is lower than " + to_string(amount) + " " + curr;
    Log::getInstance().write(msg);
    return COMMAND_FAILED;
  }

  if (curr == "ILS") { //ILS
    account->set_ils_balance(-amount);
  } else {  //USD
    account->set_usd_balance(-amount);
  }

  account->lock.writeUnlock(); // unlock to allow other operations during sleep
  bank_ptr->unlock_bank_read();

  usleep(time * 1000);

  account->lock.writeLock(); // lock again to update balance after investment

  double factor = pow(1.03, (double)time / 10.0); // 3% every 10 ms
  int final_amount = (int)(amount * factor); // rounded down

  if (curr == "ILS") {
    account->set_ils_balance(final_amount);
  } else {
    account->set_usd_balance(final_amount);
  }
  account->lock.writeUnlock();

  return COMMAND_SUCCESSFULL;
}

int ATM::sleep_func(int sleep_time_in_ms) {
  usleep(sleep_time_in_ms * 1000);
  string msg = to_string(this->get_id()) + 
                ": Currently on a scheduled break. Service will resume within " +
                to_string(sleep_time_in_ms) + " ms.";

  Log::getInstance().write(msg);
  return COMMAND_SUCCESSFULL;
}

bool ATM::is_password_correct(int acc_id, string password) {

  if (this->get_bank_ptr()->get_account(acc_id)->get_password() != password) {
    string msg = "Error " + to_string(this->get_id()) +
                 ": Your transaction failed - password for account id " +
                 to_string(acc_id) + " is incorrect";
    Log::getInstance().write(msg);
    return false;
  }

  return true;
}

// Helpers
int ATM::get_id() { return atm_id; }
Bank *ATM::get_bank_ptr() { return bank_ptr; }