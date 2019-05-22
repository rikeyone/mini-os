#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main (int argc, char *argv[])
{
	pid_t pid;
	char buf[10];
	int ret;

	if ((pid = fork()) < 0) {
		printf("fork error\n");
		exit(1);
	} else if (pid == 0) {
		read(STDIN_FILENO, buf, 10);
		buf[9] = '\0';
		printf("child read from stdin:%s\n",buf);
	} else {
		wait(NULL);
		read(STDIN_FILENO, buf, 10);
		buf[9] = '\0';
		printf("parent read from stdin:%s\n",buf);
	}
	return 0;
}
