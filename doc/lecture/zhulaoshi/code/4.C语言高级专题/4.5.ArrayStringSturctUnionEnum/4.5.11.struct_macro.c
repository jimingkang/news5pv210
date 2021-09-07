#include <stdio.h>

struct mystruct
{
	char a;			// 0
	int b;			// 4
	short c;		// 8
};

// TYPE是结构体类型，MEMBER是结构体中一个元素的元素名
// 这个宏返回的是member元素相对于整个结构体变量的首地址的偏移量，类型是int
#define offsetof(TYPE, MEMBER) ((int) &((TYPE *)0)->MEMBER)

// ptr是指向结构体元素member的指针，type是结构体类型，member是结构体中一个元素的元素名
// 这个宏返回的就是指向整个结构体变量的指针，类型是(type *)
#define container_of(ptr, type, member) ({			\
	const typeof(((type *)0)->member) * __mptr = (ptr);	\
	(type *)((char *)__mptr - offsetof(type, member)); })

int main(void)
{
	struct mystruct s1;
	struct mystruct *pS = NULL;
	
	short *p = &(s1.c);		// p就是指向结构体中某个member的指针
	
	printf("s1的指针等于：%p.\n", &s1);
	
	// 问题是要通过p来计算得到s1的指针
	pS = container_of(p, struct mystruct, c);
	printf("pS等于：%p.\n", pS);
	
/*
	struct mystruct s1;
	s1.b = 12;
	
	int *p = (int *)((char *)&s1 + 4);
	printf("*p = %d.\n", *p);
	
	int offsetofa = offsetof(struct mystruct, a);
	printf("offsetofa = %d.\n", offsetofa);
	
	int offsetofb = offsetof(struct mystruct, b);
	printf("offsetofb = %d.\n", offsetofb);
	
	int offsetofc = offsetof(struct mystruct, c);
	printf("offsetofc = %d.\n", offsetofc);
	
	printf("整个结构体变量的首地址：%p.\n", &s1);
	printf("s1.b的首地址：%p.\n", &(s1.b));
	printf("偏移量是：%d.\n", (char *)&(s1.b) - (char *)&s1);
*/	
	
	
	
	
	return 0;
}




















