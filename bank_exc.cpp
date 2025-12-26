#include "atm.h"
#include "bank.h"
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <vector>

#define SUCCESS 0
#define ERROR 1

using namespace std;

Bank *bank_ptr = nullptr;
bool is_bank_running = true;

// Provide definitions so the linker can find them.
// (Temporary no-op implementations for single-ATM bring-up.)
void *bank_func(void *arg) {
  Bank *bank = (Bank *)arg;
  int counter = 0;

  while (is_bank_running) {
        counter++;
        
        bank->make_snapshot();
        bank->print_status();
        
        // if (counter % 3 == 0) {
        //      //int percentage = (rand() % 5) + 1; 
        //      // bank->collect_commission(percentage); ... implement this function later
        // }

        usleep(10000); // Sleep for 10 milliseconds
    }

  bank->print_status();

  return nullptr;
}

void *vip_thread_func(void *object) {
  (void)object;
  return nullptr;
}

int main(int argc, char *argv[]) {
  // check amount of arguments
  if (argc < 3) {
    cerr << "Bank error: illegal arguments" << endl;
    return ERROR;
  }

  int num_atms = atoi(argv[1]);

  vector<string> atm_input_files;

  // check paths are legit
  for (int i = 2; i < argc; i++) {
    string filename = argv[i];
    ifstream file(filename);

    if (!file.is_open()) {
      cerr << "Bank error: illegal arguments" << endl;
      return ERROR;
    }
    file.close();
    atm_input_files.push_back(filename);
  }

  bank_ptr = new Bank(num_atms);
  pthread_t bank_t;
  if (pthread_create(&bank_t, NULL, bank_func, (void *)bank_ptr) != 0) {
    cerr << "Bank error: pthread_create failed" << endl;
    delete bank_ptr;
    return ERROR;
  }

  vector<ATM *> atms;
  vector<pthread_t> atm_threads(num_atms);
  for (int i = 0; i < num_atms; ++i) {
    atms.push_back(new ATM(i + 1, atm_input_files[i], bank_ptr, num_atms));

    if (pthread_create(&atm_threads[i], NULL, run_atm, (void *)atms[i]) != 0) {
      cerr << "Bank error: pthread_create failed" << endl;
      return ERROR;
    }
  }

  // vip threads
  pthread_t vip_th;
  if (pthread_create(&vip_th, NULL, vip_thread_func, (void *)bank_ptr) != 0) {
    cerr << "Bank error: pthread_create failed" << endl;
    return ERROR;
  }

  for (int i = 0; i < num_atms; ++i) {
    pthread_join(atm_threads[i], NULL);
  }

  is_bank_running = false;
  pthread_join(bank_t, NULL);

  for (ATM *atm : atms) {
    delete atm;
  }
  delete bank_ptr;

  return SUCCESS;
}
