#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <string.h>
       #include <time.h>

int main(int argc, char *argv[]) {
    srand(time(NULL));
    // TODO: controlla il numero di argomenti
    //
    int src = open(argv[1], O_RDONLY);
    int dest = open(argv[2], O_WRONLY);

    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    fcntl(dest, F_SETLKW, &lock);
    puts("locking the file\n");

    while (sendfile(dest, src, NULL, 256) > 0);

    sleep(rand() % 10 + 1);
    puts("unlocking\n");
    lock.l_type = F_UNLCK;
    fcntl(dest, F_SETLKW, &lock);
    sleep(rand() % 10 + 1);
    close(src);
    close(dest);

    return 0;
}
