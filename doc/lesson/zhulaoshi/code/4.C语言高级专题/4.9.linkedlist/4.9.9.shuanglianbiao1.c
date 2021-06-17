#include <stdio.h>
#include <stdlib.h>

// 双链表的节点
struct node 
{
	int data;					// 有效数据
	struct node *pPrev;			// 前向指针，指向前一个节点
	struct node *pNext;			// 后向指针，指向后一个节点
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
	p->pNext = NULL;		// 默认创建的节点前向后向指针都指向NULL
	
	return p;
}


int main(void)
{
	struct node *pHeader = create_node(0);		// 头指针
	
	
	
	
	return 0;
}

























