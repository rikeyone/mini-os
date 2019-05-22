#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	if (geteuid() == 0)
		printf("Run as root, euid:%ld\n", (long) geteuid());
	else
		printf("Not Run as root, euid:%ld\n", (long) geteuid());

	return 0;
}
