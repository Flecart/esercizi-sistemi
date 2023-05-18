#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <execs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_PATH 2048
#define MAX_ARGS 256
static void execute(char *pathname, char *filename) {
    // soluzione con pipe
    int fds[2];
    if (pipe(fds) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int wstatus;
    if (fork()) {
        close(fds[1]);
        wait(&wstatus);
        int fd = open(pathname, O_WRONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        // write poll input to file
        char buf[4096];
        int len;
        while ((len = read(fds[0], buf, sizeof(buf))) > 0) {
            if (write(fd, buf, len) == -1) {
                perror("write on created file");
                exit(EXIT_FAILURE);
            }
        }
        close(fd);
    } else {
        close(fds[0]);
        dup2(fds[1], STDOUT_FILENO);
        // system_nosh(filename);  // boh non compila, che strano...
        
        char *token = strtok(filename, " ");
        char *args[MAX_ARGS]; int i = 0;
        while (token != NULL && i < MAX_ARGS) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }

        if (i == MAX_ARGS) {
            fprintf(stderr, "file has too many arguments\n");
            exit(EXIT_FAILURE);
        }

        args[i] = NULL;
        execvp(args[0], args);
    }
}

static void handle_events(int fd, int wd, char *dirname) {
    char buf[4096];
    struct inotify_event *event;
    ssize_t len;

    /* Loop while events can be read from inotify file descriptor. */
    for (;;) {
        /* Read some events. */
        len = read(fd, buf, sizeof(buf));
        if (len == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        } else if (len <= 0)
            break;

        /* Loop over all events in the buffer */
        for (char *ptr = buf; ptr < buf + len;
                ptr += sizeof(struct inotify_event) + event->len) {

            event = (struct inotify_event *) ptr;

            /* Print event type */
            if (event->mask & IN_CREATE)
                printf("IN_CREATE: ");
            if (event->mask & IN_MOVED_TO)
                printf("IN_MOVED_TO: ");

            if (event->len)
                printf("%s\n", event->name);

            char path_name[MAX_PATH];
            snprintf(path_name, MAX_PATH, "%s/%s", dirname, event->name);
            execute(path_name, event->name);
        }
    }
}

int main(int argc, char* argv[]) {
    char *dirname = argv[1];
    char buf;
    int fd;

    if (argc != 2) {
        printf("Usage: %s <dirname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Create the file descriptor for accessing the inotify API */
    fd = inotify_init();
    if (fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    /* Allocate memory for watch descriptors */
    int wd = inotify_add_watch(fd, dirname, IN_CREATE | IN_MOVED_TO);
    if (wd == -1) {
        fprintf(stderr, "Cannot watch '%s': %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    handle_events(fd, wd, dirname);
    exit(EXIT_SUCCESS);
}

