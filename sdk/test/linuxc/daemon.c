/*
 * Copyright (C) 2019 xiehaocheng <xiehaocheng127@163.com>
 *
 * All Rights Reserved
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

#define CONFIGFILE "/etc/daemon.conf"
#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

#define pr_debug(fmt,...)	do{ printf("[%ld] DEBUG: "fmt,(long)getpid(),##__VA_ARGS__); fflush(stdout); }while(0)
#define pr_info(fmt,...)	do{ printf("[%ld] INFO:  "fmt,(long)getpid(),##__VA_ARGS__); fflush(stdout); }while(0)
#define pr_err(fmt,...)		do{ printf("[%ld] ERROR: "fmt,(long)getpid(),##__VA_ARGS__);fflush(stdout); }while(0)
#define err_exit(fmt,...)	do{ printf("[%ld] ERROR: "fmt,(long)getpid(),##__VA_ARGS__); exit(1); }while(0)

int file_lock(int fd)
{
	struct flock fl;

	/*
	 * Get lock for the whole file
	 */
	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return(fcntl(fd, F_SETLK, &fl));
}

int is_daemon_running(void)
{
	int		fd;
	char	buf[16];

	fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
	if (fd < 0) {
		err_exit("can't open %s: %s\n", LOCKFILE, strerror(errno));
	}
	/*
	 * Get the file lock
	 */
	if (file_lock(fd) < 0) {
		if (errno == EACCES || errno == EAGAIN) {
			close(fd);
			return 1;
		}
		err_exit("can't lock %s: %s\n", LOCKFILE, strerror(errno));
	}
	/*
	 * Cut the file to offset 0
	 */
	ftruncate(fd, 0);
	sprintf(buf, "%ld", (long)getpid());
	/*
	 * Update pid to the lock file
	 */
	write(fd, buf, strlen(buf)+1);
	return 0;
}

void reread(void)
{
	/* ... */
	pr_info("trigger re-read from config file\n");
}

void *thr_fn(void *arg)
{
	int err, signo;
	sigset_t	mask;

	sigfillset(&mask);
	for (;;) {
		err = sigwait(&mask, &signo);
		if (err != 0) {
			err_exit("sigwait failed\n");
		}

		switch (signo) {
		case SIGHUP:
			pr_info("Re-reading configuration file\n");
			reread();
			break;

		case SIGTERM:
			pr_info("Catch SIGTERM; exiting\n");
			exit(0);

		default:
			pr_info("Unexpected signal %d\n", signo);
		}
	}
	return(0);
}

void signal_setup(void)
{
	int err;
	struct sigaction	sa;
	sigset_t	mask;
	pthread_t	tid;

	/*
	 * Set SIGHUP default and block all signals.
	 */
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		err_exit("can't restore SIGHUP default\n");
	/*
	 * Block all signals in main thread
	 */
	sigfillset(&mask);
	if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
		err_exit("SIG_BLOCK error\n");

	/*
	 * Create a child thread to handle SIGHUP and SIGTERM.
	 */
	err = pthread_create(&tid, NULL, thr_fn, 0);
	if (err != 0)
		err_exit("can't create thread\n");
}

int main(int argc, char *argv[])
{
	int					i, fd0, fd1, fd2;
	pid_t				pid;
	struct rlimit		rl;
	struct sigaction	sa;

	/*
	 * Clear file creation mask.
	 */
	umask(0);

	/*
	 * Fork first time, parent should exit
	 */
	if ((pid = fork()) < 0)
		err_exit("fork failed, can't fork\n");
	else if (pid != 0)
		exit(0);

	/*
	 * setsid() become a session leader to lose controlling TTY.
	 */
	setsid();

	/*
	 * Ensure future opens won't allocate controlling TTYs.
	 */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		err_exit("sigaction can't ignore SIGHUP\n");

	/* Fork again,  parent should exit */
	if ((pid = fork()) < 0)
		err_exit("fork failed, can't fork\n");
	else if (pid != 0)
		exit(0);

	/*
	 * Change the current working directory to the root so
	 * we won't prevent file systems from being unmounted.
	 */
	if (chdir("/") < 0)
		err_exit("can't change directory to /\n");

	/*
	 * Close all open file descriptors.
	 */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		err_exit("can't get file limit\n");

	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);

	/*
	 * Normally, we should attach file descriptors 0, 1, and 2 to /dev/null.
	 * But here, we just redirect stdout and stderr to logfile for log print
	 */
	fd0 = open("/dev/null", O_RDWR);
	fd1 = open("/tmp/daemon.log", O_RDWR | O_APPEND | O_CREAT, (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH));
	fd2 = dup(1);

	/*
	 * Make sure only one daemon process is running.
	 */
	if (is_daemon_running())
		err_exit("daemon already running, just exit\n");
	/*
	 * Setup signal handlers
	 */
	signal_setup();

	pr_info("Daemon start!\n");

	/*
	 * Proceed with the rest of the daemon.
	 */
	while(1);

	exit(0);
}
