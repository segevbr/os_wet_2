#ifndef BANK_H
#define BANK_H

#include "account.h"
#include "command.h"
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
  string password;
  int ils_blc;
  int usd_blc;
} AccountData;

typedef struct Status {
  map<int, AccountData *> accounts_data;
} Status;

class Bank {
private:
  map<int, Account *> accounts;
  map<int, ATM *> atms; // track active ATMs in bank

  int bank_ils_blc;
  int bank_usd_blc;

  vector<Status> history; // history for rollback

  ReadWriteLock bank_lock;

  // VIP management
  vector<Command> vip_queue;
  pthread_mutex_t vip_lock;
  pthread_cond_t vip_cond;
  bool is_bank_running_vip; 

  vector<bool> atm_connected;

public:
  Bank(int num_atms);
  ~Bank();

  // Bank locking helpers
  void lock_bank_read() { bank_lock.readLock(); }
  void unlock_bank_read() { bank_lock.readUnlock(); }

  void collect_commission(int percentage);

  // Account management
  bool add_account(Account *new_account);
  bool remove_account(int account_id);
  Account *get_account(int account_id);

  // ATM management
  void add_atm(ATM *atm_ptr);
  bool close_atm(int atm_id, int source_atm_id);
  ATM *get_atm(int atm_id);
  bool atm_exists(int atm_id);
  bool is_atm_connected(int atm_id);

  // Rollback functions
  void make_snapshot();
  void rollback_bank(int iterations);
  void print_status();

  // VIP functions
  void add_vip_command(Command cmd);
  bool get_next_vip_command(Command &cmd);
  void stop_vip_thread();
  pthread_cond_t* get_vip_cond() { return &vip_cond;}
  pthread_mutex_t* get_vip_lock() { return &vip_lock; }
};

void *bank_func(
    void *bank_ptr); // must return void* and get void* for pthread_create()


#endif