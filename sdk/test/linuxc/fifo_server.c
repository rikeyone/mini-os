#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include "include/debug.h"


#define CMD_SIZE (1024 + 50) // must < PIPE_BUF to make it read and write atomic

static char command[CMD_SIZE];

void sigpipe_handler(int signo)
{
	pr_err("client has exited abnormally!\n");
}

int signal_setup(void)
{
	struct sigaction act, oact;

	act.sa_handler = sigpipe_handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);

	if (sigaction(SIGPIPE, &act, &oact) < 0)
		err_exit("sigaction error\n");
	return 0;
}

int fifo_setup(void)
{
	int ret;

	unlink("/tmp/fifo_server");
	ret = mkfifo("/tmp/fifo_server", (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH));
	if (ret < 0)
		err_exit("server fifo create error\n");

	return ret;
}

int check_client_fifo(char *path)
{
	int ret;
	struct stat st;

	ret = stat(path, &st);
	if (ret == -1) {
		pr_info("%s don't exsit, create it by server!\n", path);
		ret = mkfifo(path, (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH));
		if (ret < 0)
			pr_err("client fifo create error:%s\n", path);
	}
	return ret;
}

int readn(int fd, char *ptr, int size)
{
	int ret;
	int len = size;
	char *buf = ptr;

	while(len > 0) {
		ret = read(fd, buf, len);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			else {
				pr_err("read error\n");
				return -1;
			}
		} else if (ret == 0) {
			pr_info("no client exist!\n");
			break;
		} else {
			len -= ret;
			buf += ret;
		}
	}
	return size - len;
}

int main_receive(int fd, char *ptr, int max)
{
	int ret, len;
	char buf[5] = {0};

	ret = readn(fd, buf, 4);
	if (ret < 0)
		return -1;

	len = atoi(buf);
	pr_info("receive length:%d\n", len);
	if (len > CMD_SIZE)
		return -1;

	ret = readn(fd, ptr, len);
	if (ret < 0)
		return -1;

	return len;
}

/* pid:msg */
void split_command(char *cmd, int size, char **pid, char **msg)
{
	char *p;
	p = strtok(cmd, ":");
	if (p) {
		pr_info("%s pid:%s\n", __func__, p);
		*pid = p;
	}

	p = strtok(NULL, ":");
	if (p) {
		pr_info("%s msg:%s\n", __func__, p);
		*msg = p;
	}
}

void handle_msg(char *pid, char *msg)
{
	char path[1024];

	if (!pid)
		return;

	sprintf(path, "/tmp/fifo_server_%s", pid);

	if (!strcmp(msg, "disconnect")) {
		unlink(path);
	} else {
	/*
	 * any other parser for msg
	 */
	}
}

int send_msg_ack(char *pid, char *msg)
{
	int ret, fd;
	char buf[1024];
	char path[1024];

	if (!pid)
		return -1;

	sprintf(path, "/tmp/fifo_server_%s", pid);

	if (check_client_fifo(path) < 0)
		return -1;

	fd = open(path, O_WRONLY);
	if (fd < 0)
		return -1;

	sprintf(buf, "%s:ack!", msg);
	ret = write(fd, buf, strlen(buf) + 1);
	if (ret < 0)
		pr_err("write error\n");

	pr_info("write client fifo successfully!\n");
	close(fd);
	return ret;
}

/* we just print the msg */
int main_parse(char *cmd, int size)
{
	char *pid = NULL;
	char *msg = NULL;

	split_command(cmd, size, &pid, &msg);

	send_msg_ack(pid, msg);

	handle_msg(pid, msg);
}



/*
 * this is a simple server program
 *
 * you may optimize it by using multi-threads or multi-processes
 *
 */
int main(int argc, char *argv[])
{
	int fd, len;

	signal_setup();
	fifo_setup();
	fd = open("/tmp/fifo_server", O_RDWR);
	if (fd < 0)
		err_exit("server fifo open error\n");

	while(1) {
		len = main_receive(fd, command, CMD_SIZE);
		if (len < 0)
			err_exit("server fifo read error\n");

		main_parse(command, len);
	}

	return 1;
}


