#include <stdio.h>


// 给状态机定义状态集
typedef enum 
{
	STATE1,
	STATE2,
	STATE3,
	STATE4,
	STATE5,
	STATE6,
	STATE7,
}STATE;


int main(void)
{
	int num = 0;
	// current_state记录状态机的当前状态，初始为STATE1，用户每输入一个正确的
	// 密码STATE就走一步，一直到STATE为STATE7后锁就开了；其中只要有一次用户
	// 输入对不上就回到STATE1.
	STATE current_state = STATE1;		// 状态机初始状态为STATE1
	
	// 第一步：实现一个用户循环输入密码的循环
	printf("请输入密码，密码正确开锁.\n");
	while (1)
	{
		scanf("%d", &num);
		printf("num = %d.\n", num);
		
		// 在这里处理用户的本次输入
		switch (current_state)
		{
			case STATE1:
				if (num == 1)
				{
					current_state = STATE2;		// 用户输入对了一步，STATE走一步
				}
				else
				{
					current_state = STATE1;
				}
				break;
			case STATE2:
				if (num == 2)
				{
					current_state = STATE3;		// 用户输入对了一步，STATE走一步
				}
				else
				{
					current_state = STATE1;
				}
				break;
			case STATE3:
				if (num == 3)
				{
					current_state = STATE4;		// 用户输入对了一步，STATE走一步
				}
				else
				{
					current_state = STATE1;
				}
				break;
			case STATE4:
				if (num == 4)
				{
					current_state = STATE5;		// 用户输入对了一步，STATE走一步
				}
				else
				{
					current_state = STATE1;
				}
				break;
			case STATE5:
				if (num == 5)
				{
					current_state = STATE6;		// 用户输入对了一步，STATE走一步
				}
				else
				{
					current_state = STATE1;
				}
				break;
			case STATE6:
				if (num == 6)
				{
					current_state = STATE7;		// 用户输入对了一步，STATE走一步
				}
				else
				{
					current_state = STATE1;
				}
				break;
			default:
				current_state = STATE1;
		}
		
		if (current_state == STATE7)
		{
			printf("锁开了.\n");
			break;
		}
	}
	
	
	return 0;
}



























