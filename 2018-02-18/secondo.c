#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int min(int a, int b) {
    return a < b ? a : b;
}

int main(int argc, char *argv[]) {
    int NUM_CHILDS = 2;
    int opt;
    int has_opt = 0;
    while ((opt = getopt(argc, argv, "j:")) != -1) {
       switch (opt) {
       case 'j':
           NUM_CHILDS = atoi(optarg);
           has_opt = 1;
           break;
       default: /* '?' */
           fprintf(stderr, "Usage: %s [-j threads] file1 file2\n",
                   argv[0]);
           exit(EXIT_FAILURE);
       }
   }
    int non_argv_count = 0;
    char *files[2];
    if (optind < argc) {
        while (optind < argc && non_argv_count < 2) {
            files[non_argv_count] = argv[optind++];
            non_argv_count++;
        }
    }

    if (non_argv_count != 2) {
        fprintf(stderr, "Usage: %s [-j threads] file1 file2\n",
                argv[0]);
    }

    printf("%s file \n", files[0]);
    printf("%s file \n", files[1]);
    printf("%d childs \n", NUM_CHILDS);

    FILE * first = fopen(files[0], "r");
    FILE * second = fopen(files[1], "r");

    if (first == NULL || second == NULL) {
        printf("error in opening first or second file\n");
        return 1;
    }

    fseek(first, 0, SEEK_END);
    fseek(second, 0, SEEK_END);
    int len1 = ftell(first);
    int len2 = ftell(second);
    fclose(first);
    fclose(second);

    printf("%d %d from parent\n", len1, len2);
    if (len1 != len2) {
        printf("%s %s differ\n", files[0], files[1]);
        exit(EXIT_FAILURE);
    }

    pid_t childs[NUM_CHILDS];

    int fraction = (len1 + NUM_CHILDS - 1) / NUM_CHILDS;  // ceil
    for (int i = 0; i < NUM_CHILDS; i++) {
        childs[i] = fork();

        if (childs[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (childs[i] != 0) {
            first = fopen(files[0], "r");
            second = fopen(files[1], "r");

            if (first == NULL || second == NULL) {
                printf("error in opening first or second file\n");
                exit(EXIT_FAILURE);
            }
            fseek(first, min(len1, i * fraction), SEEK_SET);
            fseek(second, min(len1, i * fraction), SEEK_SET);
            break;
        }
    }

    int wstatus;
    if (childs[0] != 0) {  // solo l'unico parent dovrebbe avere pipe = 0
        printf("aaa\n");
        for (int i = 0; i < NUM_CHILDS; i++) {
            waitpid(childs[i], &wstatus, 0);
            int ret_val = WEXITSTATUS(wstatus);

            if (ret_val == EXIT_FAILURE) {
                printf("%s %s differ\n", files[0], files[1]);

                for (int j = i + 1; j < NUM_CHILDS; j++) {
                    kill(childs[i], SIGKILL);
                }
                exit(EXIT_FAILURE);
            } else {
                printf("child %d is nice\n", i);
            }
        }
    } else {
        int coso = ftell(first);
        printf("%d coso, %d\n", coso, fraction);

        for (int i = 0; i < fraction; i++) {
            int byte1 = fgetc(first);
            int byte2 = fgetc(second);

            if (byte1 != byte2) {
                exit(EXIT_FAILURE);
            }

            if (feof(first) || feof(second)) break;
        }
        exit(EXIT_SUCCESS);
    }
}
