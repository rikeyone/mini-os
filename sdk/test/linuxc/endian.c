#include<stdio.h>

typedef union {
	unsigned int num;
	unsigned char buf[2];
} un;

int main()
{
	un data;
	data.num = 0x0201;
	if (data.buf[0] == 0x01 && data.buf[1] == 0x02)
		printf("This is Little-Endian system!\n");
	else if (data.buf[0] == 0x02 && data.buf[1] == 0x01)
		printf("This is Big-Endian system!\n");
	else
		printf("Unknown system!\n");
	return 0;
}
