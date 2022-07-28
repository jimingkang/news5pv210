#include "s5pv210_sdio.h"
#include "stdio.h"

#define DEBUG_HSMMC
#ifdef  DEBUG_HSMMC
#define Debug(x...)	 printf(x)
#else
#define Debug(x...) 
#endif

#define MAX_BLOCK  65535

static int cardType; // ������
static unsigned int RCA; // ����Ե�ַ

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
		CLKCON0 |= (1<<2); // sdʱ��ʹ��
		timeout = 1000; // Wait max 10 ms
		while (!(CLKCON0 & (1<<3)))
		{
			// �ȴ�SD���ʱ���ȶ�
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
		CLKCON0 &= ~(1<<2); // sdʱ�ӽ�ֹ
	}
}

static void s5pHsmmcSetClock(unsigned int clock)
{
	unsigned int temp;
	int timeout;
	int i;
	s5pHsmmcClockOn(0); // �ر�ʱ��	
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
		// �ȴ��ڲ�ʱ�����ȶ�
		if (timeout == 0) {
			return;
		}
		timeout--;
		delay(US * 10);
	}
	
	s5pHsmmcClockOn(1); // ʹ��ʱ��
}
	
static int s5pHsmmcWaitForCommandDone(void)
{
	int i;	
	int errorState;
	
	// �ȴ��������,����ж�״̬λNORINTSTS[15]��ȷ�������Ƿ��д���
	// ��û�д����Ҽ�⵽NORINTSTS[0]�������λΪ1,��˵������ͳɹ�
	// �������˵������δ�ܳɹ�����
	for (i = 0; i < 0x20000000; i++)
	{
		if (NORINTSTS0 & (1<<15))	// ���ִ���
		{
			break;
		}
		if (NORINTSTS0 & (1<<0))
		{
			do
			{
				NORINTSTS0 = (1<<0); // ����������λ
			}
			while (NORINTSTS0 & (1<<0));				
			return 0; // ����ͳɹ�
		}
	}
	
	errorState = ERRINTSTS0 & 0x1ff; // ����ͨ�Ŵ���,CRC�������,��ʱ��
	NORINTSTS0 = NORINTSTS0; // ����жϱ�־
	ERRINTSTS0 = ERRINTSTS0; // ��������жϱ�־	
	do
	{
		NORINTSTS0 = (1<<0); // ����������λ
	}
	while (NORINTSTS0 & (1<<0));
	
	Debug("Command error, ERRINTSTS = 0x%x ", errorState);	
	return errorState; // ����ͳ���	
}

static int s5pHsmmcWaitForTransferDone(void)
{
	int errorState;
	int i;
	
	// �ȴ����ݴ������
	for (i = 0; i < 0x20000000; i++)
	{
		if (NORINTSTS0 & ( 1<< 15))		// ���ִ���
		{
			break;
		}											
		if (NORINTSTS0 & (1 << 1))		// ���ݴ�����	
		{							
			do
			{
				NORINTSTS0 |= (1 << 1); // ����������λ
			} 
			while (NORINTSTS0 & (1 << 1));									
			return 0;
		}
	}

	errorState = ERRINTSTS0 & 0x1ff; // ����ͨ�Ŵ���,CRC�������,��ʱ��
	NORINTSTS0 = NORINTSTS0; // ����жϱ�־
	ERRINTSTS0 = ERRINTSTS0; // ��������жϱ�־
	Debug("Transfer error, rHM1_ERRINTSTS = 0x%04x\r\n", errorState);	
	
	do
	{
		NORINTSTS0 = (1 << 1); // ���������������λ
	}
	while (NORINTSTS0 & (1 << 1));
	
	return errorState; // ���ݴ������		
}

static int s5pHsmmcWaitForBufferReadReady(void)
{	
	int errorState;
	
	while (1)
	{
		if (NORINTSTS0 & (1 << 15))		// ���ִ���
		{
			break;
		}
		if (NORINTSTS0 & (1 << 5))		// ������׼����
		{
			NORINTSTS0 |= (1 << 5);		// ���׼���ñ�־
			return 0;
		}				
	}
			
	errorState =ERRINTSTS0 & 0x1ff; // ����ͨ�Ŵ���,CRC�������,��ʱ��
	NORINTSTS0 = NORINTSTS0; 		// ����жϱ�־	
	ERRINTSTS0 = ERRINTSTS0; 		// ��������жϱ�־
	Debug("Read buffer error, NORINTSTS: %04x\r\n", errorState);
	return errorState;
}

static int s5pHsmmcWaitForBufferWriteReady(void)
{
	int  errorState;
	while (1)
	{
		if (NORINTSTS0 & (1 << 15))		// ���ִ���
		{
			break;
		}
		if (NORINTSTS0 & (1 << 4))		// д����׼����
		{
			NORINTSTS0 |= (1 << 4);		// ���׼���ñ�־
			return 0;
		}				
	}
			
	errorState = ERRINTSTS0 & 0x1ff;	// ����ͨ�Ŵ���,CRC�������,��ʱ��
	NORINTSTS0 = NORINTSTS0;			// ����жϱ�־
	ERRINTSTS0 = ERRINTSTS0;			// ��������жϱ�־
	Debug("Write buffer error, NORINTSTS: %04x\r\n", errorState);
	
	return errorState;
}

static int s5pHsmmcIssueCommand(unsigned char cmd, unsigned int arg,
												unsigned char data, unsigned char response)
{
	unsigned int i;
	unsigned int value;
	unsigned int errorState;

	// �����ʱ,�����������Ƿ��ѱ�ʹ��,������Ҫ�ȴ����ڷ��͵����������ܷ����������
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
		return -1; // ���ʱ
	}

	// �������ظ����æ�ź�(��R1b�ظ�),�������������Ƿ��ѱ�ʹ��
	// ����,��ȴ������߿���,��æ�ظ�����ͺ�,sd��������DAT[0]�߱���sd����æ,�����߲�����
	if (response == CMD_RESP_R1B)	 // R1b�ظ�ͨ��DAT0����æ�ź�
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

	ARGUMENT0 = arg;		// ���������д��ARGUMENT����Ĵ�����

	value = (cmd << 8);		// command index ��CMDREG����������ֵ[13:8]
	
	// CMD12����ֹ����
	if (cmd == 0x12)
	{
		value |= (0x3 << 6); // command type
	}

	// �����Ƿ���ʹ��data��,����,��д������ͺ�
	// ���������data���ϴ�������,�������贫�����ݵ����Ҫ����ʹ��data��CMDREG[5]
	if (data)
	{
		value |= (1 << 5); // ��ʹ��DAT����Ϊ�����
	}	

	// ����sd���Ļظ�����,���󲿷�������sd����ȷ��Ӧ��,������������лظ�(R1-R7��R1b)
	// ÿ�������Ӧ�Ļظ�������ο�sd���淶,�ظ����ͳ��ȿ���Ϊ136��48
	// �ظ����Ƿ����CRC������ֵ�ķ���,�������,����������������ظ�����Ӧ��CRC������ֵ�����Ƿ���ȷ��ȷ��������ȷ
	// CMDREG���úú�,���������ͻᷢ����������趨���ȵĻظ��������趨���CRC,����ֵ�����Ƿ���ȷ(���ظ��а���CRC������ֵ�����Ļ�)
	switch (response)
	{
		case CMD_RESP_NONE:
			value |= (0 << 4) | (0 << 3) | 0x0; // û�лظ�,��������CRC
			break;
		case CMD_RESP_R1:
		case CMD_RESP_R5:
		case CMD_RESP_R6:
		case CMD_RESP_R7:		
			value |= (1 << 4) | (1 << 3) | 0x2; // ���ظ��е�����,CRC
			break;
		case CMD_RESP_R2:
			value |= (0 << 4) | (1 << 3) | 0x1; // �ظ�����Ϊ136λ,����CRC
			break;
		case CMD_RESP_R3:
		case CMD_RESP_R4:
			value |= (0 << 4) | (0 << 3) | 0x2; // �ظ�����48λ,���������CRC
			break;
		case CMD_RESP_R1B:
			value |= (1 << 4) | (1 << 3) | 0x3; // �ظ���æ�ź�,��ռ��Data[0]��
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
	
	return errorState; // ����ͳ���
}

// R1�ظ��а�����32λ��card state,��ʶ���,������һ״̬ͨ��CMD13��ÿ�״̬
static int s5pHsmmcGetCardState(void)
{
	// ͨ������CMD13���Ŀǰ����״̬,���,��дʱ������transfer̬,��Ȼ��ȴ�״̬ת�������
	if (s5pHsmmcIssueCommand(CMD13, RCA << 16, 0, CMD_RESP_R1))
	{
		return -1; // ������
	}
	else
	{
		return ((RSPREG0_0 >> 9) & 0xf); // ����R1�ظ��е�[12:9]��״̬
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
	BLKSIZE0 = (7 << 12) | (8 << 0); // ���DMA�����С,blockΪ64λ8�ֽ�			
	BLKCNT0 = 1; // д����ζ�1 block��sd״̬����
	ARGUMENT0 = 0; // д���������	

	// DMA����,������		
	TRNMOD0 = (0 << 5) | (1 << 4) | (0 << 2) | (1 << 1) | (0 << 0);	
	
	// ��������Ĵ���,read SD Configuration CMD51,R1�ظ�
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
		return state;	// δʶ��Ŀ�
	}
	
	// ȡ����ѡ��,�κο������ظ�,��ѡ��Ŀ�ͨ��RCA=0ȡ��ѡ��,
	// ���ص�stand-by״̬
	s5pHsmmcIssueCommand(CMD7, 0, 0, CMD_RESP_NONE);
	for (i = 0; i < 1000; i++)
	{
		if (s5pHsmmcGetCardState() == CARD_STBY)	// CMD9��������standy-by status
		{
			break;		// ״̬��ȷ
		}
		delay(US * 100);
	}
	if (i == 1000)
	{
		return state;	// ״̬����
	}	
	
	// �����ѱ�ǿ����Ϳ��ض�����(CSD),��ÿ���Ϣ
	if (!s5pHsmmcIssueCommand(CMD9, RCA << 16, 0, CMD_RESP_R2))
	{
		pCSD++;		// ������һ�ֽ�,CSD��[127:8]λ��λ�Ĵ����е�[119:0]
		response[0] = RSPREG0_0;
		response[1] = RSPREG0_1;
		response[2] = RSPREG0_2;
		response[3] = RSPREG0_3;
		for (i = 0; i < 15; i++)	// �����ظ��Ĵ����е�[119:0]��pCSD��
		{
			*pCSD++ = ((unsigned char *)response)[i];
		}	
		state = 0;		// CSD��ȡ�ɹ�
	}

	s5pHsmmcIssueCommand(CMD7, RCA << 16, 0, CMD_RESP_R1);	// ѡ��,���ص�transfer״̬
	
	return state;
}

static int s5pHsmmcSwitch(unsigned char mode, int group, int function, unsigned char *pStatus)
{
	int errorState;
	int temp;
	int i;
	
	BLKSIZE0 = (7 << 12) | (64 << 0); // ���DMA�����С,blockΪ512λ64�ֽ�			
	BLKCNT0 = 1; // д����ζ�1 block��sd״̬����
	temp = (mode << 31U) | 0xffffff;
	temp &= ~(0xf << (group * 4));
	temp |= function << (group * 4);
	ARGUMENT0 = temp; // д���������	

	// DMA����,������		
	TRNMOD0 = (0 << 5) | (1 << 4) | (0 << 2) | (1 << 1) | (0 << 0);	
	
	// ��������Ĵ���,SWITCH_FUNC CMD6,R1�ظ�
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
	
	state = -1;		// ���ó�ʼΪδ�ɹ�
	NORINTSTSEN0 &= ~(1 << 8);		// �رտ��ж�

	// sd����ʼ����Ĭ����1�߿�
	// ���ݴ����������12.5MB/s(25M,4��)��25MB/s(50M,4��)
	// ͨ��ACMD6(CMD55+CMD6)������sdģʽ��λ��
	s5pHsmmcIssueCommand(CMD55, RCA << 16, 0, CMD_RESP_R1);
	
	if (width == 1)		// 1λ��
	{
		if (!s5pHsmmcIssueCommand(CMD6, 0, 0, CMD_RESP_R1))
		{
			HOSTCTL0 &= ~(1 << 1);
			state = 0; // ����ɹ�
		}
	}
	else		// 4λ��
	{
		if (!s5pHsmmcIssueCommand(CMD6, 2, 0, CMD_RESP_R1))
		{
			HOSTCTL0 |= (1 << 1);
			state = 0;		// ����ɹ�
		}
	}
	
	NORINTSTSEN0 |= (1 << 8);	// �򿪿��ж�	
	
	return state; // ����0Ϊ�ɹ�
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
	
	// ����IO����ΪSDIO����
	GPG0CON = 0x2222222;

	// ��ֹ������
	GPG0PUD = 0;
//	GPG0PUD = 0x2AAAA;
	
	// ʱ�ӿ��ƼĴ�������
//	CLKCON0 = (0x80 << 8) + (1 << 2) + (1 << 0);

	// ���޼Ĵ�������  
//    CAPAREG0 = CAPAREG0_V18 + CAPAREG0_V30 + CAPAREG0_V33 + CAPAREG0_SUSRES + CAPAREG0_DMA + CAPAREG0_HSPD + CAPAREG0_ADMA2 +\  
//                    CAPAREG0_MAXBLKLEN + CAPAREG0_BASECLK + CAPAREG0_TOUTUNIT + CAPAREG0_TOUTCLK;  
	// ����ʱ��Դ:channel 0 clock src = SCLKEPLL = 96M
	CLK_SRC4 = (CLK_SRC4 & (~(0xf<<0))) | (0x7<<0);
	
	// ʱ�ӷ�Ƶ:channel 0 clock = SCLKEPLL/2 = 48M
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

	// ����sd��ʱ����100k~400k,sd����ʶ��׶α���������ʱ�ӽ��з���
	s5pHsmmcSetClock(400000);	// 400k

	TIMEOUTCON0 = 0xe;			// ���ʱʱ��
	HOSTCTL0 &= ~(1 << 2);		// �����ٶ�ģʽ
	NORINTSTS0 = NORINTSTS0;	// ��������ж�״̬��־
	ERRINTSTS0 = ERRINTSTS0;	// ��������ж�״̬��־
	NORINTSTSEN0 = 0x7fff;		// [14:0]�ж�״̬ʹ��
	ERRINTSTSEN0 = 0x7ff;		// [9:0]�����ж�״̬ʹ��
	NORINTSIGEN0 = 0x7fff;		// [14:0]�ж��ź�ʹ��	
	ERRINTSIGEN0 = 0x3ff;		// [9:0]�����ж��ź�ʹ��

	s5pHsmmcIssueCommand(CMD0, 0, 0, CMD_RESP_NONE); // ��λ���п�������״̬

	cardType = UNUSABLE; // �����ͳ�ʼ��������

	// ����CMD8����鿨�Ƿ�֧��������ѹ(2.7v~3.3v)
	// ���������sd2.0���ϲű�����,��û���յ��ظ��ź�,�����Ϊsd1.0��MMC��
	// �����յ����ظ��ź�,˵��Ϊsd2.0�汾��
	// û�лظ�,MMC/sd v1.x/not card
	if (s5pHsmmcIssueCommand(CMD8, 0x1aa, 0, CMD_RESP_R7))
	{
		for (i = 0; i < 100; i++)
		{
			// ��һ������ACMD41(CMD55+CMD41),����HCSλΪ0(�Ǹ�������)
			s5pHsmmcIssueCommand(CMD55, 0, 0, CMD_RESP_R1);

			// �ӻظ���ȷ��sd����׼����,����ȷ������sd1.x�汾�Ŀ�
			// ���ظ��б���sd��δ׼����,�����ظ�����ACMD41�ȴ���׼����
			if (!s5pHsmmcIssueCommand(CMD41, 0, 0, CMD_RESP_R3))
			{
				OCR = RSPREG0_0; 		// ��ûظ���OCR(���������Ĵ���)ֵ
				if (OCR & 0x80000000)	// ���ϵ��Ƿ�����ϵ�����,�Ƿ�busy
				{
					cardType = SD_V1;	// ��ȷʶ���sd v1.x��
					Debug("SD card version 1.x is detected\r\n");
					break;
				}
			}
			else
			{
				// ���û�лظ��ź�,˵����MMC������������ʶ��Ŀ�
				// �ɽ�һ������CMD1ȷ���Ƿ�MMC��
				Debug("MMC card is not supported\r\n");
				return -1;
			}
			delay(US * 1000);				
		}
	} 
	else	// sd2.0�汾��
	{
		temp = RSPREG0_0;
		// �ӻظ���ȷ�����Ƿ����ڸõ�ѹ�¹���,��������Ϊ��������
		if (((temp & 0xff) == 0xaa) && (((temp >> 8) & 0xf) == 0x1))	// �жϿ��Ƿ�֧��2.7~3.3v��ѹ
		{
			OCR = 0;
			for (i = 0; i < 100; i++)
			{
				OCR |= (1 << 30);

				// ��һ������ACMD41(CMD55+CMD41),����HCSλΪ1��������֧�ָ������Ŀ�
				// ��������æ״̬,���ظ�����ACDM41,ֱ����׼����
				s5pHsmmcIssueCommand(CMD55, 0, 0, CMD_RESP_R1);
				s5pHsmmcIssueCommand(CMD41, OCR, 0, CMD_RESP_R3); // reday̬
				OCR = RSPREG0_0;
				if (OCR & 0x80000000)	// ���ϵ��Ƿ�����ϵ�����,�Ƿ�busy
				{
					if (OCR & (1 << 30))	// �жϿ�Ϊ��׼�����Ǹ�������
					{
						// ACMD41�ظ�׼���ú�,�ټ��ظ��е�CCSλ
						// ��λΪ1˵����sd2.0������sdhc��
						cardType = SD_HC; // ��������
						Debug("SDHC card is detected\r\n");
					}
					else
					{
						// ��Ϊ0,��˵��Ϊsd2.0��׼������
						cardType = SD_V2;
						Debug("SD version 2.0 standard card is detected\r\n");
					}
					break;
				}
				delay(US * 1000);
			}
		}
	}

	// ʶ���sd1.x,sd2.0��׼����sd2.0����������
	if (cardType == SD_HC || cardType == SD_V1 || cardType == SD_V2)
	{
		// ͨ��CMD2���󿨷�����CID(Card Identification),��ʱ������Identification̬
		s5pHsmmcIssueCommand(CMD2, 0, 0, CMD_RESP_R2);

		// ����Identification̬��,����CMD3���󿨷���һ��16λ�µ���Ե�ַ(RCA)
		// �Ժ������뿨֮��ĵ�Ե�ͨ�ž��������RCA��ַ������,��ʱ������Stand-by̬
		s5pHsmmcIssueCommand(CMD3, 0, 0, CMD_RESP_R6);
		
		RCA = (RSPREG0_0 >> 16) & 0xffff; // �ӿ��ظ��еõ�����Ե�ַ

		// ͨ������CMD7ѡ��,ʹ������transfer̬
		// ��Ϊ���Ĵ󲿷ֲ������,д,�����Ⱦ��������״̬�������е�
		// ��ʱ������ȫ׼���ý��ն�д������
		s5pHsmmcIssueCommand(CMD7, RCA<<16, 0, CMD_RESP_R1); // ѡ���ѱ�ǵĿ�,transfer״̬
		
		s5pHsmmcGetSCR(&SCR);
		if (SCR.SD_SPEC == 0)		// sd 1.0 - sd 1.01
		{
			// Version 1.0 doesn't support switching
			s5pHsmmcSetClock(24000000); // ����SDCLK = 48M/2 = 24M			
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
									s5pHsmmcSetClock(48000000); // ����SDCLK = 48M	
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
				s5pHsmmcSetClock(24000000); // ����SDCLK = 48M/2 = 24M
			}
		}
			
		if (!s5pHsmmcSetBusWidth(4))
		{
			Debug("Set bus width error\r\n");
			return -1; // λ�����ó���
		}
		
		if (s5pHsmmcGetCardState() == CARD_TRAN)	// ��ʱ��Ӧ��transfer̬
		{
			// ���ڱ�׼��,��ͨ��CMD16�����ÿ�����(����,��д)��������ĳ���(���ֽ�����)��ʵ���ֽڵĶ�д
			// �����ڸ�������������������,��������һ����ĳ��Ⱦ��ǹ̶�512�ֽڵ�
			// ͨ��ͨ��CMD16���ÿ鳤��Ϊ512�ֽ�
			if (!s5pHsmmcIssueCommand(CMD16, 512, 0, CMD_RESP_R1))	// ���ÿ鳤��Ϊ512�ֽ�
			{
				NORINTSTS0 = 0xffff;		// ����жϱ�־
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
					
					// ������Ϊ�ֽ�(c_size+1)*512K byte,��1����512 byte��,����������Ϊ		
					capacity = (c_size+1) << 10;	// block (512 byte)
				}
				
				Debug("Card Initialization succeed\r\n");				
				Debug("Capacity: %ldMB\r\n", capacity / (1024*1024 / 512));
				
				return 0;	// ��ʼ���ɹ�							
			}
		}
	}
	
	Debug("Card Initialization failed\r\n");
	
	return -1;	// �������쳣
}

int s5pHsmmcEraseBlock(unsigned int startBlock, unsigned int endBlock)
{
	int i;

	if (cardType == SD_V1 || cardType == SD_V2)
	{
		startBlock <<= 9;	// ��׼��Ϊ�ֽڵ�ַ
		endBlock <<= 9;
	}
	else if (cardType != SD_HC)
	{
		return -1;	// δʶ��Ŀ�
	}	

	s5pHsmmcIssueCommand(CMD32, startBlock, 0, CMD_RESP_R1);
	s5pHsmmcIssueCommand(CMD33, endBlock, 0, CMD_RESP_R1);
	if (!s5pHsmmcIssueCommand(CMD38, 0, 0, CMD_RESP_R1B))
	{
		for (i = 0; i < 0x10000; i++)
		{
			if (s5pHsmmcGetCardState() == CARD_TRAN)	// ������ɺ󷵻ص�transfer״̬
			{
				Debug("Erasing complete!\r\n");
				return 0;	// �����ɹ�				
			}
			delay(US * 1000);
		}		
	}		

	Debug("Erase block failed\r\n");
	
	return -1;	// ����ʧ��
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

	NORINTSTS0 = NORINTSTS0;	// ����жϱ�־
	ERRINTSTS0 = ERRINTSTS0;	// ��������жϱ�־	
	
	while (blockNumber > 0)
	{
		if (blockNumber <= MAX_BLOCK)
		{
			readBlock = blockNumber;	// ��ȡ�Ŀ���С��65536 Block
			blockNumber = 0;			// ʣ���ȡ����Ϊ0
		}
		else
		{
			readBlock = MAX_BLOCK;		// ��ȡ�Ŀ�������65536 Block,�ֶ�ζ�
			blockNumber -= readBlock;
		}

		// ����sd������������׼,��˳��д��������������Ӧ�ļĴ���		
		BLKSIZE0 = (7 << 12) | (512 << 0);	// ���DMA�����С,blockΪ512�ֽ�			
		BLKCNT0  = readBlock;				// д����ζ�block��Ŀ
		
		if (cardType == SD_HC)
		{
			address = blockAddr;			// SDHC��д���ַΪblock��ַ
		}
		else if (cardType == SD_V1 || cardType == SD_V2)
		{
			address = blockAddr << 9;		// ��׼��д���ַΪ�ֽڵ�ַ
		}

		blockAddr += readBlock;				// ��һ�ζ���ĵ�ַ
		ARGUMENT0 = address;				// д���������		
		
		if (readBlock == 1)
		{
			// ���ô���ģʽ,DMA����,������
			TRNMOD0 = (0 << 5) | (1 << 4) | (0 << 2) | (1 << 1) | (0 << 0);
			
			// ��������Ĵ���,�����CMD17,R1�ظ�
			CMDREG0 = (CMD17 << 8) | (1 << 5) | (1 << 4) | (1 << 3) | 0x2;
		}
		else
		{
			// ���ô���ģʽ,DMA����,�����			
			TRNMOD0 = (1 << 5) | (1 << 4) | (1 << 2) | (1 << 1) | (0 << 0);
			
			// ��������Ĵ���,����CMD18,R1�ظ�	
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

	return 0;	// ���п����
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
		return -1;	// ��������
	}	
	
	NORINTSTS0 = NORINTSTS0;	// ����жϱ�־
	ERRINTSTS0 = ERRINTSTS0;	// ��������жϱ�־

	while (blockNumber > 0)
	{	
		if (blockNumber <= MAX_BLOCK)
		{
			writeBlock = blockNumber;	// д��Ŀ���С��65536 Block
			blockNumber = 0;			// ʣ��д�����Ϊ0
		}
		else
		{
			writeBlock = MAX_BLOCK;		// д��Ŀ�������65536 Block,�ֶ��д
			blockNumber -= writeBlock;
		}
		
		if (writeBlock > 1)				// ���д,����ACMD23������Ԥ��������
		{
			s5pHsmmcIssueCommand(CMD55, RCA << 16, 0, CMD_RESP_R1);
			s5pHsmmcIssueCommand(CMD23, writeBlock, 0, CMD_RESP_R1);
		}

		// ����sd������������׼,��˳��д��������������Ӧ�ļĴ���			
		BLKSIZE0 = (7 << 12) | (512 << 0);	// ���DMA�����С,blockΪ512�ֽ�		
		BLKCNT0  = writeBlock;				// д��block��Ŀ	

		if (cardType == SD_HC)
		{
			address = blockAddr;	// SDHC��д���ַΪblock��ַ
		}
		else if (cardType == SD_V1 || cardType == SD_V2)
		{
			address = blockAddr << 9;	// ��׼��д���ַΪ�ֽڵ�ַ
		}

		blockAddr += writeBlock;		// ��һ��д��ַ
		ARGUMENT0 = address;			// д���������			

		// �����ض������������Ĵ��䷽ʽ,��DMA��������,ͨ��FIFO��������
		if (writeBlock == 1)
		{
			// ���ô���ģʽ,DMA����д����
			TRNMOD0 = (0 << 5) | (0 << 4) | (0 << 2) | (1 << 1) | (0 << 0);
			
			// ��������Ĵ���,����дCMD24,R1�ظ�
			CMDREG0 = (CMD24 << 8) | (1 << 5) | (1 << 4) | (1 << 3) | 0x2;			
		}
		else
		{
			// ���ô���ģʽ,DMA����д���
			TRNMOD0 = (1 << 5) | (0 << 4) | (1 << 2) | (1 << 1) | (0 << 0);
			
			// ��������Ĵ���,���дCMD25,R1�ظ�
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
			if (s5pHsmmcGetCardState() == CARD_TRAN)	// ����transfer status
			{
				break;	// ״̬��ȷ
			}
		}
		
		if (i == 0x10000000)
		{
			return -3;		// ״̬�����Programming��ʱ
		}		
	}
	
	return 0; // д����������
}

