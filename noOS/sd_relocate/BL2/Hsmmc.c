//#include "ProjectConfig.h"
#include "Hsmmc.h"
#include "main.h"
#define HSMMC_NUM       2

#if (HSMMC_NUM == 0)
#define HSMMC_BASE  (0xEB000000)
#elif (HSMMC_NUM == 1)
#define HSMMC_BASE  (0xEB100000)
#elif (HSMMC_NUM == 2)
#define HSMMC_BASE  (0xEB200000)
#elif (HSMMC_NUM == 3)
#define HSMMC_BASE  (0xEB300000)
#else
#error "Configure HSMMC: HSMMC0 ~ HSMM3(0 ~ 3)"
#endif

#define MAX_BLOCK  65535

#define SWRST_OFFSET 0x2F
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

void Delay_us(int count){

	delay(US*count);
//unsigned int t2 =100000*count;	// 0x1FFFFF;
	//while (t2>0)
	//{
	//t2--;
	//}

}

static uint8_t CardType; // ������
static uint32_t RCA; // ����Ե�ַ

static void Hsmmc_ClockOn(uint8_t On)
{
    uint32_t Timeout;
    if (On) {
        __REGw(HSMMC_BASE+CLKCON_OFFSET) |= (1<<2); // sdʱ��ʹ��
        Timeout = 1000; // Wait max 10 ms
        while (!(__REGw(HSMMC_BASE+CLKCON_OFFSET) & (1<<3))) {
            // �ȴ�SD���ʱ���ȶ�
            if (Timeout == 0) {
                return;
            }
            Timeout--;
            Delay_us(10);
        }
    } else {
        __REGw(HSMMC_BASE+CLKCON_OFFSET) &= ~(1<<2); // sdʱ�ӽ�ֹ
    }
}

static void Hsmmc_SetClock(uint32_t Clock)
{
    uint32_t Temp;
    uint32_t Timeout;
    uint32_t i;
    Hsmmc_ClockOn(0); // �ر�ʱ��   
    Temp = __REG(HSMMC_BASE+CONTROL2_OFFSET);
    // Set SCLK_MMC(48M) from SYSCON as a clock source  
    Temp = (Temp & (~(3<<4))) | (2<<4);
    Temp |= (1u<<31) | (1u<<30) | (1<<8);
    if (Clock <= 500000) {
        Temp &= ~((1<<14) | (1<<15));
        __REG(HSMMC_BASE+CONTROL3_OFFSET) = 0;
    } else {
        Temp |= ((1<<14) | (1<<15));
        __REG(HSMMC_BASE+CONTROL3_OFFSET) = (1u<<31) | (1<<23);
    }
    __REG(HSMMC_BASE+CONTROL2_OFFSET) = Temp;

    for (i=0; i<=8; i++) {
        if (Clock >= (48000000/(1<<i))) {
            break;
        }
    }
    Temp = ((1<<i) / 2) << 8; // clock div
    Temp |= (1<<0); // Internal Clock Enable
    __REGw(HSMMC_BASE+CLKCON_OFFSET) = Temp;
    Timeout = 1000; // Wait max 10 ms
    while (!(__REGw(HSMMC_BASE+CLKCON_OFFSET) & (1<<1))) {
        // �ȴ��ڲ�ʱ�����ȶ�
        if (Timeout == 0) {
            return;
        }
        Timeout--;
        Delay_us(10);
    }

    Hsmmc_ClockOn(1); // ʹ��ʱ��
}

static int32_t Hsmmc_WaitForBufferReadReady(void)
{   
    int32_t ErrorState;
    while (1) {
        if (__REGw(HSMMC_BASE+NORINTSTS_OFFSET) & (1<<15)) { // ���ִ���
            break;
        }
        if (__REGw(HSMMC_BASE+NORINTSTS_OFFSET) & (1<<5)) { // ������׼����
            __REGw(HSMMC_BASE+NORINTSTS_OFFSET) |= (1<<5); // ���׼���ñ�־
            return 0;
        }               
    }

    ErrorState = __REGw(HSMMC_BASE+ERRINTSTS_OFFSET) & 0x1ff; // ����ͨ�Ŵ���,CRC�������,��ʱ��
    __REGw(HSMMC_BASE+NORINTSTS_OFFSET) = __REGw(HSMMC_BASE+NORINTSTS_OFFSET); // ����жϱ�־    
    __REGw(HSMMC_BASE+ERRINTSTS_OFFSET) = __REGw(HSMMC_BASE+ERRINTSTS_OFFSET); // ��������жϱ�־
    printf("Read buffer error, NORINTSTS: %04x\r\n", ErrorState);
    return ErrorState;
}

static int32_t Hsmmc_WaitForBufferWriteReady(void)
{
    int32_t ErrorState;
    while (1) {
        if (__REGw(HSMMC_BASE+NORINTSTS_OFFSET) & (1<<15)) { // ���ִ���
            break;
        }
        if (__REGw(HSMMC_BASE+NORINTSTS_OFFSET) & (1<<4)) { // д����׼����
            __REGw(HSMMC_BASE+NORINTSTS_OFFSET) |= (1<<4); // ���׼���ñ�־
            return 0;
        }               
    }

    ErrorState = __REGw(HSMMC_BASE+ERRINTSTS_OFFSET) & 0x1ff; // ����ͨ�Ŵ���,CRC�������,��ʱ��
    __REGw(HSMMC_BASE+NORINTSTS_OFFSET) = __REGw(HSMMC_BASE+NORINTSTS_OFFSET); // ����жϱ�־
    __REGw(HSMMC_BASE+ERRINTSTS_OFFSET) = __REGw(HSMMC_BASE+ERRINTSTS_OFFSET); // ��������жϱ�־
    printf("Write buffer error, NORINTSTS: %04x\r\n", ErrorState);
    return ErrorState;
}

static int32_t Hsmmc_WaitForCommandDone(void)
{
    uint32_t i; 
    int32_t ErrorState;
    // �ȴ���������
    for (i=0; i<0x20000000; i++) {
        if (__REGw(HSMMC_BASE+NORINTSTS_OFFSET) & (1<<15)) { // ���ִ���
            break;
        }
        if (__REGw(HSMMC_BASE+NORINTSTS_OFFSET) & (1<<0)) {
            do {
                __REGw(HSMMC_BASE+NORINTSTS_OFFSET) = (1<<0); // ����������λ
            } while (__REGw(HSMMC_BASE+NORINTSTS_OFFSET) & (1<<0));             
            return 0; // ����ͳɹ�
        }
    }

    ErrorState = __REGw(HSMMC_BASE+ERRINTSTS_OFFSET) & 0x1ff; // ����ͨ�Ŵ���,CRC�������,��ʱ��
    __REGw(HSMMC_BASE+NORINTSTS_OFFSET) = __REGw(HSMMC_BASE+NORINTSTS_OFFSET); // ����жϱ�־
    __REGw(HSMMC_BASE+ERRINTSTS_OFFSET) = __REGw(HSMMC_BASE+ERRINTSTS_OFFSET); // ��������жϱ�־  
    do {
        __REGw(HSMMC_BASE+NORINTSTS_OFFSET) = (1<<0); // ����������λ
    } while (__REGw(HSMMC_BASE+NORINTSTS_OFFSET) & (1<<0));

    printf("Command error, ERRINTSTS = 0x%x ", ErrorState);  
    return ErrorState; // ����ͳ���    
}

static int32_t Hsmmc_WaitForTransferDone(void)
{
    int32_t ErrorState;
    uint32_t i;
    // �ȴ����ݴ������
    for (i=0; i<0x20000000; i++) {
        if (__REGw(HSMMC_BASE+NORINTSTS_OFFSET) & (1<<15)) { // ���ִ���
            break;
        }                                           
        if (__REGw(HSMMC_BASE+NORINTSTS_OFFSET) & (1<<1)) { // ���ݴ�����                                
            do {
                __REGw(HSMMC_BASE+NORINTSTS_OFFSET) |= (1<<1); // ����������λ
            } while (__REGw(HSMMC_BASE+NORINTSTS_OFFSET) & (1<<1));                                 
            return 0;
        }
    }

    ErrorState = __REGw(HSMMC_BASE+ERRINTSTS_OFFSET) & 0x1ff; // ����ͨ�Ŵ���,CRC�������,��ʱ��
    __REGw(HSMMC_BASE+NORINTSTS_OFFSET) = __REGw(HSMMC_BASE+NORINTSTS_OFFSET); // ����жϱ�־
    __REGw(HSMMC_BASE+ERRINTSTS_OFFSET) = __REGw(HSMMC_BASE+ERRINTSTS_OFFSET); // ��������жϱ�־
    printf("Transfer error, rHM1_ERRINTSTS = 0x%04x\r\n", ErrorState);   
    do {
        __REGw(HSMMC_BASE+NORINTSTS_OFFSET) = (1<<1); // ���������������λ
    } while (__REGw(HSMMC_BASE+NORINTSTS_OFFSET) & (1<<1));

    return ErrorState; // ���ݴ������        
}

static int32_t Hsmmc_IssueCommand(uint8_t Cmd, uint32_t Arg, uint8_t Data, uint8_t Response)
{
    uint32_t i;
    uint32_t Value;
    uint32_t ErrorState;
    // ���CMD���Ƿ�׼���÷�������
    for (i=0; i<0x1000000; i++) {
        if (!(__REG(HSMMC_BASE+PRNSTS_OFFSET) & (1<<0))) {
            break;
        }
    }
    if (i == 0x1000000) {
        printf("CMD line time out, PRNSTS: %04x\r\n", __REG(HSMMC_BASE+PRNSTS_OFFSET));
        return -1; // ���ʱ
    }

    // ���DAT���Ƿ�׼����
    if (Response == CMD_RESP_R1B) { // R1b�ظ�ͨ��DAT0����æ�ź�
        for (i=0; i<0x10000000; i++) {
            if (!(__REG(HSMMC_BASE+PRNSTS_OFFSET) & (1<<1))) {
                break;
            }       
        }
        if (i == 0x10000000) {
            printf("Data line time out, PRNSTS: %04x\r\n", __REG(HSMMC_BASE+PRNSTS_OFFSET));         
            return -2;
        }       
    }

    __REG(HSMMC_BASE+ARGUMENT_OFFSET) = Arg; // д���������

    Value = (Cmd << 8); // command index
    // CMD12����ֹ����
    if (Cmd == 0x12) {
        Value |= (0x3 << 6); // command type
    }
    if (Data) {
        Value |= (1 << 5); // ��ʹ��DAT����Ϊ�����
    }   

    switch (Response) {
    case CMD_RESP_NONE:
        Value |= (0<<4) | (0<<3) | 0x0; // û�лظ�,��������CRC
        break;
    case CMD_RESP_R1:
    case CMD_RESP_R5:
    case CMD_RESP_R6:
    case CMD_RESP_R7:       
        Value |= (1<<4) | (1<<3) | 0x2; // ���ظ��е�����,CRC
        break;
    case CMD_RESP_R2:
        Value |= (0<<4) | (1<<3) | 0x1; // �ظ�����Ϊ136λ,����CRC
        break;
    case CMD_RESP_R3:
    case CMD_RESP_R4:
        Value |= (0<<4) | (0<<3) | 0x2; // �ظ�����48λ,���������CRC
        break;
    case CMD_RESP_R1B:
        Value |= (1<<4) | (1<<3) | 0x3; // �ظ���æ�ź�,��ռ��Data[0]��
        break;
    default:
        break;  
    }

    __REGw(HSMMC_BASE+CMDREG_OFFSET) = Value;

    ErrorState = Hsmmc_WaitForCommandDone();
    if (ErrorState) {
        printf("Command = %d\r\n", Cmd);
    }   
    return ErrorState; // ����ͳ���
}

int32_t Hsmmc_Switch(uint8_t Mode, int32_t Group, int32_t Function, uint8_t *pStatus)
{
    int32_t ErrorState;
    int32_t Temp;
    uint32_t i;

    __REGw(HSMMC_BASE+BLKSIZE_OFFSET) = (7<<12) | (64<<0); // ���DMA�����С,blockΪ512λ64�ֽ�          
    __REGw(HSMMC_BASE+BLKCNT_OFFSET) = 1; // д����ζ�1 block��sd״̬����
    Temp = (Mode << 31U) | 0xffffff;
    Temp &= ~(0xf<<(Group * 4));
    Temp |= Function << (Group * 4);
    __REG(HSMMC_BASE+ARGUMENT_OFFSET) = Temp; // д��������� 

    // DMA����,������        
    __REGw(HSMMC_BASE+TRNMOD_OFFSET) = (0<<5) | (1<<4) | (0<<2) | (1<<1) | (0<<0);  
    // ��������Ĵ���,SWITCH_FUNC CMD6,R1�ظ�
    __REGw(HSMMC_BASE+CMDREG_OFFSET) = (CMD6<<8)|(1<<5)|(1<<4)|(1<<3)|0x2;
    ErrorState = Hsmmc_WaitForCommandDone();
    if (ErrorState) {
        printf("CMD6 error\r\n");
        return ErrorState;
    }

    ErrorState = Hsmmc_WaitForBufferReadReady();
    if (ErrorState) {
        return ErrorState;
    }

    pStatus += 64 - 1;
    for (i=0; i<64/4; i++) {
        Temp = __REG(HSMMC_BASE+BDATA_OFFSET);
        *pStatus-- = (uint8_t)Temp;
        *pStatus-- = (uint8_t)(Temp>>8);
        *pStatus-- = (uint8_t)(Temp>>16);
        *pStatus-- = (uint8_t)(Temp>>24);           
    }

    ErrorState = Hsmmc_WaitForTransferDone();
    if (ErrorState) {
        printf("Get sd status error\r\n");
        return ErrorState;
    }   
    return 0;   
}

// 512λ��sd����չ״̬λ
int32_t Hsmmc_GetSdState(uint8_t *pStatus)
{
    int32_t ErrorState;
    uint32_t Temp;
    uint32_t i;
    if (CardType == SD_HC || CardType == SD_V2 || CardType == SD_V1) {
        if (Hsmmc_GetCardState() != CARD_TRAN) { // ������transfer status
            return -1; // ��״̬����
        }       
        Hsmmc_IssueCommand(CMD55, RCA<<16, 0, CMD_RESP_R1);

        __REGw(HSMMC_BASE+BLKSIZE_OFFSET) = (7<<12) | (64<<0); // ���DMA�����С,blockΪ512λ64�ֽ�          
        __REGw(HSMMC_BASE+BLKCNT_OFFSET) = 1; // д����ζ�1 block��sd״̬����
        __REG(HSMMC_BASE+ARGUMENT_OFFSET) = 0; // д���������    

        // DMA����,������        
        __REGw(HSMMC_BASE+TRNMOD_OFFSET) = (0<<5) | (1<<4) | (0<<2) | (1<<1) | (0<<0);  
        // ��������Ĵ���,��״̬����CMD13,R1�ظ�
        __REGw(HSMMC_BASE+CMDREG_OFFSET) = (CMD13<<8)|(1<<5)|(1<<4)|(1<<3)|0x2;
        ErrorState = Hsmmc_WaitForCommandDone();
        if (ErrorState) {
            printf("CMD13 error\r\n");
            return ErrorState;
        }

        ErrorState = Hsmmc_WaitForBufferReadReady();
        if (ErrorState) {
            return ErrorState;
        }

        pStatus += 64 - 1;
        for (i=0; i<64/4; i++) {
            Temp = __REG(HSMMC_BASE+BDATA_OFFSET);
            *pStatus-- = (uint8_t)Temp;
            *pStatus-- = (uint8_t)(Temp>>8);
            *pStatus-- = (uint8_t)(Temp>>16);
            *pStatus-- = (uint8_t)(Temp>>24);           
        }

        ErrorState = Hsmmc_WaitForTransferDone();
        if (ErrorState) {
            printf("Get sd status error\r\n");
            return ErrorState;
        }

        return 0;
    }
    return -1; // ��sd��
}

// Reads the SD Configuration Register (SCR).
int32_t Hsmmc_Get_SCR(SD_SCR *pSCR)
{
    uint8_t *pBuffer;
    int32_t ErrorState;
    uint32_t Temp;
    uint32_t i;
    Hsmmc_IssueCommand(CMD55, RCA<<16, 0, CMD_RESP_R1); 
    __REGw(HSMMC_BASE+BLKSIZE_OFFSET) = (7<<12) | (8<<0); // ���DMA�����С,blockΪ64λ8�ֽ�         
    __REGw(HSMMC_BASE+BLKCNT_OFFSET) = 1; // д����ζ�1 block��sd״̬����
    __REG(HSMMC_BASE+ARGUMENT_OFFSET) = 0; // д���������    

    // DMA����,������        
    __REGw(HSMMC_BASE+TRNMOD_OFFSET) = (0<<5) | (1<<4) | (0<<2) | (1<<1) | (0<<0);  
    // ��������Ĵ���,read SD Configuration CMD51,R1�ظ�
    __REGw(HSMMC_BASE+CMDREG_OFFSET) = (CMD51<<8)|(1<<5)|(1<<4)|(1<<3)|0x2;
    ErrorState = Hsmmc_WaitForCommandDone();
    if (ErrorState) {
        printf("CMD51 error\r\n");
        return ErrorState;
    }

    ErrorState = Hsmmc_WaitForBufferReadReady();
    if (ErrorState) {
        return ErrorState;
    }

    // Wide width data (SD Memory Register)
    pBuffer = (uint8_t *)pSCR + sizeof(SD_SCR) - 1;
    for (i=0; i<8/4; i++) {
        Temp = __REG(HSMMC_BASE+BDATA_OFFSET);
        *pBuffer-- = (uint8_t)Temp;
        *pBuffer-- = (uint8_t)(Temp>>8);
        *pBuffer-- = (uint8_t)(Temp>>16);
        *pBuffer-- = (uint8_t)(Temp>>24);       
    }

    ErrorState = Hsmmc_WaitForTransferDone();
    if (ErrorState) {
        printf("Get SCR register error\r\n");
        return ErrorState;
    }

    return 0;
}

// Asks the selected card to send its cardspecific data
int32_t Hsmmc_Get_CSD(uint8_t *pCSD)
{
    uint32_t i;
    uint32_t Response[4];
    int32_t State = -1;

    if (CardType != SD_HC && CardType != SD_V1 && CardType != SD_V2) {
        return State; // δʶ��Ŀ�
    }
    // ȡ����ѡ��,�κο������ظ�,��ѡ��Ŀ�ͨ��RCA=0ȡ��ѡ��,
    // ���ص�stand-by״̬
    Hsmmc_IssueCommand(CMD7, 0, 0, CMD_RESP_NONE);
    for (i=0; i<1000; i++) {
        if (Hsmmc_GetCardState() == CARD_STBY) { // CMD9��������standy-by status                
            break; // ״̬��ȷ
        }
        Delay_us(100);
    }
    if (i == 1000) {
        return State; // ״̬����
    }   

    // �����ѱ�ǿ����Ϳ��ض�����(CSD),��ÿ���Ϣ
    if (!Hsmmc_IssueCommand(CMD9, RCA<<16, 0, CMD_RESP_R2)) {
        pCSD++; // ������һ�ֽ�,CSD��[127:8]λ��λ�Ĵ����е�[119:0]
        Response[0] = __REG(HSMMC_BASE+RSPREG0_OFFSET);
        Response[1] = __REG(HSMMC_BASE+RSPREG1_OFFSET);
        Response[2] = __REG(HSMMC_BASE+RSPREG2_OFFSET);
        Response[3] = __REG(HSMMC_BASE+RSPREG3_OFFSET); 
        for (i=0; i<15; i++) { // �����ظ��Ĵ����е�[119:0]��pCSD��
            *pCSD++ = ((uint8_t *)Response)[i];
        }   
        State = 0; // CSD��ȡ�ɹ�
    }

    Hsmmc_IssueCommand(CMD7, RCA<<16, 0, CMD_RESP_R1); // ѡ��,���ص�transfer״̬
    return State;
}

// R1�ظ��а�����32λ��card state,��ʶ���,������һ״̬ͨ��CMD13��ÿ�״̬
int32_t Hsmmc_GetCardState(void)
{
    if (Hsmmc_IssueCommand(CMD13, RCA<<16, 0, CMD_RESP_R1)) {
        return -1; // ������
    } else {
        return ((__REG(HSMMC_BASE+RSPREG0_OFFSET)>>9) & 0xf); // ����R1�ظ��е�[12:9]��״̬
    }
}

static int32_t Hsmmc_SetBusWidth(uint8_t Width)
{
    int32_t State;
    if ((Width != 1) || (Width != 4)) {
        return -1;
    }
    State = -1; // ���ó�ʼΪδ�ɹ�
    __REGw(HSMMC_BASE+NORINTSTSEN_OFFSET) &= ~(1<<8); // �رտ��ж�
    Hsmmc_IssueCommand(CMD55, RCA<<16, 0, CMD_RESP_R1);
    if (Width == 1) {
        if (!Hsmmc_IssueCommand(CMD6, 0, 0, CMD_RESP_R1)) { // 1λ��
            __REGb(HSMMC_BASE+HOSTCTL_OFFSET) &= ~(1<<1);
            State = 0; // ����ɹ�
        }
    } else {
        if (!Hsmmc_IssueCommand(CMD6, 2, 0, CMD_RESP_R1)) { // 4λ��
            __REGb(HSMMC_BASE+HOSTCTL_OFFSET) |= (1<<1);
            State = 0; // ����ɹ�
        }
    }
    __REGw(HSMMC_BASE+NORINTSTSEN_OFFSET) |= (1<<8); // �򿪿��ж�   
    return State; // ����0Ϊ�ɹ�
}

int32_t Hsmmc_EraseBlock(uint32_t StartBlock, uint32_t EndBlock)
{
    uint32_t i;

    if (CardType == SD_V1 || CardType == SD_V2) {
        StartBlock <<= 9; // ��׼��Ϊ�ֽڵ�ַ
        EndBlock <<= 9;
    } else if (CardType != SD_HC) {
        return -1; // δʶ��Ŀ�
    }   
    Hsmmc_IssueCommand(CMD32, StartBlock, 0, CMD_RESP_R1);
    Hsmmc_IssueCommand(CMD33, EndBlock, 0, CMD_RESP_R1);
    if (!Hsmmc_IssueCommand(CMD38, 0, 0, CMD_RESP_R1B)) {
        for (i=0; i<0x10000; i++) {
            if (Hsmmc_GetCardState() == CARD_TRAN) { // ������ɺ󷵻ص�transfer״̬
                printf("Erasing complete!\r\n");
                return 0; // �����ɹ�               
            }
            Delay_us(1000);         
        }       
    }       

    printf("Erase block failed\r\n");
    return -1; // ����ʧ��
}

int32_t Hsmmc_ReadBlock(uint8_t *pBuffer, uint32_t BlockAddr, uint32_t BlockNumber)
{
    uint32_t Address = 0;
    uint32_t ReadBlock;
    uint32_t i;
    uint32_t j;
    int32_t ErrorState;
    uint32_t Temp;


 printf("Hsmmc_ReadBlock\r\n");

    if (pBuffer == 0 || BlockNumber == 0) {
        return -1;
    }

    __REGw(HSMMC_BASE+NORINTSTS_OFFSET) = __REGw(HSMMC_BASE+NORINTSTS_OFFSET); // ����жϱ�־
    __REGw(HSMMC_BASE+ERRINTSTS_OFFSET) = __REGw(HSMMC_BASE+ERRINTSTS_OFFSET); // ��������жϱ�־  

    while (BlockNumber > 0) {
        if (BlockNumber <= MAX_BLOCK) {
            ReadBlock = BlockNumber; // ��ȡ�Ŀ���С��65536 Block
            BlockNumber = 0; // ʣ���ȡ����Ϊ0
        } else {
            ReadBlock = MAX_BLOCK; // ��ȡ�Ŀ�������65536 Block,�ֶ�ζ�
            BlockNumber -= ReadBlock;
        }
        // ����sd������������׼,��˳��д��������������Ӧ�ļĴ���     
        __REGw(HSMMC_BASE+BLKSIZE_OFFSET) = (7<<12) | (512<<0); // ���DMA�����С,blockΪ512�ֽ�            
        __REGw(HSMMC_BASE+BLKCNT_OFFSET) = ReadBlock; // д����ζ�block��Ŀ

        if (CardType == SD_HC) {
            Address = BlockAddr; // SDHC��д���ַΪblock��ַ
        } else if (CardType == SD_V1 || CardType == SD_V2) {
            Address = BlockAddr << 9; // ��׼��д���ַΪ�ֽڵ�ַ
        }   
        BlockAddr += ReadBlock; // ��һ�ζ���ĵ�ַ
        __REG(HSMMC_BASE+ARGUMENT_OFFSET) = Address; // д���������      

        if (ReadBlock == 1) {
            // ���ô���ģʽ,DMA����,������
            __REGw(HSMMC_BASE+TRNMOD_OFFSET) = (0<<5) | (1<<4) | (0<<2) | (1<<1) | (0<<0);
            // ��������Ĵ���,�����CMD17,R1�ظ�
            __REGw(HSMMC_BASE+CMDREG_OFFSET) = (CMD17<<8)|(1<<5)|(1<<4)|(1<<3)|0x2;
        } else {
            // ���ô���ģʽ,DMA����,�����         
            __REGw(HSMMC_BASE+TRNMOD_OFFSET) = (1<<5) | (1<<4) | (1<<2) | (1<<1) | (0<<0);
            // ��������Ĵ���,����CMD18,R1�ظ�    
            __REGw(HSMMC_BASE+CMDREG_OFFSET) = (CMD18<<8)|(1<<5)|(1<<4)|(1<<3)|0x2;         
        }   
        ErrorState = Hsmmc_WaitForCommandDone();
printf("Hsmmc_WaitForCommandDone  %d \r\n",ErrorState );

        if (ErrorState) {
            printf("Read Command error\r\n");
            return ErrorState;
        }   
        for (i=0; i<ReadBlock; i++) {
            ErrorState = Hsmmc_WaitForBufferReadReady();
           printf("Hsmmc_WaitForBufferReadReady %d \r\n",ErrorState );

            if (ErrorState) {
                return ErrorState;
            }
            if (((uint32_t)pBuffer & 0x3) == 0) {   
                for (j=0; j<512/4; j++) {
                    *(uint32_t *)pBuffer = __REG(HSMMC_BASE+BDATA_OFFSET);
                    pBuffer += 4;
                 printf("*pBuffer 4 bytes %d \r\n",*pBuffer); 
                }
            } else {
                for (j=0; j<512/4; j++) {
                    Temp = __REG(HSMMC_BASE+BDATA_OFFSET);
                    *pBuffer++ = (uint8_t)Temp;
                    *pBuffer++ = (uint8_t)(Temp>>8);
                    *pBuffer++ = (uint8_t)(Temp>>16);
                    *pBuffer++ = (uint8_t)(Temp>>24);
                   printf("*pBuffer 1 byte %d \r\n",*pBuffer); 
                }
            }  
                   
        }

        ErrorState = Hsmmc_WaitForTransferDone();
         printf("Hsmmc_WaitForTransferDone %d \r\n",ErrorState );

        if (ErrorState) {
            printf("Read block error\r\n");
            return ErrorState;
        }   
    }
    return 0; // ���п����
}

int32_t Hsmmc_WriteBlock(uint8_t *pBuffer, uint32_t BlockAddr, uint32_t BlockNumber)
{
    uint32_t Address = 0;
    uint32_t WriteBlock;
    uint32_t i;
    uint32_t j;
    int32_t ErrorState;

    if (pBuffer == 0 || BlockNumber == 0) {
        return -1; // ��������
    }   

    __REGw(HSMMC_BASE+NORINTSTS_OFFSET) = __REGw(HSMMC_BASE+NORINTSTS_OFFSET); // ����жϱ�־
    __REGw(HSMMC_BASE+ERRINTSTS_OFFSET) = __REGw(HSMMC_BASE+ERRINTSTS_OFFSET); // ��������жϱ�־

    while (BlockNumber > 0) {   
        if (BlockNumber <= MAX_BLOCK) {
            WriteBlock = BlockNumber;// д��Ŀ���С��65536 Block
            BlockNumber = 0; // ʣ��д�����Ϊ0
        } else {
            WriteBlock = MAX_BLOCK; // д��Ŀ�������65536 Block,�ֶ��д
            BlockNumber -= WriteBlock;
        }
        if (WriteBlock > 1) { // ���д,����ACMD23������Ԥ��������
            Hsmmc_IssueCommand(CMD55, RCA<<16, 0, CMD_RESP_R1);
            Hsmmc_IssueCommand(CMD23, WriteBlock, 0, CMD_RESP_R1);
        }

        // ����sd������������׼,��˳��д��������������Ӧ�ļĴ���         
        __REGw(HSMMC_BASE+BLKSIZE_OFFSET) = (7<<12) | (512<<0); // ���DMA�����С,blockΪ512�ֽ�        
        __REGw(HSMMC_BASE+BLKCNT_OFFSET) = WriteBlock; // д��block��Ŀ 

        if (CardType == SD_HC) {
            Address = BlockAddr; // SDHC��д���ַΪblock��ַ
        } else if (CardType == SD_V1 || CardType == SD_V2) {
            Address = BlockAddr << 9; // ��׼��д���ַΪ�ֽڵ�ַ
        }
        BlockAddr += WriteBlock; // ��һ��д��ַ
        __REG(HSMMC_BASE+ARGUMENT_OFFSET) = Address; // д���������          

        if (WriteBlock == 1) {
            // ���ô���ģʽ,DMA����д����
            __REGw(HSMMC_BASE+TRNMOD_OFFSET) = (0<<5) | (0<<4) | (0<<2) | (1<<1) | (0<<0);
            // ��������Ĵ���,����дCMD24,R1�ظ�
            __REGw(HSMMC_BASE+CMDREG_OFFSET) = (CMD24<<8)|(1<<5)|(1<<4)|(1<<3)|0x2;         
        } else {
            // ���ô���ģʽ,DMA����д���
            __REGw(HSMMC_BASE+TRNMOD_OFFSET) = (1<<5) | (0<<4) | (1<<2) | (1<<1) | (0<<0);
            // ��������Ĵ���,���дCMD25,R1�ظ�
            __REGw(HSMMC_BASE+CMDREG_OFFSET) = (CMD25<<8)|(1<<5)|(1<<4)|(1<<3)|0x2;                 
        }
        ErrorState = Hsmmc_WaitForCommandDone();
        if (ErrorState) {
            printf("Write Command error\r\n");
            return ErrorState;
        }   

        for (i=0; i<WriteBlock; i++) {
            ErrorState = Hsmmc_WaitForBufferWriteReady();
            if (ErrorState) {
                return ErrorState;
            }
            if (((uint32_t)pBuffer & 0x3) == 0) {
                for (j=0; j<512/4; j++) {
                    __REG(HSMMC_BASE+BDATA_OFFSET) = *(uint32_t *)pBuffer;
                    pBuffer += 4;
                }
            } else {
                for (j=0; j<512/4; j++) {
                    __REG(HSMMC_BASE+BDATA_OFFSET) = pBuffer[0] + ((uint32_t)pBuffer[1]<<8) +
                    ((uint32_t)pBuffer[2]<<16) + ((uint32_t)pBuffer[3]<<24);
                    pBuffer += 4;
                }
            }   
        }

        ErrorState = Hsmmc_WaitForTransferDone();
 printf("Write block ErrorState %d \r\n",ErrorState );
        if (ErrorState) {
            printf("Write block error\r\n");
            return ErrorState;
        }
        for (i=0; i<0x10000000; i++) {
            if (Hsmmc_GetCardState() == CARD_TRAN) { // ����transfer status
                break; // ״̬��ȷ
            }
        }
        if (i == 0x10000000) {
            return -3; // ״̬�����Programming��ʱ
        }       
    }
    return 0; // д����������
}

int Hsmmc_Init(void)
{
buzzer_on();

    int32_t Timeout;
    uint32_t Capacity;
    uint32_t i;
    uint32_t OCR;
    uint32_t Temp;
    uint8_t SwitchStatus[64];
    SD_SCR SCR;
    uint8_t CSD[16];
    uint32_t c_size, c_size_multi, read_bl_len; 
  
    // ����HSMMC�Ľӿ���������
#if (HSMMC_NUM == 0)
    // channel 0,GPG0[0:6] = CLK, CMD, CDn, DAT[0:3]
    GPG0CON_REG = 0x2222222;
    // pull up enable
    GPG0PUD_REG = 0x2aaa;
    GPG0DRV_REG = 0x3fff;
    // channel 0 clock src = SCLKEPLL = 96M
    CLK_SRC4_REG = (CLK_SRC4_REG & (~(0xf<<0))) | (0x7<<0);
    // channel 0 clock = SCLKEPLL/2 = 48M
    CLK_DIV4_REG = (CLK_DIV4_REG & (~(0xf<<0))) | (0x1<<0);    

#elif (HSMMC_NUM == 1)
    // channel 1,GPG1[0:6] = CLK, CMD, CDn, DAT[0:3]
    GPG1CON_REG = 0x2222222;
    // pull up enable
    GPG1PUD_REG = 0x2aaa;
    GPG1DRV_REG = 0x3fff;
    // channel 1 clock src = SCLKEPLL = 96M
    CLK_SRC4_REG = (CLK_SRC4_REG & (~(0xf<<4))) | (0x7<<4);
    // channel 1 clock = SCLKEPLL/2 = 48M
    CLK_DIV4_REG = (CLK_DIV4_REG & (~(0xf<<4))) | (0x1<<4);

#elif (HSMMC_NUM == 2)
    // channel 2,GPG2[0:6] = CLK, CMD, CDn, DAT[0:3]
    GPG2CON_REG = 0x2222222;
    // pull up enable
    GPG2PUD_REG = 0x2aaa;
    GPG2DRV_REG = 0x3fff;
    // channel 2 clock src = SCLKEPLL = 96M
    CLK_SRC4_REG = (CLK_SRC4_REG & (~(0xf<<8))) | (0x7<<8);
    // channel 2 clock = SCLKEPLL/2 = 48M
    CLK_DIV4_REG = (CLK_DIV4_REG & (~(0xf<<8))) | (0x1<<8);    

#elif (HSMMC_NUM == 3)
    // channel 3,GPG3[0:6] = CLK, CMD, CDn, DAT[0:3]
    GPG3CON_REG = 0x2222222;
    // pull up enable
    GPG3PUD_REG = 0x2aaa;
    GPG3DRV_REG = 0x3fff;
    // channel 3 clock src = SCLKEPLL = 96M
    CLK_SRC4_REG = (CLK_SRC4_REG & (~(0xf<<12))) | (0x7<<12);
    // channel 3 clock = SCLKEPLL/2 = 48M
    CLK_DIV4_REG = (CLK_DIV4_REG & (~(0xf<<12))) | (0x1<<12);  

#endif
    // software reset for all ��λ����SoC�������������Ǹ�λSD��
    __REGb(HSMMC_BASE+SWRST_OFFSET) = 0x1;
    Timeout = 1000; // Wait max 10 ms
    while (__REGb(HSMMC_BASE+SWRST_OFFSET) & (1<<0)) {
        if (Timeout == 0) {
            return -1; // reset timeout
        }
        Timeout--;
        Delay_us(10);
    }   
printf("sd clock");
    // �������õ���SoC��SD��������ʱ�ӣ��������õ���SD����ʱ��
    Hsmmc_SetClock(400000); // 400k
    __REGb(HSMMC_BASE+TIMEOUTCON_OFFSET) = 0xe; // ���ʱʱ��
    __REGb(HSMMC_BASE+HOSTCTL_OFFSET) &= ~(1<<2); // �����ٶ�ģʽ
    // ��������ж�״̬��־
    __REGw(HSMMC_BASE+NORINTSTS_OFFSET) = __REGw(HSMMC_BASE+NORINTSTS_OFFSET);
    // ��������ж�״̬��־
    __REGw(HSMMC_BASE+ERRINTSTS_OFFSET) = __REGw(HSMMC_BASE+ERRINTSTS_OFFSET);
    __REGw(HSMMC_BASE+NORINTSTSEN_OFFSET) = 0x7fff; // [14:0]�ж�״̬ʹ��
    __REGw(HSMMC_BASE+ERRINTSTSEN_OFFSET) = 0x3ff; // [9:0]�����ж�״̬ʹ��
    __REGw(HSMMC_BASE+NORINTSIGEN_OFFSET) = 0x7fff; // [14:0]�ж��ź�ʹ�� 
    __REGw(HSMMC_BASE+ERRINTSIGEN_OFFSET) = 0x3ff; // [9:0]�����ж��ź�ʹ��

    // �����￪ʼ��SD��ͨ�ţ�ͨ����ʵ���Ƿ�����Ȼ������Ӧ
    Hsmmc_IssueCommand(CMD0, 0, 0, CMD_RESP_NONE); // ��λ���п�������״̬    

    CardType = UNUSABLE; // �����ͳ�ʼ��������
    if (Hsmmc_IssueCommand(CMD8, 0x1aa, 0, CMD_RESP_R7)) { // û�лظ�,MMC/sd v1.x/not card
            for (i=0; i<100; i++) {
                // CMD55 + CMD41 = ACMD41
                Hsmmc_IssueCommand(CMD55, 0, 0, CMD_RESP_R1);
                if (!Hsmmc_IssueCommand(CMD41, 0, 0, CMD_RESP_R3)) { // CMD41�лظ�˵��Ϊsd��
                    OCR = __REG(HSMMC_BASE+RSPREG0_OFFSET); // ��ûظ���OCR(���������Ĵ���)ֵ
                    if (OCR & 0x80000000) { // ���ϵ��Ƿ�����ϵ�����,�Ƿ�busy
                        CardType = SD_V1; // ��ȷʶ���sd v1.x��
                        printf("SD card version 1.x is detected\r\n");
                        break;
                    }
                } else {
                    // MMC��ʶ��
                    printf("MMC card is not supported\r\n");
                    return -1;
                }
                Delay_us(1000);             
            }
    } else { // sd v2.0
        Temp = __REG(HSMMC_BASE+RSPREG0_OFFSET);
        if (((Temp&0xff) == 0xaa) && (((Temp>>8)&0xf) == 0x1)) { // �жϿ��Ƿ�֧��2.7~3.3v��ѹ
            OCR = 0;
            for (i=0; i<100; i++) {
                OCR |= (1<<30);
                Hsmmc_IssueCommand(CMD55, 0, 0, CMD_RESP_R1);
                Hsmmc_IssueCommand(CMD41, OCR, 0, CMD_RESP_R3); // reday̬
                OCR = __REG(HSMMC_BASE+RSPREG0_OFFSET);
                if (OCR & 0x80000000) { // ���ϵ��Ƿ�����ϵ�����,�Ƿ�busy
                    if (OCR & (1<<30)) { // �жϿ�Ϊ��׼�����Ǹ�������
                        CardType = SD_HC; // ��������
                        printf("SDHC card is detected\r\n");
                    } else {
                        CardType = SD_V2; // ��׼��
                        printf("SD version 2.0 standard card is detected\r\n");
                    }
                    break;
                }
                Delay_us(1000);
            }
        }
    }
    if (CardType == SD_HC || CardType == SD_V1 || CardType == SD_V2) {
        Hsmmc_IssueCommand(CMD2, 0, 0, CMD_RESP_R2); // ���󿨷���CID(��ID�Ĵ���)��,����ident
        Hsmmc_IssueCommand(CMD3, 0, 0, CMD_RESP_R6); // ���󿨷����µ�RCA(����Ե�ַ),Stand-by״̬
        RCA = (__REG(HSMMC_BASE+RSPREG0_OFFSET) >> 16) & 0xffff; // �ӿ��ظ��еõ�����Ե�ַ
        Hsmmc_IssueCommand(CMD7, RCA<<16, 0, CMD_RESP_R1); // ѡ���ѱ�ǵĿ�,transfer״̬

        Hsmmc_Get_SCR(&SCR);
        if (SCR.SD_SPEC == 0) { // sd 1.0 - sd 1.01
            // Version 1.0 doesn't support switching
            Hsmmc_SetClock(24000000); // ����SDCLK = 48M/2 = 24M          
        } else { // sd 1.10 / sd 2.0
            Temp = 0;
            for (i=0; i<4; i++) {
                if (Hsmmc_Switch(0, 0, 1, SwitchStatus) == 0) { // switch check
                    if (!(SwitchStatus[34] & (1<<1))) { // Group 1, function 1 high-speed bit 273                   
                        // The high-speed function is ready
                        if (SwitchStatus[50] & (1<<1)) { // Group, function 1 high-speed support bit 401
                            // high-speed is supported
                            if (Hsmmc_Switch(1, 0, 1, SwitchStatus) == 0) { // switch
                                if ((SwitchStatus[47] & 0xf) == 1) { // function switch in group 1 is ok?
                                    // result of the switch high-speed in function group 1
                                    printf("Switch to high speed mode: CLK @ 50M\r\n");
                                    Hsmmc_SetClock(48000000); // ����SDCLK = 48M  
                                    Temp = 1;                                   
                                }
                            }
                        }
                        break;
                    }
                } 
            }
            if (Temp == 0) {
                Hsmmc_SetClock(24000000); // ����SDCLK = 48M/2 = 24M
            }
        }

        if (!Hsmmc_SetBusWidth(4)) {
            printf("Set bus width error\r\n");
            return -1; // λ�����ó���
        }
        if (Hsmmc_GetCardState() == CARD_TRAN) { // ��ʱ��Ӧ��transfer̬
            if (!Hsmmc_IssueCommand(CMD16, 512, 0, CMD_RESP_R1)) { // ���ÿ鳤��Ϊ512�ֽ�
                __REGw(HSMMC_BASE+NORINTSTS_OFFSET) = 0xffff; // ����жϱ�־
                Hsmmc_Get_CSD(CSD);
                if ((CSD[15]>>6) == 0) { // CSD v1.0->sd V1.x, sd v2.00 standar
                    read_bl_len = CSD[10] & 0xf; // [83:80]
                    c_size_multi = ((CSD[6] & 0x3) << 1) + ((CSD[5] & 0x80) >> 7); // [49:47]
                    c_size = ((int32_t)(CSD[9]&0x3) << 10) + ((int32_t)CSD[8]<<2) + (CSD[7]>>6); // [73:62]             
                    Capacity = (c_size + 1) << ((c_size_multi + 2) + (read_bl_len-9)); // block(512 byte)
                } else {
                    c_size = ((CSD[8]&0x3f) << 16) + (CSD[7]<<8) + CSD[6]; // [69:48]
                    // ������Ϊ�ֽ�(c_size+1)*512K byte,��1����512 byte��,����������Ϊ      
                    Capacity = (c_size+1) << 10;// block (512 byte)
                }
                printf("Card Initialization succeed\r\n");               
                printf("Capacity: %ldMB\r\n", Capacity / (1024*1024 / 512));
                return 0; // ��ʼ���ɹ�                          
            }
        }
    }
    printf("Card Initialization failed\r\n");
    return -1; // �������쳣
}
