#include <stdio.h>
#include <strings.h>
#include <stdlib.h>


// 构建一个链表的节点
struct node
{
	int data;				// 有效数据
	struct node *pNext;		// 指向下一个节点的指针
};


int main(void)
{
	// 定义头指针
	struct node *pHeader = NULL;
	
	/********************************************************************/
	// 每创建一个新的节点，把这个新的节点和它前一个节点关联起来
	// 创建一个链表节点
	struct node *p = (struct node *)malloc(sizeof(struct node));
	if (NULL == p)
	{
		printf("malloc error.\n");
		return -1;
	}
	// 清理申请到的堆内存
	bzero(p, sizeof(struct node));
	// 填充节点
	p->data = 1;
	p->pNext = NULL;			// 将来要指向下一个节点的首地址
								// 实际操作时将下一个节点malloc返回的指针赋值给这个
								
	pHeader = p;	// 将本节点和它前面的头指针关联起来					
	/********************************************************************/
	
	
	/********************************************************************/
	// 每创建一个新的节点，把这个新的节点和它前一个节点关联起来
	// 创建一个链表节点
	struct node *p1 = (struct node *)malloc(sizeof(struct node));
	if (NULL == p1)
	{
		printf("malloc error.\n");
		return -1;
	}
	// 清理申请到的堆内存
	bzero(p1, sizeof(struct node));
	// 填充节点
	p1->data = 2;
	p1->pNext = NULL;			// 将来要指向下一个节点的首地址
								// 实际操作时将下一个节点malloc返回的指针赋值给这个
								
	p->pNext = p1;	// 将本节点和它前面的头指针关联起来					
	/********************************************************************/
	
	
	/********************************************************************/
	// 每创建一个新的节点，把这个新的节点和它前一个节点关联起来
	// 创建一个链表节点
	struct node *p2 = (struct node *)malloc(sizeof(struct node));
	if (NULL == p2)
	{
		printf("malloc error.\n");
		return -1;
	}
	// 清理申请到的堆内存
	bzero(p2, sizeof(struct node));
	// 填充节点
	p2->data = 3;
	p1->pNext = p2;			// 将来要指向下一个节点的首地址
								// 实际操作时将下一个节点malloc返回的指针赋值给这个				
	/********************************************************************/
	
	// 至此创建了一个有1个头指针+3个完整节点的链表。
	
	// 下面是4.9.3节的代码
	// 访问链表中的各个节点的有效数据，这个访问必须注意不能使用p、p1、p2，而只能
	// 使用pHeader。
	
	// 访问链表第1个节点的有效数据
	printf("node1 data: %d.\n", pHeader->data);	
	printf("p->data: %d.\n", p->data);			// pHeader->data等同于p->data
	
	// 访问链表第2个节点的有效数据
	printf("node2 data: %d.\n", pHeader->pNext->data);	
	printf("p1->data: %d.\n", p1->data);	
	// pHeader->pNext->data等同于p1->data
	
	// 访问链表第3个节点的有效数据
	printf("node3 data: %d.\n", pHeader->pNext->pNext->data);	
	printf("p2->data: %d.\n", p2->data);			
	// pHeader->pNext->pNext->data等同于p2->data
	
	
	return 0;
}







































