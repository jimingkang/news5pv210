#include <stdio.h>

int is_little_endian(void);
int is_little_endian2(void);
int is_little_endian3(void);

int main(void)
{
	printf("c = %d.\n", is_little_endian3());
	
	
	
	return 0;
}

// 1 Ğ¡¶Ë  0 for ´ò¶Ï
int is_little_endian(void)
{
	int i = 1;
	char c = *((char *)(&i));
	return (c == 1);
}

union myUnion
{
	int i;
	char c;
};

int is_little_endian2(void)
{
	union myUnion u1;
	u1.i = 1;
	
	return (u1.c == 1);
}

int is_little_endian3(void)
{
	unsigned int i = 0x80000000;
	char c = (char)(i & 0x01);
	
	return (c == 1);
}








