#include <stdio.h>
#include <strings.h>
#include <stdlib.h>


// ����һ������Ľڵ�
struct node
{
	int data;				// ��Ч����
	struct node *pNext;		// ָ����һ���ڵ��ָ��
};

// ���ã�����һ������ڵ�
// ����ֵ��ָ�룬ָ��ָ�����Ǳ������´�����һ���ڵ���׵�ַ
struct node * create_node(int data)
{
	struct node *p = (struct node *)malloc(sizeof(struct node));
	if (NULL == p)
	{
		printf("malloc error.\n");
		return NULL;
	}
	// �������뵽�Ķ��ڴ�
	bzero(p, sizeof(struct node));
	// ���ڵ�
	p->data = data;
	p->pNext = NULL;	
	
	return p;
}

// ˼·����ͷָ����������ֱ���ߵ�ԭ�������һ���ڵ㡣ԭ�����һ���ڵ������pNext��NULL����������ֻҪ�����ĳ�new�Ϳ����ˡ������֮���½ڵ�ͱ�������һ����
void insert_tail(struct node *pH, struct node *new)
{
	// ����������ɲ���
	// ��һ�������ҵ����������һ���ڵ�
	struct node *p = pH;
	while (NULL != p->pNext)
	{
		p = p->pNext;				// ������һ���ڵ�
	}
	
	// �ڶ��������½ڵ���뵽���һ���ڵ�β��
	p->pNext = new;
}

int main(void)
{
	// ����ͷָ��
	//struct node *pHeader = NULL;			// ����ֱ��insert_tail��δ���
	struct node *pHeader = create_node(1);
	
	insert_tail(pHeader, create_node(2));
	insert_tail(pHeader, create_node(3));
	insert_tail(pHeader, create_node(4));
/*
	pHeader = create_node(1);	
	// �����ڵ����ǰ���ͷָ���������					

	pHeader->pNext = create_node(432);		
	// �����ڵ����ǰ���ͷָ���������					

	pHeader->pNext->pNext = create_node(123);				
	// ����Ҫָ����һ���ڵ���׵�ַ

	// ���˴�����һ����1��ͷָ��+3�������ڵ������
*/

	
	// ������4.9.3�ڵĴ���
	// ���������еĸ����ڵ����Ч���ݣ�������ʱ���ע�ⲻ��ʹ��p��p1��p2����ֻ��
	// ʹ��pHeader��
	
	// ���������1���ڵ����Ч����
	printf("node1 data: %d.\n", pHeader->data);	
	//printf("p->data: %d.\n", p->data);			// pHeader->data��ͬ��p->data
	
	// ���������2���ڵ����Ч����
	printf("node2 data: %d.\n", pHeader->pNext->data);	
	//printf("p1->data: %d.\n", p1->data);	
	// pHeader->pNext->data��ͬ��p1->data
	
	// ���������3���ڵ����Ч����
	printf("node3 data: %d.\n", pHeader->pNext->pNext->data);	
	//printf("p2->data: %d.\n", p2->data);			
	// pHeader->pNext->pNext->data��ͬ��p2->data
	
	
	return 0;
}







































