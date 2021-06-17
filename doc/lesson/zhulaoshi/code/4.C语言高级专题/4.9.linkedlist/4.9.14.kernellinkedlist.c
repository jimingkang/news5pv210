#include <linux/list.h>



struct driver_info
{
	int data;
};

// driver结构体用来管理内核中的驱动
struct driver
{
	char name[20];				// 驱动名称
	int id;						// 驱动id编号
	struct driver_info info;	// 驱动信息
	struct list_head head;		// 内嵌的内核链表成员
};

struct driver2
{
	char name[20];				// 驱动名称
	int id;						// 驱动id编号
	struct driver_info info;	// 驱动信息
	//struct list_head head;		// 内嵌的内核链表成员
	struct driver *prev;
	struct driver *next;
};

// 分析driver结构体，可知：前三个成员都是数据区域成员（就是我们之前简化为int data的东西），第4个成员是一个struct list_head类型的变量，这就是一个纯链表。
// 本来driver结构体是没有链表的，也无法用链表来管理。但是我们driver内嵌的head成员本身就是一个纯链表，所以driver通过head成员给自己扩展了链表的功能。
// driver通过内嵌的方式扩展链表成员，本身不只是有了一个链表成员，关键是可以通过利用list_head本身事先实现的链表的各种操作方法来操作head。

// 最终效果：我们可以通过遍历head来实现driver的遍历；遍历head的函数在list.h中已经事先写好了，所以我们内核中去遍历driver时就不用重复去写了。
// 通过操作head来操作driver，实质上就是通过操作结构体的某个成员变量来操作整个结构体变量。这里面要借助container_of宏































