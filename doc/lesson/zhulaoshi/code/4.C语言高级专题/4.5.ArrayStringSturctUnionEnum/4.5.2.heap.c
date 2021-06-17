#include <stdio.h>
#include <stdlib.h>
 
 
int main(void)
{
	int *p = (int *)malloc(20);
	// 第二步：检验分配是否成功
	if (NULL == p)
	{
		printf("malloc error.\n");
		return -1;
	}
	
	*(p+3) = 12;
	*(p+300000) = 1234;
	printf("*(p+3) = %d.\n", *(p+3));
	printf("*(p+300000) = %d.\n", *(p+300000));		
	


/*
	int *p1 = (int *)malloc(4);		// p2-p1 = 0x10 = 16Byte
	int *p2 = (int *)malloc(4);

	printf("p1 = %p.\n", p1);		// p2-p1 = 0x10 = 16Byte
	printf("p2 = %p.\n", p2);
*/
	
/*
	int *p1 = (int *)malloc(0);
	int *p2 = (int *)malloc(0);

	printf("p1 = %p.\n", p1);		// p2-p1 = 0x10 = 16Byte
	printf("p2 = %p.\n", p2);
	*/
/*
	// 需要一个1000个int类型元素的数组
	
	// 第一步：申请和绑定
	int *p = (int *)malloc(1000*sizeof(int));
	// 第二步：检验分配是否成功
	if (NULL == p)
	{
		printf("malloc error.\n");
		return -1;
	}
	
	// 第三步：使用申请到的内存
	//p = NULL;
	//p = &a;			// 如果在free之前给p另外赋值，那么malloc申请的那段内存就丢失掉了
					// malloc后p和返回的内存相绑定，p是那段内存在当前进程的唯一联系人
					// 如果p没有free之前就丢了，那么这段内存就永远丢了。丢了的概念就是
					// 在操作系统的堆管理器中这段内存是当前进程拿着的，但是你也用不了
					// 所以你想申请新的内存来替换使用，这就叫程序“吃内存”，学名叫内存泄漏
	*(p+0) = 1;
	*(p+1) = 2;
	printf("*(p+0) = %d.\n", *(p+0));
	printf("*(p+1) = %d.\n", *(p+1));				
					
	*(p+222) = 133;
	*(p+223) = 222;				
					
	
	// 第四步：释放
	free(p);
	p = NULL;
	
	printf("*(p+222) = %d.\n", *(p+222));
	printf("*(p+223) = %d.\n", *(p+223));
*/
	return 0;
}


















