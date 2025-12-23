#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "reader_writer.h"
#include <pthread.h>
#include <string>

using namespace std;

class Account {
private:
  int id;
  int password;
  int ils_blc;
  int usd_blc;
  bool is_vip;

public:
  ReadWriteLock lock;
  Account(int id, int pass, int ils_b, int usd_b)
      : id(id), password(pass), ils_blc(ils_b), usd_blc(usd_b){};
  int get_id() const { return id; }
  int get_password() const { return password; }
  int get_ils_balance() const { return ils_blc; }
  int get_usd_balance() const { return usd_blc; }
  // bool get_is_vip() const { return is_vip; }

  void set_ils_balance(int new_ils);
  void set_usd_balance(int new_usd);
};

#endif