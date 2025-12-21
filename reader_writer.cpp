#include "reader_writer.h"
#include <pthread.h>

ReadWriteLock::ReadWriteLock() : readers(0) {
  pthread_mutex_init(&readerLock, NULL);
  pthread_mutex_init(&writerLock, NULL);
}

ReadWriteLock::~ReadWriteLock() {
  pthread_mutex_destroy(&readerLock);
  pthread_mutex_destroy(&writerLock);
}

void ReadWriteLock::readLock() {
  pthread_mutex_lock(&readerLock);
  readers++;
  if (readers == 1) {
    pthread_mutex_lock(&writerLock);
  }
  pthread_mutex_unlock(&readerLock);
}

void ReadWriteLock::readUnlock() {
  pthread_mutex_lock(&readerLock);
  readers--;
  if (readers == 0) {
    pthread_mutex_unlock(&writerLock);
  }
  pthread_mutex_unlock(&readerLock);
}

void ReadWriteLock::writeLock() { pthread_mutex_lock(&writerLock); }

void ReadWriteLock::writeUnlock() { pthread_mutex_unlock(&writerLock); }