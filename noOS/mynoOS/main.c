#include "stdio.h"
#include "main.h"
#include "int.h"
void uart_init(void);


#define KEY_EINT2		NUM_EINT2		// left
#define KEY_EINT3		NUM_EINT3		// down
#define KEY_EINT16_19	NUM_EINT16_31	// ����4�����õ�
/*
void delay(int i)
{
	volatile int j = 10000;
	while (i--)
		while(j--);
}
*/

int main(void)
{
        

	uart_init();
printf("after uart init \r\n");		
	
/*    char a = 'T';
	putc('a');
	putc('b');
	putc('c');
	while (1)
	{a=getc();
	printf("test for printf, a = %d.\r\n", a);
	}
*/
        
	key_init();
	key_polling();
	
/*
 
       
	
	// ���������Ҫʹ���жϣ���Ҫ�����жϳ�ʼ����������ʼ���жϿ�����
	system_init_exception();
key_init_interrupt();


	
	// ��isr���жϿ�����Ӳ��
	intc_setvectaddr(KEY_EINT2, isr_eint2);
	intc_setvectaddr(KEY_EINT3, isr_eint3);
	intc_setvectaddr(KEY_EINT16_19, isr_eint16171819);
	
	// ʹ���ж�
	intc_enable(KEY_EINT2);
	intc_enable(KEY_EINT3);
	intc_enable(KEY_EINT16_19);
	// ������Ӹ�����
	printf("-------------key interrupt test--------------");
	*/	
return 0;
}
