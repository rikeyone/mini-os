#include <stdio.h>

char buf[5];

int main(int argc, char *argv[])
{
	snprintf(buf, 5, "%s", argv[1]);
	printf("%s\n", buf);
	return 0;
}
