#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void action(int signo, siginfo_t *info, void *context) {

}


int main(int argc, char *argv[]) {
    if (sizeof(union sigval) < 8) {
        printf("union sigval too small on this machine to send 8 bytes\n");
        return 1;
    }    

    if (argc != 3) {
        printf("Usage: %s <rx-pid> <msg>\n", argv[0]);
        return 1;
    }
    int pid_n = atoi(argv[1]);
    if (pid_n <= 0) {
        printf("Invalid pid: %s\n", argv[1]);
        return 1;
    }

    int len = strlen(argv[2]);

    int ret_val = sigqueue(pid_n, 0, (union sigval) NULL);
    if (ret_val == -1) {
        printf("Cant find pid %s\n", argv[1]);
        return 1;
    }

    struct sigaction act;
    act.sa_sigaction = action;
    act.sa_flags = 0;
    sigaction(SIGUSR1, &act, NULL);
    char payload[9];
    while (len > 0) {
        memset(payload, 0, 9);
        strncpy(payload, argv[2], 8);
        ret_val = sigqueue(pid_n, SIGUSR1, (union sigval) {.sival_ptr = (void *) (*(long long *) payload)});
        if (ret_val == -1) {
            printf("Cant send signal to pid %s\n", argv[1]);
            return 1;
        }
        argv[2] += 8;
        len -= 8;

        pause(); // for ack
    }

    ret_val = sigqueue(pid_n, SIGUSR1, (union sigval) {.sival_ptr = (void *) -1});
    if (ret_val == -1) {
        printf("Cant send signal to pid %s\n", argv[1]);
        return 1;
    }
}
