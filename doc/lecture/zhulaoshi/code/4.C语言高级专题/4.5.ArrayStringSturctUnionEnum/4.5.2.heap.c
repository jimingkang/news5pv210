#include <stdio.h>
#include <stdlib.h>
 
 
int main(void)
{
	int *p = (int *)malloc(20);
	// �ڶ�������������Ƿ�ɹ�
	if (NULL == p)
	{
		printf("malloc error.\n");
		return -1;
	}
	
	*(p+3) = 12;
	*(p+300000) = 1234;
	printf("*(p+3) = %d.\n", *(p+3));
	printf("*(p+300000) = %d.\n", *(p+300000));		
	


/*
	int *p1 = (int *)malloc(4);		// p2-p1 = 0x10 = 16Byte
	int *p2 = (int *)malloc(4);

	printf("p1 = %p.\n", p1);		// p2-p1 = 0x10 = 16Byte
	printf("p2 = %p.\n", p2);
*/
	
/*
	int *p1 = (int *)malloc(0);
	int *p2 = (int *)malloc(0);

	printf("p1 = %p.\n", p1);		// p2-p1 = 0x10 = 16Byte
	printf("p2 = %p.\n", p2);
	*/
/*
	// ��Ҫһ��1000��int����Ԫ�ص�����
	
	// ��һ��������Ͱ�
	int *p = (int *)malloc(1000*sizeof(int));
	// �ڶ�������������Ƿ�ɹ�
	if (NULL == p)
	{
		printf("malloc error.\n");
		return -1;
	}
	
	// ��������ʹ�����뵽���ڴ�
	//p = NULL;
	//p = &a;			// �����free֮ǰ��p���⸳ֵ����ômalloc������Ƕ��ڴ�Ͷ�ʧ����
					// malloc��p�ͷ��ص��ڴ���󶨣�p���Ƕ��ڴ��ڵ�ǰ���̵�Ψһ��ϵ��
					// ���pû��free֮ǰ�Ͷ��ˣ���ô����ڴ����Զ���ˡ����˵ĸ������
					// �ڲ���ϵͳ�Ķѹ�����������ڴ��ǵ�ǰ�������ŵģ�������Ҳ�ò���
					// �������������µ��ڴ����滻ʹ�ã���ͽг��򡰳��ڴ桱��ѧ�����ڴ�й©
	*(p+0) = 1;
	*(p+1) = 2;
	printf("*(p+0) = %d.\n", *(p+0));
	printf("*(p+1) = %d.\n", *(p+1));				
					
	*(p+222) = 133;
	*(p+223) = 222;				
					
	
	// ���Ĳ����ͷ�
	free(p);
	p = NULL;
	
	printf("*(p+222) = %d.\n", *(p+222));
	printf("*(p+223) = %d.\n", *(p+223));
*/
	return 0;
}


















