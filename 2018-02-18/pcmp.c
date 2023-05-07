#include <stdlib.h>
#include <errno.h>
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

void compare(char *first_name, char *second_name, int fraction, int offset) {
    FILE *first = fopen(first_name, "r");
    FILE *second = fopen(second_name, "r");
    fseek(first, offset, SEEK_SET);
    fseek(second, offset, SEEK_SET);
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
    fclose(first);
    fclose(second);

    first = second = NULL;
    printf("%d %d from parent\n", len1, len2);

    if (len1 != len2) {
        printf("%s %s differ\n", argv[1], argv[2]);
        exit(EXIT_FAILURE);
    }

    pid_t childs[NUM_CHILDS];
    int fraction = (len1 + NUM_CHILDS - 1) / NUM_CHILDS;  // ceil
    for (int i = 0; i < NUM_CHILDS; i++) {
        childs[i] = fork();

        if (childs[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (childs[i] == 0) {
            compare(argv[1], argv[2], fraction , min(len1, i * fraction));
        }
    }

    int wstatus;
    for (int i = 0; i < NUM_CHILDS; i++) {
        waitpid(childs[i], &wstatus, 0);
        int ret_val = WEXITSTATUS(wstatus);

        if (ret_val == EXIT_FAILURE) {
            printf("%s %s differ\n", argv[1], argv[2]);

            for (int j = i + 1; j < NUM_CHILDS; j++) {
                kill(childs[i], SIGKILL);
                waitpid(childs[i], &wstatus, WNOHANG);
            }

            exit(EXIT_FAILURE);
        }
    }
}
