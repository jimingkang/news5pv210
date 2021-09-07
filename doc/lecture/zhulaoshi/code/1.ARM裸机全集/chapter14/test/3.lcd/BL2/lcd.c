#include "ascii.h"
#include "800400.h"

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

/*
#define HSPW 		10	//(0)
#define HBPD		38	//(40 - 1)
#define HFPD 		210	//(5 - 1)
#define VSPW		7	//(0)
#define VBPD 		18	//(8 - 1)
#define VFPD 		22	//(8 - 1)
*/
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


unsigned int *fb_buf = (unsigned int*)FB_ADDR;


// ��ʼ��LCD
void lcd_init(void)
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
	// ���ó���
	VIDTCON2 = (LINEVAL << 11) | (HOZVAL << 0);

	// ����windows1
	// bit[0]:ʹ��
	// bit[2~5]:24bpp
	WINCON0 |= 1<<0;
	WINCON0 &= ~(0xf << 2);
	WINCON0 |= (0xB<<2) | (1<<15);

#define LeftTopX     0
#define LeftTopY     0
#define RightBotX   799
#define RightBotY   479

	// ����windows1����������
	VIDOSD0A = (LeftTopX<<11) | (LeftTopY << 0);
	VIDOSD0B = (RightBotX<<11) | (RightBotY << 0);
	VIDOSD0C = (LINEVAL + 1) * (HOZVAL + 1);


	// ����fb�ĵ�ַ
	VIDW00ADD0B0 = FB_ADDR;
	VIDW00ADD1B0 = (((HOZVAL + 1)*4 + 0) * (LINEVAL + 1)) & (0xffffff);

	// ʹ��channel 0��������
	SHADOWCON = 0x1;
}

// x����᷽�򣬾���x�ᣬ��������������Ǻ���
static inline int put_pixel(unsigned int x,unsigned int y,unsigned int color)
{
	//if ((x<0) || (x>=XSIZE) || (y<0) || (y>=YSIZE)) 
	//	return -1;
		
	//fb_buf[y][x] = color;
	*(fb_buf + y*XSIZE + x) = color;
	
	return 0;
}


void draw_background(unsigned int color)
{
	unsigned int x,y ;
	
	for(y=0; y<YSIZE; y++)
	{
		for(x=0; x<XSIZE; x++)
		{
			//fb_buf[y][x] = color ;
			put_pixel(x, y, color);
		}
	}
}

//��Բ����
void draw_circular(unsigned int centerX, unsigned int centerY, unsigned int radius, unsigned int color)
{
	int x,y ;
	int tempX,tempY;;
    int SquareOfR = radius*radius;

	for(y=0; y<XSIZE; y++)
	{
		for(x=0; x<YSIZE; x++)
		{
			if(y<=centerY && x<=centerX)
			{
				tempY=centerY-y;
				tempX=centerX-x;                        
			}
			else if(y<=centerY&& x>=centerX)
			{
				tempY=centerY-y;
				tempX=x-centerX;                        
			}
			else if(y>=centerY&& x<=centerX)
			{
				tempY=y-centerY;
				tempX=centerX-x;                        
			}
			else
			{
				tempY = y-centerY;
				tempX = x-centerX;
			}
			if ((tempY*tempY+tempX*tempX)<=SquareOfR)
				//fb_buf[y][x] = color;
				put_pixel(x, y, color);
		}
	}
}

static void show_8_16(unsigned int x, unsigned int y, unsigned int color, unsigned char *data)  
{  
    int i, j, count = 0;  
	  
    for (j=y; j<(y+16); j++)  
    {  
        for (i=x; i<(x+8); i++)  
        {  
            if (i<XSIZE && j<YSIZE)  
            {  
            	if (data[count/8]&(1<<(count%8)))  
                	//fb_buf[j][i] = color;  
					put_pixel(i, j, color);
            }  
            count++;  
        }  
    }  
} 


void draw_ascii_ok(unsigned int x, unsigned int y, unsigned int color, unsigned char *str)
{
	int i;  
	unsigned char *ch;
    for (i=0; str[i]!='\0'; i++)  
    {  
		ch = (unsigned char *)ascii_8_16[(unsigned char)str[i]-0x20];
        show_8_16(x, y, color, ch); 
		
        x += 8;
		if (x >= XSIZE)
		{
			x -= XSIZE;
			y += 16;
		}
    }  
}


int draw_bmp24(unsigned int width, unsigned int height, unsigned int x0, unsigned int y0, const unsigned char *pbmp)
{
	unsigned int x, y;
	unsigned int p = 0;
	unsigned int c;
	
	if ((width > 800) || (height > 480))
	{
		//printf("ERROR: data resolution greater than fb\n");
		return -1;
	}
	
	for (y=y0; y<y0 + height; y++)
	{
		for (x=x0; x<x0 + width; x++) 
		{
			c = (pbmp[p+0] << 0) | (pbmp[p+1] << 8) | (pbmp[p+2] << 16);
			//c = (pbmp[p+2] << 0) | (pbmp[p+1] << 8) | (pbmp[p+0] << 16);
			*(fb_buf + y * 800 + x) = c;
			p += 3;
		}
	}
}


void lcd_test(void)
{
	lcd_init();
	
	draw_bmp24(800, 480, 0, 0, gImage_800400);
	
/*
	draw_background(0xFFFFFF);
	draw_circular(400, 240, 390, 0xDC143C);

	draw_background(0x00FFFF);
	draw_circular(400, 240, 440, 0x438722);
*/

	//draw_background(0xFFFFFF);
	//draw_ascii_ok(0, 0, 0x333333, "ABCDabcd1234!()\n5678");

}
