#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

// 12.19
//
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <fifo-name>\n", argv[0]);
        return 1;
    }

    int fifo_ret = mkfifo(argv[1], 0666);
    if (fifo_ret == -1) {
        perror("mkfifo");
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    char buf[1024];
    while (1) {
        memset(buf, 0, sizeof(buf));
        int ret = read(fd, buf, sizeof(buf));
        if (ret == -1) {
            perror("read");
            return 1;
        } else if (ret == 0) {
            fd = open(argv[1], O_RDONLY);
            if (fd == -1) {
                perror("open2");
                return 1;
            }
        } else {
            printf("%s", buf);

            if (buf[ret - 1] == '\n') buf[ret - 1] = '\0';
            if (strcmp(buf, "FINE") == 0) {
                ret = unlink(argv[1]);
                if (ret == -1) {
                    perror("unlink");
                    return 1;
                }
                break;
            }

            char *token = strtok(buf, " "); 
            int field = 0;
            pid_t pid;
            int sig_num;
            while (token != NULL) {
                if (field == 0) {
                    pid = (pid_t) atoi(token);
                } else if (field == 1) {
                    sig_num = (int) atoi(token);
                } else {
                    fprintf(stderr, "Invalid field number\n");
                    return 1;
                }
                field++;
                token = strtok(NULL, " ");
            }

            if (kill(pid, sig_num) == -1) {
                perror("kill");
                return 1;
            }
        }
    }

}
