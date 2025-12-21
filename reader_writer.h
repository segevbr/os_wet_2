#ifndef READER_WRITER_H
#define READER_WRITER_H

#include <pthread.h>

class ReadWriteLock {
private:
    pthread_mutex_t readerLock;
    pthread_mutex_t writerLock;
    int readers;

public:
    ReadWriteLock();
    ~ReadWriteLock();
    void readLock();
    void readUnlock();
    void writeLock();
    void writeUnlock();
};

#endif