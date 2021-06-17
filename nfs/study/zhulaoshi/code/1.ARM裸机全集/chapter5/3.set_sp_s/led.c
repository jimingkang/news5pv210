#define GPJ0CON		0xE0200240
#define GPJ0DAT		0xE0200244


void delay(void);

// �ú���Ҫʵ��led��˸Ч��
void led_blink(void)
{
	// led��ʼ����Ҳ���ǰ�GPJ0CON������Ϊ���ģʽ
	unsigned int *p = (unsigned int *)GPJ0CON;
	unsigned int *p1 = (unsigned int *)GPJ0DAT;
	*p = 0x11111111;
	
	while (1)
	{
		// led��
		*p1 = ((0<<3) | (0<<4) | (0<<5));
		// ��ʱ
		delay();
		// led��
		*p1 = ((1<<3) | (1<<4) | (1<<5));
		// ��ʱ
		delay();
	}
}


void delay(void)
{
	volatile unsigned int i = 900000;		// volatile �ñ�������Ҫ�Ż����������������ļ�
	while (i--);							// ��������ʱ�䣬ʵ��delay
}



#if 0
	
	// ֮���Ϊ���ܴ���
	// ��һ�������������Ŷ�����Ϊ���ģʽ�����벻��
	ldr r0, =0x11111111			// �Ӻ����=���Կ����õ���ldrαָ���Ϊ��Ҫ���������ж������
	ldr r1, =GPJ0CON			// �ǺϷ����������ǷǷ���������һ��д���붼��ldrαָ��
	str r0, [r1]				// �Ĵ������Ѱַ�������ǰ�r0�е���д�뵽r1�е���Ϊ��ַ���ڴ���ȥ

	// Ҫʵ����ˮ�ƣ�ֻҪ����ѭ����ʵ��1Ȧ����ˮ��ʾЧ������
flash:
	// ��1��������LED1������Ϩ��
	//ldr r0, =((0<<3) | (1<<4) | (1<<5))	// ��������Ŀ����ĸ����ĸ�����
	ldr r0, =~(1<<3)
	ldr r1, =GPJ0DAT
	str r0, [r1]				// ��0д�뵽GPJ0DAT�Ĵ����У����ż�����͵�ƽ��LED����
	// Ȼ����ʱ
	bl delay					// ʹ��bl���к�������
	
	// ��2��������LED2������Ϩ��	
	ldr r0, =~(1<<4)
	ldr r1, =GPJ0DAT
	str r0, [r1]				// ��0д�뵽GPJ0DAT�Ĵ����У����ż�����͵�ƽ��LED����
	// Ȼ����ʱ
	bl delay					// ʹ��bl���к�������
	
	// ��3��������LED3������Ϩ��	
	ldr r0, =~(1<<5)
	ldr r1, =GPJ0DAT
	str r0, [r1]				// ��0д�뵽GPJ0DAT�Ĵ����У����ż�����͵�ƽ��LED����
	// Ȼ����ʱ
	bl delay					// ʹ��bl���к�������
	
	b flash


// ��ʱ��������������delay
delay:
	ldr r2, =9000000
	ldr r3, =0x0
delay_loop:	
	sub r2, r2, #1				//r2 = r2 -1
	cmp r2, r3					// cmp��Ӱ��Z��־λ�����r2����r3��Z=1����һ����eq�ͻ����
	bne delay_loop
	mov pc, lr					// �������÷���

#endif

	
