#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "include/debug.h"

static int	pfd1[2], pfd2[2];

void initialize(void)
{
	if (pipe(pfd1) < 0 || pipe(pfd2) < 0)
		err_exit("pipe error");
}

void wake_parent(void)
{
	if (write(pfd2[1], "c", 1) != 1)
		err_exit("write error");
}

void wait_parent(void)
{
	char	c;

	if (read(pfd1[0], &c, 1) != 1)
		err_exit("read error");

	if (c != 'p')
		err_exit("WAIT_PARENT: incorrect data");
}

void wake_child(void)
{
	if (write(pfd1[1], "p", 1) != 1)
		err_exit("write error");
}

void wait_child(void)
{
	char	c;

	if (read(pfd2[0], &c, 1) != 1)
		err_exit("read error");

	if (c != 'c')
		err_exit("WAIT_CHILD: incorrect data");
}


int main (int argc, char *argv[])
{
	pid_t pid;
	int ret;

	initialize();
	if ((pid = fork()) < 0) {
		err_exit("fork() error\n");
	} else if (pid > 0) { //parent
		wait_child();
		pr_info("parent waken up\n");
		pr_info("parent running\n");
		pr_info("wake up child\n");
		wake_child();
		wait(NULL);
	} else {
		pr_info("child running\n");
		pr_info("wake up parent\n");
		wake_parent();
		wait_parent();
		pr_info("child waken up\n");
	}
	return 0;
}
