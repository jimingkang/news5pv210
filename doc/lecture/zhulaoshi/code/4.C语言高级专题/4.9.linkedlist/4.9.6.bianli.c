#include <stdio.h>
#include <strings.h>
#include <stdlib.h>


// 构建一个链表的节点
struct node
{
	int data;				// 有效数据
	struct node *pNext;		// 指向下一个节点的指针
};

// 作用：创建一个链表节点
// 返回值：指针，指针指向我们本函数新创建的一个节点的首地址
struct node * create_node(int data)
{
	struct node *p = (struct node *)malloc(sizeof(struct node));
	if (NULL == p)
	{
		printf("malloc error.\n");
		return NULL;
	}
	// 清理申请到的堆内存
	bzero(p, sizeof(struct node));
	// 填充节点
	p->data = data;
	p->pNext = NULL;	
	
	return p;
}

// 思路：由头指针向后遍历，直到走到原来的最后一个节点。原来最后一个节点里面的pNext是NULL，现在我们只要将它改成new就可以了。添加了之后新节点就变成了最后一个。
// 计算添加了新的节点后总共有多少个节点，然后把这个数写进头节点中。
void insert_tail(struct node *pH, struct node *new)
{
	int cnt = 0;
	// 分两步来完成插入
	// 第一步，先找到链表中最后一个节点
	struct node *p = pH;
	while (NULL != p->pNext)
	{
		p = p->pNext;				// 往后走一个节点
		cnt++;
	}
	
	// 第二步，将新节点插入到最后一个节点尾部
	p->pNext = new;
	pH->data = cnt + 1;
}

// 思路：
void insert_head(struct node *pH, struct node *new)
{
	// 第1步： 新节点的next指向原来的第一个节点
	new->pNext = pH->pNext;
	
	// 第2步： 头节点的next指向新节点的地址
	pH->pNext = new;
	
	// 第3步： 头节点中的计数要加1
	pH->data += 1;
}

// 遍历单链表，pH为指向单链表的头指针，遍历的节点数据打印出来
void bianli(struct node*pH)
{
	//pH->data				// 头节点数据，不是链表的常规数据，不要算进去了
	//struct node *p = pH;		// 错误，因为头指针后面是头节点
	struct node *p = pH->pNext;	// p直接走到第一个节点
	printf("-----------开始遍历-----------\n");
	while (NULL != p->pNext)		// 是不是最后一个节点
	{
		printf("node data: %d.\n", p->data);
		p = p->pNext;				// 走到下一个节点，也就是循环增量
	}
	printf("node data: %d.\n", p->data);
	printf("-------------完了-------------\n");
}

// 1、思考下为什么这样能解决问题；2、思考下设计链表时为什么要设计头节点
void bianli2(struct node*pH)
{
	//pH->data				// 头节点数据，不是链表的常规数据，不要算进去了
	struct node *p = pH;		// 头指针后面是头节点

	printf("-----------开始遍历-----------\n");
	while (NULL != p->pNext)		// 是不是最后一个节点
	{
		p = p->pNext;				// 走到下一个节点，也就是循环增量
		printf("node data: %d.\n", p->data);
	}

	printf("-------------完了-------------\n");
}



int main(void)
{
	// 定义头指针
	//struct node *pHeader = NULL;			// 这样直接insert_tail会段错误。
	struct node *pHeader = create_node(0);
	
	insert_head(pHeader, create_node(11));
	insert_head(pHeader, create_node(12));
	insert_head(pHeader, create_node(13));
/*
	pHeader = create_node(1);	
	// 将本节点和它前面的头指针关联起来					

	pHeader->pNext = create_node(432);		
	// 将本节点和它前面的头指针关联起来					

	pHeader->pNext->pNext = create_node(123);				
	// 将来要指向下一个节点的首地址

	// 至此创建了一个有1个头指针+3个完整节点的链表。
*/

	
	// 下面是4.9.3节的代码
	// 访问链表中的各个节点的有效数据，这个访问必须注意不能使用p、p1、p2，而只能
	// 使用pHeader。
	
	// 访问链表头节点的有效数据
	printf("beader node data: %d.\n", pHeader->data);	

	bianli2(pHeader);
	
	
/*
	// 访问链表第1个节点的有效数据
	printf("node1 data: %d.\n", pHeader->pNext->data);	
	
	// 访问链表第2个节点的有效数据
	printf("node2 data: %d.\n", pHeader->pNext->pNext->data);	
	
	// 访问链表第3个节点的有效数据
	printf("node3 data: %d.\n", pHeader->pNext->pNext->pNext->data);	
*/	
	return 0;
}







































