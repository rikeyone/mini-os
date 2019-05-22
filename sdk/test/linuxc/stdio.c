#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	char buf[10];
	int n;

	while((n = read(STDIN_FILENO, buf, 10)) > 0) {
		if (write(STDOUT_FILENO, buf, n) != n) {
			perror("Write Failed");
			exit(1);
		}
	}
	if (n < 0) {
		perror("Read Failed");
		exit(2);
	}
	return 0;
}
