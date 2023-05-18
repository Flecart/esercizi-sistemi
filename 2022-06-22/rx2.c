#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char received[2048];
int received_len = 0;

void action(int signo, siginfo_t *info, void *context) {
        printf("SignalUSR1 from %d\n", info->si_pid);

        long long value = (long long) info->si_value.sival_ptr;
        if (value == -1) {
            printf("End of message\n");
            printf("Message: %s\n", received);
            exit(EXIT_SUCCESS);
        }

        if (received_len + 8 > 2048) {
            fprintf(stderr, "ERROR: Message too long\n");
            return;
        }

        memcpy(received + received_len, &info->si_value.sival_ptr, 8);
        received_len += 8;

}


int main() {
    printf("My pid is - %d\n", getpid());
    siginfo_t info;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);

    struct sigaction act;
    act.sa_sigaction = action;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    while (1) {
        int ret_val = sigwaitinfo(&set, &info);
        printf("hello\n");
        if (ret_val == -1) {
            printf("Cant wait for signal\n");
            return 1;
        }
    }
}
