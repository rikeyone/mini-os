#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "include/debug.h"
/*
 * fcntl function has these cmd arguments:
 * F_DUPFD、F_DUPFD_CLOEXEC : dup file descriptor
 * F_GETFD、F_SETFD : set file FD_CLOEXEC flag
 * F_GETFL、F_SETFL: file open flags
 * F_GETLK、F_SETLK、F_SETLKW : file lock
 */
int main(int argc, char *argv[])
{
	int fd, ret, access;
	pid_t pid;
	char buffer[100] = {0};

	pr_info("argc:%d\n", argc);

	if (argc == 2)
		sscanf(argv[1], "%d", &fd);
	else
		fd = open("testfile", (O_RDWR|O_NONBLOCK|O_APPEND|O_SYNC|O_CREAT), (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH));

	if (fd < 0)
		err_exit("Open failed, errno = %d, %s\n", errno, strerror(errno));

	pr_info("file descriptor = %d\n", fd);

	ret = fcntl(fd, F_GETFL, 0);
	if (ret < 0)
		err_exit("fcntl failed, errno = %d, %s\n", errno, strerror(errno));

	access = ret & O_ACCMODE;
	switch (access) {
	case O_RDWR:
		pr_info("read write mode\n");
		break;
	case O_RDONLY:
		pr_info("read only mode\n");
		break;
	case O_WRONLY:
		pr_info("write only mode\n");
		break;
	default:
		pr_err("unknown access mode\n");
		break;
	}
	if (ret & O_NONBLOCK)
		pr_info("non block access mode enable\n");
	if (ret & O_APPEND)
		pr_info("append mode enable\n");
	if (ret & O_SYNC)
		pr_info("sync writes enable\n");
	if ((ret = fcntl(fd, F_GETFD, 0)) < 0)
		err_exit("Get CLOSEXEC flag failed\n");
	else
		pr_info("close on exec flag:%d\n", ret);

	if (fcntl(fd, F_SETFD, FD_CLOEXEC) < 0)
		err_exit("Set CLOSEXEC flag failed\n");

	if ((ret = fcntl(fd, F_GETFD, 0)) < 0)
		err_exit("Get CLOSEXEC flag failed\n");
	else
		pr_info("close on exec flag:%d\n", ret);

	if (argc == 2)
		exit(0);

	pid = fork();
	if (pid < 0)
		err_exit("fork failed\n");
	else if (pid > 0) {
		pr_info("parent process: fd = %d\n", fd);
		wait(NULL);
	} else {
		sprintf(buffer, "%d", fd);
		pr_info("child process: fd = %d\n", fd);
		execlp("./fcntl", "./fcntl", buffer, (char *)0);
	}
	exit(0);
}
