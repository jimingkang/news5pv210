#define GPH2CON   (*(volatile unsigned long*) 0xe0200c40)
#define GPH2DAT   (*(volatile unsigned long*) 0xe0200c44) 



/* ������ʼ������ */
void key_init()
{
	/* ����key1Ϊ�ⲿ�ж�16,key2Ϊ�ⲿ�ж�17*/
	GPH2CON = (0xf<<0)|(0xf<<4);

	/*���ó�ʼ״̬Ϊ�ߵ�ƽ*/
	GPH2DAT=0x3;
}
