#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s <symlink>\n", argv[0]);
        exit(1);
    }
    struct stat statbuf;
    int stat_ret = lstat(argv[1], &statbuf);
    if (stat_ret == -1) {
        perror("lstat");
        exit(1);
    }

    if (!S_ISLNK(statbuf.st_mode)) {
        printf("not a symlink\n");
        exit(1);
    } 

    char *abspath = realpath(argv[1], NULL);

    unlink(argv[1]);
    link(abspath, argv[1]);
    unlink(abspath);
    symlink(argv[1], abspath);

    free(abspath);
}
