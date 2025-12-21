#include "log.h"

Log::Log() {
    pthread_mutex_init(&write_lock, NULL);
    log_file.open("log.txt", ios::trunc); 
}

Log::~Log() {
    if (log_file.is_open()) {
        log_file.close();
    }
    pthread_mutex_destroy(&write_lock);
}

Log& Log::getInstance() {
    static Log instance;  //only in the first time will be created
    return instance;
}

void Log::write(const string& msg) {
    pthread_mutex_lock(&write_lock);
    if (log_file.is_open()) {
        log_file << msg << endl;
    }
    pthread_mutex_unlock(&write_lock);
}