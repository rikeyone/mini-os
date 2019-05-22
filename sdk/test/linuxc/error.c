#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	errno = EINVAL;
	printf("Error:%s, errno:%d\n", strerror(errno), errno);
	perror(argv[0]);
	exit(0);
}
