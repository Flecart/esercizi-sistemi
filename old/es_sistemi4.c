#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: abslink [symlink-filename]");
		return 1;
	}
	
	struct stat statbuf;
	
	int ret_stat = lstat(argv[1], &statbuf);
	if (ret_stat != 0) {
		printf("ERROR: got errno %d\n", errno);
		return 1;
	}
	
	printf("file mode %d\n", statbuf.st_mode);
	
	if ((statbuf.st_mode & S_IFLNK) != S_IFLNK) {
		perror("file is not a symlink");
		return 1;
	}
	
	char *abspath = realpath(argv[1], NULL);
	
	printf("found path %s\n", abspath);
	
	int wstatus;
	
	if (fork()) {
		execlp("rm", "rm", argv[1], NULL);
	} else wait(&wstatus);
	
	if (fork()) {
		// create symlink, and force creation
		execlp("ln", "ln", "-s", abspath, argv[1], NULL);
	} else {
		wait(&wstatus); // until children terminates
		printf("children terminated\n");
	}
	
	free(abspath);
	
	return 0;
}
