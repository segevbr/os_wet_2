#ifndef ATM_H
#define ATM_H

#include <string>
#include <fstream>
#include <iostream>
#include "bank.h"
#include "command.h"

using namespace std;

class ATM {
    public:
        int atm_id;
        string input_file_path; 
        ifstream input_file;
        Bank* bank_ptr;        
        bool is_running;
        
        ATM(int id, string& file_path, Bank* bank) : atm_id(id),
        input_file_path(file_path), bank_ptr(bank){};

        
        Command parse_command(const string& line);
        bool run_command(const Command& cmd);
        
        int get_id();
    };
    
void* run_atm(void* arg);


#endif 