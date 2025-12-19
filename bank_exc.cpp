#include <pthread.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include "bank.h"
#include "atm.h"

#define SUCCESS 0
#define ERROR 1

using namespace std;

ofstream log_file;

// Provide definitions so the linker can find them.
// (Temporary no-op implementations for single-ATM bring-up.)
void* bank_func(void* arg) {
    (void)arg;
    return nullptr;
}

void* vip_thread_func(void* object) {
    (void)object;
    return nullptr;
}

int main(int argc, char* argv[]){
    // check amount of arguments
    if (argc < 3) {
        cerr << "Bank error: illegal arguments" << endl;
        return ERROR;
    }  
    
    vector<string> atm_input_files;

    // check paths are legit
    for (int i = 2; i < argc; i++){
        string filename = argv[i];
        ifstream file(filename);
        
        if (!file.is_open()){
            cerr << "Bank error: illegal arguments" << endl;
            return ERROR;
        }
        file.close();
        atm_input_files.push_back(filename);
    }

    log_file.open("log.txt", ios::out);
    // is there a need to check if was opend successfully

    Bank* bank = new Bank();

    // Create a single ATM using the first input file (single-ATM implementation)
    ATM* atm = new ATM(1, atm_input_files[0], bank);

    pthread_t bank_t; // bank thread
    if (pthread_create(&bank_t, NULL, bank_func, (void*)bank) != 0){
        cerr << "Bank error: pthread_create failed" << endl;
        delete atm;
        delete bank;
        return ERROR;
    }
    
    // ATM thread and object
    pthread_t atm_th;
    if (pthread_create(&atm_th, NULL, run_atm, (void*)atm) != 0){
        cerr << "Bank error: pthread_create failed" << endl;
        pthread_join(bank_t, NULL);
        delete atm;
        delete bank;
        return ERROR;
    }

    // VIP thread creation
    pthread_t vip_th;
    if (pthread_create(&vip_th, NULL, vip_thread_func, (void*)bank) != 0){
        cerr << "Bank error: pthread_create failed" << endl;
        pthread_join(atm_th, NULL);
        pthread_join(bank_t, NULL);
        delete atm;
        delete bank;
        return ERROR;
    }

    pthread_join(atm_th, NULL);
    delete(atm);

    pthread_join(vip_th, NULL);
    pthread_join(bank_t, NULL);
    delete(bank);
    log_file.close();

    return SUCCESS;
}


