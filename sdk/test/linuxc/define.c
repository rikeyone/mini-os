#include <stdio.h>
#define NAME "old name"
#define NAME1 NAME

#ifdef NAME
#undef NAME
#endif
#define NAME "new name"
#define NAME2 NAME

int main()
{
	printf("NAME=%s, NAME1=%s, NAME2=%s\n", NAME, NAME1, NAME2);
	return 0;
}
