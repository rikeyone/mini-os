#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "include/debug.h"

static int fd1[2],fd2[2];

void sigpipe_handler(int signo)
{
	pr_info("catch signal SIGPIPE!\n");
}

int signal_setup(void)
{
	struct sigaction act, oact;

	act.sa_handler = sigpipe_handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);

	if (sigaction(SIGPIPE, &act, &oact) < 0)
		err_exit("sigaction error\n");
	return 0;
}

int main(int argc, char *argv[])
{
	int ret;
	char greet[20];
	pid_t pid;

	if (pipe(fd1) < 0)
		err_exit("pipe error\n");
	if (pipe(fd2) < 0)
		err_exit("pipe error\n");

	signal_setup();

	if ((pid = fork()) < 0) {
		err_exit("fork error\n");
	} else if (pid > 0) {
		close(fd1[0]);
		close(fd2[1]);
		memcpy(greet, "Hello, child!\n", 15);
		write(fd1[1], greet, strlen(greet) + 1);
		/* read blockly for child's message */
		read(fd2[0], greet, 16);
		pr_info("parent receive: %s\n", greet);
		waitpid(pid, NULL, 0);

		/* write to a closed pipe fd will trigger SIGPIPE */
		write(fd1[1], greet, strlen(greet) + 1);
		sleep(1);
	} else {
		close(fd1[1]);
		close(fd2[0]);
		memcpy(greet, "Hello, parent!\n", 16);
		write(fd2[1], greet, strlen(greet) + 1);
		/* read blockly for parent message */
		read(fd1[0], greet, 15);
		pr_info("child receive: %s\n", greet);
	}
	return 0;
}

