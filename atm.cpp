#include "atm.h"
#include "atm_commands.h"
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>

extern void write_to_log(const string& msg);


void* run_atm(void* arg){
    ATM* atm = (ATM*)arg;
    if (!atm) return NULL;

    atm->input_file.open(atm->input_file_path);
    if (!atm->input_file.is_open()) return NULL;

    string line;
    Command failed_cmd;
    while (atm->is_running) {
        usleep(100*1000); // sleep 0.1s
        
        Command cmd;

        if (!getline(atm->input_file, line)) {
            atm->is_running = false; // atm finished
            break;
        }
        cmd = atm->parse_command(line);
        bool success = atm->run_command(cmd);
        
        sleep(1);
    }

    if (atm->input_file.is_open()) {
        atm->input_file.close();
    }
    // write to log

    return NULL;
}

Command ATM::parse_command(const string& line){
    
}

bool ATM::run_command(const Command& cmd){

}