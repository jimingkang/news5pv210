#include "main.h"

#define GPF0CON			(*(volatile unsigned long *)0xE0200120)
#define GPF1CON			(*(volatile unsigned long *)0xE0200140)
#define GPF2CON			(*(volatile unsigned long *)0xE0200160)
#define GPF3CON			(*(volatile unsigned long *)0xE0200180)

#define GPD0CON			(*(volatile unsigned long *)0xE02000A0)
#define GPD0DAT			(*(volatile unsigned long *)0xE02000A4)

#define CLK_SRC1		(*(volatile unsigned long *)0xe0100204)
#define CLK_DIV1		(*(volatile unsigned long *)0xe0100304)
#define DISPLAY_CONTROL	(*(volatile unsigned long *)0xe0107008)

#define VIDCON0			(*(volatile unsigned long *)0xF8000000)
#define VIDCON1			(*(volatile unsigned long *)0xF8000004)
#define VIDTCON2		(*(volatile unsigned long *)0xF8000018)
#define WINCON0 		(*(volatile unsigned long *)0xF8000020)
#define WINCON2 		(*(volatile unsigned long *)0xF8000028)
#define SHADOWCON 		(*(volatile unsigned long *)0xF8000034)
#define VIDOSD0A 		(*(volatile unsigned long *)0xF8000040)
#define VIDOSD0B 		(*(volatile unsigned long *)0xF8000044)
#define VIDOSD0C 		(*(volatile unsigned long *)0xF8000048)

#define VIDW00ADD0B0 	(*(volatile unsigned long *)0xF80000A0)
#define VIDW00ADD1B0 	(*(volatile unsigned long *)0xF80000D0)

#define VIDTCON0 		(*(volatile unsigned long *)0xF8000010)
#define VIDTCON1 		(*(volatile unsigned long *)0xF8000014)

#define HSPW 			(40)				// 1~40 DCLK
#define HBPD			(10 - 1)			// 46
#define HFPD 			(240 - 1)			// 16 210 354
#define VSPW			(20)				// 1~20 DCLK
#define VBPD 			(10 - 1)			// 23
#define VFPD 			(30 - 1)			// 7 22 147



// FB��ַ
#define FB_ADDR			(0x23000000)
#define ROW				(480)
#define COL				(800)
#define HOZVAL			(COL-1)
#define LINEVAL			(ROW-1)

#define XSIZE			COL
#define YSIZE			ROW

u32 *pfb = (u32 *)FB_ADDR;


// ������ɫ����
#define BLUE	0x0000FF
#define RED		0xFF0000
#define GREEN	0x00FF00
#define WHITE	0xFFFFFF


// ��ʼ��LCD
static void lcd_init(void)
{
	// ������������LCD����
	GPF0CON = 0x22222222;
	GPF1CON = 0x22222222;
	GPF2CON = 0x22222222;
	GPF3CON = 0x22222222;

	// �򿪱���	GPD0_0��PWMTOUT0��
	GPD0CON &= ~(0xf<<0);
	GPD0CON |= (1<<0);			// output mode
	GPD0DAT &= ~(1<<0);			// output 0 to enable backlight

	// 10: RGB=FIMD I80=FIMD ITU=FIMD
	DISPLAY_CONTROL = 2<<0;

	// bit[26~28]:ʹ��RGB�ӿ�
	// bit[18]:RGB ����
	// bit[2]:ѡ��ʱ��ԴΪHCLK_DSYS=166MHz
	VIDCON0 &= ~( (3<<26)|(1<<18)|(1<<2) );

	// bit[1]:ʹ��lcd������
	// bit[0]:��ǰ֡������ʹ��lcd������
	VIDCON0 |= ( (1<<0)|(1<<1) );

	// bit[6]:ѡ����Ҫ��Ƶ
	// bit[6~13]:��Ƶϵ��Ϊ5����VCLK = 166M/(4+1) = 33M
	VIDCON0 |= 4<<6 | 1<<4;


	// H43-HSD043I9W1.pdf(p13) ʱ��ͼ��VSYNC��HSYNC���ǵ�����
	// s5pv210оƬ�ֲ�(p1207) ʱ��ͼ��VSYNC��HSYNC���Ǹ�������Ч��������Ҫ��ת
	VIDCON1 |= 1<<5 | 1<<6;

	// ����ʱ��
	VIDTCON0 = VBPD<<16 | VFPD<<8 | VSPW<<0;
	VIDTCON1 = HBPD<<16 | HFPD<<8 | HSPW<<0;
	// ���ó���(������Ļ)
	VIDTCON2 = (LINEVAL << 11) | (HOZVAL << 0);

	// ����window0
	// bit[0]:ʹ��
	// bit[2~5]:24bpp��RGB888��
	WINCON0 |= 1<<0;
	WINCON0 &= ~(0xf << 2);
	WINCON0 |= (0xB<<2) | (1<<15);

#define LeftTopX     0
#define LeftTopY     0
#define RightBotX   799
#define RightBotY   479

	// ����window0����������
	// ���õ����Դ�ռ�Ĵ�С
	VIDOSD0A = (LeftTopX<<11) | (LeftTopY << 0);
	VIDOSD0B = (RightBotX<<11) | (RightBotY << 0);
	VIDOSD0C = (LINEVAL + 1) * (HOZVAL + 1);


	// ����fb�ĵ�ַ
	VIDW00ADD0B0 = FB_ADDR;
	VIDW00ADD1B0 = (((HOZVAL + 1)*4 + 0) * (LINEVAL + 1)) & (0xffffff);

	// ʹ��channel 0��������
	SHADOWCON = 0x1;
}

// �����ص�(x, y)�����Ϊcolor��ɫ
static inline void lcd_draw_pixel(u32 x, u32 y, u32 color)
{
	*(pfb + COL * y + x) = color;
}

// ��������Ļȫ������һ����ɫcolor
static void lcd_draw_background(u32 color)
{
	u32 i, j;
	
	for (j=0; j<ROW; j++)
	{
		for (i=0; i<COL; i++)
		{
			lcd_draw_pixel(i, j, color);
		}
	}
}

static void delay(void)
{
	volatile u32 i, j;
	for (i=0; i<4000; i++)
		for (j=0; j<1000; j++);
}

void lcd_test(void)
{ int i=1;
	lcd_init();
	

// ���Ի��Ʊ���ɫ���ɹ�
	while (i>0)
	{
		lcd_draw_background(RED);
		delay();
		
		lcd_draw_background(GREEN);
		delay();
		
		lcd_draw_background(BLUE);
		delay();
		i--;
	}

}


