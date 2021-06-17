#define GPJ0CON		0xE0200240
#define GPJ0DAT		0xE0200244

#define rGPJ0CON	*((volatile unsigned int *)GPJ0CON)
#define rGPJ0DAT	*((volatile unsigned int *)GPJ0DAT)

void delay(void);

// �ú���Ҫʵ��led��˸Ч��
void led_blink(void)
{
	// led��ʼ����Ҳ���ǰ�GPJ0CON������Ϊ���ģʽ
	//volatile unsigned int *p = (unsigned int *)GPJ0CON;
	//volatile unsigned int *p1 = (unsigned int *)GPJ0DAT;
	rGPJ0CON = 0x11111111;
	
	while (1)
	{
		// led��
		rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));
		// ��ʱ
		delay();
		// led��
		rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
		// ��ʱ
		delay();
	}
}


void delay(void)
{
	volatile unsigned int i = 900000;		// volatile �ñ�������Ҫ�Ż����������������ļ�
	while (i--);							// ��������ʱ�䣬ʵ��delay
}

	
