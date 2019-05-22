#include <stdio.h>

int main(int argc, char *argv[])
{
	char buf[10];
	int n;

	while((n = fread(buf, 1, 10, stdin)) > 0) {
		printf("fread return n:%d\n", n);
		if (fwrite(buf, 1, n, stdout) != n) {
			perror("fwrite failed");
			exit(1);
		}
	}
	if (n < 0) {
		perror("fread failed");
		exit(2);
	}
	return 0;
}
