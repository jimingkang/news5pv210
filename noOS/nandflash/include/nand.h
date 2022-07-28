#ifndef S5P_NAND_H
#define S5P_NAND_H

#include "cpu_base.h"

#define S5PV210_MP0_1CON        0xE02002E0
#define S5PV210_MP0_2CON        0xE0200300
#define S5PV210_MP0_3CON        0xE0200320

#define ELFIN_NAND_BASE			0xB0E00000
#define ELFIN_NAND_ECC_BASE		0xB0E20000

//#define ELFIN_HSMMC_0_BASE		0xEB000000
//#define ELFIN_HSMMC_1_BASE		0xEB100000
//#define ELFIN_HSMMC_2_BASE		0xEB200000
//#define ELFIN_HSMMC_3_BASE		0xEB300000


#define NFCONF_OFFSET           	0x00
#define NFCONT_OFFSET           	0x04
#define NFCMMD_OFFSET           	0x08
#define NFADDR_OFFSET           	0x0c
#define NFDATA_OFFSET			    0x10
#define NFMECCDATA0_OFFSET      	0x14
#define NFMECCDATA1_OFFSET      	0x18
#define NFSECCDATA0_OFFSET      	0x1c
#define NFSBLK_OFFSET           	0x20
#define NFEBLK_OFFSET           	0x24
#define NFSTAT_OFFSET           	0x28
#define NFESTAT0_OFFSET         	0x2c
#define NFESTAT1_OFFSET         	0x30
#define NFMECC0_OFFSET          	0x34
#define NFMECC1_OFFSET          	0x38
#define NFSECC_OFFSET           	0x3c
#define NFMLCBITPT_OFFSET       	0x40

#define NFCONF				(ELFIN_NAND_BASE+NFCONF_OFFSET)
#define NFCONT				(ELFIN_NAND_BASE+NFCONT_OFFSET)
#define NFCMMD				(ELFIN_NAND_BASE+NFCMMD_OFFSET)
#define NFADDR           		(ELFIN_NAND_BASE+NFADDR_OFFSET)
#define NFDATA          		(ELFIN_NAND_BASE+NFDATA_OFFSET)
#define NFMECCDATA0     		(ELFIN_NAND_BASE+NFMECCDATA0_OFFSET)
#define NFMECCDATA1     		(ELFIN_NAND_BASE+NFMECCDATA1_OFFSET)
#define NFSECCDATA0      		(ELFIN_NAND_BASE+NFSECCDATA0_OFFSET)
#define NFSBLK          		(ELFIN_NAND_BASE+NFSBLK_OFFSET)
#define NFEBLK           		(ELFIN_NAND_BASE+NFEBLK_OFFSET)
#define NFSTAT           		(ELFIN_NAND_BASE+NFSTAT_OFFSET)
#define NFESTAT0         		(ELFIN_NAND_BASE+NFESTAT0_OFFSET)
#define NFESTAT1         		(ELFIN_NAND_BASE+NFESTAT1_OFFSET)
#define NFMECC0          		(ELFIN_NAND_BASE+NFMECC0_OFFSET)
#define NFMECC1          		(ELFIN_NAND_BASE+NFMECC1_OFFSET)
#define NFSECC           		(ELFIN_NAND_BASE+NFSECC_OFFSET)
#define NFMLCBITPT           	(ELFIN_NAND_BASE+NFMLCBITPT_OFFSET)

#define NAND_CMD_NONE		-1

#define S5P_NFSTAT_READY	(1<<0)

#define S5P_NFCONT_nFCE3	(1<<23)
#define S5P_NFCONT_nFCE2	(1<<22)
#define S5P_NFCONT_nFCE1	(1<<2)
#define S5P_NFCONT_nFCE0	(1<<1)

/* Select the chip by setting nCE to low */
#define NAND_NCE		0x01
/* Select the command latch by setting CLE to high */
#define NAND_CLE		0x02
/* Select the address latch by setting ALE to high */
#define NAND_ALE		0x04

#define NAND_CTRL_CLE		(NAND_NCE | NAND_CLE)
#define NAND_CTRL_ALE		(NAND_NCE | NAND_ALE)
#define NAND_CTRL_CHANGE	0x80

/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0		0
#define NAND_CMD_READ1		1
#define NAND_CMD_RNDOUT		5
#define NAND_CMD_PAGEPROG	0x10
#define NAND_CMD_READOOB	0x50
#define NAND_CMD_ERASE1		0x60
#define NAND_CMD_STATUS		0x70
#define NAND_CMD_STATUS_MULTI	0x71
#define NAND_CMD_SEQIN		0x80
#define NAND_CMD_RNDIN		0x85
#define NAND_CMD_READID		0x90
#define NAND_CMD_ERASE2		0xd0
#define NAND_CMD_PARAM		0xec
#define NAND_CMD_RESET		0xff

#define NAND_CMD_LOCK		0x2a
#define NAND_CMD_UNLOCK1	0x23
#define NAND_CMD_UNLOCK2	0x24

/* Extended commands for large page devices */
#define NAND_CMD_READSTART	0x30
#define NAND_CMD_RNDOUTSTART	0xE0
#define NAND_CMD_CACHEDPROG	0x15

#define NAND_STATUS_FAIL	0x01
#define NAND_STATUS_FAIL_N1	0x02
#define NAND_STATUS_TRUE_READY	0x20
#define NAND_STATUS_READY	0x40
#define NAND_STATUS_WP		0x80

typedef unsigned char uint8_t; 
typedef long loff_t;

struct nand_chip {
	unsigned char (*read_byte)(void);
	void	(*select_chip)(int chip);
	int		(*block_bad)(loff_t ofs, int getchip);
	int		(*block_markbad)(loff_t ofs);
	void	(*cmd_ctrl)(int dat,unsigned int ctrl);
	int		(*dev_ready)(void);
	void	(*cmdfunc)(unsigned command, int column, int page_addr);
	int		(*waitfunc)(struct nand_chip *this);
};

extern void nand_lowlevel_init(void);
extern void nand_init_chip(void);
extern int nand_read(void *data,unsigned int start_addr,unsigned int size);
extern int erase_flash(unsigned int start_addr);
extern int nand_write(const void *data,unsigned int start_addr,unsigned int size);


//new for sd/mmc

#define rNFCONF             ( *((volatile unsigned long *)0xB0E00000) )
#define rNFCONT             ( *((volatile unsigned long *)0xB0E00004) )
#define rNFCMMD             ( *((volatile unsigned long *)0xB0E00008) )
#define rNFADDR             ( *((volatile unsigned long *)0xB0E0000C) )
#define rNFDATA             ( *((volatile unsigned long *)0xB0E00010) )
#define rNFDATA8            ( *((volatile unsigned char *)0xB0E00010) )
#define rNFSTAT             ( *((volatile unsigned long *)0xB0E00028) )

#define rMP0_1CON       ( *((volatile unsigned long *)0xE02002E0) )
#define rMP0_2CON       ( *((volatile unsigned long *)0xE0200300) )
#define rMP0_3CON       ( *((volatile unsigned long *)0xE0200320) )

#define MAX_NAND_BLOCK                8192          /*定义nand最大块数：8192块  */
#define NAND_PAGE_SIZE                2048          /*定义一页的容量:2048 byte     */
#define NAND_BLOCK_SIZE               64            /*定义block大小：64页     */

#define TACLS                         1     // 12ns     /* 时序相关的设置          */
#define TWRPH0                        4
#define TWRPH1                        1

#define NAND_CMD_READ_1st             0x00          /* 命令                       */
#define NAND_CMD_READ_2st             0x30

#define NAND_CMD_READ_CB_1st          0x00
#define NAND_CMD_READ_CB_2st          0x35

#define NAND_CMD_RANDOM_WRITE         0x85
#define NAND_CMD_RANDOM_READ_1st      0x05
#define NAND_CMD_RANDOM_READ_2st      0xe0

#define NAND_CMD_READ_ID              0x90
#define NAND_CMD_RESET                0xff
#define NAND_CMD_READ_STATUS          0x70

#define NAND_CMD_WRITE_PAGE_1st       0x80
#define NAND_CMD_WRITE_PAGE_2st       0x10

#define NAND_CMD_BLOCK_ERASE_1st      0x60
#define NAND_CMD_BLOCK_ERASE_2st      0xd0


#define ECC_EN                        (1<<4)
#define CONTROL_EN                    (1<<0)

void nand_init(void);
void nand_read_id(void);
int nand_block_erase(unsigned long block_num);
int copy_nand_to_sdram(unsigned char *sdram_addr, unsigned long nand_addr, unsigned long length);
int copy_sdram_to_nand(unsigned char *sdram_addr, unsigned long nand_addr, unsigned long length);
int nand_page_read(unsigned int pgaddr, unsigned char *buf, unsigned int length);
int nand_page_write(unsigned int pgaddr, const unsigned char *buf, unsigned int length);
int nand_random_read(unsigned long pgaddr,unsigned short offset, unsigned char *data);
int nand_random_write(unsigned long pgaddr,unsigned short offset,unsigned char wrdata);

#endif

