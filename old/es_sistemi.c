#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>

int transform(char *name) {
    struct stat filestat;
    int ret_val = lstat(name, &filestat);
    if (ret_val == -1) {
        printf("got error \n");
        return 1;
    }

    int is_symlink = 0;
    if (S_ISLNK(filestat.st_mode)) {
        is_symlink = 1;
    }
    
    char *abspath = realpath(name, NULL);

    if (is_symlink) {
        unlink(name);
        symlink(abspath, name);
    }

    free(abspath);
}

char *concat(char *first, char *second) {
    int first_len = strlen(first);
    char *dest = (char *) malloc(first_len + strlen(second) + 2);
    strcpy(dest, first);
    dest[first_len] = '/';
    dest[first_len + 1] = '\0';
    strcat(dest, second);
    return dest;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Expected 2 arguments, got %d", argc);
        return 1;
    }

    DIR * dir = opendir(argv[1]);
    if (dir == NULL) {
        printf("could not open dir\n");
        return 1;
    }

    struct dirent *d = readdir(dir);  // get rid of first directory.

    while ((d = readdir(dir)) != NULL) {
        char *endstr = concat(argv[1], d->d_name);
        printf("name is %s\n", endstr);
        transform(endstr);
        free(endstr);
    }
}
