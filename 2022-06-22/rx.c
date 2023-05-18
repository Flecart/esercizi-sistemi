#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

void action(int signo, siginfo_t *info, void *context) {
    printf("Signal %d from %d\n", signo, info->si_pid);
    long long value = (long long) info->si_value.sival_ptr;
    char received[9];
    memset(received, 0, 9);
    memcpy(received, &info->si_value.sival_ptr, 8);
    printf("Message: %s\n", received);
}

int main() {
    printf("My pid is %d\n", getpid());

    struct sigaction act;

    act.sa_handler = NULL;
    act.sa_flags = SA_SIGINFO;
 // // // // // // // // //    sigemptyset(&act.sa_mask);
    act.sa_sigaction = action;

    int ret_val = sigaction(SIGUSR1, &act, NULL);
    if (ret_val == -1) {
        printf("Cant set sigaction\n");
        return 1;
    }

    pause();
}
