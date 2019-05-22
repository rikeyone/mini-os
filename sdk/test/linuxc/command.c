#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

#define pr_debug(fmt,...)   do{ printf("[%ld] DEBUG: "fmt,(long)getpid(),##__VA_ARGS__); fflush(stdout); }while(0)
#define pr_info(fmt,...)    do{ printf("[%ld] INFO:  "fmt,(long)getpid(),##__VA_ARGS__); fflush(stdout); }while(0)
#define pr_err(fmt,...)     do{ printf("[%ld] ERROR: "fmt,(long)getpid(),##__VA_ARGS__);fflush(stdout); }while(0)
#define err_exit(fmt,...)   do{ printf("[%ld] ERROR: "fmt,(long)getpid(),##__VA_ARGS__); exit(1); }while(0)

int main(int argc, char *argv[])
{
	pid_t pid;
	int len, i, status;
	char **buf;

	if (argc == 1)
		err_exit("Usage: need at least one argument for this program.\n");

	len = (argc - 1) + 3; //(char *) 0 at last
	buf = (char **) malloc(sizeof(char *) * len);
	if (!buf)
		err_exit("malloc error\n");

	/*
	 * fill buf for exec argv
	 */
	buf[0] = "bash";
	buf[1] = "-c";
	buf[len-1] = (char *) 0;
	for (i = 1; i < argc; i++) {
		buf[i+1] = argv[i];
	}

	/*
	 * print buf for debug
	 */
	for (i = 0; i < len; i++)
		pr_info("argv[%d]=%s\n", i, buf[i]);

	if ((pid = fork()) < 0) {
		err_exit("fork error, %s\n", strerror(errno));
	} else if (pid == 0) {
	/* child process */
		execv("/bin/bash", buf);
		_exit(127);
	} else {
	/* pid > 0 : parent process */
		while (waitpid(pid, &status, 0) < 0) {
			if (errno != EINTR) {
				pr_err("waitpid error, %s\n", strerror(errno));
				status = -1;
			}

		}
	}
	free(buf);
	return status;
}
