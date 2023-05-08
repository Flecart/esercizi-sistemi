#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
# define MAX_PATH 2048

/* Read all available inotify events from the file descriptor 'fd'.
   wd is the table of watch descriptors for the directories in argv.
   argc is the length of wd and argv.
   argv is the list of watched directories.
   Entry 0 of wd and argv is unused. */

static void execute(const char *filename) {
    int wstatus;
    if (fork()) {
        execlp("/bin/bash", "bash", "-c", filename, NULL);
    } else {
        wait(&wstatus);
    }
}

    static void
handle_events(int fd, int wd, char *dirname)
{
    /* Some systems cannot read integer variables if they are not
       properly aligned. On other systems, incorrect alignment may
       decrease performance. Hence, the buffer used for reading from
       the inotify file descriptor should have the same alignment as
       struct inotify_event. */

    char buf[4096]
        __attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    ssize_t len;

    /* Loop while events can be read from inotify file descriptor. */

    for (;;) {
        /* Read some events. */
        len = read(fd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        /* If the nonblocking read() found no events to read, then
           it returns -1 with errno set to EAGAIN. In that case,
           we exit the loop. */
        if (len <= 0)
            break;

        /* Loop over all events in the buffer */

        for (char *ptr = buf; ptr < buf + len;
                ptr += sizeof(struct inotify_event) + event->len) {

            event = (const struct inotify_event *) ptr;

            /* Print event type */

            if (event->mask & IN_CREATE)
                printf("IN_CREATE: ");
            if (event->mask & IN_MOVED_TO)
                printf("IN_MOVED_TO: ");

            if (event->len)
                printf("%s\n", event->name);

            execute(event->name);

            char path_name[MAX_PATH];
            snprintf(path_name, MAX_PATH, "%s/%s", dirname, event->name);
            unlink(path_name);
        }
    }
}



    int
main(int argc, char* argv[])
{

    char *dirname = "./exec";
    char buf;
    int fd, i, poll_num;
    nfds_t nfds;
    struct pollfd fds[2];

    if (argc != 1) {
        printf("Usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Press ENTER key to terminate.\n");

    /* Create the file descriptor for accessing the inotify API */

    fd = inotify_init();
    if (fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    /* Allocate memory for watch descriptors */

    int wd = inotify_add_watch(fd, dirname, IN_CREATE | IN_MOVED_TO);
    if (wd == -1) {
        fprintf(stderr, "Cannot watch '%s': %s\n",
                argv[i], strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Prepare for polling */

    nfds = 2;

    /* Console input */

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    /* Inotify input */

    fds[1].fd = fd;
    fds[1].events = POLLIN;

    /* Wait for events and/or terminal input */

    printf("Listening for events.\n");
    while (1) {
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1) {
            if (errno == EINTR)
                continue;
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if (poll_num > 0) {
            if (fds[0].revents & POLLIN) {
                /* Console input is available. Empty stdin and quit */
                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }

            if (fds[1].revents & POLLIN) {
                /* Inotify events are available */
                handle_events(fd, wd, dirname);
            }
        }
    }

    printf("Listening for events stopped.\n");

    /* Close inotify file descriptor */

    close(fd);
    exit(EXIT_SUCCESS);
}

