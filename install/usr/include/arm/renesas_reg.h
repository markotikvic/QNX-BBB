/*
 * $QNXLicenseC:
 * Copyright 2007, QNX Software Systems. All Rights Reserved.
 * 
 * You must obtain a written license from and pay applicable license fees to QNX 
 * Software Systems before you may reproduce, modify or distribute this software, 
 * or any work that includes all or part of this software.   Free development 
 * licenses are available for evaluation and non-commercial purposes.  For more 
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *  
 * This file may contain contributions from others.  Please review this entire 
 * file for other proprietary rights or license notices, as well as the QNX 
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/ 
 * for other information.
 * $
 */

#ifndef _REG_H
#define _REG_H

#define		BIT0	0x00000001
#define		BIT1	0x00000002
#define		BIT2	0x00000004
#define		BIT3	0x00000008
#define		BIT4	0x00000010
#define		BIT5	0x00000020
#define		BIT6	0x00000040
#define		BIT7	0x00000080
#define		BIT8	0x00000100
#define		BIT9	0x00000200
#define		BIT10	0x00000400
#define		BIT11	0x00000800
#define		BIT12	0x00001000
#define		BIT13	0x00002000
#define		BIT14	0x00004000
#define		BIT15	0x00008000
#define		BIT16	0x00010000
#define		BIT17	0x00020000
#define		BIT18	0x00040000
#define		BIT19	0x00080000
#define		BIT20	0x00100000
#define		BIT21	0x00200000
#define		BIT22	0x00400000
#define		BIT23	0x00800000
#define		BIT24	0x01000000
#define		BIT25	0x02000000
#define		BIT26	0x04000000
#define		BIT27	0x08000000
#define		BIT28	0x10000000
#define		BIT29	0x20000000
#define		BIT30	0x40000000
#define		BIT31	0x80000000


/*L2 Cache*/
#define CACHE_L2C310_BASE		(0xF0100000)

#define MERAM_BASE	0xE8080000

#define HPB_BASE	0xE6000000

#define RWDT_BASE	0xE6020000

#define SWDT_BASE	0xE6030000

#define GPIO_BASE	0xE6050000

#define PFC_BASE	0xE6060000	

#define CMT1_BASE	0xE6130000

#define CPG_BASE	0xE6150000

#define MMCIF0_BASE	0xEE200000
#define MMCIF1_BASE	0xEE220000

#define SCIF0_BASE	0xE6450000

#define SDHI0_BASE	0xEE100000
#define SDHI1_BASE		0xEE120000
#define SDHI2_BASE		0xEE140000
#define SDHI3_BASE		0xEE160000

#define SBSC1_BASE	0xFE400000

#define BSC_BASE	0xFEC10000

#define RWTCSRA0	(RWDT_BASE + 0x04)
#define RWTCSRB0	(RWDT_BASE + 0x08)
#define SWTCSRA0	(SWDT_BASE + 0x04)

/* CPG Symbols */
#define FRQCRA		(CPG_BASE + 0x0000)
#define FRQCRB		(CPG_BASE + 0x0004)
#define FRQCRD		(CPG_BASE + 0x00E4)
#define VCLKCR1		(CPG_BASE + 0x0008)
#define VCLKCR2		(CPG_BASE + 0x000C)
#define VCLKCR3		(CPG_BASE + 0x001C)
#define VCLKCR4		(CPG_BASE + 0x0014)
#define ZBCKCR		(CPG_BASE + 0x0010)
#define SDCKCR		(CPG_BASE + 0x0074)
#define SD2CKCR		(CPG_BASE + 0x0078)
#define SD3CKCR		(CPG_BASE + 0x026C)
#define MMC0CKCR	(CPG_BASE + 0x0240)
#define MMC1CKCR	(CPG_BASE + 0x0244)
#define FSIACKCR	(CPG_BASE + 0x0018)
#define FSIBCKCR	(CPG_BASE + 0x0090)
#define MPCKCR		(CPG_BASE + 0x0080)
#define SPUACKCR	(CPG_BASE + 0x0084)
#define HSICKCR		(CPG_BASE + 0x008C)
#define DSITCKCR	(CPG_BASE + 0x0060)
#define DSI0PCKCR	(CPG_BASE + 0x0064)
#define DSI0PHYCR	(CPG_BASE + 0x006C)
#define MPMODE		(CPG_BASE + 0x00CC)
#define PLLECR		(CPG_BASE + 0x00D0)
#define PLL0CR		(CPG_BASE + 0x00D8)
#define PLL1CR		(CPG_BASE + 0x0028)
#define PLL2CR		(CPG_BASE + 0x002C)
#define PLL3CR		(CPG_BASE + 0x00DC)
#define PLL0STPCR	(CPG_BASE + 0x00F0)
#define PLL1STPCR	(CPG_BASE + 0x00C8)
#define PLL2STPCR	(CPG_BASE + 0x00F8)
#define PLL3STPCR	(CPG_BASE + 0x00FC)
#define MSTPSR0		(CPG_BASE + 0x0030)
#define MSTPSR1		(CPG_BASE + 0x0038)
#define MSTPSR2		(CPG_BASE + 0x0040)
#define MSTPSR3		(CPG_BASE + 0x0048)
#define MSTPSR4		(CPG_BASE + 0x004C)
#define MSTPSR5		(CPG_BASE + 0x003C)
#define MSTPSR6		(CPG_BASE + 0x01C0)
#define MSTPSR7		(CPG_BASE + 0x01C4)
#define RMSTPCR0	(CPG_BASE + 0x0110)
#define RMSTPCR1	(CPG_BASE + 0x0114)
#define RMSTPCR2	(CPG_BASE + 0x0118)
#define RMSTPCR3	(CPG_BASE + 0x011C)
#define RMSTPCR4	(CPG_BASE + 0x0120)
#define RMSTPCR5	(CPG_BASE + 0x0124)
#define RMSTPCR6	(CPG_BASE + 0x0128)
#define RMSTPCR7	(CPG_BASE + 0x012c)
#define RMSTPCR8	(CPG_BASE + 0x0980)
#define RMSTPCR9	(CPG_BASE + 0x0984)
#define RMSTPCR10	(CPG_BASE + 0x0988)
#define RMSTPCR11	(CPG_BASE + 0x098c)

	
#define SMSTPCR0	(CPG_BASE + 0x0130)
#define SMSTPCR1	(CPG_BASE + 0x0134)
#define SMSTPCR2	(CPG_BASE + 0x0138)
#define SMSTPCR3	(CPG_BASE + 0x013C)
#define SMSTPCR4	(CPG_BASE + 0x0140)
#define SMSTPCR5	(CPG_BASE + 0x0144)
#define SMSTPCR6	(CPG_BASE + 0x0148)
#define SMSTPCR7	(CPG_BASE + 0x014C)
#define SMSTPCR8	(CPG_BASE + 0x0990)
#define SMSTPCR9	(CPG_BASE + 0x0994)
#define SMSTPCR10	(CPG_BASE + 0x0998)
#define SMSTPCR11	(CPG_BASE + 0x099C)
#define MMSTPCR0	(CPG_BASE + 0x0150)
#define MMSTPCR1	(CPG_BASE + 0x0154)
#define MMSTPCR2	(CPG_BASE + 0x0158)
#define MMSTPCR3	(CPG_BASE + 0x015C)
#define MMSTPCR4	(CPG_BASE + 0x0160)
#define MMSTPCR5	(CPG_BASE + 0x0164)
#define SRCR0		(CPG_BASE + 0x80A0)
#define SRCR1		(CPG_BASE + 0x80A8)
#define SRCR2		(CPG_BASE + 0x80B0)
#define SRCR3		(CPG_BASE + 0x80B8)
#define SRCR4		(CPG_BASE + 0x80BC)
#define SRCR5		(CPG_BASE + 0x80C4)
#define SRCR6		(CPG_BASE + 0x01C8)
#define SRCR7		(CPG_BASE + 0x01CC)
#define SRCR8		(CPG_BASE + 0x0920)
#define SRCR9		(CPG_BASE + 0x0924)
#define SRCR10		(CPG_BASE + 0x0928)
#define SRCR11		(CPG_BASE + 0x092C)

#define CKSCR		(CPG_BASE + 0x00C0)
#define VREFCR		(CPG_BASE + 0x00EC)
#define PCLKCR		(CPG_BASE + 0x1020)

//tmu
#define SH_MMR_TMU_TSTR0			0xE61E0004

#define SH_MMR_TMU_TCOR0			0xE61E0008
#define SH_MMR_TMU_TCNT0			0xE61E000c
#define SH_MMR_TMU_TCR0				0xE61E0010

#define SH_MMR_TMU_TCOR1			0xE61E0014
#define SH_MMR_TMU_TCNT1			0xE61E0018
#define SH_MMR_TMU_TCR1				0xE61E001c

#define SH_MMR_TMU_TCOR2			0xE61E0020
#define SH_MMR_TMU_TCNT2			0xE61E0024
#define SH_MMR_TMU_TCR2				0xE61E0028

/*INTCA INTERRUPT NUMBER */
#define    INTCA_CMT1_CMT10			(120+32)
#define    INTCA_CMT1_CMT11			(121+32)
#define    INTCA_CMT1_CMT12			(122+32)
#define    INTCA_CMT1_CMT13			(123+32)
#define    INTCA_CMT1_CMT14			(124+32)
#define    INTCA_CMT1_CMT15			(125+32)
#define    INTCA_CMT1_CMT16			(126+32)
#define    INTCA_CMT1_CMT17			(127+32)
#define    INTCA_SCIFA0				(144+32)
#define    INTCA_SCIFA1				(145+32)
//#define    Reserved				(146+32)
//#define    Reserved				(147+32)
#define    INTCA_SCIFB0				(148+32)
#define    INTCA_SCIFB1				(149+32)
#define    INTCA_SCIFB2				(150+32)
#define    INTCA_SCIFB3				(151+32)
#define    INTCA_SCIF0				(152+32)
#define    INTCA_SCIF1				(153+32)

#define    INTCA0_PINT1				(33+32)
#define    INTCA0_INTCS				(50+32)
#define    INTCSYS_IIC0				(174+32)
#define    INTCSYS_IIC1				(175+32)
#define    INTCSYS_IIC2				(176+32)
#define    INTCSYS_IIC3				(173+32)

#define    INTCSYS_I2C0				(287+32)
#define    INTCSYS_I2C1				(288+32)
#define    INTCSYS_I2C2				(286+32)
#define    INTCSYS_I2C3				(290+32)


/* I2C */
#define IIC0_BADR			(0xE6820000)
#define	ICDR0_ADDR			(IIC0_BADR+0x0000)
#define	ICCR0_ADDR			(IIC0_BADR+0x0004)
#define	ICSR0_ADDR			(IIC0_BADR+0x0008)
#define	ICIC0_ADDR			(IIC0_BADR+0x000C)
#define	ICCL0_ADDR			(IIC0_BADR+0x0010)
#define	ICCH0_ADDR			(IIC0_BADR+0x0014)
#define	ICTC0_ADDR			(IIC0_BADR+0x0028)

#define IIC1_BADR			(0xE6822000)
#define	ICDR1_ADDR			(IIC1_BADR+0x0000)
#define	ICCR1_ADDR			(IIC1_BADR+0x0004)
#define	ICSR1_ADDR			(IIC1_BADR+0x0008)
#define	ICIC1_ADDR			(IIC1_BADR+0x000C)
#define	ICCL1_ADDR			(IIC1_BADR+0x0010)
#define	ICCH1_ADDR			(IIC1_BADR+0x0014)
#define	ICTC1_ADDR			(IIC1_BADR+0x0028)

#define IIC2_BADR			(0xE6824000)
#define	ICDR2_ADDR			(IIC2_BADR+0x0000)
#define	ICCR2_ADDR			(IIC2_BADR+0x0004)
#define	ICSR2_ADDR			(IIC2_BADR+0x0008)
#define	ICIC2_ADDR			(IIC2_BADR+0x000C)
#define	ICCL2_ADDR			(IIC2_BADR+0x0010)
#define	ICCH2_ADDR			(IIC2_BADR+0x0014)
#define	ICTC2_ADDR			(IIC2_BADR+0x0028)

#define PINTADR				0xE60580C0
#define PINTCDR				0xE60580c4

/*Watchdog Timer */
#define		RWTCNT0			0xE6020000
//#define		RWTCSRA0	0xE6020004
#define		WDOG_CNT_OFF	(RWTCNT0-RWTCNT0)
#define		WDOG_CSRA_OFF	(RWTCSRA0-RWTCNT0)
#define		WDOG_CSRB_OFF	(RWTCSRB0-RWTCNT0)
#define 	RWT_TME		(1<<7)
#define 	RWT_WOVF	(1<<4)


#define IRQC0_BASE	0xe61c0000
#define IRQC0_INTREQ_STS0	(IRQC0_BASE + 0x000)	/* R */
#define IRQC0_INTEN_STS0	(IRQC0_BASE + 0x004)	/* R/WC1 */
#define IRQC0_INTEN_SET0	(IRQC0_BASE + 0x008)	/* W */
#define IRQC0_WAKREQ_STS0	(IRQC0_BASE + 0x080)	/* R */
#define IRQC0_WAKEN_STS0	(IRQC0_BASE + 0x084)	/* R/WC1 */
#define IRQC0_WAKEN_SET0	(IRQC0_BASE + 0x088)	/* W */
#define IRQC0_DETECT_STATUS	(IRQC0_BASE + 0x100)	/* R/WC1 */
#define IRQC0_CONFIG_00		(IRQC0_BASE + 0x180)	/* R/W */

#define IRQC1_BASE	0xe61c0200
#define IRQC1_INTREQ_STS0	(IRQC1_BASE + 0x000)	/* R */
#define IRQC1_INTEN_STS0	(IRQC1_BASE + 0x004)	/* R/WC1 */
#define IRQC1_INTEN_SET0	(IRQC1_BASE + 0x008)	/* W */
#define IRQC1_WAKREQ_STS0	(IRQC1_BASE + 0x080)	/* R */
#define IRQC1_WAKEN_STS0	(IRQC1_BASE + 0x084)	/* R/WC1 */
#define IRQC1_WAKEN_SET0	(IRQC1_BASE + 0x088)	/* W */
#define IRQC1_DETECT_STATUS	(IRQC1_BASE + 0x100)	/* R/WC1 */
#define IRQC1_MONITOR_STATUS	(IRQC1_BASE + 0x128)
#define IRQC1_CONFIG_00		(IRQC1_BASE + 0x180)	/* R/W */

/*
 * -----------------------------------------------------------------------
 * Interrupt Distributor CPU interface
 * Register offsets from GIC_BASE
 * -----------------------------------------------------------------------
 */
#define GIC400				0xF1000000
#define	GIC_CPU_BASE		0x2000
#define	GIC_DIST_BASE		0x1000

/*
 * PFC
 */
 
 //register offset
#define PFC_PMMR_OFFSET           0x0000
#define PFC_GPSR0_OFFSET          0x0004
#define PFC_GPSR1_OFFSET          0x0008
#define PFC_GPSR2_OFFSET          0x000C
#define PFC_GPSR3_OFFSET          0x0010
#define PFC_GPSR4_OFFSET          0x0014
#define PFC_IPSR0_OFFSET          0x0020
#define PFC_IPSR1_OFFSET          0x0024
#define PFC_IPSR2_OFFSET          0x0028
#define PFC_IPSR3_OFFSET          0x002C
#define PFC_IPSR4_OFFSET          0x0030
#define PFC_IPSR5_OFFSET          0x0034
#define PFC_IPSR6_OFFSET          0x0038
#define PFC_IPSR7_OFFSET          0x003C
#define PFC_IPSR8_OFFSET          0x0040
#define PFC_IPSR9_OFFSET          0x0044
#define PFC_IPSR10_OFFSET         0x0048
#define PFC_MOD_SEL_OFFSET        0x0050
#define PFC_MOD_SEL2_OFFSET       0x0054
#define PFC_PUPR0_OFFSET          0x0100
#define PFC_PUPR1_OFFSET          0x0104
#define PFC_PUPR2_OFFSET          0x0108
#define PFC_PUPR3_OFFSET          0x010C
#define PFC_PUPR4_OFFSET          0x0110
#define PFC_PUPR5_OFFSET          0x0114
#define PFC_IOCTRL0_OFFSET        0x0060
#define PFC_IOCTRL1_OFFSET        0x0064
#define PFC_IOCTRL2_OFFSET        0x0068
#define PFC_IOCTRL3_OFFSET        0x006C
#define PFC_TSREG_OFFSET          0x0080
 
#define		PFC_PMMR				0xE6060000	

#define		PFC_GPSR0				0xE6060004	
#define		PFC_GPSR1				0xE6060008	
#define		PFC_GPSR2				0xE606000C	
#define		PFC_GPSR3				0xE6060010	
#define		PFC_GPSR4				0xE6060014	
#define		PFC_GPSR5				0xE6060018	
#define		PFC_GPSR6				0xE606001C	

#define		PFC_IPSR0				0xE6060020	
#define		PFC_IPSR1				0xE6060024	
#define		PFC_IPSR2				0xE6060028	
#define		PFC_IPSR3				0xE606002C	
#define		PFC_IPSR4				0xE6060030	
#define		PFC_IPSR5				0xE6060034	
#define		PFC_IPSR6				0xE6060038	
#define		PFC_IPSR7				0xE606003C	
#define		PFC_IPSR8				0xE6060040	
#define		PFC_IPSR9				0xE6060044	
#define		PFC_IPSR10				0xE6060048	
#define		PFC_IPSR11				0xE606004C	
#define		PFC_IPSR12				0xE6060050	
#define		PFC_IPSR13				0xE6060054	
#define		PFC_IPSR14				0xE6060058	
#define		PFC_IPSR15				0xE606005c	
#define		PFC_IPSR16				0xE6060160	

#define		PFC_MOD_SEL				0xE6060090	
#define		PFC_MOD_SEL2			0xE6060094	
#define		PFC_MOD_SEL3			0xE6060098	
#define		PFC_PUPR0				0xE6060100	
#define		PFC_PUPR1				0xE6060104	
#define		PFC_PUPR2				0xE6060108	
#define		PFC_PUPR3				0xE606010C	
#define		PFC_PUPR4				0xE6060110	
#define		PFC_PUPR5				0xE6060114	
#define		PFC_PUPR6				0xE6060118	
#define		PFC_IOCTRL				0xE6060070	
#define		PFC_IOCTRL0				0xE6060060	
#define		PFC_IOCTRL1				0xE6060064	
#define		PFC_IOCTRL4				0xE6060084	
#define		PFC_IOCTRL5				0xE6060088	
#define		PFC_IOCTRL6				0xE606008C	

#define		PFC_DDR3GPEN			0xE6060240	
#define		PFC_DDR3GPOE			0xE6060244	
#define		PFC_DDR3GPOD			0xE6060248	
#define		PFC_DDR3GPID			0xE606024C	

#define GPIO_REGSIZE               0x00005000
 
#define GPIO2_REGBASE              (0xE6052000)
 //register offset
#define GPIO_IOINTSEL_OFFSET      0x0000
#define GPIO_INOUTSEL_OFFSET      0x0004
#define GPIO_OUTDT_OFFSET         0x0008
#define GPIO_INDT_OFFSET          0x000C
#define GPIO_INTDT_OFFSET         0x0010
#define GPIO_INTCLR_OFFSET        0x0014
#define GPIO_INTMSK_OFFSET        0x0018
#define GPIO_MSKCLR_OFFSET        0x001C
#define GPIO_POSNEG_OFFSET        0x0020
#define GPIO_EDGLEVEL_OFFSET      0x0024
#define GPIO_FILONOFF_OFFSET      0x0028

#define 	GPIO_IOINTSEL2		(GPIO2_REGBASE+GPIO_IOINTSEL_OFFSET)
#define 	GPIO_INOUTSEL2		(GPIO2_REGBASE+GPIO_INOUTSEL_OFFSET)
#define 	GPIO_OUTDT2			(GPIO2_REGBASE+GPIO_OUTDT_OFFSET)
#define 	GPIO_POSNEG2		(GPIO2_REGBASE+GPIO_POSNEG_OFFSET)
	
#define 	GPIO5_REGBASE		0xE6055000
#define 	GPIO_IOINTSEL5		(GPIO_BASE + 0x5000)
#define 	GPIO_INOUTSEL5		(GPIO_BASE + 0x5004)
#define 	GPIO_OUTDT5			(GPIO_BASE + 0x5008)
#define 	GPIO_INDT5			(GPIO_BASE + 0x500C)
#define 	GPIO_INTDT5			(GPIO_BASE + 0x5010)
#define 	GPIO_INTCLR5		(GPIO_BASE + 0x5014)
#define 	GPIO_INTMSK5		(GPIO_BASE + 0x5018)
#define 	GPIO_MSKCLR5		(GPIO_BASE + 0x501C)
#define 	GPIO_POSNEG5		(GPIO_BASE + 0x5020)
#define 	GPIO_EDGLEVEL5		(GPIO_BASE + 0x5024)
#define 	GPIO_FILONOFF5		(GPIO_BASE + 0x5028)
#define 	GPIO_INTMSKS5		(GPIO_BASE + 0x5038)
#define 	GPIO_MSKCLRS5		(GPIO_BASE + 0x503C)
#define 	GPIO_OUTDTSEL5		(GPIO_BASE + 0x5040)
#define 	GPIO_OUTDTH5		(GPIO_BASE + 0x5044)
#define 	GPIO_OUTDTL5		(GPIO_BASE + 0x5048)
#define 	GPIO_BOTHEDGE5		(GPIO_BASE + 0x504C)

#define 	SH_BRG_DL			0x30
#define 	SH_BRG_CKS			0x34
#define 	SH_SCIF_FIFO_LEN	16

#define		CPG_SMSTPCR0		0xE6150130
#define		CPG_SMSTPCR1		0xE6150134
#define		CPG_SMSTPCR2		0xE6150138
#define		CPG_SMSTPCR3		0xE615013c
#define		CPG_SMSTPCR4		0xE6150140
#define		CPG_SMSTPCR5		0xE6150144
#define		CPG_SMSTPCR6		0xE6150148
#define		CPG_SMSTPCR7		0xE615014c
#define		CPG_SMSTPCR8		0xE6150990
#define		CPG_SMSTPCR9		0xE6150994
#define		CPG_SMSTPCR10		0xE6150998
#define		CPG_SMSTPCR11		0xE615099c

#define		CPG_SRCR0				0xE61500a0
#define		CPG_SRCR1				0xE61500a8
#define		CPG_SRCR2				0xE61500b0
#define		CPG_SRCR3				0xE61500b8
#define		CPG_SRCR4				0xE61500bc
#define		CPG_SRCR5				0xE61500c4
#define		CPG_SRCR6				0xE61501c8
#define		CPG_SRCR7				0xE61501cc
#define		CPG_SRCR8				0xE6150920
#define		CPG_SRCR9				0xE6150924
#define		CPG_SRCR10				0xE6150928
#define		CPG_SRCR11				0xE615092c

#define		SRSTCLR0				0xE6150940
#define		SRSTCLR1				0xE6150944
#define		SRSTCLR2				0xE6150948
#define		SRSTCLR3				0xE615094c
#define		SRSTCLR4				0xE6150950
#define		SRSTCLR5				0xE6150954
#define		SRSTCLR6				0xE6150958
#define		SRSTCLR7				0xE615095c
#define		SRSTCLR8				0xE6150960
#define		SRSTCLR9				0xE6150964
#define		SRSTCLR10				0xE6150968
#define		SRSTCLR11				0xE615096c

#define		CPG_FRQMR1				0xFFC80014	
#define		CPG_MSTPCR0				0xFFC80030	
#define		CPG_MSTPCR1				0xE6150114	
#define		CPG_MSTPCR3				0xFFC8003C	
#define		CPG_MSTPCR4				0xFFC80050	
#define		CPG_MSTPCR5				0xFFC80054	
#define		CPG_MSTPCR6				0xFFC80058	
#define		CPG_MSTPCR7				0xFFC80040	

#define		CPG_MSTPSR1				0xFFC80044	
#define		CPG_MSTPSR4				0xFFC80048	
#define		CPG_MSTPSR6				0xFFC8004C	


#define		CPG_SH4AIFC				0xFE400000	
#define		CPG_SH4ASTBCR			0xFE400004	



#define		RST_WDT_WDTST			0xFFCC0000	
#define		RST_WDT_WDTCSR			0xFFCC0004	
#define		RST_WDT_WDTBST			0xFFCC0008	
#define		RST_WDT_WDTCNT			0xFFCC0010	
#define		RST_WDT_WDTBCNT			0xFFCC0018	
#define		RST_WDT_MODEMR			0xFFCC0020	
#define		RST_WDT_MRST0			0xFFCC0024	
#define		RST_WDT_MRST1			0xFFCC0028	
#define		RST_WDT_MRST2			0xFFCC002C	
#define		RST_WDT_MRST4			0xFFCC0034	
#define		RST_WDT_MRST5			0xFFCC0038	
#define		RST_WDT_MRST6			0xFFCC003c	
#define		RST_WDT_MRST7			0xFFCC0040	
#define		RST_WDT_MRST8			0xFFCC0044	
#define		RST_WDT_SH4AWDTST		0xFE400080	
#define		RST_WDT_SH4AWDTCSR		0xFE400084	
#define		RST_WDT_SH4AWDTBST		0xFE400088	
#define		RST_WDT_SH4AWDTCNT		0xFE400090	
#define		RST_WDT_SH4AWDTBCNT		0xFE400098	
#define		RST_WDT_SH4ARESETVEC	0xFE400008	

//++++++++ USB Module  ++++++++++++++++//
#define 	BASE_USBCTR		0xE6590000

#define 	HSUSB_SYSCFG		BASE_USBCTR + 0x0000
#define 	HSUSB_BUSWAIT		BASE_USBCTR + 0x0002
#define 	HSUSB_SYSSTS		BASE_USBCTR + 0x0004
#define 	HSUSB_DVSTCTR		BASE_USBCTR + 0x0008
#define 	HSUSB_TESTMODE		BASE_USBCTR + 0x000c

#define 	HSUSB_INTENB0		BASE_USBCTR + 0x0030
#define 	HSUSB_INTSTS0		BASE_USBCTR + 0x0040

#define 	HSUSB_UGCTRL		BASE_USBCTR + 0x0180
#define 	HSUSB_UGCTRL2		BASE_USBCTR + 0x0184
#define 	HSUSB_UGSTS		BASE_USBCTR + 0x0190

/* Watch dog timer */
#define WDTRSTCR	0xE6160054 
#define WTCNT_OFF	(0x00)
#define WTCSRA_OFF	(0x04)
#define WTCSRB_OFF	(0x08)

#define WTCSRA_WRFLG	(1 << 5)
#define WTCSRA_WOVF	(1 << 4)
#define WTCSRA_TME		(1 << 7)	
 
 /* SPI numbers for ARM*/
#define SPI_IRQ0					0x00+32			
#define SPI_IRQ1					0x01+32
#define SPI_IRQ2					0x02+32
#define SPI_IRQ3					0x03+32
#define SPI_GPIO0					0x04+32
#define SPI_GPIO1					0x05+32
#define SPI_GPIO2					0x06+32
#define SPI_GPIO3					0x07+32
#define SPI_GPIO4					0x08+32
#define SPI_GPIO5					0x09+32
#define SPI_MTSB0HS					0x0C+32
#define SPI_MTSB1HS					0x0D+32
#define SPI_MTSB2HS					0x0E+32
#define SPI_MTSB3HS					0x0F+32
#define SPI_MTSB4HS					0x10+32
#define SPI_MTSB5HS					0x11+32
#define SPI_MTSB6HS					0x12+32
#define SPI_MTSB7HS					0x13+32
#define SPI_MTSB8HS					0x14+32
#define SPI_MTSB9HS					0x15+32
#define SPI_MTSB10HS				0x16+32
#define SPI_MTSB11HS				0x17+32
#define SPI_MTSB12HS				0x18+32
#define SPI_MTSB13HS				0x19+32
#define SPI_MTSB14HS				0x1A+32
#define SPI_MTSB15HS				0x1B+32
#define SPI_MTSB16HS				0x1C+32
#define SPI_MTSB17HS				0x1D+32
#define SPI_MTSB18HS				0x1E+32
#define SPI_MTSB19HS				0x1F+32
#define SPI_MTSB0LS					0x20+32
#define SPI_MTSB1LS					0x21+32
#define SPI_MTSB2LS					0x22+32
#define SPI_MTSB3LS					0x23+32
#define SPI_MTSB4LS					0x24+32
#define SPI_MTSB5LS					0x25+32
#define SPI_MTSB6LS					0x26+32
#define SPI_MTSB7LS					0x27+32
#define SPI_MTSB8LS					0x28+32
#define SPI_MTSB9LS					0x29+32
#define SPI_MTSB10LS				0x2A+32
#define SPI_MTSB11LS				0x2B+32
#define SPI_MTSB12LS				0x2C+32
#define SPI_MTSB13LS				0x2D+32
#define SPI_MTSB14LS				0x2E+32
#define SPI_MTSB15LS				0x2F+32
#define SPI_MTSB16LS				0x30+32
#define SPI_MTSB17LS				0x31+32
#define SPI_MTSB18LS				0x32+32
#define SPI_MTSB19LS				0x33+32
#define SPI_MTSB20LS				0x34+32
#define SPI_MTSB21LS				0x35+32
#define SPI_MTSB22LS				0x36+32
#define SPI_MTSB23LS				0x37+32
#define SPI_MTSB24LS				0x38+32
#define SPI_MTSB25LS				0x39+32
#define SPI_MTSB26LS				0x3A+32
#define SPI_MTSB27LS				0x3B+32
#define SPI_MTSB28LS				0x3C+32
#define SPI_MTSB29LS				0x3D+32
#define SPI_MTSB30LS				0x3E+32
#define SPI_MTSB31LS				0x3F+32
//#define SPI_Reserved				0x40+32
//#define SPI_Reserved				0x41+32
//#define SPI_Reserved				0x42+32
//#define SPI_Reserved				0x43+32
//#define SPI_Reserved				0x44+32
#define Thermal_Sensor				0x45+32
#define SPI_CC4_2_0					0x46+32
#define SPI_CC4_2_1					0x47+32
#define SPI_CA15_IRQPNU_CORE0		0x48+32
#define SPI_CA15_IRQPNU_CORE1 		0x49+32
#define SPI_CA15_IRQPNU_CORE2		0x4A+32
#define SPI_CA15_IRQPNU_CORE3		0x4B+32
#define CA15_CTIIRQ_CORE0			0x4C+32
#define CA15_CTIIRQ_CORE1 			0x4D+32
#define CA15_CTIIRQ_CORE2 			0x4E+32
#define CA15_CTIIRQ_CORE3 			0x4F+32
#define SPI_CA15_nINTERRIRQ 		0x50+32
#define SPI_CA15_nAXIERRIRQ			0x51+32
#define SPI_CA7_IRQPNU_CORE0		0x52+32
#define SPI_CA7_IRQPNU_CORE1		0x53+32
#define SPI_CA7_IRQPNU_CORE2		0x54+32
#define SPI_CA7_IRQPNU_CORE3		0x55+32
#define CA7_CTIIRQ_CORE0			0x56+32
#define CA7_CTIIRQ_CORE1			0x57+32
#define CA7_CTIIRQ_CORE2			0x58+32
#define CA7_CTIIRQ_CORE3			0x59+32
//#define SPI_Reserved				0x5A+32
#define SPI_CA7_nAXIERRIRQ			0x5B+32
#define SPI_CCI400					0x5C+32
#define SPI_CCI400_OVF				0x5D+32
#define SPI_TDBG					0x5E+32
#define SPI_HUDI					0x5F+32
#define SPI_LBSC_WT0				0x60+32
#define SPI_LBSC_ATA				0x61+32
#define SPI_LBSC_DMAC0				0x62+32
#define SPI_LBSC_DMAC1				0x63+32
#define SPI_LBSC_DMAC2				0x64+32
#define SPI_USB3_0_HOST				0x65+32
//#define SPI_Reserved				0x66+32
//#define SPI_Reserved				0x67+32
//#define SPI_Reserved				0x69+32
#define SPI_SATA0					0x69+32
#define SPI_SATA1					0x6A+32
#define SPI_USB2_0_597				0x6B+32
#define SPI_USB2_0_EHCI_0			0x6C+32
#define SPI_USBHSDMAC1				0x6D+32
#define SPI_USBHSDMAC2				0x6D+32
#define SPI_DDM						0x6F+32
#define SPI_USB2_0_EHCI_1			0x70+32
#define SPI_USB2_0_EHCI_2			0x71+32
#define SPI_R_GP2					0x72+32
//#define SPI_Reserved				0x73+32
#define SPI_PCIe					0x74+32
#define SPI_PCIe_DMA				0x75+32
#define SPI_PCIe_ERROR				0x76+32
#define SPI_SGX_3DG					0x77+32
#define SPI_CMT1_0					0x78+32
#define SPI_CMT1_1					0x79+32
#define SPI_CMT1_2					0x7A+32
#define SPI_CMT1_3					0x7B+32
#define SPI_CMT1_4					0x7C+32
#define SPI_CMT1_5					0x7D+32
#define SPI_CMT1_6					0x7E+32
#define SPI_CMT1_7					0x7F+32
#define SPI_TMU1_TUNI0				0x80+32
#define SPI_TMU1_TUNI1				0x81+32
#define SPI_TMU1_TUNI2				0x82+32
#define SPI_TMU3_TUNI0				0x83+32
#define SPI_TMU3_TUNI1				0x84+32
#define SPI_TMU3_TUNI2				0x85+32
#define SPI_SECURE_TIMER			0x86+32
#define SPI_TPU0					0x87+32
#define SPI_TMU0_TUNI0				0x88+32
#define SPI_TMU0_TUNI1				0x89+32
#define SPI_TMU0_TUNI2				0x8A+32
#define SPI_SECURE_UP_TIMER			0x8B+32
#define SPI_RWDT					0x8C+32
#define SPI_SECURE_RWDT				0x8D+32
#define SPI_CMT0_0					0x8E+32

#define SPI_SCIFA0					0x90+32
#define SPI_SCIFA1					0x91+32
//#define SPI_Reserved				0x92+32
#define SPI_TMU1_TUNI3				0x93+32
#define SPI_SCIFB0					0x94+32
#define SPI_SCIFB1					0x95+32
#define SPI_SCIFB2					0x96+32
#define SPI_SCIFA2					0x97+32
#define SPI_SCIF0					152+32
#define SPI_SCIF1					153+32
#define SPI_HSCIF0					154+32
#define SPI_HSCIF1					155+32
#define SPI_MSIOF0					156+32
#define SPI_MSIOF1					157+32
#define SPI_MSIOF2					158+32
#define SPI_MSIOF3					159+32
//#define SPI_Reserved					160+32
//#define SPI_Reserved					161+32
#define SPI_ETHER_Fast				162+32
#define SPI_ETHER_AVB				163+32
#define SPI_SCIF2					164+32
#define SPI_SDHI0					165+32
#define SPI_SDHI1					166+32
#define SPI_SDHI2					167+32
#define SPI_SDHI3					168+32
#define SPI_MMC0					169+32
#define SPI_MMC1					170+32
#define SPI_MFIS					171+32
#define SPI_MFIS_CA7_Core0			172+32
#define SPI_IIC_DVFS				173+32
#define SPI_IIC0					174+32
#define SPI_IIC1					175+32
#define SPI_IIC2					176+32
#define SPI_MFIS_CA7_Core1			177+32
#define SPI_MFIS_CA7_Core2			178+32
#define SPI_MFIS_CA7_Core3			179+32
#define SPI_MFIS_CA15_Core0			180+32
#define SPI_MFIS_CA15_Core1			181+32
#define SPI_MFIS_CA15_Core2			182+32
#define SPI_MFIS_CA15_Core3			183+32
#define SPI_QSPI					184+32
#define SPI_STB						185+32
#define SPI_RCAN0					186+32
#define SPI_RCAN1					187+32
#define SPI_VIN0					188+32
#define SPI_VIN1					189+32
#define SPI_VIN2					190+32
#define SPI_VIN3					191+32
#define SPI_IMR_X2_0				192+32
#define SPI_IMR_X2_1				193+32
#define SPI_IMR_LSX_0				194+32
#define SPI_IMR_LSX_1				195+32
#define SPI_IMP_X4					196+32
#define SPI_SYSDMAC0_ERR			197+32
#define SPI_IPMMUDS0				198+32
#define SPI_IPMMUDS0_SEC			199+32
#define SPI_SYSDMAC_DEI0			200+32
#define SPI_SYSDMAC_DEI1			201+32
#define SPI_SYSDMAC_DEI2			202+32
#define SPI_SYSDMAC_DEI3			203+32
#define SPI_SYSDMAC_DEI4			204+32
#define SPI_SYSDMAC_DEI5			205+32
#define SPI_SYSDMAC_DEI6			206+32
#define SPI_SYSDMAC_DEI7			207+32
#define SPI_SYSDMAC_DEI8			208+32
#define SPI_SYSDMAC_DEI9			209+32
#define SPI_SYSDMAC_DEI10			210+32
#define SPI_SYSDMAC_DEI11			211+32
#define SPI_SYSDMAC_DEI12			212+32
#define SPI_SYSDMAC_DEI13			213+32
#define SPI_SYSDMAC_DEI14			214+32
//#define SPI_Reserved				215+32
#define SPI_SYSDMAC_DEI15			216+32
#define SPI_SYSDMAC_DEI16			217+32
#define SPI_SYSDMAC_DEI17			218+32
#define SPI_SYSDMAC_DEI18			219+32
#define SPI_SYSDMAC1_ERR			220+32


#define SPI_I2C2	 			286+32
#define SPI_I2C0	 			287+32
#define SPI_I2C1	 			288+32
#define SPI_I2C3	 			290+32

#define SPI_AUDIODMAC_DEI0 		(320+32)
#define SPI_AUDIODMAC_DEI1 		(321+32)

/* QSPI */
#define	QSPI_BASE		0xE6B10000
#define	QSPI_SPCR		(QSPI_BASE + 0x00)	// Control register
#define	QSPI_SSLP		(QSPI_BASE + 0x01)	// Slave select polarity register
#define	QSPI_SPPCR		(QSPI_BASE + 0x02)	// Pin control register
#define	QSPI_SPSR		(QSPI_BASE + 0x03)	// Status register
#define	QSPI_SPDR		(QSPI_BASE + 0x04)	// Data register
#define	QSPI_SPSCR		(QSPI_BASE + 0x08)	// Sequence control register
#define	QSPI_SPSSR		(QSPI_BASE + 0x09)	// Sequence status register
#define	QSPI_SPBR		(QSPI_BASE + 0x0A)	// Bit rate register
#define	QSPI_SPDCR		(QSPI_BASE + 0x0B)	// Data control register
#define	QSPI_SPCKD		(QSPI_BASE + 0x0C)	// Clock delay register
#define	QSPI_SSLND		(QSPI_BASE + 0x0D)	// Slave select negation delay register
#define	QSPI_SPND		(QSPI_BASE + 0x0E)	// Next-access delay register
#define	QSPI_SPCMD0		(QSPI_BASE + 0x10)	// Command register 0
#define	QSPI_SPCMD1		(QSPI_BASE + 0x12)	// Command register 1
#define	QSPI_SPCMD2		(QSPI_BASE + 0x14)	// Command register 2
#define	QSPI_SPCMD3		(QSPI_BASE + 0x16)	// Command register 3
#define	QSPI_SPBFCR		(QSPI_BASE + 0x18)	// Buffer control register
#define	QSPI_SPBDCR		(QSPI_BASE + 0x1A)	// Buffer data count register
#define	QSPI_SPBMUL0	(QSPI_BASE + 0x1C)	// Transfer data length multiplier setting register 0
#define	QSPI_SPBMUL1	(QSPI_BASE + 0x20)	// Transfer data length multiplier setting register 1
#define	QSPI_SPBMUL2	(QSPI_BASE + 0x24)	// Transfer data length multiplier setting register 2
#define	QSPI_SPBMUL3	(QSPI_BASE + 0x28)	// Transfer data length multiplier setting register 3

/* SCIF0 */
#define	SCIF0_BASE		0xE6E60000

#define	SCIF0_SCSMR		(SCIF0_BASE + 0x00)		// 16
#define	SCIF0_SCBRR		(SCIF0_BASE + 0x04)		// 8
#define	SCIF0_SCSCR		(SCIF0_BASE + 0x08)		// 16
#define	SCIF0_SCFTDR		(SCIF0_BASE + 0x0C)		// 8
#define	SCIF0_SCFSR		(SCIF0_BASE + 0x10)		// 16
#define	SCIF0_SCFRDR		(SCIF0_BASE + 0x14)		// 8
#define	SCIF0_SCFCR		(SCIF0_BASE + 0x18)		// 16
#define	SCIF0_SCFDR		(SCIF0_BASE + 0x1C)		// 16
#define	SCIF0_SCSPTR		(SCIF0_BASE + 0x20)		// 16
#define	SCIF0_SCLSR		(SCIF0_BASE + 0x24)		// 16
#define	SCIF0_DL			(SCIF0_BASE + 0x30)		// 16
#define	SCIF0_CKS		(SCIF0_BASE + 0x34)		// 16
#define	SCIF0_SCFER		(SCIF0_BASE + 0x44)		// 16

#endif /* _REG_H */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/arm/renesas_reg.h $ $Rev: 758169 $")
#endif
