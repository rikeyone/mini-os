#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sig_handler(int sig)
{
	printf("signal catch:%d\n", sig);
}

int handle_test(void)
{
	signal(SIGABRT, sig_handler);
	printf("abort after 3 seconds\n");
	sleep(3);
	abort();
}

int block_test(void)
{
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGABRT);
	sigprocmask(SIG_BLOCK, &mask, NULL);
	printf("abort after 3 seconds\n");
	sleep(3);
	abort();
}

int main()
{
//	handle_test();
	block_test();
	while(1);
}
