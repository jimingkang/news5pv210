#define GPH2CON   (*(volatile unsigned long*) 0xe0200c40)
#define GPH2DAT   (*(volatile unsigned long*) 0xe0200c44) 

#define GPH0CON		0xE0200C00
#define GPH0DAT		0xE0200C04
#define GPH2CON		0xE0200C40
#define GPH2DAT		0xE0200C44
#define rGPH0CON	(*(volatile unsigned int *)GPH0CON)
#define rGPH0DAT	(*(volatile unsigned int *)GPH0DAT)
#define rGPH2CON	(*(volatile unsigned int *)GPH2CON)
#define rGPH2DAT	(*(volatile unsigned int *)GPH2DAT)



/* 按键初始化函数 */
void key_init()
{
	/* 设置key1为外部中断16,key2为外部中断17*/
//	GPH2CON = (0xf<<0)|(0xf<<4);

	/*设置初始状态为高电平*/
//	GPH2DAT=0x3;

	rGPH0CON |= (0xFF<<8);
	// GPH2CON的bit0～15全部设置为0，即可
	rGPH2CON |= (0xFFFF<<0);

}
