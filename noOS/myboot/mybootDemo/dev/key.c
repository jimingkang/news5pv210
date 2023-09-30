#define GPH2CON   (*(volatile unsigned long*) 0xe0200c40)
#define GPH2DAT   (*(volatile unsigned long*) 0xe0200c44) 



/* 按键初始化函数 */
void key_init()
{
	/* 设置key1为外部中断16,key2为外部中断17*/
	GPH2CON = (0xf<<0)|(0xf<<4);

	/*设置初始状态为高电平*/
	GPH2DAT=0x3;
}
