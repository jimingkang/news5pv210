#include <stdio.h>
#include <string.h>


#define MAX_LINE_LENGTH		256			// �����г��ȣ�����ܳ����������


int main(void)
{
	char str[MAX_LINE_LENGTH];			// ��������û��������������
	
	while (1)
	{
		// ��ӡ��������ʾ����ע�ⲻ�ܼӻ���
		printf("aston#");
		// ���str�����Դ���µ��ַ���
		memset(str, 0, sizeof(str));
		// shell��һ������ȡ�û����������
		scanf("%s", str);
		// shell�ڶ����������û���������
		
		// shell�������������û���������
		printf("%s\n", str);
	}

	return 0;
}






















