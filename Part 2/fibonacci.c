/* Same as multthread, but with mutexes */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* shared data between threads */
int *fib;
int count = 0;
pthread_mutex_t lock_x;

/* The thread will begin control in this function*/
void *runner(void *param) {
    pthread_mutex_lock (&lock_x);
    if(count == 0)
        fib[count++] = 0;
    else if (count == 1)
        fib[count++] = 1;
    else {
        int sum = fib[count-1] + fib[count-2];
        fib[count++] = sum;
    }
    pthread_mutex_unlock (&lock_x);
    pthread_exit(0);
}

int main(int argc, char **argv) {
    static char usage[] = "Usage: fibthread sequenceNumber\n";
    if (argc != 2) {
        fprintf(stderr, usage, argv[0]);
        return EXIT_FAILURE;
    }
    int fib_num;
    char ch;
    
    /* Check that there are only numbers in input */
    if ((sscanf(argv[1], "%i%c", &fib_num, &ch) != 1)) {
        printf("whatevs %d %c\n", fib_num, ch);
        fprintf(stderr, usage, argv[0]);
        return EXIT_FAILURE;
    }
    
    /* Check that the number is at least 0 */
    if (fib_num < 0) {
        printf("The fibonacci sequence number needs to be greater or equal to 0\n");
        return EXIT_FAILURE;
    }
    
    fib = (int*)malloc(sizeof(int) * (fib_num + 1));
    
    if (fib == NULL) {
        printf("Memory allocation wasn't successful\n");
        return EXIT_FAILURE;
    }
    
    pthread_t thr[fib_num + 1];
    int i, rc;

    /* initialize pthread mutex protecting "shared_x" */
    pthread_mutex_init(&lock_x, NULL);

    /* create threads */
    for (i = 0; i < fib_num + 1; ++i) {
        if ((rc = pthread_create(&thr[i], NULL, runner, NULL))) {
            fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
            return EXIT_FAILURE;
        }
    }
    
    /* block until all threads complete */
    for (i = 0; i < fib_num + 1; ++i) {
        pthread_join(thr[i], NULL);
    }
    
    printf("Fibonacci Sequence with n=%i: ", fib_num);
    for (i = 0; i < fib_num + 1; ++i) {
        if (i != fib_num)
            printf("%i, ", fib[i]);
        else
            printf("%i\n", fib[i]);
    }
    
    pthread_mutex_destroy(&lock_x);
    return EXIT_SUCCESS;
}
