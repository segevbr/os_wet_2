#include "atm.h"
#include "atm_commands.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

extern void write_to_log(const string &msg);

void *run_atm(void *arg) { // todo why void*
  ATM *atm = (ATM *)arg;
  if (!atm)
    return NULL;

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