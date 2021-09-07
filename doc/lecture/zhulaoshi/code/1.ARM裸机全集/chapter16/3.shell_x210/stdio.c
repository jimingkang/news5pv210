void uart_putc(char c);
char uart_getc(void);

// 从stdio输出一个字符c
void putchar(char c)
{
	// 碰到用户输出'\n'时，实际输出"\r\n"
	// windows中按下回车键等效于"\r\n"，在linux中按下回车键等效于'\n'
	if (c == '\n')
		uart_putc('\r');
/*	
	if (c == '\b')
	{
		uart_putc('\b');
		uart_putc(' ');
	}
*/	
	uart_putc(c);
}

// 从stdio输出一个字符串p
void puts(const char *p)
{
	while (*p != '\0')
	{
		putchar(*p);
		p++;
	}
}

// 从stdio输入一个字符
char getchar(void)
{
	char c;
	c = uart_getc();
	if (c == '\r')
	{
		return '\n';
	}
	
	return c;
}

// 从stdio输入一个字符串
// 返回值指向传进来的数组的首地址的，目的是实现函数的级联调用
char *gets(char *p)
{
	char *p1 = p;
	char ch;
	
	// 用户的一次输入是以'\n'为结束标志的
	while ((ch = getchar()) != '\n')
	{
		// 回显
		if (ch != '\b')
		{
			// 用户输入的不是退格键
			putchar(ch);				// 回显
			*p++ = ch;					// 存储ch，等效于 *p = ch; p++;
		}
		else
		{
			// 用户输入的是退格键
			// \b只会让secureCRT终端输出指针向后退一格，但是那个要删掉的字符还在
			// 删掉的方法就是下面3行
			if (p > p1)
			{
				putchar('\b');
				putchar(' ');
				putchar('\b');				// 3行处理退格键回显
				p--;						// 退一格，指针指向了要删除的那个格子
				*p = '\0';					// 填充'\0'以替换要删除的那个字符
			}	
		}
		
	}
	// 遇到'\n'行结束，添加'\0'作为字符串结尾。
	*p = '\0';
	putchar('\n');
	
	return p1;
}














