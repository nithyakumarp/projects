// Author: Nat Tuck
// CS3650 starter code

#ifndef BARRIER_H
#define BARRIER_H

#include <pthread.h>

typedef struct barrier {
    //Done: make a barrier
    pthread_mutex_t barrier;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    int count;
    int seen; // Done: use mutexes and condvars
} barrier;

barrier* make_barrier(int nn);
void barrier_wait(barrier* bb);
void free_barrier(barrier* bb);


#endif

