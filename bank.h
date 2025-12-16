#ifndef BANK_H
#define BANK_H

#include <stack>
#include "account.h"

typedef struct Status {
    vector<Account*> accounts_list;
} Status;

class Bank {
private:
    stack<Status*> status_stack[120]; 
public:
    Bank();
};

void* bank_func(void* bank_ptr); // must return void* and get void* for pthread_create()


#endif