#ifndef COMMANDS_H
#define COMMANDS_H

#include <vector>
#include <string>

using namespace std;

enum CommandType {
    CMD_OPEN, CMD_DEPOSIT, CMD_WITHDRAW, CMD_BALANCE, CMD_CLOSE,
    CMD_TRANSFER, CMD_CLOSE_ATM, CMD_ROLLBACK, CMD_EXCHANGE, CMD_INVEST
};

typedef struct Command {
    CommandType type;
    vector<string> args;
    bool is_persistent;
    int vip_priority;
    string cmd_string;
};

#endif