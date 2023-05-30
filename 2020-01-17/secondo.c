#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define COPY 1
#define LINK 2
#define SYMLINK 3 

ino_t file_inode;
int flags = 0;

char *move_to_first(char *dirname);
void print_output(char *prefix, char *dirname, char *filename);
void copy_file(char *srcfile, char *dest);
void check(char *absfilepath, char *dirname);
void handle_file(char *filepath, char *currname, struct dirent *dir);

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "cls")) != -1) {
        switch (opt) {
            case 'c':
                flags = COPY;
                break;
            case 'l':
                flags = LINK;
                break;
            case 's':
                flags = SYMLINK;
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-c | -l | -s] <filename> <dirname>\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (optind != 2 && optind != 1) {
        fprintf(stderr, "Exactly one or no argument is needed\n");
        fprintf(stderr, "Usage: %s [-c | -l | -s] <filename> <dirname>\n",
                argv[0]);
        exit(EXIT_FAILURE);
    } 

    if (argc != 2 + optind) {
        fprintf(stderr, "Usage: %s [-c | -l | -s] <filename> <dirname>\n",
                argv[0]);
        return 1;
    }

    struct stat sb;
    if (lstat(argv[optind], &sb) == -1) {
        perror("lstat file");
        return 1;
    }

    if (!S_ISREG(sb.st_mode)) {
        printf("%s is not a file\n", argv[1]);
        return 1;
    } else {
        file_inode = sb.st_ino;
    }

    if (lstat(argv[optind + 1], &sb) == -1) {
        perror("lstat directory");
        return 1;
    }

    if (!S_ISDIR(sb.st_mode)) {
        printf("%s is not a directory\n", argv[2]);
        return 1;
    }

    char *filepath = realpath(argv[optind], NULL);
    if (filepath == NULL) {
        perror("realpath");
        return 1;
    }
    check(filepath, argv[optind + 1]);
    free(filepath);
}

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

void copy_file(char *srcfile, char *dest) {
    int fd = open(srcfile, O_RDONLY);
    if (fd == -1) {
        printf("srcfile: %s\n", srcfile);
        perror("open-src");
        exit(EXIT_FAILURE);
    }

    int fd2 = open(dest, O_WRONLY | O_CREAT, 0666);
    if (fd2 == -1) {
        perror("open-dest");
        exit(EXIT_FAILURE);
    }

    char buf[1024];
    int nread;
    while ((nread = read(fd, buf, sizeof(buf))) > 0) {
        if (write(fd2, buf, nread) != nread) {
            perror("write");
            exit(EXIT_FAILURE);
        }
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

        if (dir->d_type == DT_DIR) {
            char newname[strlen(dirname) + strlen(dir->d_name) + 2];
            sprintf(newname, "%s/%s", dirname, dir->d_name);
            check(absfilepath, newname);
        } else {
            handle_file(absfilepath, dirname, dir);
        }
    }

    if (closedir(d) != 0) {
        perror("closedir");
        exit(EXIT_FAILURE);
    }
}

void handle_file(char *absfilepath, char *dirname, struct dirent *dir) {
    char newname[strlen(dirname) + strlen(dir->d_name) + 2];
    sprintf(newname, "%s/%s", dirname, dir->d_name);
    char *linkpath = realpath(newname, NULL);

    switch(flags) {
        case 0: 
            if (dir->d_type == DT_REG && dir->d_ino == file_inode) {
                print_output("link ", dirname, dir->d_name);
            } else if (dir->d_type == DT_LNK && strcmp(linkpath, absfilepath) == 0) {
                print_output("symlink ", dirname, dir->d_name);
            }
            break;

        case COPY:
            if ((dir->d_type == DT_REG && dir->d_ino == file_inode)
                    || 
                    (dir->d_type == DT_LNK && strcmp(linkpath, absfilepath) == 0)) {
                if (unlink(newname) != 0) {
                    perror("unlink");
                    exit(EXIT_FAILURE);
                }
                copy_file(absfilepath, newname);
            }
            break;

        case LINK:
            if (dir->d_type == DT_LNK && strcmp(linkpath, absfilepath) == 0) {
                if (unlink(newname) != 0) {
                    perror("unlink");
                    exit(EXIT_FAILURE);
                }
                if (link(absfilepath, newname) != 0) {
                    perror("link");
                    exit(EXIT_FAILURE);
                }
            }
            break;

        case SYMLINK:
            if (dir->d_type == DT_REG && dir->d_ino == file_inode) {
                if (unlink(newname) != 0) {
                    perror("unlink");
                    exit(EXIT_FAILURE);
                }
                if (symlink(absfilepath, newname) != 0) {
                    perror("symlink");
                    exit(EXIT_FAILURE);
                }
            }
            break;
        default:
            // non dovrebbe mai succedere
            break;
    }

    free(linkpath);
}
