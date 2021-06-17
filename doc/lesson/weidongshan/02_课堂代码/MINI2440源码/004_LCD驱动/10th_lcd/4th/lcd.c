#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>

#include <asm/mach/map.h>
#include <asm/arch/regs-lcd.h>
#include <asm/arch/regs-gpio.h>
#include <asm/arch/fb.h>

static int s3c_lcdfb_setcolreg(unsigned int regno, unsigned int red,
			     unsigned int green, unsigned int blue,
			     unsigned int transp, struct fb_info *info);


struct lcd_regs {
	unsigned long	lcdcon1;
	unsigned long	lcdcon2;
	unsigned long	lcdcon3;
	unsigned long	lcdcon4;
	unsigned long	lcdcon5;
    unsigned long	lcdsaddr1;
    unsigned long	lcdsaddr2;
    unsigned long	lcdsaddr3;
    unsigned long	redlut;
    unsigned long	greenlut;
    unsigned long	bluelut;
    unsigned long	reserved[9];
    unsigned long	dithmode;
    unsigned long	tpal;
    unsigned long	lcdintpnd;
    unsigned long	lcdsrcpnd;
    unsigned long	lcdintmsk;
    unsigned long	lpcsel;
};

static struct fb_ops s3c_lcdfb_ops = {
	.owner		= THIS_MODULE,
	.fb_setcolreg	= s3c_lcdfb_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
};


static struct fb_info *s3c_lcd;
static volatile unsigned long *gpbcon;
static volatile unsigned long *gpbdat;
static volatile unsigned long *gpccon;
static volatile unsigned long *gpdcon;
static volatile unsigned long *gpgcon;
static volatile struct lcd_regs* lcd_regs;
static u32 pseudo_palette[16];


/* from pxafb.c */
static inline unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}


static int s3c_lcdfb_setcolreg(unsigned int regno, unsigned int red,
			     unsigned int green, unsigned int blue,
			     unsigned int transp, struct fb_info *info)
{
	unsigned int val;
	
	if (regno > 16)
		return 1;

	/* ��red,green,blue��ԭɫ�����val */
	val  = chan_to_field(red,	&info->var.red);
	val |= chan_to_field(green, &info->var.green);
	val |= chan_to_field(blue,	&info->var.blue);
	
	//((u32 *)(info->pseudo_palette))[regno] = val;
	pseudo_palette[regno] = val;
	return 0;
}

static int lcd_init(void)
{
	/* 1. ����һ��fb_info */
	s3c_lcd = framebuffer_alloc(0, NULL);

	/* 2. ���� */
	/* 2.1 ���ù̶��Ĳ��� */
	strcpy(s3c_lcd->fix.id, "mylcd");
	s3c_lcd->fix.smem_len = 320*240*32/8;        /* MINI2440��LCDλ����24,����2440������4�ֽڼ�32λ(�˷�1�ֽ�) */
	s3c_lcd->fix.type     = FB_TYPE_PACKED_PIXELS;
	s3c_lcd->fix.visual   = FB_VISUAL_TRUECOLOR; /* TFT */
	s3c_lcd->fix.line_length = 320*4;
	
	/* 2.2 ���ÿɱ�Ĳ��� */
	s3c_lcd->var.xres           = 320;
	s3c_lcd->var.yres           = 240;
	s3c_lcd->var.xres_virtual   = 320;
	s3c_lcd->var.yres_virtual   = 240;
	s3c_lcd->var.bits_per_pixel = 32;

	/* RGB:565 */
	s3c_lcd->var.red.offset     = 16;
	s3c_lcd->var.red.length     = 8;
	
	s3c_lcd->var.green.offset   = 8;
	s3c_lcd->var.green.length   = 8;

	s3c_lcd->var.blue.offset    = 0;
	s3c_lcd->var.blue.length    = 8;

	s3c_lcd->var.activate       = FB_ACTIVATE_NOW;
	
	
	/* 2.3 ���ò������� */
	s3c_lcd->fbops              = &s3c_lcdfb_ops;
	
	/* 2.4 ���������� */
	s3c_lcd->pseudo_palette = pseudo_palette; 
	//s3c_lcd->screen_base  = ;  /* �Դ�������ַ */ 
	s3c_lcd->screen_size   = 320*240*32/8;

	/* 3. Ӳ����صĲ��� */
	/* 3.1 ����GPIO����LCD */
	gpbcon = ioremap(0x56000010, 8);
	gpbdat = gpbcon+1;
	gpccon = ioremap(0x56000020, 4);
	gpdcon = ioremap(0x56000030, 4);
	gpgcon = ioremap(0x56000060, 4);

    *gpccon  = 0xaaaaaaaa;   /* GPIO�ܽ�����VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND */
	*gpdcon  = 0xaaaaaaaa;   /* GPIO�ܽ�����VD[23:8] */
	
//	*gpbcon &= ~(3);  /* GPB0����Ϊ������� */
//	*gpbcon |= 1;
//	*gpbdat &= ~1;     /* ����͵�ƽ */

	*gpgcon |= (3<<8); /* GPG4����LCD_PWREN */
	
	/* 3.2 ����LCD�ֲ�����LCD������, ����VCLK��Ƶ�ʵ� */
	lcd_regs = ioremap(0x4D000000, sizeof(struct lcd_regs));

	/* 
	 * MINI2440 LCD 3.5Ӣ�� ZQ3506_V0 SPEC.pdf ��11��12ҳ
	 * 
	 * LCD�ֲ�11,12ҳ��2440�ֲ�"Figure 15-6. TFT LCD Timing Example"һ�ԱȾ�֪������������
	 */

	/* bit[17:8]: VCLK = HCLK / [(CLKVAL+1) x 2], LCD�ֲ�11 (Dclk=6.4MHz~11MHz)
	 *            7.1MHz = 100MHz / [(CLKVAL+1) x 2]
	 *            CLKVAL = 6
	 * bit[6:5]: 0b11, TFT LCD
	 * bit[4:1]: 0b1101, 24 bpp for TFT
	 * bit[0]  : 0 = Disable the video output and the LCD control signal.
	 */
	lcd_regs->lcdcon1  = (6<<8) | (3<<5) | (0x0d<<1);

	/* ��ֱ�����ʱ�����
	 * ���������ֲ�
	 * bit[31:24]: VBPD, VSYNC֮���ٹ��೤ʱ����ܷ�����1������
	 *             LCD�ֲ� tvb=18
	 *             VBPD=17
	 * bit[23:14]: ������, 240, ����LINEVAL=240-1=239
	 * bit[13:6] : VFPD, �������һ������֮���ٹ��೤ʱ��ŷ���VSYNC
	 *             LCD�ֲ�tvf=4, ����VFPD=4-1=3
	 * bit[5:0]  : VSPW, VSYNC�źŵ�������, LCD�ֲ�tvp=1, ����VSPW=1-1=0
	 */
	 
    /* ʹ����Щ��ֵ, ͼ�������Ƶ�����, Ӧ���������ֲ��ʱ��
	 * �Լ�΢��һ��, �����ƶ���VBPD��VFPD
	 * ����(VBPD+VFPD)����, ��СVBPDͼ������, ȡVBPD=11, VFPD=9
	 * ���Լ���, ������10���
	 */
  //lcd_regs->lcdcon2  = (17<<24) | (239<<14) | (3<<6) | (0<<0);
	lcd_regs->lcdcon2  = (11<<24) | (239<<14) | (9<<6) | (0<<0);


	/* ˮƽ�����ʱ�����
	 * bit[25:19]: HBPD, VSYNC֮���ٹ��೤ʱ����ܷ�����1������
	 *             LCD�ֲ� thb=38
	 *             HBPD=37
	 * bit[18:8]: ������, 320, ����HOZVAL=320-1=319
	 * bit[7:0] : HFPD, �������һ�������һ����������֮���ٹ��೤ʱ��ŷ���HSYNC
	 *             LCD�ֲ�thf>=2, th=408=thp+thb+320+thf, thf=49, HFPD=49-1=48
	 */

    /* ʹ����Щ��ֵ, ͼ�������Ƶ�����, Ӧ���������ֲ��ʱ��
	 * �Լ�΢��һ��, �����ƶ���HBPD��HFPD
	 * ����(HBPD+HFPD)����, ����HBPDͼ������, ȡHBPD=69, HFPD=16
	 * ���Լ���, ������10���
	 */

//	lcd_regs->lcdcon3 = (37<<19) | (319<<8) | (48<<0);
	lcd_regs->lcdcon3 = (69<<19) | (319<<8) | (16<<0);

	/* ˮƽ�����ͬ���ź�
	 * bit[7:0]	: HSPW, HSYNC�źŵ�������, LCD�ֲ�Thp=1, ����HSPW=1-1=0
	 */	
	lcd_regs->lcdcon4 = 0;

	/* �źŵļ��� 
	 * bit[11]: 1=565 format, ����24bpp���������
	 * bit[10]: 0 = The video data is fetched at VCLK falling edge
	 * bit[9] : 1 = HSYNC�ź�Ҫ��ת,���͵�ƽ��Ч 
	 * bit[8] : 1 = VSYNC�ź�Ҫ��ת,���͵�ƽ��Ч 
	 * bit[6] : 0 = VDEN���÷�ת
	 * bit[3] : 0 = PWREN���0
	 *
	 * BSWP = 0, HWSWP = 0, BPP24BL = 0 : ��bpp=24ʱ,2440���ÿһ�����ط���32λ��4�ֽ�,��һ���ֽ��ǲ�ʹ�õ�? ��2440�ֲ�P412
         * bit[12]: 0, LSB valid, ������ֽڲ�ʹ��
	 * bit[1] : 0 = BSWP
	 * bit[0] : 0 = HWSWP
	 */
	lcd_regs->lcdcon5 = (0<<10) | (1<<9) | (1<<8) | (0<<12) | (0<<1) | (0<<0);
	
	/* 3.3 �����Դ�(framebuffer), ���ѵ�ַ����LCD������ */
	s3c_lcd->screen_base = dma_alloc_writecombine(NULL, s3c_lcd->fix.smem_len, &s3c_lcd->fix.smem_start, GFP_KERNEL);
	
	lcd_regs->lcdsaddr1  = (s3c_lcd->fix.smem_start >> 1) & ~(3<<30);
	lcd_regs->lcdsaddr2  = ((s3c_lcd->fix.smem_start + s3c_lcd->fix.smem_len) >> 1) & 0x1fffff;
	lcd_regs->lcdsaddr3  = (320*32/16);  /* һ�еĳ���(��λ: 2�ֽ�) */	
	
	//s3c_lcd->fix.smem_start = xxx;  /* �Դ�������ַ */
	/* ����LCD */
	lcd_regs->lcdcon1 |= (1<<0); /* ʹ��LCD������ */
	lcd_regs->lcdcon5 |= (1<<3); /* ʹ��LCD����: LCD_PWREN */
//	*gpbdat |= 1;     /* MINI2440�ı����·Ҳ��ͨ��LCD_PWREN�����Ƶ�, ����Ҫ�����ı������� */

	/* 4. ע�� */
	register_framebuffer(s3c_lcd);
	
	return 0;
}

static void lcd_exit(void)
{
	unregister_framebuffer(s3c_lcd);
	lcd_regs->lcdcon1 &= ~(1<<0); /* �ر�LCD������ */
	lcd_regs->lcdcon1 &= ~(1<<3); /* �ر�LCD���� */
//	*gpbdat &= ~1;     /* �رձ��� */
	dma_free_writecombine(NULL, s3c_lcd->fix.smem_len, s3c_lcd->screen_base, s3c_lcd->fix.smem_start);
	iounmap(lcd_regs);
	iounmap(gpbcon);
	iounmap(gpccon);
	iounmap(gpdcon);
	iounmap(gpgcon);
	framebuffer_release(s3c_lcd);
}

module_init(lcd_init);
module_exit(lcd_exit);

MODULE_LICENSE("GPL");


