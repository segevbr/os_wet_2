#ifndef BANK_H
#define BANK_H

#include "account.h"
#include "reader_writer.h"
#include <fstream>
#include <iostream>
#include <map>
#include <pthread.h>
#include <stack>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

typedef struct Status {
  vector<Account *> accounts_list;
} Status;

class Bank {
private:
  map<int, Account *> accounts;
  ReadWriteLock bank_lock;
  stack<Status *> status_stack[120];

public:
  Bank();
};

void *bank_func(
    void *bank_ptr); // must return void* and get void* for pthread_create()

#endif