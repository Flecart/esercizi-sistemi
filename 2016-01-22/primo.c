#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int get_num_prefix(const char *name) {
    int num = 0;
    while (name != NULL && isdigit(*name)) {
        int new_val = 10 * num + (int)(*name - '0');
        name++;
        if (new_val > 0) {  // check for overflow 
            num = new_val;
        } else break;
    }

    return num;
}

int numsort(const struct dirent **dir1, const struct dirent **dir2) {
    return get_num_prefix((*dir1)->d_name) > get_num_prefix((*dir2)->d_name);
}

int filter(const struct dirent *dir) {
    const char *name = dir->d_name;
    if (strlen(name) < 1) return 0;
    else return isdigit(name[0]);
}

int main(int argc, char *argv[]) {
    struct dirent **dir;
    int num;
    if ((num = scandir(argv[1], &dir, filter, numsort)) >= 0) {
        for (int i = 0; i < num; i++) {
            printf("%s\n", dir[i]->d_name);
            free(dir[i]);
        }
    } else {
        perror("scandir");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
