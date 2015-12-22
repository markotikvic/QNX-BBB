/*
 * $QNXLicenseC:
 * Copyright 2007, 2008, QNX Software Systems. 
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


#ifndef	_OMAP4_MMC_INCLUDED
#define	_OMAP4_MMC_INCLUDED

#define OMAP_L3INIT_PRM_SIZE					0x100
#define L3INIT_PRM_BASE							0x4a307300

#define RM_L3INIT_MMC_CONTEXT(_base)			( (_base) == OMAP_MMCHS1_BASE ? RM_L3INIT_MMC1_CONTEXT_BASE : RM_L3INIT_MMC2_CONTEXT_BASE )
#define RM_L3INIT_MMC1_CONTEXT_BASE				0x0000002c
#define RM_L3INIT_MMC2_CONTEXT_BASE				0x00000034
	#define RM_L3INIT_MMC_CONTEXT_LOSTCONTEXT_RFF	0x2

#define OMAP_L3INIT_CM2_SIZE					0x100

#define L3INIT_CM2_BASE							0x4a009300
#define CM_L3INIT_CLKSTCTRL						0x00000000
	#define CM_L3INIT_CLKSTCTRL_MSK					0x03
	#define CM_L3INIT_CLKSTCTRL_HW_AUTO				0x03
	#define CM_L3INIT_CLKSTCTRL_SW_WKUP				0x02
	#define CM_L3INIT_CLKSTCTRL_SW_SLEEP			0x01

#define CM_L3INIT_HSMMC_CLKCTRL(_base)			( (_base) == OMAP_MMCHS1_BASE ? CM_L3INIT_HSMMC1_CLKCTRL_BASE : CM_L3INIT_HSMMC2_CLKCTRL_BASE )
#define CM_L3INIT_HSMMC1_CLKCTRL_BASE			0x00000028
#define CM_L3INIT_HSMMC2_CLKCTRL_BASE			0x00000030
	#define CM_L3INIT_CLKCTRL_CKLSEL_96			0x01000000
	#define CM_L3INIT_CLKCTRL_STBYST			0x00040000
	#define CM_L3INIT_CLKCTRL_IDLEST_MSK		0x00030000
	#define CM_L3INIT_CLKCTRL_MODULEMODE_MSK	0x00000003
	#define CM_L3INIT_CLKCTRL_MODULEMODE_ENB	0x00000002

// MMCHS base address, irq
#define OMAP_MMCHS1_BASE		0x4809c000
#define OMAP_MMCHS2_BASE		0x480b4000
#define OMAP_MMCHS3_BASE		0x480ad000
#define OMAP_MMCHS4_BASE		0x480d1000
#define OMAP_MMCHS5_BASE		0x480d5000

#define	OMAP_MMCHS1_IRQ			83
#define	OMAP_MMCHS2_IRQ			86
#define	OMAP_MMCHS3_IRQ			94

#define OMAP_MMCHS_CLK_DFLT		96000000

#define	OMAP_MMCHS_SIZE			512

#define	MMCHS_FIFO_SIZE			512

// Register Descriptions
#define	OMAP_MMCHS_HL_REV				0x000	// (OMAP 4 only)
#define	OMAP_MMCHS_HL_HWINFO			0x004	// (OMAP 4 only)
	#define HWINFO_MADMA_EN				0x01
#define	OMAP_MMCHS_HL_SYSCONFIG			0x010	// (OMAP 4 only)
	#define HL_SYSCONFIG_IDLEMODE_MSK	0x0c
	#define HL_SYSCONFIG_IDLEMODE_SMART	0x08

#define	OMAP_MMCHS_SYSCONFIG	0x110	// System Configuration Register
	#define	SYSCONFIG_STANDBYMODE_MSK			(3 << 12)
	#define	SYSCONFIG_STANDBYMODE_SMART_WAKEUP	(3 << 12)
	#define	SYSCONFIG_STANDBYMODE_SMART			(2 << 12)
	#define	SYSCONFIG_STANDBYMODE_NOSTANBY		(1 << 12)

	#define	SYSCONFIG_CLOCKACTIVITY_MSK			(3 << 8)
	#define	SYSCONFIG_CLOCKACTIVITY_IFACE_FUNC	(3 << 8)
	#define	SYSCONFIG_CLOCKACTIVITY_FUNC		(2 << 8)
	#define	SYSCONFIG_CLOCKACTIVITY_IFACE		(1 << 8)

	#define	SYSCONFIG_SIDLEMODE_MSK				(3 << 3)
	#define	SYSCONFIG_SIDLEMODE_SMART_WAKEUP	(3 << 3)
	#define	SYSCONFIG_SIDLEMODE_SMART			(2 << 3)
	#define	SYSCONFIG_SIDLEMODE_IGNORE			(1 << 3)
	#define	SYSCONFIG_ENAWAKEUP					(1 << 2)	// Wakeup feature enable
	#define	SYSCONFIG_SOFTRESET					(1 << 1)	// Software reset
	#define	SYSCONFIG_AUTOIDLE					(1 << 0)	// Clock auto idle

	#define SYSCONFIG_CONFIG				(	SYSCONFIG_AUTOIDLE |				\
												SYSCONFIG_ENAWAKEUP |				\
												SYSCONFIG_SIDLEMODE_SMART_WAKEUP |	\
												SYSCONFIG_CLOCKACTIVITY_FUNC |		\
												SYSCONFIG_STANDBYMODE_SMART_WAKEUP )

	#define SYSCONFIG_CONFIG_MSK			(	SYSCONFIG_AUTOIDLE |			\
												SYSCONFIG_ENAWAKEUP |			\
												SYSCONFIG_SIDLEMODE_MSK |		\
												SYSCONFIG_CLOCKACTIVITY_MSK |	\
												SYSCONFIG_STANDBYMODE_MSK )

#define	OMAP_MMCHS_SYSSTATUS	0x114	// System Status Register
	#define	SYSSTATUS_RESETDONE	(1 << 0)	// Reset done

#define	OMAP_MMCHS_CSRE			0x124	// Card status response error
#define	OMAP_MMCHS_SYSTEST		0x128	// System Test Register
#define	OMAP_MMCHS_CON			0x12C	// Configuration register
	#define	CON_OD				(1 <<  0)	// Card Open Drain Mode
	#define	CON_INIT			(1 <<  1)	// Send initialization stream
	#define	CON_DW8				(1 <<  5)	// MMC 8-bit width
	#define CON_DEBOUNCE_1MS	(2 <<  9)
	#define CON_DEBOUNCE_8MS	(3 <<  9)
	#define	CON_PADEN			(1 << 15)	// Control Power for MMC Lines
	#define	CON_CLKEXTFREE		(1 << 16)	// External clock free running
	#define CON_DDR				(1 << 19)	// Dual Data Rate mode (omap 4)
	#define CON_DMA_MNS			(1 << 20)	// DMA Master Select (omap 4)

#define	OMAP_MMCHS_PWCNT		0x130	// Power counter register
#define	OMAP_MMCHS_BLK			0x204	// Transfer Length Configuration register
#define	OMAP_MMCHS_ARG			0x208	// Command argument Register
#define	OMAP_MMCHS_CMD			0x20C	// Command and transfer mode register
	#define	CMD_DE				(1 <<  0)	// DMA Enable
	#define	CMD_BCE				(1 <<  1)	// Block Count Enable
	#define	CMD_ACEN			(1 <<  2)	// Auto CMD12 Enable
	#define	CMD_DDIR			(1 <<  4)	// Data Transfer Direction Read
	#define	CMD_MBS				(1 <<  5)	// Multi Block Select
	#define	CMD_RSP_TYPE_136	(1 << 16)	// Response length 136 bit
	#define	CMD_RSP_TYPE_48		(2 << 16)	// Response length 48 bit
	#define	CMD_RSP_TYPE_48b	(3 << 16)	// Response length 48 bit with busy after response
	#define	CMD_CCCE			(1 << 19)	// Comamnd CRC Check Enable
	#define	CMD_CICE			(1 << 20)	// Comamnd Index Check Enable
	#define	CMD_DP				(1 << 21)	// Data Present
	#define	CMD_TYPE_CMD12		(3 << 22)	// CMD12 or CMD52 "I/O Abort"

#define	OMAP_MMCHS_RSP10		0x210	// Command response[31:0] Register
#define	OMAP_MMCHS_RSP32		0x214	// Command response[63:32] Register
#define	OMAP_MMCHS_RSP54		0x218	// Command response[95:64] Register
#define	OMAP_MMCHS_RSP76		0x21C	// Command response[127:96] Register
#define	OMAP_MMCHS_DATA			0x220	// Data Register
#define	OMAP_MMCHS_PSTATE		0x224	// Present state register
	#define	PSTATE_CMDI			(1 <<  0)	// Command inhibit (mmci_cmd).
	#define	PSTATE_DATI			(1 <<  1)	// Command inhibit (mmci_dat).
	#define	PSTATE_DLA			(1 <<  2)	// mmci_dat line active
	#define	PSTATE_WTA			(1 <<  8)	// Write Transfer Active
	#define	PSTATE_RTA			(1 <<  9)	// Read Transfer Active
	#define	PSTATE_BWE			(1 << 10)	// Buffer Write Ready
	#define	PSTATE_BRE			(1 << 11)	// Buffer Read Ready

#define	OMAP_MMCHS_HCTL			0x228	// Host Control register
	#define	HCTL_LED			(1 << 0)	// LED (not supported)
	#define	HCTL_DTW4			(1 << 1)	// Data transfer width 4 bit
	#define HCTL_HSE			(1 << 2)	// High Speed (omap 4)
	#define HCTL_DMAS_ADMA2		(2 << 3)	// DMA Select ADMA2 (omap 4)
	#define	HCTL_SDBP			(1 << 8)	// SD bus power
	#define	HCTL_SDVS1V8		(5 << 9)	// SD bus voltage 1.8V
	#define	HCTL_SDVS3V0		(6 << 9)	// SD bus voltage 3.0V
	#define	HCTL_SDVS3V3		(7 << 9)	// SD bus voltage 3.3V

#define	OMAP_MMCHS_SYSCTL		0x22C	// SD system control register
	#define	SYSCTL_ICE			(1 << 0)	// Internal Clock Enable
	#define	SYSCTL_ICS			(1 << 1)	// Internal Clock Stable
	#define	SYSCTL_CEN			(1 << 2)	// Clock Enable
	#define	SYSCTL_DTO_MASK		(0xF << 16)	// Data timeout counter value and busy timeout
	#define	SYSCTL_DTO_MAX		(0xE << 16)	// Timeout = TCF x 2^27
	#define	SYSCTL_SRA			(1 << 24)	// Software reset for all
	#define	SYSCTL_SRC			(1 << 25)	// Software reset for mmci_cmd line
	#define	SYSCTL_SRD			(1 << 26)	// Software reset for mmci_dat line

#define	OMAP_MMCHS_STAT			0x230	// Interrupt status register
#define	OMAP_MMCHS_IE			0x234	// Interrupt SD enable register
#define	OMAP_MMCHS_ISE			0x238	// Interrupt signal enable register
	#define	INTR_CC				(1 <<  0)	// Command Complete
	#define	INTR_TC				(1 <<  1)	// Transfer Complete
	#define	INTR_BGE			(1 <<  2)	// Block Gap Event
	#define INTR_DMA			(1 <<  3)	// DMA interupt
	#define	INTR_BWR			(1 <<  4)	// Buffer Write Ready interrupt
	#define	INTR_BRR			(1 <<  5)	// Buffer Read Ready interrupt
	#define INTR_CINS			(1 <<  6)	// Card Insertion
	#define INTR_CREM			(1 <<  7)	// Card Removal
	#define	INTR_CIRQ			(1 <<  8)	// Card interrupt
	#define	INTR_OBI			(1 <<  9)	// Out-Of-Band interrupt
	#define INTR_BSR			(1 << 10)	// Boot Status
	#define	INTR_ERRI			(1 << 15)	// Error interrupt
	#define	INTR_CTO			(1 << 16)	// Command Timeout error
	#define	INTR_CCRC			(1 << 17)	// Command CRC error
	#define	INTR_CEB			(1 << 18)	// Command End Bit error
	#define	INTR_CIE			(1 << 19)	// Command Index error
	#define	INTR_DTO			(1 << 20)	// Data Timeout error
	#define	INTR_DCRC			(1 << 21)	// Data CRC error
	#define	INTR_DEB			(1 << 22)	// Data End Bit error
	#define	INTR_ACE			(1 << 24)	// ACMD12 error
	#define INTR_ADMAE			(1 << 25)	// ADMA Error
	#define	INTR_CERR			(1 << 28)	// Card error
	#define	INTR_BADA			(1 << 29)	// Bad Access to Data Space
	#define INTR_DFLTS			0x110f8000
	#define INTR_ALL			0x33ff87ff // 0x117F8033

#define	OMAP_MMCHS_AC12			0x23C	// Auto CMD12 Error Status Register
#define	OMAP_MMCHS_CAPA			0x240	// Capabilities register
	#define CAPA_BIT64			(1 << 28)	// 64 bit System Bus Support
	#define	CAPA_VS1V8			(1 << 26)	// 1.8V
	#define	CAPA_VS3V0			(1 << 25)	// 3.0V
	#define	CAPA_VS3V3			(1 << 24)	// 3.3V
	#define CAPA_SRS			(1 << 23)	// Suspend/Resume Support
	#define CAPA_DS				(1 << 22)	// DMA Support
	#define CAPA_HS				(1 << 21)	// High Speed Support
	#define CAPA_AD2S			(1 << 19)	// ADMA2 Support

#define	OMAP_MMCHS_CUR_CAPA		0x248	// Maximum current capabilities Register

#define	OMAP_MMCHS_FE			0x250	// Force Event (OMAP 4 only)
#define	OMAP_MMCHS_ADMAES		0x254	// ADMA Error Status (OMAP 4 only)
#define	OMAP_MMCHS_ADMASAL		0x258	// ADMA System Address (OMAP 4 only)

#define	OMAP_MMCHS_REV			0x2FC	// Versions Register
	#define REV_SREV(_r)		( ( (_r) >> 16 ) & 0xff)
	#define REV_SREV_V1			0x0
	#define REV_SREV_V2			0x1


// SDMA registers
#define	OMAP_DMA4_BASE			0x4a056000
#define	OMAP_DMA4_SIZE			0x1000

#define	DMA4_IRQSTATUS(j)		(0x08 + (j) * 4)	// j = 0 - 3
#define	DMA4_IRQENABLE(j)		(0x18 + (j) * 4)	// j = 0 - 3
#define	DMA4_SYSSTATUS			(0x28)
#define	DMA4_OCP_SYSCONFIG		(0x2C)
#define	DMA4_CAPS_0				(0x64)
#define	DMA4_CAPS_2				(0x6C)
#define	DMA4_CAPS_3				(0x70)
#define	DMA4_CAPS_4				(0x74)
#define	DMA4_GCR				(0x78)

#define	DMA4_CH_BASE(i)			((i) * 0x60)	// i = 0 - 31
#define	DMA4_CCR				0x80
#define	DMA4_CLNK_CTRL			0x84
#define	DMA4_CICR				0x88
#define	DMA4_CSR				0x8C
	#define	DMA4_CSR_ERROR		((1 << 11) | (1 << 10) | (1 << 9) | (1 << 8))
#define	DMA4_CSDP				0x90
#define	DMA4_CEN				0x94
#define	DMA4_CFN				0x98
#define	DMA4_CSSA				0x9C
#define	DMA4_CDSA				0xA0
#define	DMA4_CSE				0xA4
#define	DMA4_CSF				0xA8
#define	DMA4_CDE				0xAC
#define	DMA4_CDF				0xB0
#define	DMA4_CSAC				0xB4
#define	DMA4_CDAC				0xB8
#define	DMA4_CCEN				0xBC
#define	DMA4_CCFN				0xC0
#define	DMA4_COLOR				0xC4
#define	DMA4_CCR_SYNCHRO_CONTROL(s)		(((s) & 0x1F) | (((s) >> 5) << 19))

#define	DMA4_MMC1_TX			61
#define	DMA4_MMC1_RX			62
#define	DMA4_MMC2_TX			47
#define	DMA4_MMC2_RX			48
#define	DMA4_MMC3_TX			77
#define	DMA4_MMC3_RX			78
#define	DMA4_MMC4_TX			56
#define	DMA4_MMC4_RX			57
#define	DMA4_MMC5_TX			58
#define	DMA4_MMC5_RX			59

// 32 bit ADMA descriptor defination
typedef struct _sdhci_adma32_t {
	uint16_t	attr;
	uint16_t	len;
	uint32_t	addr;
} sdhci_adma32_t;

#define SDHCI_ADMA2_VALID	(1 << 0)	// valid
#define SDHCI_ADMA2_END		(1 << 1)	// end of descriptor, transfer complete interrupt will be generated
#define SDHCI_ADMA2_INT		(1 << 2)	// generate DMA interrupt, will not be used
#define SDHCI_ADMA2_NOP		(0 << 4)	// no OP, go to the next desctiptor
#define SDHCI_ADMA2_TRAN	(2 << 4)	// transfer data
#define SDHCI_ADMA2_LINK	(3 << 4) 	// link to another descriptor

typedef	struct _omap_mmc {
	void			*hba;
	void			*bshdl;
#define OMAP_OFLAG_PM		0x01
#define OMAP_OFLAG_IDLE		0x00
#define OMAP_OFLAG_ACTIVE	0x02
	uint32_t		oflags;

	uint32_t		mmc_clock;

	uintptr_t		mmc_base;
	unsigned		mmc_pbase;

	uintptr_t		l3init_cm2_base;
	uintptr_t		l3init_prm_base;

	int				mmc;
	uint32_t		cap;

		// Flags for data transfer
	uint32_t		dcmd;
	uint32_t		dmask;
	int				dsize;

	uintptr_t		sdma_base;
	uintptr_t		sdma_cbase;
	int				sdma_chnl;
	int				sdma_rreq;	// DMA request line for Rx
	int				sdma_treq;	// DMA request line for Tx

#define SDHCI_ADMA_DESC_MAX		256
	sdhci_adma32_t	*adma;
	uint32_t		admap;

	int				blksz;

} omap_ext_t;

extern int omap4_attach( SIM_HBA *hba );
int omap_core_pm( SIM_HBA *hba, int op );

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devb/mmcsd/chipsets/sim_omap4.h $ $Rev: 680332 $")
#endif
