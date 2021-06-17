#include <stdio.h>

//#pragma pack(128)

// 分析过程：
// 首先是整个结构体，整个结构体变量4字节对齐是由编译器保证的，我们不用操心。
// 然后是第一个元素a，a的开始地址就是整个结构体的开始地址，所以自然是4字节对齐的。但是a
// 的结束地址要由下一个元素说了算。
// 然后是第二个元素b，因为上一个元素a本身占4字节，本身就是对齐的。所以留给b的开始地址也是
// 4字节对齐地址，所以b可以直接放（b放的位置就决定了a一共占4字节，因为不需要填充）。
// b的起始地址定了后，结束地址不能定（因为可能需要填充），结束地址要看下一个元素来定。
// 然后是第三个元素c，short类型需要2字节对齐（short类型元素必须放在类似0，2，4，8这样的
// 地址处，不能放在1，3这样的奇数地址处），因此c不能紧挨着b来存放，解决方案是在b之后添加1
// 字节的填充（padding），然后再开始放c。c放完之后还没结束
// 当整个结构体的所有元素都对齐存放后，还没结束，因为整个结构体大小还要是4的整数倍。
struct mystruct1
{					// 1字节对齐	4字节对齐
    int a;			// 4			4
    char b;			// 1			2(1+1)
    short c;		// 2			2
};

struct mystruct11
{					// 1字节对齐	4字节对齐
    int a;			// 4			4
    char b;			// 1			2(1+1)
    short c;		// 2			2
}__attribute__((packed));

typedef struct mystruct111
{					// 1字节对齐	4字节对齐		2字节对齐
    int a;			// 4			4				4
    char b;			// 1			2(1+1)			2
    short c;		// 2			2				2
	short d;		// 2			4(2+2)			2
}__attribute__((aligned(1024))) My111;

typedef struct mystruct2
{					// 1字节对齐	4字节对齐
    char a;			// 1			4(1+3)
    int b;			// 4			4
    short c;		// 2			4(2+2)
}MyS2;

struct mystruct21
{					// 1字节对齐	4字节对齐
    char a;			// 1			4(1+3)
    int b;			// 4			4
    short c;		// 2			4(2+2)
} __attribute__((packed));


/*
typedef struct 
{
    int a;
    short b;
    static int c;
}MyS3;
*/

typedef struct myStruct5
{							// 1字节对齐	4字节对齐
    int a;					// 4			4
    struct mystruct1 s1;	// 7			8
    double b;				// 8			8
    int c;					// 4			4	
}MyS5;

struct stu
{							// 1字节对齐	4字节对齐
	char sex;				// 1			4(1+3)
	int length;				// 4			4
	char name[10];			// 10			12(10+2)
};

//#pragma pack()

int main(void)
{
	printf("sizeof(struct mystruct1) = %d.\n", sizeof(struct mystruct1));
	printf("sizeof(struct mystruct2) = %d.\n", sizeof(struct mystruct2));
	printf("sizeof(struct mystruct5) = %d.\n", sizeof(MyS5));
	printf("sizeof(struct stu) = %d.\n", sizeof(struct stu));
	
	printf("sizeof(struct mystruct11) = %d.\n", sizeof(struct mystruct11));
	printf("sizeof(struct mystruct21) = %d.\n", sizeof(struct mystruct21));
	
	printf("sizeof(struct mystruct111) = %d.\n", sizeof(My111));
	
	struct mystruct1 s1;
	struct mystruct1 __attribute__((packed)) s2 ;		// 定义变量时加这个没作用
	printf("sizeof(s1) = %d.\n", sizeof(s1));
	printf("sizeof(s2) = %d.\n", sizeof(s2));
	
	struct stu s3 __attribute__((aligned(2)));
	printf("sizeof(s3) = %d.\n", sizeof(s3));

	
	return 0;
}

































