#ifndef COMMANDS_H
#define COMMANDS_H

#include <vector>
#include <string>

using namespace std;

enum CommandType {
    CMD_OPEN, CMD_DEPOSIT, CMD_WITHDRAW, CMD_BALANCE, CMD_CLOSE,
    CMD_TRANSFER, CMD_CLOSE_ATM, CMD_ROLLBACK, CMD_EXCHANGE, CMD_INVEST,
    CMD_SLEEP
};

enum CommandStatus {
    COMMAND_SUCCESSFULL = 0,
    COMMAND_FAILED = 1
};

typedef struct Command {
    CommandType type;
    int vip_priority;
    string cmd_string;
    int atm_id; // for vip commands
} Command;

#endif