#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>


int set_signal(int signo, void func(int signo))
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	} else {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}
	if (sigaction(signo, &act, &oact) < 0) {
		printf("sigaction error,%s\n",strerror(errno));
		return 1;
	}
	return 0;
}

void signal_handler(int signo)
{
	switch (signo) {
	case SIGALRM:
		printf("SIGALRM triggered\n");
		break;
	case SIGINT:
		printf("SIGINT triggered\n");
		break;
	case SIGQUIT:
		printf("SIGQUIT triggered\n");
		break;
	case SIGTERM:
		printf("process terminated!!!\n");
		break;
	default:
		printf("Do nothing for signal:%d\n", signo);
		break;
	}
}

int sigprocmask_test(void)
{
	sigset_t set, oset, pendset;

	if (set_signal(SIGINT, signal_handler) < 0) {
		printf("set signal error, exit\n");
		exit(1);
	}
	if (set_signal(SIGQUIT, signal_handler) < 0) {
		printf("set signal error, exit\n");
		exit(1);
	}

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	if (sigprocmask(SIG_BLOCK, &set, &oset) < 0) {
		printf("sigprocmask error, exit\n");
		exit(1);
	}
	printf("blocked signal SIGINT, please try ctrl-c!\n");
	printf("use ctrl-\\ to wake up process using SIGQUIT\n");
	printf("wait for a signal by pause...\n");
	pause();

	if (sigpending(&pendset) < 0) {
		printf("sigpending error\n");
		exit(1);
	}

	if (sigismember(&pendset, SIGINT)) {
		printf("SIGINT is pending\n");
	}

	if (sigprocmask(SIG_SETMASK, &oset, NULL) < 0) {
		printf("sigprocmask error, exit\n");
		exit(1);
	}
	printf("reset signal mask to default, please try ctrl-c again!\n");

	/*
	 * during reset and pause, it has a time window here which is unsafe
	 * this time window may make the pause wait for another signal to wake up
	 * but not the pending one
	 */

	pause();
}

int sigsuspend_test(void)
{
	sigset_t set, oset, pendset, waitset;

	if (set_signal(SIGINT, signal_handler) < 0) {
		printf("set signal error, exit\n");
		exit(1);
	}
	if (set_signal(SIGQUIT, signal_handler) < 0) {
		printf("set signal error, exit\n");
		exit(1);
	}

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	if (sigprocmask(SIG_BLOCK, &set, &oset) < 0) {
		printf("sigprocmask error, exit\n");
		exit(1);
	}
	printf("blocked signal SIGINT, please try ctrl-c!\n");
	printf("use ctrl-\\ to wake up process using SIGQUIT\n");
	printf("wait for a signal by pause...\n");
	pause();

	if (sigpending(&pendset) < 0) {
		printf("sigpending error\n");
		exit(1);
	}

	if (sigismember(&pendset, SIGINT)) {
		printf("SIGINT is pending\n");
	}

	printf("sigsuspend wait for a signal without SIGQUIT\n");
	sigemptyset(&waitset);
	sigaddset(&waitset, SIGQUIT);

	//suspend return -1 as success return
	if (sigsuspend(&waitset) != -1) {
		printf("sigsuspend error, exit\n");
		exit(1);
	}
	printf("sigsuspend get an signal and returned\n");
	printf("sigsuspend should reset signal mask to the status before sigsuspend, please try ctrl-c again!\n");
	pause();
}

/* alarm test */
int alarm_test(void)
{
	char buf[20];
	int n;

	if (set_signal(SIGALRM, signal_handler) < 0) {
		printf("set signan error, exit\n");
		exit(1);
	}
	printf("alarm timer will expire 10 seconds later\n");
	alarm(10);

	printf("read blockly from stdin for 20 characters, timeout 10 seconds\n");
	n = read(STDIN_FILENO, buf, 20);
	if (n < 0) {
		if (errno == EINTR)
			printf("read interrupted by signal, exit\n");
		exit(1);
	}

	printf("read returned successfully, cancel the alarm timer\n");
	alarm(0);
	write(STDOUT_FILENO, buf, n);
	exit(0);

}

void usage(int argc, char *argv[])
{
	printf("Usage:\n");
	printf("\t %s [-a] \"alarm test\"\n", argv[0]);
	printf("\t %s [-m] \"sigprocmask test\"\n", argv[0]);
	printf("\t %s [-s] \"sigsuspend test\"\n", argv[0]);
}

int main(int argc, char *argv[])
{
	int opt, ret, i;
	pid_t pid;

	if (argc == 1) {
		usage(argc, argv);
		exit(1);
	}

	while((opt = getopt(argc, argv, "sam")) != -1) {
		switch(opt) {
		case 'a':
			alarm_test();
			break;
		case 'm':
			sigprocmask_test();
			break;
		case 's':
			sigsuspend_test();
			break;
		default:
			printf("unknown option:%c\n", optopt);
			usage(argc, argv);
			break;
		}
	}
	return 0;
}

