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

// ���½ڵ�new���뵽����pH��β��
void insert_tail(struct node *pH, struct node *new)
{
	// ��һ�����ߵ������β�ڵ�
	struct node *p = pH;
	while (NULL != p->pNext)
	{
		p = p->pNext;			// ��һ��ѭ���߹���ͷ�ڵ�
	}
	// ѭ��������p��ָ����ԭ�������һ���ڵ�
	// �ڶ��������½ڵ���뵽ԭ����β�ڵ�ĺ���
	p->pNext = new;				// ����ָ��������ˡ��½ڵ�ĵ�ַ��ǰ�ڵ��next
	new->pPrev = p;				// ǰ��ָ��������ˡ��½ڵ��prev��ǰ�ڵ�ĵ�ַ
								// ǰ�ڵ��prev���½ڵ��nextָ��δ�䶯
}

// ���½ڵ�newǰ��������pH�С�
// �㷨����ͼʾ�������ӣ�һ����4��ָ����Ҫ��ֵ��ע�����˳��
void insert_head(struct node *pH, struct node *new)
{
	// �½ڵ��nextָ��ָ��ԭ���ĵ�1����Ч�ڵ�ĵ�ַ
	new->pNext = pH->pNext;
	
	// ԭ����1����Ч�ڵ��prevָ��ָ���½ڵ�ĵ�ַ
	if (NULL != pH->pNext)
		pH->pNext->pPrev = new;
	
	// ͷ�ڵ��nextָ��ָ���½ڵ��ַ
	pH->pNext = new;
	
	// �½ڵ��prevָ��ָ��ͷ�ڵ�ĵ�ַ
	new->pPrev = pH;
}

// �������һ��˫����
void bianli(struct node *pH)
{
	struct node *p = pH;
	
	while (NULL != p->pNext)
	{
		p = p->pNext;
		
		printf("data = %d.\n", p->data);
	}
}

// ǰ�����һ��˫����������pTailҪָ������ĩβ
void qianxiang_bianli(struct node *pTail)
{
	struct node *p = pTail;
	
	while (NULL != p->pPrev)
	{
		printf("data = %d.\n", p->data);
		
		p = p->pPrev;
	}
}

int main(void)
{
	struct node *pHeader = create_node(0);		// ͷָ��
	
	insert_head(pHeader, create_node(11));
	insert_head(pHeader, create_node(12));
	insert_head(pHeader, create_node(13));
	
	
	/*
	// ����
	printf("node 1 data: %d.\n", pHeader->pNext->data);
	printf("node 2 data: %d.\n", pHeader->pNext->pNext->data);
	printf("node 3 data: %d.\n", pHeader->pNext->pNext->pNext->data);

	struct node *p = pHeader->pNext->pNext->pNext;		// pָ�������һ���ڵ�
	printf("node 3 data: %d.\n", p->data);
	printf("node 2 data: %d.\n", p->pPrev->data);
	printf("node 1 data: %d.\n", p->pPrev->pPrev->data);
	*/
	
	//bianli(pHeader);
	
	struct node *p = pHeader->pNext->pNext->pNext;
	qianxiang_bianli(p);
	
	
	return 0;
}

























