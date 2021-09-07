#ifndef __CAL_H__
#define __CAL_H__



typedef int (*pFunc)(int, int);


// 结构体是用来做计算器的，计算器工作时需要计算原材料
struct cal_t
{
	int a;
	int b;
	pFunc p;
};



// 函数原型声明
int calculator(const struct cal_t *p);












#endif




















