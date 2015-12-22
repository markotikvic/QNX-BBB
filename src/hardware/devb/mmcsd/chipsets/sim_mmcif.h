/*
 * $QNXLicenseC:
 * Copyright 2013, QNX Software Systems. 
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"). You 
 * may not reproduce, modify or distribute this software except in 
 * compliance with the License. You may obtain a copy of the License 
 * at: http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" basis, 
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as 
 * contributors under the License or as licensors under other terms.  
 * Please review this entire file for other proprietary rights or license 
 * notices, as well as the QNX Development Suite License Guide at 
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#ifndef	_MMCIF_INCLUDED
#define	_MMCIF_INCLUDED
#include 	<hw/renesas_dma.h>

#define	MMCIIF_CHANNELS			2

#define CONFIG_MMCIF_FREQ	104000000
#define	PCLK				(CONFIG_SH_MMCIF_FREQ/2)

#define	__REG8(x)		(*((volatile uint8_t  *)(x)))
#define	__REG16(x)		(*((volatile uint16_t *)(x)))
#define	__REG32(x)		(*((volatile uint32_t *)(x)))

#define CE_CMD_SET		0x00000000
#define CE_ARG			0x00000008
#define CE_ARG_CMD12	0x0000000C
#define CE_CMD_CTRL		0x00000010
#define CE_BLOCK_SET	0x00000014
#define CE_CLK_CTRL		0x00000018
#define CE_BUF_ACC		0x0000001C
#define CE_RESP3		0x00000020
#define CE_RESP2		0x00000024
#define CE_RESP1		0x00000028
#define CE_RESP0		0x0000002C
#define CE_RESP_CMD12	0x00000030
#define CE_DATA			0x00000034
#define CE_INT			0x00000040
#define CE_INT_MASK		0x00000044
#define CE_HOST_STS1	0x00000048
#define CE_HOST_STS2	0x0000004C
#define CE_CLK_CTRL2	0x00000070
#define CE_VERSION		0x0000007C




/* CE_CMD_SET */
#define CMD_MASK		0x3f000000
#define CMD_SET_RTYP_NO		((0 << 23) | (0 << 22))
#define CMD_SET_RTYP_6B		((0 << 23) | (1 << 22)) /* R1/R1b/R3/R4/R5 */
#define CMD_SET_RTYP_17B	((1 << 23) | (0 << 22)) /* R2 */
#define CMD_SET_RBSY		(1 << 21) /* R1b */
#define CMD_SET_CCSEN		(1 << 20)
#define CMD_SET_WDAT		(1 << 19) /* 1: on data, 0: no data */
#define CMD_SET_DWEN		(1 << 18) /* 1: write, 0: read */
#define CMD_SET_CMLTE		(1 << 17) /* 1: multi block trans, 0: single */
#define CMD_SET_CMD12EN		(1 << 16) /* 1: CMD12 auto issue */
#define CMD_SET_RIDXC_INDEX	((0 << 15) | (0 << 14)) /* index check */
#define CMD_SET_RIDXC_BITS	((0 << 15) | (1 << 14)) /* check bits check */
#define CMD_SET_RIDXC_NO	((1 << 15) | (0 << 14)) /* no check */
#define CMD_SET_CRC7C		((0 << 13) | (0 << 12)) /* CRC7 check*/
#define CMD_SET_CRC7C_BITS	((0 << 13) | (1 << 12)) /* check bits check*/
#define CMD_SET_CRC7C_INTERNAL	((1 << 13) | (0 << 12)) /* internal CRC7 check*/
#define CMD_SET_CRC16C		(1 << 10) /* 0: CRC16 check*/
#define CMD_SET_CRCSTE		(1 << 8) /* 1: not receive CRC status */
#define CMD_SET_TBIT		(1 << 7) /* 1: tran mission bit "Low" */
#define CMD_SET_OPDM		(1 << 6) /* 1: open/drain */
#define CMD_SET_CCSH		(1 << 5)
#define CMD_SET_DATW_1		((0 << 1) | (0 << 0)) /* 1bit */
#define CMD_SET_DATW_4		((0 << 1) | (1 << 0)) /* 4bit */
#define CMD_SET_DATW_8		((1 << 1) | (0 << 0)) /* 8bit */

/* CE_CMD_CTRL */
#define CMD_CTRL_BREAK		(1 << 0)

/* CE_BLOCK_SET */
#define BLOCK_SIZE_MASK		0x0000ffff

/* CE_CLK_CTRL */
#define CLK_ENABLE		(1 << 24) /* 1: output mmc clock */
#define CLK_CLEAR		((1 << 19) | (1 << 18) | (1 << 17) | (1 << 16))
#define CLK_SUP_PCLK		((1 << 19) | (1 << 18) | (1 << 17) | (1 << 16))
#define SRSPTO_256		((1 << 13) | (0 << 12)) /* resp timeout */
#define SRBSYTO_29		((1 << 11) | (1 << 10) |	\
				 (1 << 9) | (1 << 8)) /* resp busy timeout */
#define SRWDTO_29		((1 << 7) | (1 << 6) |		\
				 (1 << 5) | (1 << 4)) /* read/write timeout */
#define SCCSTO_29		((1 << 3) | (1 << 2) |		\
				 (1 << 1) | (1 << 0)) /* ccs timeout */

/* CE_BUF_ACC */
#define BUF_ACC_DMAWEN		(1 << 25)
#define BUF_ACC_DMAREN		(1 << 24)
#define BUF_ACC_BUSW_32		(0 << 17)
#define BUF_ACC_BUSW_16		(1 << 17)
#define BUF_ACC_ATYP		(1 << 16)

/* CE_INT */
#define INT_CCSDE		(1 << 29)
#define INT_CMD12DRE		(1 << 26)
#define INT_CMD12RBE		(1 << 25)
#define INT_CMD12CRE		(1 << 24)
#define INT_DTRANE		(1 << 23)
#define INT_BUFRE		(1 << 22)
#define INT_BUFWEN		(1 << 21)
#define INT_BUFREN		(1 << 20)
#define INT_CCSRCV		(1 << 19)
#define INT_RBSYE		(1 << 17)
#define INT_CRSPE		(1 << 16)
#define INT_CMDVIO		(1 << 15)
#define INT_BUFVIO		(1 << 14)
#define INT_WDATERR		(1 << 11)
#define INT_RDATERR		(1 << 10)
#define INT_RIDXERR		(1 << 9)
#define INT_RSPERR		(1 << 8)
#define INT_CCSTO		(1 << 5)
#define INT_CRCSTO		(1 << 4)
#define INT_WDATTO		(1 << 3)
#define INT_RDATTO		(1 << 2)
#define INT_RBSYTO		(1 << 1)
#define INT_RSPTO		(1 << 0)
#define INT_ERR_STS		(INT_CMDVIO | INT_BUFVIO | INT_WDATERR |  \
				 INT_RDATERR | INT_RIDXERR | INT_RSPERR | \
				 INT_CCSTO | INT_CRCSTO | INT_WDATTO |    \
				 INT_RDATTO | INT_RBSYTO | INT_RSPTO)

				 
/* CE_INT_MASK */
#define MASK_ALL		0x00000000
#define MASK_MCCSDE		(1 << 29)
#define MASK_MCMD12DRE		(1 << 26)
#define MASK_MCMD12RBE		(1 << 25)
#define MASK_MCMD12CRE		(1 << 24)
#define MASK_MDTRANE		(1 << 23)
#define MASK_MBUFRE		(1 << 22)
#define MASK_MBUFWEN		(1 << 21)
#define MASK_MBUFREN		(1 << 20)
#define MASK_MCCSRCV		(1 << 19)
#define MASK_MRBSYE		(1 << 17)
#define MASK_MCRSPE		(1 << 16)
#define MASK_MCMDVIO		(1 << 15)
#define MASK_MBUFVIO		(1 << 14)
#define MASK_MWDATERR		(1 << 11)
#define MASK_MRDATERR		(1 << 10)
#define MASK_MRIDXERR		(1 << 9)
#define MASK_MRSPERR		(1 << 8)
#define MASK_MCCSTO		(1 << 5)
#define MASK_MCRCSTO		(1 << 4)
#define MASK_MWDATTO		(1 << 3)
#define MASK_MRDATTO		(1 << 2)
#define MASK_MRBSYTO		(1 << 1)
#define MASK_MRSPTO		(1 << 0)


/* CE_HOST_STS1 */
#define STS1_CMDSEQ		(1 << 31)

/* CE_HOST_STS2 */
#define STS2_CRCSTE		(1 << 31)
#define STS2_CRC16E		(1 << 30)
#define STS2_AC12CRCE		(1 << 29)
#define STS2_RSPCRC7E		(1 << 28)
#define STS2_CRCSTEBE		(1 << 27)
#define STS2_RDATEBE		(1 << 26)
#define STS2_AC12REBE		(1 << 25)
#define STS2_RSPEBE		(1 << 24)
#define STS2_AC12IDXE		(1 << 23)
#define STS2_RSPIDXE		(1 << 22)
#define STS2_CCSTO		(1 << 15)
#define STS2_RDATTO		(1 << 14)
#define STS2_DATBSYTO		(1 << 13)
#define STS2_CRCSTTO		(1 << 12)
#define STS2_AC12BSYTO		(1 << 11)
#define STS2_RSPBSYTO		(1 << 10)
#define STS2_AC12RSPTO		(1 << 9)
#define STS2_RSPTO		(1 << 8)
#define STS2_CRC_ERR		(STS2_CRCSTE | STS2_CRC16E |		\
				 STS2_AC12CRCE | STS2_RSPCRC7E | STS2_CRCSTEBE)
#define STS2_TIMEOUT_ERR	(STS2_CCSTO | STS2_RDATTO |		\
				 STS2_DATBSYTO | STS2_CRCSTTO |		\
				 STS2_AC12BSYTO | STS2_RSPBSYTO |	\
				 STS2_AC12RSPTO | STS2_RSPTO)

/* CE_VERSION */
#define SOFT_RST_ON		(1 << 31)
#define SOFT_RST_OFF		(0 << 31)

#define CLKDEV_SD_DATA			25000000	/* 25 MHz */
#define CLKDEV_HS_DATA			50000000	/* 50 MHz */
#define CLKDEV_HS52_DATA		52000000	/* 52 MHz */
#define CLKDEV_MMC_DATA			20000000	/* 20MHz */
#define CLKDEV_MMC26_DATA		26000000	/* 26MHz */
#define CLKDEV_INIT			400000		/* 100 - 400 KHz */


#define MMC_MODE_HS		0x001
#define MMC_MODE_HS_52MHz	0x010
#define MMC_MODE_4BIT		0x100
#define MMC_MODE_8BIT		0x200

/*

#define MMC_CMD_GO_IDLE_STATE		0
#define MMC_CMD_SEND_OP_COND		1
#define MMC_CMD_ALL_SEND_CID		2
#define MMC_CMD_SET_RELATIVE_ADDR	3
#define MMC_CMD_SET_DSR			4
#define MMC_CMD_SWITCH			6
#define MMC_CMD_SELECT_CARD		7
#define MMC_CMD_SEND_EXT_CSD		8
#define MMC_CMD_SEND_CSD		9
#define MMC_CMD_SEND_CID		10
#define MMC_CMD_STOP_TRANSMISSION	12
#define MMC_CMD_SEND_STATUS		13
#define MMC_CMD_SET_BLOCKLEN		16
#define MMC_CMD_READ_SINGLE_BLOCK	17
#define MMC_CMD_READ_MULTIPLE_BLOCK	18
#define MMC_CMD_WRITE_SINGLE_BLOCK	24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK	25
#define MMC_CMD_APP_CMD			55

#define SD_CMD_SEND_RELATIVE_ADDR	3
#define SD_CMD_SWITCH_FUNC		6
#define SD_CMD_SEND_IF_COND		8

#define SD_CMD_APP_SET_BUS_WIDTH	6
#define SD_CMD_APP_SEND_OP_COND		41
#define SD_CMD_APP_SEND_SCR		51
*/

/* SCR definitions in different words */
#define SD_HIGHSPEED_BUSY	0x00020000
#define SD_HIGHSPEED_SUPPORTED	0x00020000


#define MMC_RSP_PRESENT (1 << 0)
#define MMC_RSP_136     (1 << 1)                /* 136 bit response */
#define MMC_RSP_CRC     (1 << 2)                /* expect valid crc */
#define MMC_RSP_BUSY    (1 << 3)                /* card may send busy */
#define MMC_RSP_OPCODE  (1 << 4)                /* response contains opcode */

//#define MMC_RSP_NONE    (0)
//#define MMC_RSP_R1      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1b	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE| \
			MMC_RSP_BUSY)
//#define MMC_RSP_R2      (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
//#define MMC_RSP_R3      (MMC_RSP_PRESENT)
//#define MMC_RSP_R4      (MMC_RSP_PRESENT)
//#define MMC_RSP_R5      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
//#define MMC_RSP_R6      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
//#define MMC_RSP_R7      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)


#define 	DMAC_DMARS_SDHI0_TX		0xC1		//Transmit empty transfer request
#define 	DMAC_DMARS_SDHI0_RX		0xC2		//Transmit empty transfer request
#define		SDHI_TMOUT				10000			/* Timeout loop counter for polling */

#define SD_CMD_C_CMD				(0 << 6)
#define SD_CMD_C_ACMD				(1 << 6)
#define SD_CMD_MD_MASK				(7 << 8)
#define SD_CMD_MD_NORMAL			(0 << 8)	/* Mode determined from command */
#define SD_CMD_MD_EXTEND			(3 << 8)
#define SD_CMD_MD_EXTEND_R1			(4 << 8)
#define SD_CMD_MD_EXTEND_R1B		(5 << 8)
#define SD_CMD_MD_EXTEND_R2			(6 << 8)
#define SD_CMD_MD_EXTEND_R3			(7 << 8)
#define SD_CMD_MD_DATA				(1 << 11)
#define SD_CMD_MD_NO_DATA			(0 << 11)
#define SD_CMD_MD_READ				(1 << 12)
#define SD_CMD_MD_WRITE				(0 << 12)
#define SD_CMD_MD_MULTI_BLOCK		(1 << 13)
#define SD_CMD_MD_SINGLE_BLOCK		(0 << 13)
#define SD_CMD_MD_CMD12_AUTO		(0 << 14)
#define SD_CMD_MD_CMD12_NO_AUTO		(1 << 14)
#define SD_INFO2_ERROR				 0x807F		/* Mask for any error */
#define SD_STOP_STP					(1 << 0)
#define SD_STOP_SEC					(1 << 8)


/*
*
 * Command Type Register (SD_CMD) bit defination
 */
#define	SD_CMD_MD7						~(1 << 15)	// CMD12 is not automatically issued at multiple block transfer.
#define	SD_CMD_MD6						(1 << 14)	// CMD12 is not automatically issued at multiple block transfer.
#define	SD_CMD_MD5						(1 << 13)	// Multiple block transfer
													//0: Single block transfer
													//1: Multiple block transfer
#define	SD_CMD_MD4						(1 << 12)	// Write/Read Mode (enabled when the command with data is handled)
													//0: Write (SD host interface -> SD card)
													//1: Read (SD host interface <- SD card)												
#define	SD_CMD_MD3						(1 << 11)	// Data Mode (Command Type)
													//0: Command without data transfer (bc, bcr, ac)
													//1: Command with data transfer (adtc)
													
/*
 * SD Card Interrupt Flag Register 1 (SD_INFO1) bit defination
 */
#define	SD_INFO1_INFO4					(1 << 4)	// ISDCD Card Insertion
#define	SD_INFO1_INFO3					(1 << 3)	// ISDCD Card Removal
#define	SD_INFO1_INFO2					(1 << 2)	// Access End
#define	SD_INFO1_INFO0					(1 << 0)	// Response End

/*
 * The SD card interrupt flag register 2 (SD_INFO2) bit defination
 */
#define	SD_INFO2_ILA					(1<<15)		//Illegal Access Error
#define	SD_INFO2_CBSY					(1<<14)		//Command Type Register Busy
#define	SD_INFO2_BWE					(1<<9)		//SD_BUF Write Enable	
#define	SD_INFO2_BRE					(1<<8)		//SD_BUF Read Enable
#define	SD_INFO2_ERR6					(1<<6)		//Response Timeout
#define	SD_INFO2_ERR5					(1<<5)		//SD_BUF Illegal Read Access
#define	SD_INFO2_ERR4					(1<<4)		//SD_BUF Illegal Write Access
#define	SD_INFO2_ERR3					(1<<3)		//Timeout (except response timeout)
#define	SD_INFO2_ERR2					(1<<2)		//END Error
#define	SD_INFO2_ERR1					(1<<1)		//CRC Error
#define	SD_INFO2_ERR0					(1<<0)		//CMD Error

/*
 * SDIO_INFO1 Interrupt Mask Register (SDIO_INFO1_MASK) bit defination
 */
#define	SDIO_INFO1_MASK_MEXWT					(1 << 15)	// EXWT interrupt masked
#define	SDIO_INFO1_MASK_MEXPUB52				(1 << 14)	// EXPUB52 interrupt masked
#define	SDIO_INFO1_MASK_IOMSK					(1 << 0)	// IOIRQ interrupt masked

/*
 * SD Card Access Control Option Register (SD_OPTION) bit defination
 */
#define	SD_OPTION_WIDTH_1	(1 << 15)	// Data Bus Width 1 bit
#define	SD_OPTION_WIDTH_4	(1 << 15)	// Data Bus Width 4 bit
/*
 * DMA Mode Enable Register (CC_EXT_MODE) bit defination
 */
#define	CC_EXT_MODE_SDRWEN						(1 <<5)		// SD_BUF Read/Write Status Output Enable
#define	CC_EXT_MODE_DMASDRW						(1 << 1)	// SD_BUF Read/Write DMA Transfer

typedef struct sysdmac_channel
{
	void 	*dma_chn;
} sysdmac_channel_t;

typedef	struct _mmcif_ext 
{
	int			channel;
	uintptr_t	base;
	int			blksz;
	uint32_t	clock;
	void		*hba;
	uint32_t	buf_base;
	uint8_t 	*buf;
	int         chid;
	int         coid;
	int         iid[4];
	int 		irq;
	int			prio; //interrup priority
	int			int_expected;
    struct sigevent  event;
	
	uintptr_t	mmcif_up;	
	uint32_t	mmc_pbase;
	uint8_t 	CardInsertion;
	uint8_t 	ResponseEnd;
	
	uint8_t 	use_dma;	
	sysdmac_channel_t write_chn;
	sysdmac_channel_t read_chn;
	dma_functions_t dmafuncs;

} mmcif_ext_t;


extern int mmcif_init(SIM_HBA *hba);
int mmcif_clock_enable(mmcif_ext_t *mmcif, int channel);
int mmcif_port_init(int channel);
int mmcif_reset(int channel);
#endif



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devb/mmcsd/chipsets/sim_mmcif.h $ $Rev: 748852 $")
#endif
