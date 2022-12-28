#ifndef __S5PV210_SDIO_H__
#define __S5PV210_SDIO_H__
#include "regAddr.h"

#define	CMD0	0
#define	CMD1	1
#define	CMD2	2	
#define	CMD3	3	
#define	CMD6	6
#define	CMD7	7
#define	CMD8	8
#define	CMD9	9
#define	CMD13	13
#define	CMD16	16
#define	CMD17	17
#define	CMD18	18
#define	CMD23	23	
#define	CMD24	24
#define	CMD25	25	
#define	CMD32	32	
#define	CMD33	33	
#define	CMD38	38	
#define	CMD41	41	
#define CMD51	51
#define	CMD55	55	

// 卡类型
#define UNUSABLE		0
#define SD_V1			1	
#define	SD_V2			2	
#define	SD_HC			3
#define	MMC				4
	
// 卡状态
#define CARD_IDLE		0
#define CARD_READY		1
#define CARD_IDENT		2
#define CARD_STBY		3
#define CARD_TRAN		4
#define CARD_DATA		5
#define CARD_RCV		6
#define CARD_PRG		7
#define CARD_DIS		8

// 卡回复类型	
#define CMD_RESP_NONE	0
#define CMD_RESP_R1		1
#define CMD_RESP_R2		2
#define CMD_RESP_R3		3
#define CMD_RESP_R4		4
#define CMD_RESP_R5		5
#define CMD_RESP_R6		6
#define CMD_RESP_R7		7
#define CMD_RESP_R1B	8

#define  CLKCON_OFFSET 0x002C
#define CONTROL2_OFFSET  0x0080
#define  CONTROL3_OFFSET 0x0084
#define ARGUMENT_OFFSET 0x0008
#define  BLKSIZE_OFFSET  0x0004
#define  BLKCNT_OFFSET  0x0006
 #define TRNMOD_OFFSET 0x000C
#define CMDREG_OFFSET 0x000E
 #define  RSPREG0_OFFSET 0x0010
 #define  RSPREG1_OFFSET 0x0014
 #define  RSPREG2_OFFSET 0x0018
 #define  RSPREG3_OFFSET 0x001C
 #define BDATA_OFFSET  0x0020
#define PRNSTS_OFFSET  0x0024
#define NORINTSTS_OFFSET  0x0030
#define  ERRINTSTS_OFFSET 0x0032
#define HOSTCTL_OFFSET 0x0028
#define NORINTSTSEN_OFFSET 0x0034
#define ERRINTSTSEN_OFFSET  0x0036
#define NORINTSIGEN_OFFSET 0x0038
#define ERRINTSIGEN_OFFSET 0x003A
#define TIMEOUTCON_OFFSET 0x002E
#define CLK_SRC4_REG (*((volatile unsigned int *)0xE0100210))
#define CLK_DIV4_REG (*((volatile unsigned int *)0xE0100310))

#define  GPG2CON_REG (*((volatile unsigned int *)0xE02001E0))
#define  GPG2PUD_REG  (*((volatile unsigned int *)0xE02001E8))
#define  GPG2DRV_REG  (*((volatile unsigned int *)0xE02001EC))

#define  GPG3CON_REG (*((volatile unsigned int *)0xE0200200))
#define  GPG3PUD_REG  (*((volatile unsigned int *)0xE0200208))
#define  GPG3DRV_REG  (*((volatile unsigned int *)0xE020020C))

#define  GPG1CON_REG (*((volatile unsigned int *)0xE02001C0))
#define  GPG1PUD_REG  (*((volatile unsigned int *)0xE02001C8))
#define  GPG1DRV_REG  (*((volatile unsigned int *)0xE02001Cc))

#define  GPG0CON_REG (*((volatile unsigned int *) 0xE02001A0))
#define  GPG0PUD_REG  (*((volatile unsigned int *)0xE02001A8))
#define  GPG0DRV_REG  (*((volatile unsigned int *)0xE02001AC))

	
typedef struct {
	unsigned int RESERVED1;
	unsigned int RESERVED2 : 16;	
	unsigned int SD_BUS_WIDTHS : 4;
	unsigned int SD_SECURITY : 3;	
	unsigned int DATA_STAT_AFTER_ERASE : 1;
	unsigned int SD_SPEC : 4;	
	unsigned int SCR_STRUCTURE : 4;
} SD_SCR;

extern int s5pHsmmcInit(void);
extern int s5pHsmmcEraseBlock(unsigned int startBlock, unsigned int endBlock);
extern int s5pHsmmcReadBlock(unsigned char *pBuffer, unsigned int blockAddr, unsigned int blockNumber);
extern int s5pHsmmcwriteBlock(unsigned char *pBuffer, unsigned int blockAddr, unsigned int blockNumber);

#endif	/* __S5PV210_SDIO_H__ */

