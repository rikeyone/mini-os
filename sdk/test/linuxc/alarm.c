#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include "include/debug.h"

void sig_func(int signo)
{
	pr_info("SIGALRM received!\n");
}

 int set_signal(int signo, void func(int signo))
 {
     struct sigaction act, oact;

     act.sa_handler = func;
     sigemptyset(&act.sa_mask);
     act.sa_flags = 0;
 #ifdef SA_INTERRUPT
     act.sa_flags |= SA_INTERRUPT;
 #endif

     if (sigaction(signo, &act, &oact) < 0) {
         printf("sigaction error,%s\n",strerror(errno));
         return 1;
     }
     return 0;
 }

int main()
{
	char buf[10];
	int fd;

	set_signal(SIGALRM, sig_func);
	alarm(5);
	pr_info("block read\n");
	read(STDIN_FILENO, buf, 10);
	pr_info("read returns\n");

	return 0;
}
