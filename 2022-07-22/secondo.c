#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

#define MAX_PATH 2048

void print_struct(int dirfd, int indentation);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <dir>\n", argv[0]);
        return 1;
    }

    struct stat s;
    if (stat(argv[1], &s) != 0) {
        printf("Error: %s does not exist\n", argv[1]);
        return 1;
    }

    if (!S_ISDIR(s.st_mode)) {
        printf("Error: %s is not a directory\n", argv[1]);
        return 1;
    }

    int dirfd = open(argv[1], O_RDONLY);
    if (dirfd == -1) {
        printf("Error: cannot open %s\n", argv[1]);
        return 1;
    }
 
    print_struct(dirfd, 0);
}

void print_struct(int dirfd, int indentation) {
    DIR *d = fdopendir(dirfd);
    if (d == NULL) {
        printf("Error: cannot open in print_struct\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        for (int i = 0; i < indentation; i++) {
            printf("  ");
        }
        printf("%s\n", entry->d_name);

        if (entry->d_type == DT_DIR) {
            int newdirfd = openat(dirfd, entry->d_name, O_RDONLY);
            if (newdirfd == -1) {
                printf("Error: cannot open %s\n", entry->d_name);
                continue;
            }

            print_struct(newdirfd, indentation + 1);
        }
    }
}
