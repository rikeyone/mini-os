#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

static void show_time_t(void)
{
	time_t now;

	now = time(NULL);
	printf("time_t now: %ld\n",(long) now);
}

static void show_timeval(void)
{
	struct timeval tv;
	int ret;

	gettimeofday(&tv, NULL);
	printf("timeval.tv_sec:%ld  timeval.tv_usec:%ld\n",
			(long)tv.tv_sec, (long)tv.tv_usec);
}

static void show_timespec(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_REALTIME, &ts);
	printf("timespec.tv_sec:%ld  timespec.tv_nsec:%ld\n",
			(long)ts.tv_sec, (long)ts.tv_nsec);
}

static void show_utc_time(void)
{
	struct tm *gm;
	time_t now;

	now = time(NULL);
	gm = gmtime(&now);
	printf("UTC:%d-%d-%d %d:%d:%d\n",
			1900+gm->tm_year, gm->tm_mon, gm->tm_mday,
			gm->tm_hour, gm->tm_min, gm->tm_sec);

}

static void show_localtime(void)
{
	struct tm *gm;
	time_t now;

	now = time(NULL);
	gm = localtime(&now);
	printf("LOCAL:%d-%d-%d %d:%d:%d\n",
			1900+gm->tm_year, gm->tm_mon, gm->tm_mday,
			gm->tm_hour, gm->tm_min, gm->tm_sec);

}

int main(int argc, char *argv[])
{
	show_time_t();
	show_timeval();
	show_timespec();
	show_utc_time();
	show_localtime();
	exit(0);
}
