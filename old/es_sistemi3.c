#include <stdio.h>
#include <dlfcn.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char *abspath = realpath(argv[1], NULL);
    void *handle = dlopen(abspath, RTLD_NOW);

    if (handle == NULL) {
        return execv(argv[1], argv + 1);
    }

    int (*main)(int, char **) = (int (*)(int, char **)) dlsym(handle, "main");

    char *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    main(argc - 1, argv + 1);
    free(abspath);
}
