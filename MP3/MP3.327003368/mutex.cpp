#include <pthread.h>
#include "mutex.hpp"

Mutex::Mutex() {
    pthread_mutex_init(&m, 0);
}

Mutex::~Mutex(){
    pthread_mutex_destroy(&m);
}

void Mutex::Lock() {
    pthread_mutex_lock(&m);
}

void Mutex::Unlock() {
    pthread_mutex_unlock(&m);
}