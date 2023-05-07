#include <unistd.h>
#include <sys/eventfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define MAX_SLEEP 5

volatile int number; 
volatile int efd_prod;
volatile int efd_cons;

void produci() {
    while (1) {
        long long int buf;
        read(efd_cons, &buf, sizeof(buf)); // wait until the produced value is consumed

        number =  rand();
        printf("Producer generated %d\n", number);

        buf = 1;
        write(efd_prod, &buf, sizeof(buf));
        sleep(number % MAX_SLEEP);
    }
}

void consuma() {
    while (1) {
        long long int buf;
        read(efd_prod, &buf, sizeof(buf)); // wait until it has produced a value

        printf("Consumer is consuming %d\n", number);

        buf = 1;
        write(efd_cons, &buf, sizeof(buf));
        sleep(rand() % MAX_SLEEP);
    }
}

int main(int argc, char *argv[]) {
    // sono le 9.18
    efd_prod = eventfd(0, EFD_SEMAPHORE); if (efd_prod == -1)
        handle_error("eventfd");
    efd_cons = eventfd(1, EFD_SEMAPHORE); if (efd_cons == -1)
        handle_error("eventfd");

    pthread_t producer, consumer;
    
    int err;
    err = pthread_create(&producer, NULL, (void *) produci, NULL);
    if (err != 0) handle_error_en(err, "pthread_create");
    err = pthread_create(&consumer, NULL, (void *) consuma, NULL);
    if (err != 0) handle_error_en(err, "pthread_create");

    void *res;
    err = pthread_join(producer, &res);
    if (err != 0) handle_error_en(err, "pthread_join");
    err = pthread_join(consumer, &res);
    if (err != 0) handle_error_en(err, "pthread_join");
}
