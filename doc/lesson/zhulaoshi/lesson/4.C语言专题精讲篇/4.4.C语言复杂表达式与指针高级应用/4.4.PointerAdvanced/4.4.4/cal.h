#ifndef __CAL_H__
#define __CAL_H__



typedef int (*pFunc)(int, int);


// �ṹ�����������������ģ�����������ʱ��Ҫ����ԭ����
struct cal_t
{
	int a;
	int b;
	pFunc p;
};



// ����ԭ������
int calculator(const struct cal_t *p);












#endif




















