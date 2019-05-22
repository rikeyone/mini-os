#include <stdio.h>

//typedef struct a A;
#define A struct a

struct a {
	A *p;
	int value;
};

int main()
{
	A test;
	test.value = 10;
	test.p = &test;
	printf("test.value:%d\n test.p->value:%d\n", test.value, (test.p)->value);
	return 0;
}
