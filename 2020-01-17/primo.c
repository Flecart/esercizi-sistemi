#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

ino_t file_inode;

char *move_to_first(char *dirname) {
    while (*dirname != '/' && *dirname != '\0') {
        dirname++;
    }

    // nel caso ci fossero piu` slash consecutivi
    while (*dirname == '/') {
        dirname++;
    }
    return dirname;
}

void print_output(char *prefix, char *dirname, char *filename) {
    char *dir = move_to_first(dirname);
    if (*dir == '\0') {
        printf("%s%s\n", prefix, filename);
    } else {
        printf("%s%s/%s\n", prefix, dir, filename);
    }
}

void check(char *absfilepath, char *dirname) {
    DIR *d;
    struct dirent *dir;

    if ((d = opendir(dirname)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    
    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
            continue;
        }

        char newname[strlen(dirname) + strlen(dir->d_name) + 2];
        sprintf(newname, "%s/%s", dirname, dir->d_name);
        if (dir->d_type == DT_DIR) {
            check(absfilepath, newname);
        } else if (dir->d_type == DT_REG) {
            if (dir->d_ino == file_inode) {
                print_output("link ", dirname, dir->d_name);
            }
        } else if (dir->d_type == DT_LNK) {
            char *linkpath = realpath(newname, NULL);
            if (strcmp(linkpath, absfilepath) == 0) {
                print_output("symlink ", dirname, dir->d_name);
            }
            free(linkpath);
        }
    }

    if (closedir(d) != 0) {
        perror("closedir");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <file> <directory>\n", argv[0]);
        return 1;
    }

    // check if argv[1] is a file
    struct stat sb;
    if (lstat(argv[1], &sb) == -1) {
        perror("lstat file");
        return 1;
    }

    if (!S_ISREG(sb.st_mode)) {
        printf("%s is not a file\n", argv[1]);
        return 1;
    } else {
        file_inode = sb.st_ino;
    }

    // check if argv[2] is a directory
    if (lstat(argv[2], &sb) == -1) {
        perror("lstat directory");
        return 1;
    }

    if (!S_ISDIR(sb.st_mode)) {
        printf("%s is not a directory\n", argv[2]);
        return 1;
    }

    char *filepath = realpath(argv[1], NULL);
    check(filepath, argv[2]);
    free(filepath);
}
