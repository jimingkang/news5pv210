/* linux/include/asm-arm/plat-s3c24xx/regs-s3c2412-iis.h
 *
 * Copyright 2007 Simtec Electronics <linux@simtec.co.uk>
 *	http://armlinux.simtec.co.uk/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * S3C2412 IIS register definition
*/

#ifndef __ASM_ARCH_REGS_S3C2412_IIS_H
#define __ASM_ARCH_REGS_S3C2412_IIS_H

#define S3C2412_IISCON			(0x00)
#define S3C2412_IISMOD			(0x04)
#define S3C2412_IISFIC			(0x08)
#define S3C2412_IISPSR			(0x0C)
#define S3C2412_IISTXD			(0x10)
#define S3C2412_IISRXD			(0x14)
#define S5P_IISFICS			(0x18)
#define S5P_IISTXDS			(0x1C)
#define S5P_IISAHB			(0x20)
#define S5P_IISSTR			(0x24)
#define S5P_IISSIZE			(0x28)
#define S5P_IISTRNCNT			(0x2C)
#define S5P_IISADDR0			(0x30)
#define S5P_IISADDR1			(0x34)
#define S5P_IISADDR2			(0x38)
#define S5P_IISADDR3			(0x3C)

#define S5P_IISCON_FTXSURSTAT		(1 << 24)
#define S5P_IISCON_FTXSURINTEN		(1 << 23)
#define S5P_IISCON_TXSDMAPAUSE		(1 << 20)
#define S5P_IISCON_TXSDMACTIVE		(1 << 18)
#define S5P_IISCON_FTXURSTATUS		(1 << 17)
#define S5P_IISCON_FTXURINTEN		(1 << 16)

#define S3C2412_IISCON_LRINDEX		(1 << 11)
#define S3C2412_IISCON_TXFIFO_EMPTY	(1 << 10)
#define S3C2412_IISCON_RXFIFO_EMPTY	(1 << 9)
#define S3C2412_IISCON_TXFIFO_FULL	(1 << 8)
#define S3C2412_IISCON_RXFIFO_FULL	(1 << 7)
#define S3C2412_IISCON_TXDMA_PAUSE	(1 << 6)
#define S3C2412_IISCON_RXDMA_PAUSE	(1 << 5)
#define S3C2412_IISCON_TXCH_PAUSE	(1 << 4)
#define S3C2412_IISCON_RXCH_PAUSE	(1 << 3)
#define S3C2412_IISCON_TXDMA_ACTIVE	(1 << 2)
#define S3C2412_IISCON_RXDMA_ACTIVE	(1 << 1)
#define S3C2412_IISCON_IIS_ACTIVE	(1 << 0)

#define S3C64XX_IISMOD_OPPCLK		(3<<30)

#define S5P_IISMOD_TXSLP		(1<<28)
#define S5P_IISMOD_BLCPMASK		(3<<24)
#define S5P_IISMOD_BLCSMASK		(3<<26)

#define S3C64XX_IISMOD_BLC_16BIT	(0 << 13)
#define S3C64XX_IISMOD_BLC_8BIT		(1 << 13)
#define S3C64XX_IISMOD_BLC_24BIT	(2 << 13)
#define S3C64XX_IISMOD_BLC_MASK		(3 << 13)

#define S3C64XX_IISMOD_IMSMASK		(3 << 10)
#define S3C64XX_IISMOD_IMS_PCLK		(0 << 10)
#define S3C64XX_IISMOD_IMS_SYSMUX	(1 << 10)
#define S3C64XX_CLKSRC_I2SEXT		(3 << 10)
#define S3C64XX_CDCLKSRC_EXT		(5 << 10)

#define S3C2412_IISMOD_MASTER_INTERNAL	(0 << 10)
#define S3C2412_IISMOD_MASTER_EXTERNAL	(1 << 10)
#define S3C2412_IISMOD_SLAVE		(2 << 10)
#define S3C2412_IISMOD_MASTER_MASK	(3 << 10)
#define S3C2412_IISMOD_MODE_TXONLY	(0 << 8)
#define S3C2412_IISMOD_MODE_RXONLY	(1 << 8)
#define S3C2412_IISMOD_MODE_TXRX	(2 << 8)
#define S3C2412_IISMOD_MODE_MASK	(3 << 8)
#define S3C2412_IISMOD_LR_LLOW		(0 << 7)
#define S3C2412_IISMOD_LR_RLOW		(1 << 7)
#define S3C2412_IISMOD_SDF_IIS		(0 << 5)
#define S3C2412_IISMOD_SDF_MSB		(1 << 5)
#define S3C2412_IISMOD_SDF_LSB		(2 << 5)
#define S3C2412_IISMOD_SDF_MASK		(3 << 5)
#define S3C2412_IISMOD_RCLK_256FS	(0 << 3)
#define S3C2412_IISMOD_RCLK_512FS	(1 << 3)
#define S3C2412_IISMOD_RCLK_384FS	(2 << 3)
#define S3C2412_IISMOD_RCLK_768FS	(3 << 3)
#define S3C2412_IISMOD_RCLK_MASK 	(3 << 3)
#define S3C2412_IISMOD_BCLK_32FS	(0 << 1)
#define S3C2412_IISMOD_BCLK_48FS	(1 << 1)
#define S3C2412_IISMOD_BCLK_16FS	(2 << 1)
#define S3C2412_IISMOD_BCLK_24FS	(3 << 1)
#define S3C2412_IISMOD_BCLK_MASK	(3 << 1)
#define S3C2412_IISMOD_8BIT		(1 << 0)

#define S3C64XX_IISMOD_CDCLKCON		(1 << 12)

#define S3C2412_IISPSR_PSREN		(1 << 15)

#define S3C2412_IISFIC_TXFLUSH		(1 << 15)
#define S3C2412_IISFIC_RXFLUSH		(1 << 7)
#define S3C2412_IISFIC_TXCOUNT(x)	(((x) >>  8) & 0xf)
#define S3C2412_IISFIC_RXCOUNT(x)	(((x) >>  0) & 0xf)

#define S5P_IISAHB_INTENLVL3	(1<<27)
#define S5P_IISAHB_INTENLVL2	(1<<26)
#define S5P_IISAHB_INTENLVL1	(1<<25)
#define S5P_IISAHB_INTENLVL0	(1<<24)
#define S5P_IISAHB_LVL3INT	(1<<23)
#define S5P_IISAHB_LVL2INT	(1<<22)
#define S5P_IISAHB_LVL1INT	(1<<21)
#define S5P_IISAHB_LVL0INT	(1<<20)
#define S5P_IISAHB_CLRLVL3	(1<<19)
#define S5P_IISAHB_CLRLVL2	(1<<18)
#define S5P_IISAHB_CLRLVL1	(1<<17)
#define S5P_IISAHB_CLRLVL0	(1<<16)
#define S5P_IISAHB_DMARLD	(1<<5)
#define S5P_IISAHB_DISRLDINT	(1<<3)
#define S5P_IISAHB_DMAEND	(1<<2)
#define S5P_IISAHB_DMACLR	(1<<1)
#define S5P_IISAHB_DMAEN	(1<<0)

#define S5P_IISSIZE_SHIFT	(16)
#define S5P_IISSIZE_TRNMSK	(0xffff)
#define S5P_IISTRNCNT_MASK	(0xffffff)

#define S5P_IISADDR_MASK	(0x3fffff)
#define S5P_IISADDR_SHIFT	(10)
#define S5P_IISADDR_ENSTOP	(1<<0)

/* clock sources */
#define S3C_IISMOD_IMSMASK	(3<<10)
#define S3C_IISMOD_MSTPCLK	(0<<10)
#define S3C_IISMOD_MSTCLKAUDIO	(1<<10)
#define S3C_IISMOD_SLVPCLK	(2<<10)
#define S3C_IISMOD_SLVI2SCLK	(3<<10)

#define S3C_CLKSRC_PCLK		S3C_IISMOD_MSTPCLK
#define S3C_CLKSRC_CLKAUDIO	S3C_IISMOD_MSTCLKAUDIO
#define S3C_CLKSRC_SLVPCLK	S3C_IISMOD_SLVPCLK
#define S3C_CLKSRC_I2SEXT	S3C_IISMOD_SLVI2SCLK
#define S3C_CDCLKSRC_INT	(4<<10)
#define S3C_CDCLKSRC_EXT	(5<<10)

#if 1
#define S3C2412_IISCON			(0x00)
#define S3C2412_IISMOD			(0x04)
#define S3C2412_IISFIC			(0x08)
#define S3C2412_IISPSR			(0x0C)
#define S3C2412_IISTXD			(0x10)
#define S3C2412_IISRXD			(0x14)

#define S3C2412_IISCON_LRINDEX		(1 << 11)
#define S3C2412_IISCON_TXFIFO_EMPTY	(1 << 10)
#define S3C2412_IISCON_RXFIFO_EMPTY	(1 << 9)
#define S3C2412_IISCON_TXFIFO_FULL	(1 << 8)
#define S3C2412_IISCON_RXFIFO_FULL	(1 << 7)
#define S3C2412_IISCON_TXDMA_PAUSE	(1 << 6)
#define S3C2412_IISCON_RXDMA_PAUSE	(1 << 5)
#define S3C2412_IISCON_TXCH_PAUSE	(1 << 4)
#define S3C2412_IISCON_RXCH_PAUSE	(1 << 3)
#define S3C2412_IISCON_TXDMA_ACTIVE	(1 << 2)
#define S3C2412_IISCON_RXDMA_ACTIVE	(1 << 1)
#define S3C2412_IISCON_IIS_ACTIVE	(1 << 0)

#define S3C64XX_IISMOD_BLC_16BIT	(0 << 13)
#define S3C64XX_IISMOD_BLC_8BIT		(1 << 13)
#define S3C64XX_IISMOD_BLC_24BIT	(2 << 13)
#define S3C64XX_IISMOD_BLC_MASK		(3 << 13)

#define S3C64XX_IISMOD_IMS_PCLK		(0 << 10)
#define S3C64XX_IISMOD_IMS_SYSMUX	(1 << 10)

#define S3C2412_IISMOD_MASTER_INTERNAL	(0 << 10)
#define S3C2412_IISMOD_MASTER_EXTERNAL	(1 << 10)
#define S3C2412_IISMOD_SLAVE		(2 << 10)
#define S3C2412_IISMOD_MASTER_MASK	(3 << 10)
#define S3C2412_IISMOD_MODE_TXONLY	(0 << 8)
#define S3C2412_IISMOD_MODE_RXONLY	(1 << 8)
#define S3C2412_IISMOD_MODE_TXRX	(2 << 8)
#define S3C2412_IISMOD_MODE_MASK	(3 << 8)
#define S3C2412_IISMOD_LR_LLOW		(0 << 7)
#define S3C2412_IISMOD_LR_RLOW		(1 << 7)
#define S3C2412_IISMOD_SDF_IIS		(0 << 5)
#define S3C2412_IISMOD_SDF_MSB		(1 << 5)
#define S3C2412_IISMOD_SDF_LSB		(2 << 5)
#define S3C2412_IISMOD_SDF_MASK		(3 << 5)
#define S3C2412_IISMOD_RCLK_256FS	(0 << 3)
#define S3C2412_IISMOD_RCLK_512FS	(1 << 3)
#define S3C2412_IISMOD_RCLK_384FS	(2 << 3)
#define S3C2412_IISMOD_RCLK_768FS	(3 << 3)
#define S3C2412_IISMOD_RCLK_MASK 	(3 << 3)
#define S3C2412_IISMOD_BCLK_32FS	(0 << 1)
#define S3C2412_IISMOD_BCLK_48FS	(1 << 1)
#define S3C2412_IISMOD_BCLK_16FS	(2 << 1)
#define S3C2412_IISMOD_BCLK_24FS	(3 << 1)
#define S3C2412_IISMOD_BCLK_MASK	(3 << 1)
#define S3C2412_IISMOD_8BIT		(1 << 0)

#define S3C64XX_IISMOD_CDCLKCON		(1 << 12)

#define S3C2412_IISPSR_PSREN		(1 << 15)

#define S3C2412_IISFIC_TXFLUSH		(1 << 15)
#define S3C2412_IISFIC_RXFLUSH		(1 << 7)
#define S3C2412_IISFIC_TXCOUNT(x)	(((x) >>  8) & 0xf)
#define S3C2412_IISFIC_RXCOUNT(x)	(((x) >>  0) & 0xf)
#endif

#if 1
#define S3C64XX_IIS0CON		(0x00)
#define S3C64XX_IIS0MOD		(0x04)
#define S3C64XX_IIS0FIC		(0x08)
#define S3C64XX_IIS0PSR		(0x0C)
#define S3C64XX_IIS0TXD		(0x10)
#define S3C64XX_IIS0RXD		(0x14)

#define S3C64XX_IISCON_LRINDEX		(1<<8)
#define S3C64XX_IISCON_TXFIFORDY	(1<<7)
#define S3C64XX_IISCON_RXFIFORDY	(1<<6)
#define S3C64XX_IISCON_TXDMAEN		(1<<5)
#define S3C64XX_IISCON_RXDMAEN		(1<<4)
#define S3C64XX_IISCON_TXIDLE		(1<<3)
#define S3C64XX_IISCON_RXIDLE		(1<<2)
#define S3C64XX_IISCON_PSCEN		(1<<1)
#define S3C64XX_IISCON_IISEN		(1<<0)

//#define S3C64XX_IISMOD_MPLL	  (1<<9)
#define S3C64XX_IISMOD_MPLL		(0x01<<10)
#define S3C64XX_IISMOD_SLAVE		(1<<8)
#define S3C64XX_IISMOD_NOXFER		(0<<6)
#define S3C64XX_IISMOD_RXMODE		(1<<6)
#define S3C64XX_IISMOD_TXMODE		(2<<6)
#define S3C64XX_IISMOD_TXRXMODE		(3<<6)
#define S3C64XX_IISMOD_LR_LLOW		(0<<5)
#define S3C64XX_IISMOD_LR_RLOW		(1<<5)
#define S3C64XX_IISMOD_IIS		(0<<4)
#define S3C64XX_IISMOD_MSB		(1<<4)
#define S3C64XX_IISMOD_8BIT		(0<<3)
#define S3C64XX_IISMOD_16BIT		(1<<3)
#define S3C64XX_IISMOD_BITMASK		(1<<3)
#define S3C64XX_IISMOD_256FS		(0<<2)
#define S3C64XX_IISMOD_384FS		(1<<2)
#define S3C64XX_IISMOD_16FS		(0<<0)
#define S3C64XX_IISMOD_32FS		(1<<0)
#define S3C64XX_IISMOD_48FS		(2<<0)
#define S3C64XX_IISMOD_FS_MASK		(3<<0)

#define S3C64XX_IIS0MOD_DCE_MASK	(0x3<<16)
#define S3C64XX_IIS0MOD_DCE_SD2		(0x1<<17)
#define S3C64XX_IIS0MOD_DCE_SD1		(0x1<<16)
#define S3C64XX_IIS0MOD_BLC_MASK	(0x3<<13)
#define S3C64XX_IIS0MOD_BLC_16BIT	(0x0<<13)
#define S3C64XX_IIS0MOD_BLC_08BIT	(0x1<<13)
#define S3C64XX_IIS0MOD_BLC_24BIT	(0x2<<13)
#define S3C64XX_IIS0MOD_CLK_MASK	(0x7<<10)
#define S3C64XX_IIS0MOD_INTERNAL_CLK	(0x0<<12)
#define S3C64XX_IIS0MOD_EXTERNAL_CLK	(0x1<<12)
#define S3C64XX_IIS0MOD_IMS_INTERNAL_MASTER	(0x0<<10)
#define S3C64XX_IIS0MOD_IMS_EXTERNAL_MASTER	(0x1<<10)
#define S3C64XX_IIS0MOD_IMS_SLAVE	(0x2<<10)
#define S3C64XX_IIS0MOD_MODE_MASK	(0x3<<8)
#define S3C64XX_IIS0MOD_TXMODE		(0x0<<8)
#define S3C64XX_IIS0MOD_RXMODE		(0x1<<8)
#define S3C64XX_IIS0MOD_TXRXMODE	(0x2<<8)
#define S3C64XX_IIS0MOD_FM_MASK		(0x3<<5)
#define S3C64XX_IIS0MOD_IIS		(0x0<<5)
#define S3C64XX_IIS0MOD_MSB		(0x1<<5)
#define S3C64XX_IIS0MOD_LSB		(0x2<<5)
#define S3C64XX_IIS0MOD_FS_MASK		(0x3<<3)
#define S3C64XX_IIS0MOD_768FS		(0x3<<3)
#define S3C64XX_IIS0MOD_384FS		(0x2<<3)
#define S3C64XX_IIS0MOD_512FS		(0x1<<3)
#define S3C64XX_IIS0MOD_256FS		(0x0<<3)
#define S3C64XX_IIS0MOD_BFS_MASK	(0x3<<1)
#define S3C64XX_IIS0MOD_48FS		(0x1<<1)
#define S3C64XX_IIS0MOD_32FS		(0x0<<1)

#define S3C64XX_IISPSR			(0x08)
#define S3C64XX_IISPSR_INTMASK		(31<<5)
#define S3C64XX_IISPSR_INTSHIFT		(5)
#define S3C64XX_IISPSR_EXTMASK		(31<<0)
#define S3C64XX_IISPSR_EXTSHFIT		(0)

#define S3C64XX_IISFCON		(0x0c)

#define S3C64XX_IISFCON_TXDMA		(1<<15)
#define S3C64XX_IISFCON_RXDMA		(1<<14)
#define S3C64XX_IISFCON_TXENABLE	(1<<13)
#define S3C64XX_IISFCON_RXENABLE	(1<<12)
#define S3C64XX_IISFCON_TXMASK		(0x3f << 6)
#define S3C64XX_IISFCON_TXSHIFT		(6)
#define S3C64XX_IISFCON_RXMASK		(0x3f)
#define S3C64XX_IISFCON_RXSHIFT		(0)

#define S3C64XX_IISFIFO		(0x10)
#define S3C64XX_IISFIFORX	(0x14)

#define S3C64XX_IIS0CON_I2SACTIVE	(0x1<<0)
#define S3C64XX_IIS0CON_RXDMACTIVE	(0x1<<1)
#define S3C64XX_IIS0CON_I2SACTIVE	(0x1<<0)
#define S3C64XX_IIS0CON_TXDMACTIVE	(0x1<<2)

#define S3C64XX_IIS_TX_FLUSH	(0x1<<15)
#define S3C64XX_IIS_RX_FLUSH	(0x1<<7)

#define S3C64XX_IISCON_FTXURINTEN 	(0x1<<16)

#define S3C64XX_IIS0MOD_24BIT		(0x2<<13)
#define S3C64XX_IIS0MOD_8BIT		(0x1<<13)
#define S3C64XX_IIS0MOD_16BIT		(0x0<<13)
#endif

#if 1
//hqf 20110218
//urbetter disable it
#define USE_CLKAUDIO	1 /* use it for LPMP3 mode */

/* DMA Period size - By Jung  */
#define LP_DMA_PERIOD 	(0x2000)
#define LP_TXBUFF_ADDR  (0xC0000000)
#define LP_MAX_DMABUFF  (0xC0010000)
#define LP_PERIOD_MUL	(2)

/* Clock dividers */
#define S3C_DIV_MCLK	0
#define S3C_DIV_BCLK	1
#define S3C_DIV_PRESCALER	2

#define S3C_IISCON		(0x00)
#define S3C_IISMOD		(0x04)
#define S3C_IISFIC		(0x08)
#define S3C_IISPSR		(0x0C)
#define S3C_IISTXD		(0x10)
#define S3C_IISRXD		(0x14)

#define S3C_IISFICS		(0x18)
#define S3C_IISTXDS		(0x1c)
#define S3C_IISAHB		(0x20)
#define S3C_IISSTR		(0x24)
#define S3C_IISSIZE		(0x28)
#define S3C_IISTRNCNT		(0x2c)
#define S3C_IISADDR0		(0x30)
#define S3C_IISADDR1		(0x34)
#define S3C_IISADDR2		(0x38)
#define S3C_IISADDR3		(0x3c)

#define S3C_IISLVLINTMASK		(0xf<<20)
#define S3C_IISADDR0_LVLSTOP	(0x1<<0)
#define S3C_IISCON_I2SACTIVE	(0x1<<0)
#define S3C_IISCON_RXDMACTIVE	(0x1<<1)
#define S3C_IISCON_TXDMACTIVE	(0x1<<2)
#define S3C_IISCON_RXCHPAUSE	(0x1<<3)
#define S3C_IISCON_TXCHPAUSE	(0x1<<4)
#define S3C_IISCON_RXDMAPAUSE	(0x1<<5)
#define S3C_IISCON_TXDMAPAUSE	(0x1<<6)
#define S3C_IISCON_FRXFULL		(0x1<<7)

#define S3C_IISCON_FTX0FULL		(0x1<<8)

#define S3C_IISCON_FRXEMPT		(0x1<<9)
#define S3C_IISCON_FTX0EMPT		(0x1<<10)
#define S3C_IISCON_LRI		(0x1<<11)

#define S3C_IISCON_FTX1FULL		(0x1<<12)
#define S3C_IISCON_FTX2FULL		(0x1<<13)
#define S3C_IISCON_FTX1EMPT		(0x1<<14)
#define S3C_IISCON_FTX2EMPT		(0x1<<15)

#define S3C_IISCON_FTXURINTEN	(0x1<<16)
#define S3C_IISCON_FTXURSTATUS	(0x1<<17)

#define S3C_IISCON_TXSDMACTIVE	(0x1<<18)
#define S3C_IISCON_TXSDMAPAUSE	(0x1<<20)
#define S3C_IISCON_FTXSFULL	(0x1<<21)
#define S3C_IISCON_FTXSEMPT	(0x1<<22)
#define S3C_IISCON_FTXSURINTEN	(0x1<<23)
#define S3C_IISCON_FTXSURSTAT	(0x1<<24)
#define S3C_IISCON_FRXOFINTEN	(0x1<<25)
#define S3C_IISCON_FRXOFSTAT	(0x1<<26)

#define S3C_IISCON_SWRESET	(0x1<<31)

#define S3C_IISMOD_BFSMASK		(3<<1)
#define S3C_IISMOD_32FS		(0<<1)
#define S3C_IISMOD_48FS		(1<<1)
#define S3C_IISMOD_16FS		(2<<1)
#define S3C_IISMOD_24FS		(3<<1)

#define S3C_IISMOD_RFSMASK		(3<<3)
#define S3C_IISMOD_256FS		(0<<3)
#define S3C_IISMOD_512FS		(1<<3)
#define S3C_IISMOD_384FS		(2<<3)
#define S3C_IISMOD_768FS		(3<<3)

#define S3C_IISMOD_SDFMASK		(3<<5)
#define S3C_IISMOD_IIS		(0<<5)
#define S3C_IISMOD_MSB		(1<<5)
#define S3C_IISMOD_LSB		(2<<5)

#define S3C_IISMOD_LRP		(1<<7)

#define S3C_IISMOD_TXRMASK		(3<<8)
#define S3C_IISMOD_TX		(0<<8)
#define S3C_IISMOD_RX		(1<<8)
#define S3C_IISMOD_TXRX		(2<<8)

#define S3C_IISMOD_IMSMASK		(3<<10)
#define S3C_IISMOD_MSTPCLK		(0<<10)
#define S3C_IISMOD_MSTCLKAUDIO	(1<<10)
#define S3C_IISMOD_SLVPCLK		(2<<10)
#define S3C_IISMOD_SLVI2SCLK	(3<<10)

#define S3C_IISMOD_CDCLKCON		(1<<12)

#define S3C_IISMOD_BLCMASK		(3<<13)
#define S3C_IISMOD_16BIT		(0<<13)
#define S3C_IISMOD_8BIT		(1<<13)
#define S3C_IISMOD_24BIT		(2<<13)

#define S3C_IISMOD_SD1EN		(1<<16)
#define S3C_IISMOD_SD2EN		(1<<17)

#define S3C_IISMOD_CCD1MASK		(3<<18)
#define S3C_IISMOD_CCD1ND		(0<<18)
#define S3C_IISMOD_CCD11STD		(1<<18)
#define S3C_IISMOD_CCD12NDD		(2<<18)

#define S3C_IISMOD_CCD2MASK		(3<<20)
#define S3C_IISMOD_CCD2ND		(0<<20)
#define S3C_IISMOD_CCD21STD		(1<<20)
#define S3C_IISMOD_CCD22NDD		(2<<20)

#define S3C_IISMOD_BLCPMASK		(3<<24)
#define S3C_IISMOD_P16BIT		(0<<24)
#define S3C_IISMOD_P8BIT		(1<<24)
#define S3C_IISMOD_P24BIT		(2<<24)
#define S3C_IISMOD_BLCSMASK		(3<<26)
#define S3C_IISMOD_S16BIT		(0<<26)
#define S3C_IISMOD_S8BIT		(1<<26)
#define S3C_IISMOD_S24BIT		(2<<26)
#define S3C_IISMOD_TXSLP		(1<<28)
#define S3C_IISMOD_OPMSK		(3<<30)
#define S3C_IISMOD_OPCCO		(0<<30)
#define S3C_IISMOD_OPCCI		(1<<30)
#define S3C_IISMOD_OPBCO		(2<<30)
#define S3C_IISMOD_OPPCLK		(3<<30)

#define S3C_IISFIC_FRXCNTMSK	(0x7f<<0)
#define S3C_IISFIC_RFLUSH		(1<<7)
#define S3C_IISFIC_FTX0CNTMSK	(0x7f<<8)
#define S3C_IISFIC_TFLUSH		(1<<15)

#define S3C_IISPSR_PSVALA		(0x3f<<8) 
#define S3C_IISPSR_PSVALA_PLBK		(0x5<<8)
#define S3C_IISPSR_PSRAEN		(1<<15)

#define S3C_IISAHB_INTENLVL3	(1<<27)
#define S3C_IISAHB_INTENLVL2	(1<<26)
#define S3C_IISAHB_INTENLVL1	(1<<25)
#define S3C_IISAHB_INTENLVL0	(1<<24)
#define S3C_IISAHB_LVL3INT	(1<<23)
#define S3C_IISAHB_LVL2INT	(1<<22)
#define S3C_IISAHB_LVL1INT	(1<<21)
#define S3C_IISAHB_LVL0INT	(1<<20)
#define S3C_IISAHB_CLRLVL3	(1<<19)
#define S3C_IISAHB_CLRLVL2	(1<<18)
#define S3C_IISAHB_CLRLVL1	(1<<17)
#define S3C_IISAHB_CLRLVL0	(1<<16)
#define S3C_IISAHB_DMARLD	(1<<5)
#define S3C_IISAHB_DISRLDINT	(1<<3)
#define S3C_IISAHB_DMAEND	(1<<2)
#define S3C_IISAHB_DMACLR	(1<<1)
#define S3C_IISAHB_DMAEN	(1<<0)

#define S3C_IISSIZE_SHIFT	(16)
#define S3C_IISSIZE_TRNMSK	(0xffff)
#define S3C_IISTRNCNT_MASK	(0xffffff)

#define S3C_IISADDR_MASK	(0x3fffff)
#define S3C_IISADDR_SHIFT	(10)
#define S3C_IISADDR_ENSTOP	(1<<0)

/* clock sources */
#define S3C_CLKSRC_PCLK		S3C_IISMOD_MSTPCLK
#define S3C_CLKSRC_CLKAUDIO	S3C_IISMOD_MSTCLKAUDIO
#define S3C_CLKSRC_SLVPCLK	S3C_IISMOD_SLVPCLK
#define S3C_CLKSRC_I2SEXT	S3C_IISMOD_SLVI2SCLK
#define S3C_CDCLKSRC_INT	(4<<10)
#define S3C_CDCLKSRC_EXT	(5<<10)

#define S5P_CLKSRC_PCLK (0)
#define S5P_CLKSRC_MUX  (1)             
#define S5P_CLKSRC_CDCLK    (2)

#if defined(CONFIG_CPU_S5PV210)
/* Physical Address of Audio Subsystem - By Jung */
#define S5P_ASS_CLK_PABASE  0xEEE10000  /* ASS Clock CON Register Base */
#define S5P_ASS_COM_PABASE  0xEEE20000  /* ASS COMMBOX Register Base */
#define S5P_ASS_SRBUF_BASE  0xC0000000  /* ASS SRAM Buffer Base */
#endif

/* Offset of Audio Subsysteam Register */
#define S5P_ASS_INTR				(0x00)
#define S5P_ASS_INST_START_ADDR		(0x14)
#define S5P_ASS_RESET				(0x100)
#define S5P_ASS_RP_PENDING			(0x104)
#define S5P_ASS_FRM_SIZE			(0x108)
#define S5P_ASS_RP_BOOT				(0x12C)
#define S5P_ASS_PAD_PDN_CTRL		(0x204)
#define S5P_ASS_MISC				(0x208)

/* Offset of CLKCON Register */
#define S5P_ASS_CLKCON				(0x00)

/* Audio Subsytem Interrupt Register */
#define S5P_ASS_INTR_WAKEUP			(0x1<<0)
#define S5P_ASS_CLKCON_EPLL			(0x1<<0)
#define S5P_ASS_RP_BOOT_IN			(0x1<<0)

#define IRQ_S3C_IISV32		IRQ_I2S1
#define IRQ_S3C_IISV50		IRQ_I2S0

#define S3C_DMACH_I2S_OUT	DMACH_I2S0_OUT
#define S3C_DMACH_I2S_IN	DMACH_I2S0_IN 
#define S3C_IIS_PABASE		S5PV210_PA_IIS0
#define S3C_IISIRQ		IRQ_S3C_IISV50
#define PCLKCLK			"i2s_v50"

#ifdef CONFIG_SND_WM8580_MASTER /* ?? */
#define EXTPRNT "i2s_cdclk0"
#else
#if defined(CONFIG_SND_SMDKC100_WM8580)
#define EXTPRNT "fout_epll"
#elif defined(CONFIG_SND_SMDKC110_WM8580)
#define EXTPRNT "i2smain_clk"
#elif defined(CONFIG_SND_UNIVERSAL_WM8994)|| defined(CONFIG_SND_UNIVERSAL_WM8994_MODULE)
#define EXTPRNT "i2smain_clk"
#endif
#endif

#if defined(CONFIG_SND_SMDKC100_WM8580)
#define EXTCLK			"i2sclkd2"
#define RATESRCCLK 		EXTPRNT
#else
#define EXTCLK			"i2sclk"
#define RATESRCCLK 		"fout_epll"
#endif

#define PLBK_CHAN		6
#define S3C_DESC		"S3C AP I2S-V5.0 Interface"

/* dma_state */
#define S3C_I2SDMA_START   1
#define S3C_I2SDMA_STOP    2
#define S3C_I2SDMA_FLUSH   3
#define S3C_I2SDMA_SUSPEND 4
#define S3C_I2SDMA_RESUME  5

struct s5p_i2s_pdata {
	u32 *p_rate;
	unsigned  dma_prd;
	void *dma_token;
	dma_addr_t dma_saddr;
	unsigned int dma_size;
	int dma_state;
	void (*dma_cb)(void *dt, int bytes_xfer);
	void (*dma_setcallbk)(void (*dcb)(void *, int), unsigned chunksize);
	void (*dma_getpos)(dma_addr_t *src, dma_addr_t *dst);
	int (*dma_enqueue)(void *id);
	void (*dma_ctrl)(int cmd);
	struct snd_soc_dai i2s_dai;
	void (*set_mode)(void);
	spinlock_t lock;
};

#if 0
struct s3c_i2sv2_rate_calc {
        unsigned int    clk_div;        /* for prescaler */
        unsigned int    fs_div;         /* for root frame clock */
}; 

extern int s3c_i2sv2_iis_calc_rate1(struct s3c_i2sv2_rate_calc *info,
                                   unsigned int *fstab,
                                   unsigned int rate, struct clk *clk);
#endif

//extern struct clk *s5p_i2s_get_clock(struct snd_soc_dai *dai);
//extern struct clk *s3c64xx_i2s_get_clock(struct snd_soc_dai *dai);
#endif




#endif /* __ASM_ARCH_REGS_S3C2412_IIS_H */

