include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // 8:54
    struct stat statbuf;
    int stat_ret = lstat(argv[1], &statbuf);
    if (S_ISLNKstat_ret) {
        // TODO:
    }

    if (!IS_

    char *abspath = realpath(argv[1], NULL);

    // TODO: check is symlink
    unlink(argv[1]);
    link(abspath, argv[1]);
    unlink(abspath);
    symlink(argv[1], abspath);

    printf("%s\n", abspath);

    free(abspath);
}
