#include <stdio.h>
#include <sys/inotify.h>
#include <errno.h>
#include <unistd.h>

// as docs said, return value is event + MAX_NAME of the file.
typedef struct event {
    struct inotify_event ev;
    char name[256 + 1];
} event_t;

char *get_name(char *dir_name, char *file_name) {
    int dir_len = strlen(dir_name);
    int file_len = strlen(file_name);
    char *space = (char *) malloc(dir_len + file_len + 2);
    
    strcpy(space, dir_name);
    space[dir_len] = '/';
    strcat(space, file_name);
    space[1 + dir_len + file_len] = '\0';

    return space 
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("USAGE: doanddelete <directory>\n");
        return 1;
    }
    
    int init = inotify_init();

    // TODO: make dir checks on argv[1]

    int ret_val = inotify_add_watch(init, argv[1], IN_CREATE | IN_MOVED_TO);
    if (ret_val == -1) {
        printf("inotify_add_watch failed with errno: %d\n", errno);
        return 1;
    }
    
    struct event eventt;

    int num_bytes_read;
    while ((num_bytes_read = read(init, &eventt, sizeof(struct event))) > 0) {
        printf("mmmm %d\n", eventt.ev.wd);        
        printf("nameis %s\n", eventt.name);

        char * name = get_name(argv[1], eventt.name);
        printf("concatenaeted name is %s\n", name);

        if (fork()) {
            execl(name);
        }
        unlink(name); 
    }
    // TODO: handle eintr
    printf("%ld aaaa\n", sizeof(struct inotify_event));
    printf("im out %d errno: %d\n", num_bytes_read, errno);
// 18:59
}
