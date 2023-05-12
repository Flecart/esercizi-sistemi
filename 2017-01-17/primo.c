#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#define MAXPATH 2048
#define MAXFILES 100000

int dir_count;
int file_count;
int files[MAXFILES];

void rec_check(char *dirname) {
    DIR *curr_dir = opendir(dirname);
    if (curr_dir == NULL) {
        perror("opendir");
     exit(EXIT_FAILURE);
    }

    char filename[MAXPATH];
    struct dirent *dir;
    while ((dir = readdir(curr_dir)) != NULL) {
        if (strcmp(".", dir->d_name) == 0 || strcmp("..", dir->d_name) == 0) continue;
        snprintf(filename, MAXPATH, "%s/%s", dirname, dir->d_name);
        struct stat sb;

        if (stat(filename, &sb) == -1) {
            perror("stat");
            exit(EXIT_FAILURE);
        }

        if ((sb.st_mode & S_IFMT) == S_IFDIR) {
            dir_count++;
            rec_check(filename);
        } else if ((sb.st_mode & S_IFMT) == S_IFREG) {
            files[file_count++] = (int) sb.st_ino;
        }

    }
}

int compare_ints(const void * a, const void * b) {
    return (*(int *)a) - (*(int *) b);
}

int count_files() {
    int same = 1;
    for (int i = 1; i < file_count; i++) {
        if (files[i] != files[i - 1]) same++;
    }
    return same;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("[USAGE] %s [dir-name]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    rec_check(argv[1]);

    printf("%d number of dirs\n", dir_count);
    qsort(files, file_count, sizeof(int), compare_ints);
    printf("%d number of files\n", count_files());
}
