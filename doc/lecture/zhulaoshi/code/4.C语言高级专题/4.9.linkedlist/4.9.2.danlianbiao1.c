#include <stdio.h>
#include <strings.h>
#include <stdlib.h>


// ����һ������Ľڵ�
struct node
{
	int data;				// ��Ч����
	struct node *pNext;		// ָ����һ���ڵ��ָ��
};


int main(void)
{
	// ����ͷָ��
	struct node *pHeader = NULL;
	
	/********************************************************************/
	// ÿ����һ���µĽڵ㣬������µĽڵ����ǰһ���ڵ��������
	// ����һ������ڵ�
	struct node *p = (struct node *)malloc(sizeof(struct node));
	if (NULL == p)
	{
		printf("malloc error.\n");
		return -1;
	}
	// �������뵽�Ķ��ڴ�
	bzero(p, sizeof(struct node));
	// ���ڵ�
	p->data = 1;
	p->pNext = NULL;			// ����Ҫָ����һ���ڵ���׵�ַ
								// ʵ�ʲ���ʱ����һ���ڵ�malloc���ص�ָ�븳ֵ�����
								
	pHeader = p;	// �����ڵ����ǰ���ͷָ���������					
	/********************************************************************/
	
	
	/********************************************************************/
	// ÿ����һ���µĽڵ㣬������µĽڵ����ǰһ���ڵ��������
	// ����һ������ڵ�
	struct node *p1 = (struct node *)malloc(sizeof(struct node));
	if (NULL == p1)
	{
		printf("malloc error.\n");
		return -1;
	}
	// �������뵽�Ķ��ڴ�
	bzero(p1, sizeof(struct node));
	// ���ڵ�
	p1->data = 2;
	p1->pNext = NULL;			// ����Ҫָ����һ���ڵ���׵�ַ
								// ʵ�ʲ���ʱ����һ���ڵ�malloc���ص�ָ�븳ֵ�����
								
	p->pNext = p1;	// �����ڵ����ǰ���ͷָ���������					
	/********************************************************************/
	
	
	/********************************************************************/
	// ÿ����һ���µĽڵ㣬������µĽڵ����ǰһ���ڵ��������
	// ����һ������ڵ�
	struct node *p2 = (struct node *)malloc(sizeof(struct node));
	if (NULL == p2)
	{
		printf("malloc error.\n");
		return -1;
	}
	// �������뵽�Ķ��ڴ�
	bzero(p2, sizeof(struct node));
	// ���ڵ�
	p2->data = 3;
	p1->pNext = p2;			// ����Ҫָ����һ���ڵ���׵�ַ
								// ʵ�ʲ���ʱ����һ���ڵ�malloc���ص�ָ�븳ֵ�����				
	/********************************************************************/
	
	// ���˴�����һ����1��ͷָ��+3�������ڵ������
	
	// ������4.9.3�ڵĴ���
	// ���������еĸ����ڵ����Ч���ݣ�������ʱ���ע�ⲻ��ʹ��p��p1��p2����ֻ��
	// ʹ��pHeader��
	
	// ���������1���ڵ����Ч����
	printf("node1 data: %d.\n", pHeader->data);	
	printf("p->data: %d.\n", p->data);			// pHeader->data��ͬ��p->data
	
	// ���������2���ڵ����Ч����
	printf("node2 data: %d.\n", pHeader->pNext->data);	
	printf("p1->data: %d.\n", p1->data);	
	// pHeader->pNext->data��ͬ��p1->data
	
	// ���������3���ڵ����Ч����
	printf("node3 data: %d.\n", pHeader->pNext->pNext->data);	
	printf("p2->data: %d.\n", p2->data);			
	// pHeader->pNext->pNext->data��ͬ��p2->data
	
	
	return 0;
}







































