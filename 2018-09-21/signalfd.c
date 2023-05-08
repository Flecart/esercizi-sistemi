#include <sys/signalfd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
#include <unistd.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[]) {
    int log_file = open("log.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);

    int sigfd = signalfd(-1, &mask, 0);
    if (sigfd == -1)
        handle_error("signalfd");

    struct signalfd_siginfo fdsi;
    for (;;) {
        ssize_t s = read(sigfd, &fdsi, sizeof(fdsi));
        if (s != sizeof(fdsi))
            handle_error("read");
        
        printf("processing signal, got: %d\n", fdsi.ssi_signo);
        if (fdsi.ssi_signo == SIGUSR1) {
            printf("Got SIGUSR1\n");
        } else if (fdsi.ssi_signo == SIGUSR2) {
            printf("Got SIGUSR2\n");
        } else {
            printf("Read unexpected signal\n");
        }
    }


    return EXIT_SUCCESS;
}
