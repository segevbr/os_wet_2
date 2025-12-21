#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>

using namespace std;

class Log {
private:
    ofstream log_file;
    pthread_mutex_t write_lock;

    Log(); 
    
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

public:
    ~Log();

    static Log& getInstance();

    void write(const string& msg);
};

#endif