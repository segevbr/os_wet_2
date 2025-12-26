#include  "account.h"

// For the setters we can use += and for deposit the argument is positive, 
// for withdraw the argument is negative and the logic still holds.

void Account::set_ils_balance(int new_ils) {
  ils_blc += new_ils;
}

void Account::set_usd_balance(int new_usd) {
  usd_blc += new_usd;
}