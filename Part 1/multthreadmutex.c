#ifdef __APPLE__

#ifndef PTHREAD_BARRIER_H_
#define PTHREAD_BARRIER_H_

#include <pthread.h>
#include <errno.h>

typedef int pthread_barrierattr_t;
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int tripCount;
} pthread_barrier_t;


int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
    if(count == 0)
    {
        errno = EINVAL;
        return -1;
    }
    if(pthread_mutex_init(&barrier->mutex, 0) < 0)
    {
        return -1;
    }
    if(pthread_cond_init(&barrier->cond, 0) < 0)
    {
        pthread_mutex_destroy(&barrier->mutex);
        return -1;
    }
    barrier->tripCount = count;
    barrier->count = 0;

    return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    ++(barrier->count);
    if(barrier->count >= barrier->tripCount)
    {
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return 1;
    }
    else
    {
        pthread_cond_wait(&barrier->cond, &(barrier->mutex));
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}

#endif // PTHREAD_BARRIER_H_
#endif // __APPLE__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


int SharedVariable = 0; /* shared data between threads */
void *SimpleThread(void *whichPtr); /* thread functoin */

#ifdef PTHREAD_SYNC
pthread_mutex_t lock_x;
pthread_barrier_t barrier;
#endif

int main(int argc, char **argv) {
    #ifndef PTHREAD_SYNC
    char usage[] = "Usage: multthread numberOfThreads\n";
    #endif
    #ifdef PTHREAD_SYNC
    char usage[] = "Usage: multthreadmutex numberOfThreads\n";
    #endif
    
    if (argc != 2) {
        fprintf(stderr, usage, argv[0]);
        return EXIT_FAILURE;
    }
    int num_threads;
    char ch;
    if (sscanf(argv[1], "%i%c", &num_threads, &ch) != 1) {
        fprintf(stderr, usage, argv[0]);
        return EXIT_FAILURE;
    }
    if (num_threads < 1) {
        printf("The  number of threads must be greater than 0.\n");
        return EXIT_FAILURE;
    }
    
    pthread_t thr[num_threads];
    int i, rc;

    #ifdef PTHREAD_SYNC
    pthread_mutex_init(&lock_x, NULL); /* initialize pthread mutex protecting "shared_x" */
    pthread_barrier_init(&barrier, NULL, num_threads); /* create a barrier object with a count of NUM_THREADS */
    #endif

    /* create threads */
    for (i = 0; i < num_threads; ++i) {
        if ((rc = pthread_create(&thr[i], NULL, SimpleThread, (void *)i))) {
            fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
            return EXIT_FAILURE;
        }
    }
    /* block until all threads complete */
    for (i = 0; i < num_threads; ++i) {
        pthread_join(thr[i], NULL);
    }
    
    #ifdef PTHREAD_SYNC
    pthread_mutex_destroy(&lock_x);
    pthread_barrier_destroy(&barrier);
    #endif
    return EXIT_SUCCESS;
}

/* thread function */
void *SimpleThread(void *whichPtr) {
    int which = (int)whichPtr;
    int num, val;
    for(num = 0; num < 20; num++) {
        if (random() > RAND_MAX / 2)
            usleep(10);
        #ifdef PTHREAD_SYNC
        pthread_mutex_lock (&lock_x);
        #endif
        val = SharedVariable;
        printf("*** thread %d sees value %d\n", which, val);
        SharedVariable = val + 1;
        #ifdef PTHREAD_SYNC
        pthread_mutex_unlock (&lock_x);
        #endif
    }
    #ifdef PTHREAD_SYNC
    pthread_barrier_wait(&barrier);
    #endif
    val = SharedVariable;
    printf("Thread %d sees final value %d\n", which, val);
    pthread_exit(0);
}
