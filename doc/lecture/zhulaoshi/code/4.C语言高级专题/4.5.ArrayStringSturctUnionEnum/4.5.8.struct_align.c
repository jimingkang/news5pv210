#include <stdio.h>

//#pragma pack(128)

// �������̣�
// �����������ṹ�壬�����ṹ�����4�ֽڶ������ɱ�������֤�ģ����ǲ��ò��ġ�
// Ȼ���ǵ�һ��Ԫ��a��a�Ŀ�ʼ��ַ���������ṹ��Ŀ�ʼ��ַ��������Ȼ��4�ֽڶ���ġ�����a
// �Ľ�����ַҪ����һ��Ԫ��˵���㡣
// Ȼ���ǵڶ���Ԫ��b����Ϊ��һ��Ԫ��a����ռ4�ֽڣ�������Ƕ���ġ���������b�Ŀ�ʼ��ַҲ��
// 4�ֽڶ����ַ������b����ֱ�ӷţ�b�ŵ�λ�þ;�����aһ��ռ4�ֽڣ���Ϊ����Ҫ��䣩��
// b����ʼ��ַ���˺󣬽�����ַ���ܶ�����Ϊ������Ҫ��䣩��������ַҪ����һ��Ԫ��������
// Ȼ���ǵ�����Ԫ��c��short������Ҫ2�ֽڶ��루short����Ԫ�ر����������0��2��4��8������
// ��ַ�������ܷ���1��3������������ַ���������c���ܽ�����b����ţ������������b֮�����1
// �ֽڵ���䣨padding����Ȼ���ٿ�ʼ��c��c����֮��û����
// �������ṹ�������Ԫ�ض������ź󣬻�û��������Ϊ�����ṹ���С��Ҫ��4����������
struct mystruct1
{					// 1�ֽڶ���	4�ֽڶ���
    int a;			// 4			4
    char b;			// 1			2(1+1)
    short c;		// 2			2
};

struct mystruct11
{					// 1�ֽڶ���	4�ֽڶ���
    int a;			// 4			4
    char b;			// 1			2(1+1)
    short c;		// 2			2
}__attribute__((packed));

typedef struct mystruct111
{					// 1�ֽڶ���	4�ֽڶ���		2�ֽڶ���
    int a;			// 4			4				4
    char b;			// 1			2(1+1)			2
    short c;		// 2			2				2
	short d;		// 2			4(2+2)			2
}__attribute__((aligned(1024))) My111;

typedef struct mystruct2
{					// 1�ֽڶ���	4�ֽڶ���
    char a;			// 1			4(1+3)
    int b;			// 4			4
    short c;		// 2			4(2+2)
}MyS2;

struct mystruct21
{					// 1�ֽڶ���	4�ֽڶ���
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
{							// 1�ֽڶ���	4�ֽڶ���
    int a;					// 4			4
    struct mystruct1 s1;	// 7			8
    double b;				// 8			8
    int c;					// 4			4	
}MyS5;

struct stu
{							// 1�ֽڶ���	4�ֽڶ���
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
	struct mystruct1 __attribute__((packed)) s2 ;		// �������ʱ�����û����
	printf("sizeof(s1) = %d.\n", sizeof(s1));
	printf("sizeof(s2) = %d.\n", sizeof(s2));
	
	struct stu s3 __attribute__((aligned(2)));
	printf("sizeof(s3) = %d.\n", sizeof(s3));

	
	return 0;
}

































