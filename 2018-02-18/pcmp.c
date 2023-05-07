#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_CHILDS 2

int min(int a, int b) {
    return a < b ? a : b;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("[usage] %s file1 file2\n", argv[0]);
        return 1;
    }

    FILE * first = fopen(argv[1], "r");
    FILE * second = fopen(argv[2], "r");

    if (first == NULL || second == NULL) {
        printf("error in opening first or second file\n");
        return 1;
    }

    fseek(first, 0, SEEK_END);
    fseek(second, 0, SEEK_END);
    int len1 = ftell(first);
    int len2 = ftell(second);
    rewind(first);
    rewind(second);

    printf("%d %d from parent\n", len1, len2);

    pid_t childs[NUM_CHILDS];

    int fraction = (len1 + NUM_CHILDS - 1) / NUM_CHILDS;  // ceil
    for (int i = 0; i < NUM_CHILDS; i++) {
        childs[i] = fork();

        if (childs[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (childs[i] != 0) {
            fseek(first, min(len1, i * fraction), SEEK_SET);
            fseek(second, min(len1, i * fraction), SEEK_SET);
            break;
        }
    }

    int wstatus;

    if (childs[0] != 0) {  // solo l'unico parent dovrebbe avere pipe = 0
        for (int i = 0; i < NUM_CHILDS; i++) {
            waitpid(childs[i], &wstatus, 0);
            int ret_val = WEXITSTATUS(wstatus);

            if (ret_val == EXIT_FAILURE) {
                printf("%s %s differ\n", argv[1], argv[2]);
                
                for (int j = i + 1; j < NUM_CHILDS; j++) {
                    kill(childs[i], SIGKILL);
                }
                exit(EXIT_FAILURE);
            }
        }
    } else {
        for (int i = 0; i < fraction; i++) {
            int byte1 = fgetc(first);
            int byte2 = fgetc(second);

            if (byte1 != byte2) {
                exit(EXIT_FAILURE);
            }

            if (byte1 == EOF || byte2 == EOF) break;
        }
        exit(EXIT_SUCCESS);
    }

    fclose(first);
    fclose(second);
}
