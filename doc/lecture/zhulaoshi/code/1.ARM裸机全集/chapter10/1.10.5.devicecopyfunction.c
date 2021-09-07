typedef unsigned int bool;

// 第一种方法：宏定义
#define CopySDMMCtoMem(z,a,b,c,e)(((bool(*)(int, unsigned int, unsigned short, unsigned int*, bool))(*((unsigned int *)0xD0037F98)))(z,a,b,c,e))


// 第二种方法：用函数指针方式调用
typedef bool(*pCopySDMMC2Mem)(int, unsigned int, unsigned short, unsigned int*, bool);


// 实际使用时
pCopySDMMC2Mem p1 = (pCopySDMMC2Mem)0xD0037F98;
p1(x, x, x, x, x);		// 第一种调用方法
(*p1)(x, x, x, x, x);	// 第二种调用方法
*p1(x, x, x, x, x);		// 错误，因为p1先和()结合，而不是先和*结合。












