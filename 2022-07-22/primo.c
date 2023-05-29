#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#define MAX_PATH 2048

void print_struct(const char *dirname, int indentation);

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

    print_struct(argv[1], 0);

}

void print_struct(const char *dirname, int indentation) {
    DIR *d = opendir(dirname);
    if (d == NULL) {
        printf("Error: cannot open %s\n", dirname);
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
            char path[MAX_PATH];
            snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);
            print_struct(path, indentation + 1);
        }
    }
}
