

void puts(const char *p);
char *gets(char *p);
void uart_init(void);

// C��׼����Ҳ�и�memset�������������������õ����Լ�д�ģ�û�ñ�׼��
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
	char buf[100] = {0};		// �����ݴ��û����������
	
	uart_init();
	
	puts("x210 simple shell:\n");
	while(1)
	{
		
		puts("aston#");
		memset(buf, 0, sizeof(buf));		// bufŪ�ɾ��ô洢����û�����
		gets(buf);							// ��ȡ�û��������buf��
		puts("��������ǣ�");
		puts(buf);
		puts("\n");
	}
	
	return 0;
}