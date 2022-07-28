#include "s5pv210_sdio.h"
#include "stdio.h"

#define DEBUG_HSMMC
#ifdef  DEBUG_HSMMC
#define Debug(x...)	 printf(x)
#else
#define Debug(x...) 
#endif

#define MAX_BLOCK  65535

static int cardType; // 卡类型
static unsigned int RCA; // 卡相对地址

// t=2000 -> 1 ms
#define MS	2000
#define US	2
static void delay(unsigned int t)
{
	unsigned int t2 = 0xFFFF;	// 0x1FFFFF;
	while (t--)
	{
		for (; t2; t2--);
	}
}
static void s5pHsmmcClockOn(int on)
{
	int timeout;
	if (on)
	{
		CLKCON0 |= (1<<2); // sd时钟使能
		timeout = 1000; // Wait max 10 ms
		while (!(CLKCON0 & (1<<3)))
		{
			// 等待SD输出时钟稳定
			if (timeout == 0)
			{
				return;
			}
			timeout--;
			delay(US * 10);
		}
	}
	else
	{
		CLKCON0 &= ~(1<<2); // sd时钟禁止
	}
}

static void s5pHsmmcSetClock(unsigned int clock)
{
	unsigned int temp;
	int timeout;
	int i;
	s5pHsmmcClockOn(0); // 关闭时钟	
	temp = CONTROL2_0;
	// Set SCLK_MMC(48M) from SYSCON as a clock source	
	temp = (temp & (~(3<<4))) | (2<<4);
	temp |= (1u<<31) | (1u<<30) | (1<<8);
	if (clock <= 500000)
	{
		temp &= ~((1<<14) | (1<<15));
		CONTROL3_0 = 0;
	}
	else
	{
		temp |= ((1<<14) | (1<<15));
		CONTROL3_0 = (1u<<31) | (1<<23);
	}
	CONTROL2_0 = temp;
	
	for (i=0; i<=8; i++)
	{
		if (clock >= (48000000/(1<<i)))
		{
			break;
		}
	}
	temp = ((1<<i) / 2) << 8; // clock div
	temp |= (1<<0); // Internal Clock Enable
	CLKCON0 = temp;
	timeout = 1000; // Wait max 10 ms
	while (!(CLKCON0 & (1<<1))) {
		// 等待内部时钟振荡稳定
		if (timeout == 0) {
			return;
		}
		timeout--;
		delay(US * 10);
	}
	
	s5pHsmmcClockOn(1); // 使能时钟
}
	
static int s5pHsmmcWaitForCommandDone(void)
{
	int i;	
	int errorState;
	
	// 等待命令完成,检查中断状态位NORINTSTS[15]以确定命令是否有错误
	// 若没有错误并且检测到NORINTSTS[0]命令完成位为1,则说明命令发送成功
	// 其它情况说明命令未能成功发送
	for (i = 0; i < 0x20000000; i++)
	{
		if (NORINTSTS0 & (1<<15))	// 出现错误
		{
			break;
		}
		if (NORINTSTS0 & (1<<0))
		{
			do
			{
				NORINTSTS0 = (1<<0); // 清除命令完成位
			}
			while (NORINTSTS0 & (1<<0));				
			return 0; // 命令发送成功
		}
	}
	
	errorState = ERRINTSTS0 & 0x1ff; // 可能通信错误,CRC检验错误,超时等
	NORINTSTS0 = NORINTSTS0; // 清除中断标志
	ERRINTSTS0 = ERRINTSTS0; // 清除错误中断标志	
	do
	{
		NORINTSTS0 = (1<<0); // 清除命令完成位
	}
	while (NORINTSTS0 & (1<<0));
	
	Debug("Command error, ERRINTSTS = 0x%x ", errorState);	
	return errorState; // 命令发送出错	
}

static int s5pHsmmcWaitForTransferDone(void)
{
	int errorState;
	int i;
	
	// 等待数据传输完成
	for (i = 0; i < 0x20000000; i++)
	{
		if (NORINTSTS0 & ( 1<< 15))		// 出现错误
		{
			break;
		}											
		if (NORINTSTS0 & (1 << 1))		// 数据传输完	
		{							
			do
			{
				NORINTSTS0 |= (1 << 1); // 清除传输完成位
			} 
			while (NORINTSTS0 & (1 << 1));									
			return 0;
		}
	}

	errorState = ERRINTSTS0 & 0x1ff; // 可能通信错误,CRC检验错误,超时等
	NORINTSTS0 = NORINTSTS0; // 清除中断标志
	ERRINTSTS0 = ERRINTSTS0; // 清除错误中断标志
	Debug("Transfer error, rHM1_ERRINTSTS = 0x%04x\r\n", errorState);	
	
	do
	{
		NORINTSTS0 = (1 << 1); // 出错后清除数据完成位
	}
	while (NORINTSTS0 & (1 << 1));
	
	return errorState; // 数据传输出错		
}

static int s5pHsmmcWaitForBufferReadReady(void)
{	
	int errorState;
	
	while (1)
	{
		if (NORINTSTS0 & (1 << 15))		// 出现错误
		{
			break;
		}
		if (NORINTSTS0 & (1 << 5))		// 读缓存准备好
		{
			NORINTSTS0 |= (1 << 5);		// 清除准备好标志
			return 0;
		}				
	}
			
	errorState =ERRINTSTS0 & 0x1ff; // 可能通信错误,CRC检验错误,超时等
	NORINTSTS0 = NORINTSTS0; 		// 清除中断标志	
	ERRINTSTS0 = ERRINTSTS0; 		// 清除错误中断标志
	Debug("Read buffer error, NORINTSTS: %04x\r\n", errorState);
	return errorState;
}

static int s5pHsmmcWaitForBufferWriteReady(void)
{
	int  errorState;
	while (1)
	{
		if (NORINTSTS0 & (1 << 15))		// 出现错误
		{
			break;
		}
		if (NORINTSTS0 & (1 << 4))		// 写缓存准备好
		{
			NORINTSTS0 |= (1 << 4);		// 清除准备好标志
			return 0;
		}				
	}
			
	errorState = ERRINTSTS0 & 0x1ff;	// 可能通信错误,CRC检验错误,超时等
	NORINTSTS0 = NORINTSTS0;			// 清除中断标志
	ERRINTSTS0 = ERRINTSTS0;			// 清除错误中断标志
	Debug("Write buffer error, NORINTSTS: %04x\r\n", errorState);
	
	return errorState;
}

static int s5pHsmmcIssueCommand(unsigned char cmd, unsigned int arg,
												unsigned char data, unsigned char response)
{
	unsigned int i;
	unsigned int value;
	unsigned int errorState;

	// 命令发送时,需检查命令线是否已被使用,否则需要等待正在发送的命令发送完才能发送这个命令
	for (i = 0; i < 0x1000000; i++)
	{
		if (!(PRNSTS0 & (1 << 0)))
		{
			break;
		}
	}

	if (i == 0x1000000)
	{
		Debug("CMD line time out, PRNSTS: %04x\r\n", PRNSTS0);
		return -1; // 命令超时
	}

	// 如果命令回复会带忙信号(如R1b回复),则需检查数据线是否已被使用
	// 若是,则等待数据线空闲,带忙回复命令发送后,sd卡会拉低DAT[0]线表明sd卡正忙,数据线不可用
	if (response == CMD_RESP_R1B)	 // R1b回复通过DAT0反馈忙信号
	{
		for (i = 0; i < 0x10000000; i++)
		{
			if (!(PRNSTS0 & (1 << 1)))
			{
				break;
			}		
		}
		if (i == 0x10000000)
		{
			Debug("Data line time out, PRNSTS: %04x\r\n", PRNSTS0);			
			return -2;
		}		
	}

	ARGUMENT0 = arg;		// 把命令参数写入ARGUMENT这个寄存器中

	value = (cmd << 8);		// command index 在CMDREG中设置命令值[13:8]
	
	// CMD12可终止传输
	if (cmd == 0x12)
	{
		value |= (0x3 << 6); // command type
	}

	// 设置是否需使用data线,如块读,块写等命令发送后
	// 会紧接着在data线上传输数据,其它不需传输数据的命令不要设置使用data线CMDREG[5]
	if (data)
	{
		value |= (1 << 5); // 需使用DAT线作为传输等
	}	

	// 设置sd卡的回复类型,绝大部分命令在sd卡正确响应后,都会对主机进行回复(R1-R7，R1b)
	// 每个命令对应的回复类型请参考sd卡规范,回复类型长度可能为136或48
	// 回复中是否包含CRC或命令值的反馈,如果包含,则告诉主控制器检查回复中相应的CRC或命令值反馈是否正确以确定传输正确
	// CMDREG设置好后,主控制器就会发送命令并接收设定长度的回复并根据设定检查CRC,命令值反馈是否正确(若回复中包含CRC或命令值反馈的话)
	switch (response)
	{
		case CMD_RESP_NONE:
			value |= (0 << 4) | (0 << 3) | 0x0; // 没有回复,不检查命令及CRC
			break;
		case CMD_RESP_R1:
		case CMD_RESP_R5:
		case CMD_RESP_R6:
		case CMD_RESP_R7:		
			value |= (1 << 4) | (1 << 3) | 0x2; // 检查回复中的命令,CRC
			break;
		case CMD_RESP_R2:
			value |= (0 << 4) | (1 << 3) | 0x1; // 回复长度为136位,包含CRC
			break;
		case CMD_RESP_R3:
		case CMD_RESP_R4:
			value |= (0 << 4) | (0 << 3) | 0x2; // 回复长度48位,不包含命令及CRC
			break;
		case CMD_RESP_R1B:
			value |= (1 << 4) | (1 << 3) | 0x3; // 回复带忙信号,会占用Data[0]线
			break;
		default:
			break;	
	}
	
	CMDREG0 = value;	
	
	errorState = s5pHsmmcWaitForCommandDone();
	if (errorState)
	{
		Debug("Command = %d\r\n", cmd);
	}
	
	return errorState; // 命令发送出错
}

// R1回复中包含了32位的card state,卡识别后,可在任一状态通过CMD13获得卡状态
static int s5pHsmmcGetCardState(void)
{
	// 通过发送CMD13获得目前卡的状态,块读,块写时必须在transfer态,不然需等待状态转换的完成
	if (s5pHsmmcIssueCommand(CMD13, RCA << 16, 0, CMD_RESP_R1))
	{
		return -1; // 卡出错
	}
	else
	{
		return ((RSPREG0_0 >> 9) & 0xf); // 返回R1回复中的[12:9]卡状态
	}
}

// Reads the SD Configuration Register (SCR).
int s5pHsmmcGetSCR(SD_SCR *pSCR)
{
	unsigned char *pBuffer;
	int errorState;
	unsigned int temp;
	unsigned int i;
	
	s5pHsmmcIssueCommand(CMD55, RCA << 16, 0, CMD_RESP_R1); 
	BLKSIZE0 = (7 << 12) | (8 << 0); // 最大DMA缓存大小,block为64位8字节			
	BLKCNT0 = 1; // 写入这次读1 block的sd状态数据
	ARGUMENT0 = 0; // 写入命令参数	

	// DMA禁能,读单块		
	TRNMOD0 = (0 << 5) | (1 << 4) | (0 << 2) | (1 << 1) | (0 << 0);	
	
	// 设置命令寄存器,read SD Configuration CMD51,R1回复
	CMDREG0 = (CMD51 << 8) | (1 << 5) | (1 << 4) | (1 << 3) | 0x2;
	
	errorState = s5pHsmmcWaitForCommandDone();
	if (errorState) {
		Debug("CMD51 error\r\n");
		return errorState;
	}

	errorState = s5pHsmmcWaitForBufferReadReady();
	if (errorState)
	{
		return errorState;
	}
	
	// Wide width data (SD Memory Register)
	pBuffer = (unsigned char *)pSCR + sizeof(SD_SCR) - 1;
	for (i = 0; i < 8/4; i++) {
		temp = BDATA0;
		*pBuffer-- = (unsigned char)temp;
		*pBuffer-- = (unsigned char)(temp >> 8);
		*pBuffer-- = (unsigned char)(temp >> 16);
		*pBuffer-- = (unsigned char)(temp >> 24);		
	}
	
	errorState = s5pHsmmcWaitForTransferDone();
	if (errorState)
	{
		Debug("Get SCR register error\r\n");
		return errorState;
	}

	return 0;
}

// Asks the selected card to send its cardspecific data
int s5pHsmmcGetCSD(unsigned char *pCSD)
{
	int i;
	unsigned int response[4];
	int state = -1;

	if (cardType != SD_HC && cardType != SD_V1 && cardType != SD_V2)
	{
		return state;	// 未识别的卡
	}
	
	// 取消卡选择,任何卡均不回复,已选择的卡通过RCA=0取消选择,
	// 卡回到stand-by状态
	s5pHsmmcIssueCommand(CMD7, 0, 0, CMD_RESP_NONE);
	for (i = 0; i < 1000; i++)
	{
		if (s5pHsmmcGetCardState() == CARD_STBY)	// CMD9命令需在standy-by status
		{
			break;		// 状态正确
		}
		delay(US * 100);
	}
	if (i == 1000)
	{
		return state;	// 状态错误
	}	
	
	// 请求已标记卡发送卡特定数据(CSD),获得卡信息
	if (!s5pHsmmcIssueCommand(CMD9, RCA << 16, 0, CMD_RESP_R2))
	{
		pCSD++;		// 跳过第一字节,CSD中[127:8]位对位寄存器中的[119:0]
		response[0] = RSPREG0_0;
		response[1] = RSPREG0_1;
		response[2] = RSPREG0_2;
		response[3] = RSPREG0_3;
		for (i = 0; i < 15; i++)	// 拷贝回复寄存器中的[119:0]到pCSD中
		{
			*pCSD++ = ((unsigned char *)response)[i];
		}	
		state = 0;		// CSD获取成功
	}

	s5pHsmmcIssueCommand(CMD7, RCA << 16, 0, CMD_RESP_R1);	// 选择卡,卡回到transfer状态
	
	return state;
}

static int s5pHsmmcSwitch(unsigned char mode, int group, int function, unsigned char *pStatus)
{
	int errorState;
	int temp;
	int i;
	
	BLKSIZE0 = (7 << 12) | (64 << 0); // 最大DMA缓存大小,block为512位64字节			
	BLKCNT0 = 1; // 写入这次读1 block的sd状态数据
	temp = (mode << 31U) | 0xffffff;
	temp &= ~(0xf << (group * 4));
	temp |= function << (group * 4);
	ARGUMENT0 = temp; // 写入命令参数	

	// DMA禁能,读单块		
	TRNMOD0 = (0 << 5) | (1 << 4) | (0 << 2) | (1 << 1) | (0 << 0);	
	
	// 设置命令寄存器,SWITCH_FUNC CMD6,R1回复
	CMDREG0 = (CMD6 << 8) | (1 << 5) | (1 << 4) | (1 << 3) | 0x2;
	
	errorState = s5pHsmmcWaitForCommandDone();
	if (errorState)
	{
		Debug("CMD6 error\r\n");
		return errorState;
	}

	errorState = s5pHsmmcWaitForBufferReadReady();
	if (errorState) {
		return errorState;
	}
	
	pStatus += 64 - 1;
	for (i = 0; i < 64/4; i++) {
		temp = BDATA0;
		*pStatus-- = (unsigned char)temp;
		*pStatus-- = (unsigned char)(temp >> 8);
		*pStatus-- = (unsigned char)(temp >> 16);
		*pStatus-- = (unsigned char)(temp >> 24);			
	}
		
	errorState = s5pHsmmcWaitForTransferDone();
	if (errorState)
	{
		Debug("Get sd status error\r\n");
		return errorState;
	}	
	
	return 0;	
}

static int s5pHsmmcSetBusWidth(unsigned char width)
{
	int state;
	
	if ((width != 1) || (width != 4))
	{
		return -1;
	}
	
	state = -1;		// 设置初始为未成功
	NORINTSTSEN0 &= ~(1 << 8);		// 关闭卡中断

	// sd卡初始化后默认是1线宽
	// 数据传输速率最高12.5MB/s(25M,4线)或25MB/s(50M,4线)
	// 通过ACMD6(CMD55+CMD6)来设置sd模式的位宽
	s5pHsmmcIssueCommand(CMD55, RCA << 16, 0, CMD_RESP_R1);
	
	if (width == 1)		// 1位宽
	{
		if (!s5pHsmmcIssueCommand(CMD6, 0, 0, CMD_RESP_R1))
		{
			HOSTCTL0 &= ~(1 << 1);
			state = 0; // 命令成功
		}
	}
	else		// 4位宽
	{
		if (!s5pHsmmcIssueCommand(CMD6, 2, 0, CMD_RESP_R1))
		{
			HOSTCTL0 |= (1 << 1);
			state = 0;		// 命令成功
		}
	}
	
	NORINTSTSEN0 |= (1 << 8);	// 打开卡中断	
	
	return state; // 返回0为成功
}

int s5pHsmmcInit(void)
{
	int timeout;
	int i;
	unsigned int capacity;
	unsigned int OCR;
	unsigned int temp;
	unsigned char switchStatus[64];
	SD_SCR SCR;
	unsigned char CSD[16];
	unsigned int c_size, c_size_multi, read_bl_len;	
	
	// 配制IO引脚为SDIO功能
	GPG0CON = 0x2222222;

	// 禁止上下拉
	GPG0PUD = 0;
//	GPG0PUD = 0x2AAAA;
	
	// 时钟控制寄存器配置
//	CLKCON0 = (0x80 << 8) + (1 << 2) + (1 << 0);

	// 容限寄存器配置  
//    CAPAREG0 = CAPAREG0_V18 + CAPAREG0_V30 + CAPAREG0_V33 + CAPAREG0_SUSRES + CAPAREG0_DMA + CAPAREG0_HSPD + CAPAREG0_ADMA2 +\  
//                    CAPAREG0_MAXBLKLEN + CAPAREG0_BASECLK + CAPAREG0_TOUTUNIT + CAPAREG0_TOUTCLK;  
	// 配制时钟源:channel 0 clock src = SCLKEPLL = 96M
	CLK_SRC4 = (CLK_SRC4 & (~(0xf<<0))) | (0x7<<0);
	
	// 时钟分频:channel 0 clock = SCLKEPLL/2 = 48M
	CLK_DIV4 = (CLK_DIV4 & (~(0xf<<0))) | (0x1<<0);
	
	// software reset for all
	SWRST0 = 1;

	timeout = 1000;
	temp = SWRST0;
	
	while (SWRST0 & (1<<0))
	{
		if (timeout == 0)
		{
			return -1;		// reset timeout
		}
		timeout--;
		delay(US * 10);
	}

	// 设置sd卡时钟在100k~400k,sd卡在识别阶段必须用慢速时钟进行访问
	s5pHsmmcSetClock(400000);	// 400k

	TIMEOUTCON0 = 0xe;			// 最大超时时间
	HOSTCTL0 &= ~(1 << 2);		// 正常速度模式
	NORINTSTS0 = NORINTSTS0;	// 清除正常中断状态标志
	ERRINTSTS0 = ERRINTSTS0;	// 清除错误中断状态标志
	NORINTSTSEN0 = 0x7fff;		// [14:0]中断状态使能
	ERRINTSTSEN0 = 0x7ff;		// [9:0]错误中断状态使能
	NORINTSIGEN0 = 0x7fff;		// [14:0]中断信号使能	
	ERRINTSIGEN0 = 0x3ff;		// [9:0]错误中断信号使能

	s5pHsmmcIssueCommand(CMD0, 0, 0, CMD_RESP_NONE); // 复位所有卡到空闲状态

	cardType = UNUSABLE; // 卡类型初始化不可用

	// 发送CMD8来检查卡是否支持主机电压(2.7v~3.3v)
	// 这个命令在sd2.0以上才被定义,若没有收到回复信号,则可能为sd1.0或MMC卡
	// 若接收到卡回复信号,说明为sd2.0版本卡
	// 没有回复,MMC/sd v1.x/not card
	if (s5pHsmmcIssueCommand(CMD8, 0x1aa, 0, CMD_RESP_R7))
	{
		for (i = 0; i < 100; i++)
		{
			// 进一步发送ACMD41(CMD55+CMD41),参数HCS位为0(非高容量卡)
			s5pHsmmcIssueCommand(CMD55, 0, 0, CMD_RESP_R1);

			// 从回复中确定sd卡己准备好,即可确定这是sd1.x版本的卡
			// 若回复中表明sd卡未准备好,则需重复发送ACMD41等待卡准备好
			if (!s5pHsmmcIssueCommand(CMD41, 0, 0, CMD_RESP_R3))
			{
				OCR = RSPREG0_0; 		// 获得回复的OCR(操作条件寄存器)值
				if (OCR & 0x80000000)	// 卡上电是否完成上电流程,是否busy
				{
					cardType = SD_V1;	// 正确识别出sd v1.x卡
					Debug("SD card version 1.x is detected\r\n");
					break;
				}
			}
			else
			{
				// 如果没有回复信号,说明是MMC卡或其它不能识别的卡
				// 可进一步发送CMD1确定是否MMC卡
				Debug("MMC card is not supported\r\n");
				return -1;
			}
			delay(US * 1000);				
		}
	} 
	else	// sd2.0版本卡
	{
		temp = RSPREG0_0;
		// 从回复中确定卡是否能在该电压下工作,不能则认为卡不可用
		if (((temp & 0xff) == 0xaa) && (((temp >> 8) & 0xf) == 0x1))	// 判断卡是否支持2.7~3.3v电压
		{
			OCR = 0;
			for (i = 0; i < 100; i++)
			{
				OCR |= (1 << 30);

				// 进一步发送ACMD41(CMD55+CMD41),参数HCS位为1表明主机支持高容量的卡
				// 若卡处于忙状态,则重复发送ACDM41,直到卡准备好
				s5pHsmmcIssueCommand(CMD55, 0, 0, CMD_RESP_R1);
				s5pHsmmcIssueCommand(CMD41, OCR, 0, CMD_RESP_R3); // reday态
				OCR = RSPREG0_0;
				if (OCR & 0x80000000)	// 卡上电是否完成上电流程,是否busy
				{
					if (OCR & (1 << 30))	// 判断卡为标准卡还是高容量卡
					{
						// ACMD41回复准备好后,再检查回复中的CCS位
						// 该位为1说明是sd2.0高容量sdhc卡
						cardType = SD_HC; // 高容量卡
						Debug("SDHC card is detected\r\n");
					}
					else
					{
						// 若为0,则说明为sd2.0标准容量卡
						cardType = SD_V2;
						Debug("SD version 2.0 standard card is detected\r\n");
					}
					break;
				}
				delay(US * 1000);
			}
		}
	}

	// 识别出sd1.x,sd2.0标准卡或sd2.0高容量卡后
	if (cardType == SD_HC || cardType == SD_V1 || cardType == SD_V2)
	{
		// 通过CMD2请求卡发送其CID(Card Identification),此时卡进入Identification态
		s5pHsmmcIssueCommand(CMD2, 0, 0, CMD_RESP_R2);

		// 卡在Identification态后,发送CMD3请求卡发布一个16位新的相对地址(RCA)
		// 以后主机与卡之间的点对点通信均会以这个RCA地址来进行,此时卡进入Stand-by态
		s5pHsmmcIssueCommand(CMD3, 0, 0, CMD_RESP_R6);
		
		RCA = (RSPREG0_0 >> 16) & 0xffff; // 从卡回复中得到卡相对地址

		// 通过发送CMD7选择卡,使卡进入transfer态
		// 因为卡的大部分操作如读,写,擦除等均是在这个状态下来进行的
		// 此时卡已完全准备好接收读写命令了
		s5pHsmmcIssueCommand(CMD7, RCA<<16, 0, CMD_RESP_R1); // 选择已标记的卡,transfer状态
		
		s5pHsmmcGetSCR(&SCR);
		if (SCR.SD_SPEC == 0)		// sd 1.0 - sd 1.01
		{
			// Version 1.0 doesn't support switching
			s5pHsmmcSetClock(24000000); // 设置SDCLK = 48M/2 = 24M			
		}
		else	// sd 1.10 / sd 2.0
		{
			temp = 0;
			for (i = 0; i < 4; i++)
			{
				if (s5pHsmmcSwitch(0, 0, 1, switchStatus) == 0)		// switch check
				{
					if (!(switchStatus[34] & (1 << 1)))	// Group 1, function 1 high-speed bit 273
					{
						// The high-speed function is ready
						if (switchStatus[50] & (1 << 1))	// Group, function 1 high-speed support bit 401
						{
							// high-speed is supported
							if (s5pHsmmcSwitch(1, 0, 1, switchStatus) == 0)		// switch
							{
								if ((switchStatus[47] & 0xf) == 1)		// function switch in group 1 is ok?
								{
									// result of the switch high-speed in function group 1
									Debug("Switch to high speed mode: CLK @ 50M\r\n");
									s5pHsmmcSetClock(48000000); // 设置SDCLK = 48M	
									temp = 1;									
								}
							}
						}
						break;
					}
				} 
			}
			
			if (temp == 0)
			{
				s5pHsmmcSetClock(24000000); // 设置SDCLK = 48M/2 = 24M
			}
		}
			
		if (!s5pHsmmcSetBusWidth(4))
		{
			Debug("Set bus width error\r\n");
			return -1; // 位宽设置出错
		}
		
		if (s5pHsmmcGetCardState() == CARD_TRAN)	// 此时卡应在transfer态
		{
			// 对于标准卡,可通过CMD16来设置块命令(如块读,块写)所操作块的长度(以字节数计)可实现字节的读写
			// 但对于高容量卡这个命令将被忽略,高容量卡一个块的长度均是固定512字节的
			// 通常通过CMD16设置块长度为512字节
			if (!s5pHsmmcIssueCommand(CMD16, 512, 0, CMD_RESP_R1))	// 设置块长度为512字节
			{
				NORINTSTS0 = 0xffff;		// 清除中断标志
				s5pHsmmcGetCSD(CSD);
				if ((CSD[15] >> 6) == 0)	// CSD v1.0->sd V1.x, sd v2.00 standar
				{
					read_bl_len = CSD[10] & 0xf;	// [83:80]
					c_size_multi = ((CSD[6] & 0x3) << 1) + ((CSD[5] & 0x80) >> 7);	// [49:47]
					c_size = ((int)(CSD[9] & 0x3) << 10) + ((int)CSD[8] << 2) + (CSD[7] >> 6);	// [73:62]				
					capacity = (c_size + 1) << ((c_size_multi + 2) + (read_bl_len-9));	// block(512 byte)
				} 
				else
				{
					c_size = ((CSD[8] & 0x3f) << 16) + (CSD[7] << 8) + CSD[6];	// [69:48]
					
					// 卡容量为字节(c_size+1)*512K byte,以1扇区512 byte字,卡的扇区数为		
					capacity = (c_size+1) << 10;	// block (512 byte)
				}
				
				Debug("Card Initialization succeed\r\n");				
				Debug("Capacity: %ldMB\r\n", capacity / (1024*1024 / 512));
				
				return 0;	// 初始化成功							
			}
		}
	}
	
	Debug("Card Initialization failed\r\n");
	
	return -1;	// 卡工作异常
}

int s5pHsmmcEraseBlock(unsigned int startBlock, unsigned int endBlock)
{
	int i;

	if (cardType == SD_V1 || cardType == SD_V2)
	{
		startBlock <<= 9;	// 标准卡为字节地址
		endBlock <<= 9;
	}
	else if (cardType != SD_HC)
	{
		return -1;	// 未识别的卡
	}	

	s5pHsmmcIssueCommand(CMD32, startBlock, 0, CMD_RESP_R1);
	s5pHsmmcIssueCommand(CMD33, endBlock, 0, CMD_RESP_R1);
	if (!s5pHsmmcIssueCommand(CMD38, 0, 0, CMD_RESP_R1B))
	{
		for (i = 0; i < 0x10000; i++)
		{
			if (s5pHsmmcGetCardState() == CARD_TRAN)	// 擦除完成后返回到transfer状态
			{
				Debug("Erasing complete!\r\n");
				return 0;	// 擦除成功				
			}
			delay(US * 1000);
		}		
	}		

	Debug("Erase block failed\r\n");
	
	return -1;	// 擦除失败
}

int s5pHsmmcReadBlock(unsigned char *pBuffer, unsigned int blockAddr, unsigned int blockNumber)
{
	unsigned int address = 0;
	unsigned int readBlock;
	unsigned int i;
	unsigned int j;
	int errorState;
	unsigned int temp;
	
	if (pBuffer == 0 || blockNumber == 0)
	{
		Debug("s5pHsmmcReadBlock error -1\r\n");
		return -1;
	}

	NORINTSTS0 = NORINTSTS0;	// 清除中断标志
	ERRINTSTS0 = ERRINTSTS0;	// 清除错误中断标志	
	
	while (blockNumber > 0)
	{
		if (blockNumber <= MAX_BLOCK)
		{
			readBlock = blockNumber;	// 读取的块数小于65536 Block
			blockNumber = 0;			// 剩余读取块数为0
		}
		else
		{
			readBlock = MAX_BLOCK;		// 读取的块数大于65536 Block,分多次读
			blockNumber -= readBlock;
		}

		// 根据sd主机控制器标准,按顺序写入主机控制器相应的寄存器		
		BLKSIZE0 = (7 << 12) | (512 << 0);	// 最大DMA缓存大小,block为512字节			
		BLKCNT0  = readBlock;				// 写入这次读block数目
		
		if (cardType == SD_HC)
		{
			address = blockAddr;			// SDHC卡写入地址为block地址
		}
		else if (cardType == SD_V1 || cardType == SD_V2)
		{
			address = blockAddr << 9;		// 标准卡写入地址为字节地址
		}

		blockAddr += readBlock;				// 下一次读块的地址
		ARGUMENT0 = address;				// 写入命令参数		
		
		if (readBlock == 1)
		{
			// 设置传输模式,DMA禁能,读单块
			TRNMOD0 = (0 << 5) | (1 << 4) | (0 << 2) | (1 << 1) | (0 << 0);
			
			// 设置命令寄存器,单块读CMD17,R1回复
			CMDREG0 = (CMD17 << 8) | (1 << 5) | (1 << 4) | (1 << 3) | 0x2;
		}
		else
		{
			// 设置传输模式,DMA禁能,读多块			
			TRNMOD0 = (1 << 5) | (1 << 4) | (1 << 2) | (1 << 1) | (0 << 0);
			
			// 设置命令寄存器,多块读CMD18,R1回复	
			CMDREG0 = (CMD18 << 8) | (1 << 5) | (1 << 4) | (1 << 3) | 0x2;			
		}	
		
		errorState = s5pHsmmcWaitForCommandDone();
		if (errorState)
		{
			Debug("read commond error\r\n");
			return errorState;
		}

		for (i = 0; i < readBlock; i++)
		{
			errorState = s5pHsmmcWaitForBufferReadReady();
			if (errorState)
			{
				Debug("s5pHsmmcWaitForBufferReadReady error\r\n");
				return errorState;
			}
			if (((unsigned int)pBuffer & 0x3) == 0)
			{	
				for (j = 0; j < 512/4; j++)
				{
					*(unsigned int *)pBuffer = BDATA0;
					pBuffer += 4;
				}
			}
			else
			{
				for (j = 0; j < 512/4; j++)
				{
					temp =BDATA0;
					*pBuffer++ = (unsigned char)temp;
					*pBuffer++ = (unsigned char)(temp >> 8);
					*pBuffer++ = (unsigned char)(temp >> 16);
					*pBuffer++ = (unsigned char)(temp >> 24);
				}
			}						
		}
		
		errorState = s5pHsmmcWaitForTransferDone();
		if (errorState)
		{
			Debug("Read block error\r\n");
			return errorState;
		}	
	}

	return 0;	// 所有块读完
}

int s5pHsmmcwriteBlock(unsigned char *pBuffer, unsigned int blockAddr, unsigned int blockNumber)
{
	unsigned int address = 0;
	unsigned int writeBlock;
	unsigned int i;
	unsigned int j;
	int errorState;

	if (pBuffer == 0 || blockNumber == 0)
	{
		return -1;	// 参数错误
	}	
	
	NORINTSTS0 = NORINTSTS0;	// 清除中断标志
	ERRINTSTS0 = ERRINTSTS0;	// 清除错误中断标志

	while (blockNumber > 0)
	{	
		if (blockNumber <= MAX_BLOCK)
		{
			writeBlock = blockNumber;	// 写入的块数小于65536 Block
			blockNumber = 0;			// 剩余写入块数为0
		}
		else
		{
			writeBlock = MAX_BLOCK;		// 写入的块数大于65536 Block,分多次写
			blockNumber -= writeBlock;
		}
		
		if (writeBlock > 1)				// 多块写,发送ACMD23先设置预擦除块数
		{
			s5pHsmmcIssueCommand(CMD55, RCA << 16, 0, CMD_RESP_R1);
			s5pHsmmcIssueCommand(CMD23, writeBlock, 0, CMD_RESP_R1);
		}

		// 根据sd主机控制器标准,按顺序写入主机控制器相应的寄存器			
		BLKSIZE0 = (7 << 12) | (512 << 0);	// 最大DMA缓存大小,block为512字节		
		BLKCNT0  = writeBlock;				// 写入block数目	

		if (cardType == SD_HC)
		{
			address = blockAddr;	// SDHC卡写入地址为block地址
		}
		else if (cardType == SD_V1 || cardType == SD_V2)
		{
			address = blockAddr << 9;	// 标准卡写入地址为字节地址
		}

		blockAddr += writeBlock;		// 下一次写地址
		ARGUMENT0 = address;			// 写入命令参数			

		// 设置特定主机控制器的传输方式,如DMA传输数据,通过FIFO传输数据
		if (writeBlock == 1)
		{
			// 设置传输模式,DMA禁能写单块
			TRNMOD0 = (0 << 5) | (0 << 4) | (0 << 2) | (1 << 1) | (0 << 0);
			
			// 设置命令寄存器,单块写CMD24,R1回复
			CMDREG0 = (CMD24 << 8) | (1 << 5) | (1 << 4) | (1 << 3) | 0x2;			
		}
		else
		{
			// 设置传输模式,DMA禁能写多块
			TRNMOD0 = (1 << 5) | (0 << 4) | (1 << 2) | (1 << 1) | (0 << 0);
			
			// 设置命令寄存器,多块写CMD25,R1回复
			CMDREG0 = (CMD25 << 8) | (1 << 5) | (1 << 4) | (1 << 3) | 0x2;					
		}

		errorState = s5pHsmmcWaitForCommandDone();
		if (errorState)
		{
			Debug("Write Command error\r\n");
			return errorState;
		}	

		for (i = 0; i < writeBlock; i++)
		{
			errorState = s5pHsmmcWaitForBufferWriteReady();
			if (errorState)
			{
				return errorState;
			}

			if (((unsigned int)pBuffer & 0x3) == 0)
			{
				for (j = 0; j < 512/4; j++)
				{
					BDATA0 = *(unsigned int *)pBuffer;
					pBuffer += 4;
				}
			}
			else
			{
				for (j = 0; j < 512/4; j++)
				{
					BDATA0 = pBuffer[0] + ((unsigned int)pBuffer[1] << 8) +
					((unsigned int)pBuffer[2] << 16) + ((unsigned int)pBuffer[3] << 24);
					pBuffer += 4;
				}
			}	
		}
		
		errorState = s5pHsmmcWaitForTransferDone();
		if (errorState)
		{
			Debug("Write block error\r\n");
			return errorState;
		}
		
		for (i = 0; i < 0x10000000; i++)
		{
			if (s5pHsmmcGetCardState() == CARD_TRAN)	// 需在transfer status
			{
				break;	// 状态正确
			}
		}
		
		if (i == 0x10000000)
		{
			return -3;		// 状态错误或Programming超时
		}		
	}
	
	return 0; // 写完所有数据
}

