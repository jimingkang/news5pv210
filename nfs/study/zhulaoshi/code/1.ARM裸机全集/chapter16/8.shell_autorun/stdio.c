void uart_putc(char c);
char uart_getc(void);

// ��stdio���һ���ַ�c
void putchar(char c)
{
	// �����û����'\n'ʱ��ʵ�����"\r\n"
	// windows�а��»س�����Ч��"\r\n"����linux�а��»س�����Ч��'\n'
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

// ��stdio���һ���ַ���p
void puts(const char *p)
{
	while (*p != '\0')
	{
		putchar(*p);
		p++;
	}
}

// ��stdio����һ���ַ�
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

// ��stdio����һ���ַ���
// ����ֵָ�򴫽�����������׵�ַ�ģ�Ŀ����ʵ�ֺ����ļ�������
char *gets(char *p)
{
	char *p1 = p;
	char ch;
	
	// �û���һ����������'\n'Ϊ������־��
	while ((ch = getchar()) != '\n')
	{
		// ����
		if (ch != '\b')
		{
			// �û�����Ĳ����˸��
			putchar(ch);				// ����
			*p++ = ch;					// �洢ch����Ч�� *p = ch; p++;
		}
		else
		{
			// �û���������˸��
			// \bֻ����secureCRT�ն����ָ�������һ�񣬵����Ǹ�Ҫɾ�����ַ�����
			// ɾ���ķ�����������3��
			if (p > p1)
			{
				putchar('\b');
				putchar(' ');
				putchar('\b');				// 3�д����˸������
				p--;						// ��һ��ָ��ָ����Ҫɾ�����Ǹ�����
				*p = '\0';					// ���'\0'���滻Ҫɾ�����Ǹ��ַ�
			}	
		}
		
	}
	// ����'\n'�н��������'\0'��Ϊ�ַ�����β��
	*p = '\0';
	putchar('\n');
	
	return p1;
}














