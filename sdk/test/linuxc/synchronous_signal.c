#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "include/debug.h"

static volatile sig_atomic_t sigflag; /* set nonzero by sig handler */
static sigset_t mask, omask, zeromask;

static void sig_usr(int signo)	/* one signal handler for SIGUSR1 and SIGUSR2 */
{
	pr_info("catch signal:%d\n", signo);
	sigflag = 1;
}

void initialize(void)
{
	if (signal(SIGUSR1, sig_usr) == SIG_ERR)
		err_exit("signal(SIGUSR1) error");
	if (signal(SIGUSR2, sig_usr) == SIG_ERR)
		err_exit("signal(SIGUSR2) error");

	sigemptyset(&zeromask);
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	sigaddset(&mask, SIGUSR2);

	if (sigprocmask(SIG_BLOCK, &mask, &omask) < 0)
		err_exit("SIG_BLOCK error");
}

void wake_parent(pid_t pid)
{
	kill(pid, SIGUSR2);		/* tell parent we're done */
}

void wait_parent(void)
{
	while (sigflag == 0)
		sigsuspend(&zeromask);	/* and wait for parent */
	sigflag = 0;

	/* Reset signal mask to original value */
	if (sigprocmask(SIG_SETMASK, &omask, NULL) < 0)
		err_exit("SIG_SETMASK error");
}

void wake_child(pid_t pid)
{
	kill(pid, SIGUSR1);			/* tell child we're done */
}

void wait_child(void)
{
	while (sigflag == 0)
		sigsuspend(&zeromask);
	sigflag = 0;

	/* Reset signal mask to original value */
	if (sigprocmask(SIG_SETMASK, &omask, NULL) < 0)
		err_exit("SIG_SETMASK error");
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
		wake_child(pid);
		wait(NULL);
	} else {
		pr_info("child running\n");
		pr_info("wake up parent\n");
		wake_parent(getppid());
		wait_parent();
		pr_info("child waken up\n");
	}
	return 0;
}
