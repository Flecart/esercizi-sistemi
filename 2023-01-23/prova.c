#include <stdio.h>
       #include <sys/types.h>
       #include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    pid_t mypid = getpid();

    printf("pid = %d\n", mypid);
    printf("SIGUSR1: %d\n", SIGUSR1);

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);

    int ret = sigwait(&set, NULL);
    if (ret == -1) {
        perror("sigwait");
        return 1;
    }
    return 0;
}
