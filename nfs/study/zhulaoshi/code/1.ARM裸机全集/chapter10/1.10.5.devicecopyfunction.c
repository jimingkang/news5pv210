typedef unsigned int bool;

// ��һ�ַ������궨��
#define CopySDMMCtoMem(z,a,b,c,e)(((bool(*)(int, unsigned int, unsigned short, unsigned int*, bool))(*((unsigned int *)0xD0037F98)))(z,a,b,c,e))


// �ڶ��ַ������ú���ָ�뷽ʽ����
typedef bool(*pCopySDMMC2Mem)(int, unsigned int, unsigned short, unsigned int*, bool);


// ʵ��ʹ��ʱ
pCopySDMMC2Mem p1 = (pCopySDMMC2Mem)0xD0037F98;
p1(x, x, x, x, x);		// ��һ�ֵ��÷���
(*p1)(x, x, x, x, x);	// �ڶ��ֵ��÷���
*p1(x, x, x, x, x);		// ������Ϊp1�Ⱥ�()��ϣ��������Ⱥ�*��ϡ�












