#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

void test_func()
{
    static count = 0;

    printf("count is %d\n", count++);
}

void set_signal()
{
    struct sigaction act;

    act.sa_handler = test_func;
    act.sa_flags  = 0;

    sigemptyset(&act.sa_mask);
    sigaction(SIGPROF, &act, NULL);
}

void set_time()
{
    struct itimerval val;

    val.it_value.tv_sec = 1;
    val.it_value.tv_usec = 0;

    val.it_interval = val.it_value;

    setitimer(ITIMER_PROF, &val, NULL);
}

int main(int argc, char **argv)
{
    set_signal();
    set_time();
    while(1);

    return 0;
}
