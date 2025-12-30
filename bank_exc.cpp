#include "atm.h"
#include "bank.h"
#include "log.h"
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
    
    // Take commissions every 3 iterations since 3*10ms = 30ms
    if (counter % 3 == 0) {
      int percentage = (rand() % 5) + 1; // random percentage between 1 and 5
      bank->collect_commission(percentage);
    }

    usleep(10000); // Sleep for 10ms
  }

  bank->print_status();

  return nullptr;
}

void *vip_thread_func(void *arg) {
  Bank *bank = (Bank *)arg;
  Command cmd;
  
  // VIP thread loop
  while (bank->get_next_vip_command(cmd)) {
      ATM* atm = bank->get_atm(cmd.atm_id);
      if (atm) {
          atm->run_command(cmd);
      }
  }
  return nullptr;
}

int main(int argc, char *argv[]) {
  // Initialize log to preven thread race condition
  Log::getInstance();

  srand(time(NULL)); // seed random generator 

  // check amount of arguments
  if (argc < 3) {
    cerr << "Bank error: illegal arguments" << endl;
    return ERROR;
  }

  int num_atms = argc - 2;
  int vip_thread_num = stoi(argv[1]);

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
  vector<pthread_t> vip_thread(vip_thread_num);
  for (int i = 0; i < vip_thread_num; ++i) {
    if (pthread_create(&vip_thread[i], NULL, vip_thread_func, (void *)bank_ptr) != 0) {
      cerr << "Bank error: pthread_create failed" << endl;
      return ERROR;
    }
  }

  for (int i = 0; i < num_atms; ++i) {
    pthread_join(atm_threads[i], NULL);
  }

  // Signal VIP threads to stop
  bank_ptr->stop_vip_thread();
  for (int i = 0; i < vip_thread_num; ++i) {
    pthread_join(vip_thread[i], NULL);
  }

  is_bank_running = false;
  pthread_join(bank_t, NULL);

  for (ATM *atm : atms) {
    delete atm;
  }
  delete bank_ptr;

  return SUCCESS;
}
