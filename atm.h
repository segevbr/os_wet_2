#ifndef ATM_H
#define ATM_H

#include <string>
#include <fstream>
#include <iostream>
#include "bank.h"
#include "command.h"

using namespace std;

class ATM {
    private:
        int atm_id;
        string input_file_path; 
        ifstream input_file;
        Bank* bank_ptr;        
        bool is_running;
    public:
        ATM(int id, string& file_path, Bank* bank) : atm_id(id),
        input_file_path(file_path), bank_ptr(bank){};

        int get_id();
};
    
void* run_atm(void* atm);
Command parse_command(const string& line);
bool run_command(const Command& cmd);


#endif 