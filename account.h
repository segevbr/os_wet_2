#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <pthread.h>
#include <string>

using namespace std;

class Account {
    private:
        int id;
        string password;
        int ils_blc;
        int usd_blc;
        bool is_vip;
    public:
        Account(int id, string pass, int ils_b, int usd_b, bool is_vip) : id(id), 
                password(pass), ils_blc(ils_b), usd_blc(usd_b), is_vip(is_vip){};
};

#endif 