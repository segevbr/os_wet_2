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

class ATM; // forward declaration

typedef struct AccountData {
  int id;
  int password;
  int ils_blc;
  int usd_blc;
} AccountData;

typedef struct Status {
  map<int, AccountData*> accounts_data;
} Status;

class Bank {
private:
map<int, Account *> accounts;
map<int, ATM*> atms; // track active ATMs in bank

vector<Status> history; // history for rollback

ReadWriteLock bank_lock;

//vector<Command> vip_queue;
pthread_mutex_t vip_lock;

public:
  Bank();
  ~Bank();

  // Account management
  void add_account(Account *new_account);
  void remove_account(int account_id);
  Account *get_account(int account_id);
  bool account_exists(int account_id);
  // ATM management
  void add_atm(ATM* atm_ptr);
  void close_atm(int atm_id);
  ATM* get_atm(int atm_id);
  bool atm_exists(int atm_id);

  // Rollback functions
  void make_snapshot();
  void rollback_bank(int iterations);
  void print_status();
}; 

void *bank_func(
    void *bank_ptr); // must return void* and get void* for pthread_create()

#endif