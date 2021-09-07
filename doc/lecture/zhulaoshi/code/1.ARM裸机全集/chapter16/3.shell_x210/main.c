

void puts(const char *p);
char *gets(char *p);
void uart_init(void);

// C标准库中也有个memset函数，但是我们这里用的是自己写的，没用标准库
void memset(char *p, int val, int length)
{
	int i;
	
	for (i=0; i<length; i++)
	{
		p[i] = val;
	}
}



int main(void)
{
	char buf[100] = {0};		// 用来暂存用户输入的命令
	
	uart_init();
	
	puts("x210 simple shell:\n");
	while(1)
	{
		
		puts("aston#");
		memset(buf, 0, sizeof(buf));		// buf弄干净好存储这次用户输入
		gets(buf);							// 读取用户输入放入buf中
		puts("您输入的是：");
		puts(buf);
		puts("\n");
	}
	
	return 0;
}