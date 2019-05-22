#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <pthread.h>
#include "include/debug.h"


void alarm_func(int signo)
{
	pr_info("SIGALRM receive!\n");
}

void *thr_fn(void *arg)
{
	int err, signo;
	sigset_t	set,mask;

	signal(SIGALRM, alarm_func);

	sigfillset(&mask);
	sigdelset(&mask, SIGALRM);
	if ((err = pthread_sigmask(SIG_SETMASK, &mask, NULL)) != 0)
		err_exit("SIG_BLOCK error\n");

	sigfillset(&set);
	sigdelset(&set, SIGALRM);
	for (;;) {
		err = sigwait(&set, &signo);
		if (err != 0) {
			err_exit("sigwait failed\n");
		}

		switch (signo) {
		case SIGTERM:
			pr_info("Catch SIGTERM; exiting\n");
			break;
		case SIGQUIT:
			pr_info("Catch SIGQUIT; exiting\n");
			exit(0);
			break;
		case SIGINT:
			pr_info("Catch SIGINT; exiting\n");
			break;

		default:
			pr_info("Unexpected signal %d\n", signo);
		}
	}
	return(0);
}

pthread_t setup_signal_thread(void)
{
	int err;
	struct sigaction	sa;
	sigset_t	mask;
	pthread_t	tid;

	/*
	 * Block all signals in main thread
	 */
	sigfillset(&mask);
	if ((err = pthread_sigmask(SIG_SETMASK, &mask, NULL)) != 0)
		err_exit("SIG_BLOCK error\n");

	/*
	 * Create a child thread to handle SIGHUP and SIGTERM.
	 */
	err = pthread_create(&tid, NULL, thr_fn, 0);
	if (err != 0)
		err_exit("can't create thread\n");
	return tid;
}

int main()
{
	char buf[10];
	pthread_t tid;
	void *tret;

	tid = setup_signal_thread();

	pr_info("signal thread created\n");
	read(STDIN_FILENO, buf, 10);
	pr_info("read was interrupted!\n");
}
