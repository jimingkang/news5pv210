#include "i2c.h"
#include "lib.h"
#include "led.h"
 
#define WRDATA      (1)
#define RDDATA      (2)
 
typedef struct tI2C {
    unsigned char *pData;   /* 数据缓冲区 */
    volatile int DataCount; /* 等待传输的数据长度 */
    volatile int Status;    /* 状态 */
    volatile int Mode;      /* 模式：读/写 */
    volatile int Pt;        /* pData中待传输数据的位置 */
}t210_I2C, *pt210_I2C;
 
static t210_I2C g_t210_I2C;
 
void i2c_init(void)
{
	/* 选择引脚功能：GPD1_0:IIC0_SDA, GPD1_1:IIC0_SCL */
	GPD1CON |= 0x22;		// GPD1CON[7:4] = 0b0010,GPD1CON[3:0] = 0b0010
	GPD1PUD |= 0x5;			// GPD1PUD[3:0] = 0b0101  禁止上拉
 
	/* bit[7] = 1, 使能ACK
	*  bit[6] = 0, IICCLK = PCLK/16
	*  bit[5] = 1, 使能中断
	*  bit[3:0] = 0xf, Tx clock = IICCLK/16
	*  PCLK = 66.7MHz, IICCLK = 4.1MHz
	*/
	IICCON = (1<<7) | (0<<6) | (1<<5) | (0xf);  // 0xaf
 
	IICSTAT = 0x10;     // I2C串行输出使能(Rx/Tx)
}
 
/*
 * 主机发送
 * slvAddr : 从机地址，buf : 数据存放的缓冲区，len : 数据长度 
 */
void i2c_write(unsigned int slvAddr, unsigned char *buf, int len)
{
    g_t210_I2C.Mode = WRDATA;   // 写操作
    g_t210_I2C.Pt   = 0;        // 索引值初始为0
    g_t210_I2C.pData = buf;     // 保存缓冲区地址
    g_t210_I2C.DataCount = len; // 传输长度
    
    IICDS   = slvAddr;
    IICSTAT = 0xf0;             // 主机发送，启动，后续的传输工作将在中断服务程序中完成
    
    /* 等待直至数据传输完毕 */    
    while (g_t210_I2C.DataCount != -1);
}
        
/*
 * 主机接收
 * slvAddr : 从机地址，buf : 数据存放的缓冲区，len : 数据长度 
 */
void i2c_read(unsigned int slvAddr, unsigned char *buf, int len)
{
    g_t210_I2C.Mode = RDDATA;   // 读操作
    g_t210_I2C.Pt   = -1;       // 索引值初始化为-1，表示第1个中断时不接收数据(地址中断)
    g_t210_I2C.pData = buf;     // 保存缓冲区地址
    g_t210_I2C.DataCount = len; // 传输长度
    
    IICDS        = slvAddr;
    IICSTAT      = 0xb0;        // 主机接收，启动,后续的传输工作将在中断服务程序中完成
    
    /* 等待直至数据传输完毕 */    
    while (g_t210_I2C.DataCount != 0);
}
 
/*  真正的I2C中断服务函数 */
void do_i2c_irq(void) 
{
	unsigned int i;			//用于简单延时
	switch (g_t210_I2C.Mode)
	{    
		case WRDATA:	//写中断
		{
			if((g_t210_I2C.DataCount--) == 0)
			{
				// 下面两行用来恢复I2C操作，发出P信号
				IICSTAT = 0xd0;		// 发出P信号，但由于IICCON[4]仍为1，P信号实际还没有发出
				IICCON  = 0xaf;		//当清除IICCON[4]后，P信号才真正发出信号
				delay(10000);  		// 等待一段时间以便P信号已经发出
				break;    
			}
 
			IICDS = g_t210_I2C.pData[g_t210_I2C.Pt++];
 
			// 将数据写入IICDS后，需要一段时间才能出现在SDA线上
			for (i = 0; i < 10; i++);   
 
			IICCON = 0xaf;      // 恢复I2C传输,IICCON[4] = 0
			break;
		}
 
		case RDDATA:  //读中断
		{
			if (g_t210_I2C.Pt == -1)
			{
				// 这次中断是发送I2C设备地址后发生的，没有数据
				// 只接收一个数据时，不要发出ACK信号
				g_t210_I2C.Pt = 0;
				if(g_t210_I2C.DataCount == 1)
					IICCON = 0x2f;   // 恢复I2C传输，开始接收数据，接收到数据时不发出ACK
				else 
					IICCON = 0xaf;   // 恢复I2C传输，开始接收数据,接收到数据时发出ACK
				break;
			}
 
			g_t210_I2C.pData[g_t210_I2C.Pt++] = IICDS;
			g_t210_I2C.DataCount--;
 
			if (g_t210_I2C.DataCount == 0)
			{
 
				// 下面两行恢复I2C操作，发出P信号
				IICSTAT = 0x90;
				IICCON  = 0xaf;
				delay(10000);  // 等待一段时间以便P信号已经发出
				break;    
			}      
			else
			{           
				// 接收最后一个数据时，不要发出ACK信号
				if(g_t210_I2C.DataCount == 1)
					IICCON = 0x2f;   // 恢复I2C传输，接收到下一数据时无ACK
				else 
					IICCON = 0xaf;   // 恢复I2C传输，接收到下一数据时发出ACK
			}
			break;
		}
 
		default:
		    break;      
	}
	// 清中断向量
	VIC0ADDRESS = 0x0;
	VIC1ADDRESS = 0x0;
	VIC2ADDRESS = 0x0;
	VIC3ADDRESS = 0x0;
	IICCON &= ~(1<<4); 	//第4位用于中断的标志，当接收或发送数据后一定要对该位，进行清零，以清除中断标志
} 
 
/* 需要根据AT24Cxx芯片手册的读字节协议 */
unsigned char at24cxx_read(unsigned char address)
{
	unsigned char val;
	i2c_write(0xA0, &address, 1);	//AT24C02的设备地址为0xa0
	i2c_read(0xA0, (unsigned char *)&val, 1);
	return val;
}
 
/* 需要根据AT24Cxx芯片手册的写字节协议 */
void at24cxx_write(unsigned char address, unsigned char data)
{
	unsigned char val[2];
	val[0] = address;
	val[1] = data;
	i2c_write(0xA0, val, 2);
}
