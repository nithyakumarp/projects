// Author: Nat Tuck
// CS3650 starter code

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include "barrier.h"

//makes a barrier, initializes the mutexes and condition variable.
barrier*
make_barrier(int nn)
{
    int rv;
    barrier* bb = malloc(sizeof(barrier));
    assert(bb != 0);
    /*
    rv = pthread_mutex_init(&(bb->barrier),NULL);
    if (rv != 0) {
        perror("mutex_init(barrier)");
        abort();
    }
*/
    rv = pthread_mutex_init(&(bb->mutex),NULL);
    if (rv != 0) {
        perror("mutex_init(mutex)");
        abort();
    }
    rv= pthread_cond_init(&(bb->cond),NULL);
    if (rv != 0) {
        perror("mutex_init(mutex)");
        abort();
    }
    //bb->count = nn;
    bb->seen  = nn; //Sets seen to the value passes ie number of threads

    return bb;
}

//barrier_wait function waits till all the threads reaches that point
void
barrier_wait(barrier* bb)
{   
    //locking the barrier wait
    int rv = pthread_mutex_lock(&(bb->mutex));
    if(rv!=0){
        perror("lock(mutex)");
        abort();
    }
    //each time a thread reaches this point seen is decremented
    bb->seen--;
    if(bb->seen==0){
        pthread_cond_broadcast(&(bb->cond));
    }
    //if(bb->seen>0)
    else
    {
        //all the other threads except the last one waits here and continues when condition variable is recieved
        pthread_cond_wait(&(bb->cond),&(bb->mutex));
    }
    rv = pthread_mutex_unlock(&(bb->mutex));
    if(rv!=0){
        perror("unlock(mutex)");
        abort();
    }
    //barrier is locked
    /*rv = pthread_mutex_lock(&(bb->barrier));
    if(rv!=0){
        perror("lock(barrier)");
        abort();
    }
    //Whenever seen is zero, it broadcasts the condition variable
    
    
    //barrier is unlocked
    rv = pthread_mutex_unlock(&(bb->barrier));
    if(rv!=0){
        perror("wait(barrier)");
        abort();
    }   */
}

//freeing barrier memory
void
free_barrier(barrier* bb)
{
    // Done: More cleanup? NO
    free(bb);
}

