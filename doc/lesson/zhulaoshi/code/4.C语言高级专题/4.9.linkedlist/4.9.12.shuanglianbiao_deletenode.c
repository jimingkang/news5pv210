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

// 将新节点new插入到链表pH的尾部
void insert_tail(struct node *pH, struct node *new)
{
	// 第一步先走到链表的尾节点
	struct node *p = pH;
	while (NULL != p->pNext)
	{
		p = p->pNext;			// 第一次循环走过了头节点
	}
	// 循环结束后p就指向了原来的最后一个节点
	// 第二步：将新节点插入到原来的尾节点的后面
	p->pNext = new;				// 后向指针关联好了。新节点的地址和前节点的next
	new->pPrev = p;				// 前向指针关联好了。新节点的prev和前节点的地址
								// 前节点的prev和新节点的next指针未变动
}

// 将新节点new前插入链表pH中。
// 算法参照图示进行连接，一共有4个指针需要赋值。注意的是顺序。
void insert_head(struct node *pH, struct node *new)
{
	// 新节点的next指针指向原来的第1个有效节点的地址
	new->pNext = pH->pNext;
	
	// 原来第1个有效节点的prev指针指向新节点的地址
	if (NULL != pH->pNext)
		pH->pNext->pPrev = new;
	
	// 头节点的next指针指向新节点地址
	pH->pNext = new;
	
	// 新节点的prev指针指向头节点的地址
	new->pPrev = pH;
}

// 后向遍历一个双链表
void bianli(struct node *pH)
{
	struct node *p = pH;
	
	if (NULL == p)
	{
		return;
	}
	
	while (NULL != p->pNext)
	{
		p = p->pNext;
		
		printf("data = %d.\n", p->data);
	}
}

// 前向遍历一个双遍历，参数pTail要指向链表末尾
void qianxiang_bianli(struct node *pTail)
{
	struct node *p = pTail;
	
	while (NULL != p->pPrev)
	{
		printf("data = %d.\n", p->data);
		
		p = p->pPrev;
	}
}

// 从链表pH中删除一个节点，节点中的数据是data
int delete_node(struct node *pH, int data)
{
	struct node *p = pH;
	
	if (NULL == p)
	{
		return -1;
	}
	
	while (NULL != p->pNext)
	{
		p = p->pNext;
		
		// 在这里先判断当前节点是不是我们要删除的那个节点
		if (p->data == data)
		{
			// 找到了，删除之。当前上下文是：当前节点为p
			if (NULL == p->pNext)
			{
				// 尾节点
// p表示当前节点地址，p->pNext表示后一个节点地址，p->pPrev表示前一个节点的地址
				p->pPrev->pNext = NULL;
				//p->pPrev = NULL;			可以省略，因为后面整个都被销毁了
				// 销毁p节点
				//free(p);
			}
			else
			{
				// 不是尾节点，普通节点
				
				// 前一个节点的next指针指向后一个节点的首地址
				p->pPrev->pNext = p->pNext;
				
				// 当前节点的prev和next指针都不用管，因为后面会整体销毁整个节点
				
				// 后一个节点的prev指针指向前一个节点的首地址
				p->pNext->pPrev = p->pPrev;
				
				//free(p);
			}
			free(p);
			
			return 0;
		}
	}
	
	printf("未找到目标节点.\n");
	return -1;
}



int main(void)
{
	//struct node *pHeader = create_node(0);		// 头指针
	struct node *pHeader = NULL;
	
	//insert_head(pHeader, create_node(11));
	//insert_head(pHeader, create_node(12));
	//insert_head(pHeader, create_node(13));
	
	
	/*
	// 遍历
	printf("node 1 data: %d.\n", pHeader->pNext->data);
	printf("node 2 data: %d.\n", pHeader->pNext->pNext->data);
	printf("node 3 data: %d.\n", pHeader->pNext->pNext->pNext->data);

	struct node *p = pHeader->pNext->pNext->pNext;		// p指向了最后一个节点
	printf("node 3 data: %d.\n", p->data);
	printf("node 2 data: %d.\n", p->pPrev->data);
	printf("node 1 data: %d.\n", p->pPrev->pPrev->data);
	*/
	
	//bianli(pHeader);
	
	//struct node *p = pHeader->pNext->pNext->pNext;
	//qianxiang_bianli(p);
	
	
	bianli(pHeader);
	delete_node(pHeader, 11);
	printf("after delete node------------------\n");
	bianli(pHeader);
	
	return 0;
}

























