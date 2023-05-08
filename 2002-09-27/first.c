#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

#define N 20
int main(int argc, char *argv[]) {
    int pipefd[N][2];

    for (int i = 0; i < N; i++) {
        if (pipe(pipefd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        // reseed so random is actually random
        // between forks
        srand(time(NULL) ^ (getpid()<<16));
        pid_t cpid;
        cpid = fork();
        if (cpid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        long long int buf;
        if (cpid == 0) {
            pid_t my_pid = getpid();
            pid_t parent_pid = getppid();
            printf("i is %d\n", i);
            printf("parent id is %d\n", parent_pid);
            printf("my pid is %d\n", my_pid);

            if (i == N - 1) {
                int num = rand() % 100;
                printf("Sending generated number %d\n", num);
                write(pipefd[i][1], &num, sizeof(num));
            }
        } else {
            int wstatus;
            wait(&wstatus);

            int val;
            read(pipefd[i][0], &val, sizeof(int));
            printf("me, the  %d th process, got number %d\n",i, val);

            int is_add = rand();
            if (is_add % 2) val++;
            else val--;

            if (i > 0) {
                printf("sending %d to parent process\n", val);
                write(pipefd[i - 1][1], &val, sizeof(int));
            }

            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}
