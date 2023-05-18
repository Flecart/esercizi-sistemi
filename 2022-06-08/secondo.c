#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

void invert(char *syml, char * target) {
    unlink(syml);
    link(target, syml);
    unlink(target);
    symlink(syml, target);
}

void invert_dir(char *dir) {
    DIR *dirp = opendir(dir);
    if (dirp == NULL) {
        perror("opendir");
        exit(1);
    }
    struct dirent *dirent;
    while ((dirent = readdir(dirp)) != NULL) {
        if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0) {
            continue;
        }
        char *path = malloc(strlen(dir) + strlen(dirent->d_name) + 2);
        sprintf(path, "%s/%s", dir, dirent->d_name);
        struct stat statbuf;
        int stat_ret = lstat(path, &statbuf);
        if (stat_ret == -1) {
            perror("lstat");
            exit(1);
        }
        if (S_ISLNK(statbuf.st_mode)) {
            char *abspath = realpath(path, NULL);
            invert(path, abspath);
            free(abspath);
        } else if (S_ISDIR(statbuf.st_mode)) {
            invert_dir(path);
        }
        free(path);
    }
    closedir(dirp);
}


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

    if (S_ISLNK(statbuf.st_mode)) {
        char *abspath = realpath(argv[1], NULL);
        invert(argv[1], abspath);
        free(abspath);
    } else if (S_ISDIR(statbuf.st_mode)) {
        invert_dir(argv[1]);
    } else {
        printf("not a symlink nor a dir\n");
        exit(1);
    }
}
