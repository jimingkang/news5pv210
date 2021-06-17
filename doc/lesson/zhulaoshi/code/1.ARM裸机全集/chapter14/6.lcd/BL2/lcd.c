#include "main.h"
#include "ascii.h"
#include "800480.h"

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

// ���ƺ��ߣ���ʼ����Ϊ(x1, y)��(x2, y),��ɫ��color
static void lcd_draw_hline(u32 x1, u32 x2, u32 y, u32 color)
{
	u32 x;
	
	for (x = x1; x<x2; x++)
	{
		lcd_draw_pixel(x, y, color);
	}
}

// �������ߣ���ʼ����Ϊ(x, y1)��(x, y2),��ɫ��color
static void lcd_draw_vline(u32 x, u32 y1, u32 y2, u32 color)
{
	u32 y;
	
	for (y = y1; y<y2; y++)
	{
		lcd_draw_pixel(x, y, color);
	}
}

// glib���еĻ��ߺ��������Ի�б�ߣ������˷ֱ���(x1, y1)��(x2, y2)
void glib_line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color)
{
	int dx,dy,e;
	dx=x2-x1; 
	dy=y2-y1;
    
	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;  
				while(x1<=x2)
				{
					lcd_draw_pixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					lcd_draw_pixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					lcd_draw_pixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else	 // 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					lcd_draw_pixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					lcd_draw_pixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					lcd_draw_pixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					lcd_draw_pixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					lcd_draw_pixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
}

//��Բ������Բ��������(centerX, centerY)���뾶��radius��Բ����ɫ��color
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
				lcd_draw_pixel(x, y, color);
		}
	}
}


// д��
// д�ֵ����Ͻ�����(x, y)���ֵ���ɫ��color���ֵ���ģ��Ϣ�洢��data��
static void show_8_16(unsigned int x, unsigned int y, unsigned int color, unsigned char *data)  
{  
// count��¼��ǰ���ڻ��Ƶ����صĴ���
    int i, j, count = 0;  
	  
    for (j=y; j<(y+16); j++)  
    {  
        for (i=x; i<(x+8); i++)  
        {  
            if (i<XSIZE && j<YSIZE)  
            {  
			// ������(i, j)������ش��ж���0����1�������1дcolor�������0ֱ������
            	if (data[count/8] & (1<<(count%8)))   
					lcd_draw_pixel(i, j, color);
            }  
            count++;  
        }  
    }  
} 

// д�ַ���
// �ַ�����ʼ�������Ͻ�Ϊ(x, y)���ַ���������ɫ��color,�ַ�������Ϊstr
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
			x -= XSIZE;			// �س�
			y += 16;			// ����
		}
    }  
}

// ��800��480��ͼ��ͼ�����ݴ洢��pData��ָ���������
void lcd_draw_picture(const unsigned char *pData)
{
	u32 x, y, color, p = 0;
	
	for (y=0; y<480; y++)
	{
		for (x=0; x<800; x++)
		{
			// �����ｫ�����(x, y)���Ǹ������������Ӧ����ɫֵ����
			color = ((pData[p+2] << 0) | (pData[p+1] << 8) | (pData[p+0] << 16));
			lcd_draw_pixel(x, y, color);
			p += 3;
		}
	}
}


void lcd_test(void)
{
	lcd_init();
	
	lcd_draw_picture(gImage_800480);
/*
	// ����дӢ����ĸ
	lcd_draw_background(WHITE);	
	draw_ascii_ok(0, 0, RED, "ABCDabcd1234!@#$%");
*/
	
/*
	// ���Ի�б��
	lcd_draw_background(WHITE);			// ���û�м���һ�У�����ɫ�ǻ������̵�
	glib_line(0, 479, 799, 0, RED);
	draw_circular(399, 239, 50, GREEN);
*/
	
/*	
	// ���Ի����ߡ�����
	lcd_draw_background(WHITE);
	lcd_draw_hline(350, 450, 240, RED);
	lcd_draw_vline(400, 190, 290, GREEN);
*/

/*	
// ���Ի��Ʊ���ɫ���ɹ�
	while (1)
	{
		lcd_draw_background(RED);
		delay();
		
		lcd_draw_background(GREEN);
		delay();
		
		lcd_draw_background(BLUE);
		delay();
	}
	*/
}


