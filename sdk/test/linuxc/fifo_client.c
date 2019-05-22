#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "include/debug.h"

#define CMD_SIZE (1024+50)
char command[CMD_SIZE];
char ack[CMD_SIZE];

int main (int argc, char *argv[])
{
	int len, ret, fd1, fd2, count = 0;
	char path[1024];
	struct stat st;

	if (argc != 2) {
		pr_info("Usage: %s msg\n", argv[0]);
		exit(1);
	}
	fd1 = open("/tmp/fifo_server", O_RDWR);
	if (fd1 < 0) {
		pr_err("open error\n");
		exit(1);
	}

	len = snprintf(command + 4, CMD_SIZE, "%d:%s", (int)getpid(), argv[1]);
	len++; //add for '\0'
	snprintf(command, 4, "%d", len);

	ret = write(fd1, command, len + 4);
	if (ret < 0) {
		pr_err("write error\n");
		return -1;
	}
	snprintf(path, 1024, "/tmp/fifo_server_%d", (int)getpid());

	do {
		ret = stat(path, &st);
		if (ret < 0) {
			pr_info("client fifo doesn't exist, wait for %d times\n", count);
			usleep(3000);
			count++;
		} else if (ret == 0)
			break;
	} while (count >= 3);


	/* open it  read only, so server close this client fifo will make read return */
	fd2 = open(path, O_RDONLY);
	if (fd2 < 0) {
		pr_err("open error\n");
		exit(1);
	}
	len = read(fd2, ack, CMD_SIZE);
	pr_info("receive:%s\n", ack);

	return 0;
}
