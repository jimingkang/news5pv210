#include <stdio.h>
#include <stdlib.h>

// ˫����Ľڵ�
struct node 
{
	int data;					// ��Ч����
	struct node *pPrev;			// ǰ��ָ�룬ָ��ǰһ���ڵ�
	struct node *pNext;			// ����ָ�룬ָ���һ���ڵ�
};

struct node *create_node(int data)
{
	struct node *p = (struct node *)malloc(sizeof(struct node));
	if (NULL == p)
	{
		printf("malloc error.\n");
		return NULL;
	}
	p->data = data;
	p->pPrev = NULL;
	p->pNext = NULL;		// Ĭ�ϴ����Ľڵ�ǰ�����ָ�붼ָ��NULL
	
	return p;
}


int main(void)
{
	struct node *pHeader = create_node(0);		// ͷָ��
	
	
	
	
	return 0;
}

























