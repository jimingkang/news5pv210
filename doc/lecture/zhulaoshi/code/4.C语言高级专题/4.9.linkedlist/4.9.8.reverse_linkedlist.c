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

// 从链表pH中删除节点，待删除的节点的特征是数据区等于data
// 返回值：当找到并且成功删除了节点则返回0，当未找到节点时返回-1
int delete_node(struct node*pH, int data)
{
	// 找到这个待删除的节点，通过遍历链表来查找
	struct node *p = pH;			// 用来指向当前节点
	struct node *pPrev = NULL;		// 用来指向当前节点的前一个节点

	while (NULL != p->pNext)		// 是不是最后一个节点
	{
		pPrev = p;					// 在p走向下一个节点前先将其保存
		p = p->pNext;				// 走到下一个节点，也就是循环增量
		// 判断这个节点是不是我们要找的那个节点
		if (p->data == data)
		{
			// 找到了节点，处理这个节点
			// 分为2种情况，一个是找到的是普通节点，另一个是找到的是尾节点
			// 删除节点的困难点在于：通过链表的遍历依次访问各个节点，找到这个节点
			// 后p指向了这个节点，但是要删除这个节点关键要操作前一个节点，但是这
			// 时候已经没有指针指向前一个节点了，所以没法操作。解决方案就是增加
			// 一个指针指向当前节点的前一个节点
			if (NULL == p->pNext)
			{
				// 尾节点
				pPrev->pNext = NULL;		// 原来尾节点的前一个节点变成新尾节点
				free(p);					// 释放原来的尾节点的内存
			}
			else
			{
				// 普通节点
				pPrev->pNext = p->pNext;	// 要删除的节点的前一个节点和它的后一个节点相连，这样就把要删除的节点给摘出来了
				free(p);
			}
			// 处理完成之后退出程序
			return 0;
		}
	}
	// 到这里还没找到，说明链表中没有我们想要的节点
	printf("没找到这个节点.\n");
	return -1;
}

// 将pH指向的链表逆序
void reverse_linkedlist(struct node *pH)
{
	struct node *p = pH->pNext;		// pH指向头节点，p指向第1个有效节点
	struct node *pBack;				// 保存当前节点的后一个节点地址
	
	// 当链表没有有效节点或者只有一个有效节点时，逆序不用做任何操作
	if ((NULL ==p) || (NULL == p->pNext))
		return;
	
	// 当链表有2个及2个以上节点时才需要真正进行逆序操作
	while (NULL != p->pNext)		// 是不是最后一个节点
	{
		// 原链表中第一个有效节点将是逆序后新链表的尾节点，尾节点的pNext指向NULL
		pBack = p->pNext;			// 保存p节点后面一个节点地址
		if (p == pH->pNext)
		{
			// 原链表第一个有效节点
			p->pNext = NULL;
		}
		else
		{
			// 原链表的非第1个有效节点
			p->pNext = pH->pNext;
		}
		pH->pNext = p;
		
		//p = p->pNext;		// 这样已经不行了，因为p->pNext已经被改过了
		p = pBack;			// 走到下一个节点
	}
	// 循环结束后，最后一个节点仍然缺失
	insert_head(pH, p);
}


int main(void)
{
	// 定义头指针
	//struct node *pHeader = NULL;			// 这样直接insert_tail会段错误。
	struct node *pHeader = create_node(0);
	
	insert_tail(pHeader, create_node(11));
	insert_tail(pHeader, create_node(12));
	insert_tail(pHeader, create_node(13));
	insert_tail(pHeader, create_node(14));

	bianli2(pHeader);
	
	reverse_linkedlist(pHeader);
	printf("------------------逆序后-------------\n");
	bianli2(pHeader);
	
	return 0;
}







































