#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

long long 

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

    // vediamo che il sistema dell'università
    if (strlen(argv[2]) > 8) {
        printf("Message too long\n");
        return 1;
    }

    int ret_val = sigqueue(pid_n, 0, (union sigval) NULL);
    if (ret_val == -1) {
        printf("Cant find pid %s\n", argv[1]);
        return 1;
    }

    char payload[9];
    memset(payload, 0, 9);
    strncpy(payload, argv[2], 8); // we don't want to put argv[2] to the message
    
    ret_val = sigqueue(pid_n, SIGUSR1, (union sigval) {.sival_ptr = (void *) (*(long long *) payload)});
    if (ret_val == -1) {
        printf("Cant send signal to pid %s\n", argv[1]);
        return 1;
    }
}
