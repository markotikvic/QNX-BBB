/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
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


#ifndef	__ARM_MX6SX_H_INCLUDED
#define	__ARM_MX6SX_H_INCLUDED


/*
 * System Memory Map for the i.MX6SoloX
 */

/*
 * BOOT ROM (ROMCP)
 */
#define MX6SX_BOOTROM_BASE                       0x00000000
#define MX6SX_BOOTROM_SILICON_REV                0x48

#define MX6SX_WEIM_CS0_BASE                      0x50000000  /* NOR or SRAM */
#define MX6SX_QSPI1_MEMORY_BASE                  0x60000000
#define MX6SX_QSPI2_MEMORY_BASE                  0x70000000

/*
 * QSPI NOR Flash
 */
#define MX6SX_QSPI1_A1_BASE                      0x60000000
#define MX6SX_QSPI1_A2_BASE                      0x64000000
#define MX6SX_QSPI1_B1_BASE                      0x68000000
#define MX6SX_QSPI1_B2_BASE                      0x6C000000

#define MX6SX_QSPI2_A1_BASE                      0x70000000
#define MX6SX_QSPI2_A2_BASE                      0x74000000
#define MX6SX_QSPI2_B1_BASE                      0x78000000
#define MX6SX_QSPI2_B2_BASE                      0x7C000000

/*
 * Enhanced Secured Digital Host Controllers
 */
#define MX6SX_USDHC1_BASE                        0x02190000
#define MX6SX_USDHC2_BASE                        0x02194000
#define MX6SX_USDHC3_BASE                        0x02198000
#define MX6SX_USDHC4_BASE                        0x0219C000
#define MX6SX_USDHC_SIZE                         0x4000
#define MX6SX_USDHC_MAP_SIZE                     0x100

#define MX6SX_USDHC1_IRQ                         54
#define MX6SX_USDHC2_IRQ                         55
#define MX6SX_USDHC3_IRQ                         56
#define MX6SX_USDHC4_IRQ                         57

/*
 * Multi Mode DDR Controller (MMDC)
 */
#define MX6SX_MMDC_BASE                          0x021B0000
#define MX6SX_MMDC_SIZE                          0x4000

/*
 * UARTs
 */
#define MX6SX_UART1_BASE                         0x02020000
#define MX6SX_UART2_BASE                         0x021E8000
#define MX6SX_UART3_BASE                         0x021EC000
#define MX6SX_UART4_BASE                         0x021F0000
#define MX6SX_UART5_BASE                         0x021F4000

#define MX6SX_UART_SIZE                          0x4000

#define MX6SX_UART1_IRQ                          58
#define MX6SX_UART2_IRQ                          59
#define MX6SX_UART3_IRQ                          60
#define MX6SX_UART4_IRQ                          61
#define MX6SX_UART5_IRQ                          62

/*
 * USB Base Address
 */
#define MX6SX_USB_BASE                           0x02184000
#define MX6SX_USB_SIZE                           0x4000

#define MX6SX_USBOTG1_BASE                       0x02184000
#define MX6SX_USBOTG2_BASE                       0x02184200
#define MX6SX_USBH1_BASE                         0x02184400
#define MX6SX_USB_OTHER_BASE                     0x02184800

#define MX6SX_USB_CMD                            0x140

#define MX6SX_USBH1_IRQ                          72
#define MX6SX_USBOTG1_IRQ                        75
#define MX6SX_USBOTG2_IRQ                        74

/* USB non-core registers */
#define MX6SX_USB_OTG1_CTRL						0x00
#define MX6SX_USB_OTG2_CTRL						0x04
#define MX6SX_USB_UH1_CTRL						0x08
#define MX6SX_USB_UH1_HSIC_CTRL					0x10
#define MX6SX_USB_OTG1_PHY_CTRL_0				0x18
#define MX6SX_USB_OTG2_PHY_CTRL_0				0x1c
/* USB non-core register bit masks */
#define MX6SX_USB_OTG_CTRL__PWR_POL_MASK       0x200
#define MX6SX_USB_OTG_CTRL__OC_POL_MASK        0x100

/*
 * ANATOP
 * Used for configuring PLLs, PFDs
 */
#define MX6SX_ANATOP_BASE                        0x020C8000
#define MX6SX_ANATOP_SIZE                        0x1000

#define MX6SX_ANATOP_PLL1_SYS                    0x00
#define MX6SX_ANATOP_PLL3_USB1                   0x10
#define MX6SX_ANATOP_PLL7_USB2                   0x20
#define MX6SX_ANATOP_PLL2_SYS_BUS                0x30
#define MX6SX_ANATOP_PLL4_AUDIO                  0x70
#define MX6SX_ANATOP_PLL4_AUDIO_NUM              0x80
#define MX6SX_ANATOP_PLL4_AUDIO_DENOM            0x90
#define MX6SX_ANATOP_PLL5_VIDEO                  0xA0
#define MX6SX_ANATOP_PLL5_VIDEO_SET              0xA4
#define MX6SX_ANATOP_PLL5_VIDEO_CLR              0xA8
#define MX6SX_ANATOP_PLL5_VIDEO_NUM              0xB0
#define MX6SX_ANATOP_PLL5_VIDEO_DENOM            0xC0
#define MX6SX_ANATOP_PLL6_ENET                   0xE0
#define MX6SX_ANATOP_PFD_480                     0xF0
#define MX6SX_ANATOP_PFD_480_SET                 0xF4
#define MX6SX_ANATOP_PFD_480_CLR                 0xF8
#define MX6SX_ANATOP_PFD_480_TOG                 0xFC
#define MX6SX_ANATOP_PFD_528                     0x100
#define MX6SX_ANATOP_PFD_528_SET                 0x104
#define MX6SX_ANATOP_PFD_528_CLR                 0x108
#define MX6SX_ANATOP_PFD_528_TOG                 0x10c
#define MX6SX_ANATOP_MISC2                       0x170
#define MX6SX_ANATOP_PFD0_OFFSET					0
#define MX6SX_ANATOP_PFD0_MASK					(0x3f << MX6SX_ANATOP_PFD0_OFFSET)
#define MX6SX_ANATOP_PFD1_OFFSET					8
#define MX6SX_ANATOP_PFD1_MASK					(0x3f << MX6SX_ANATOP_PFD1_OFFSET)
#define MX6SX_ANATOP_PFD2_OFFSET					16
#define MX6SX_ANATOP_PFD2_MASK					(0x3f << MX6SX_ANATOP_PFD2_OFFSET)
#define MX6SX_ANATOP_PFD3_OFFSET					24
#define MX6SX_ANATOP_PFD3_MASK					(0x3f << MX6SX_ANATOP_PFD3_OFFSET)
#define MX6SX_ANATOP_PFD0_GATE					7
#define MX6SX_ANATOP_PFD1_GATE					15
#define MX6SX_ANATOP_PFD2_GATE					23
#define MX6SX_ANATOP_PFD3_GATE					31
#define MX6SX_ANATOP_PFD_ALL_CLOCK_GATES         0x80808080

#define ANATOP_SETREG_OFFSET                    0x04
#define ANATOP_CLRREG_OFFSET                    0x08
#define ANATOP_TOGGLE_OFFSET                    0x0C

/* Audio, Video PLL (PLL4,5) offsets */
#define ANATOP_PLLAUDIOVIDEO_TEST_DIV_SEL_MASK	 (0x3 << 19)
#define ANATOP_PLLAUDIOVIDEO_TEST_DIV_SEL_OFFSET (19)

/* ENET PLL (PLL8) offsets */
#define ANATOP_PLL8_ENET_LOCK                   (1 << 31)
#define ANATOP_PLL8_ENET_ENABLE_SATA            (1 << 20)
#define ANATOP_PLL8_ENET_ENABLE_PCIE            (1 << 19)
#define ANATOP_PLL8_ENET_PFD_OFFSET_EN          (1 << 18)
#define ANATOP_PLL8_ENET_DITHER_ENABLE          (1 << 17)
#define ANATOP_PLL8_ENET_BYPASS                 (1 << 16)
#define ANATOP_PLL8_ENET_BYPASS_CLK_OSC         (0x0 << 14)
#define ANATOP_PLL8_ENET_BYPASS_CLK_ANACLK_1    (0x1 << 14)
#define ANATOP_PLL8_ENET_BYPASS_CLK_ANACLK_2    (0x2 << 14)
#define ANATOP_PLL8_ENET_BYPASS_CLK_XOR         (0x3 << 14)
#define ANATOP_PLL8_ENET_ENABLE	                (1 << 13)
#define ANATOP_PLL8_ENET_POWERDOWN              (1 << 12)
#define ANATOP_PLL8_ENET_REF_MASK               (0x3 << 0)
#define ANATOP_PLL8_ENET_REF_ENET_25M           (0x0 << 0)
#define ANATOP_PLL8_ENET_REF_ENET_50M           (0x1 << 0)
#define ANATOP_PLL8_ENET_REF_ENET_100M          (0x2 << 0)
#define ANATOP_PLL8_ENET_REF_ENET_125M          (0x3 << 0)

// Registers below are not documented, but used by Linux code and critical for PLL, chip information
#define MX6SX_ANADIG_REG_CORE                    0x140
#define MX6SX_ANADIG_MISC1                       0x160
#define MX6SX_ANADIG_MISC2                       0x170
#define MX6SX_ANADIG_CHIP_INFO                   0x260

#define ANADIG_REG_CORE_REG2_TRG_MASK           0x7C0000

#define ANADIG_MISC2_CONTROL3_MASK              (0x3 << 30)
#define ANADIG_MISC2_CONTROL3_OFFSET            (30)


/*
 * USB PHY0, 1
 */
#define MX6SX_USBPHY0_BASE                       0x020C9000
#define MX6SX_USBPHY1_BASE                       0x020CA000

#define MX6SX_USBPHY_PWD                         0x00
#define MX6SX_USBPHY_TX                          0x10
#define MX6SX_USBPHY_RX                          0x20
#define MX6SX_USBPHY_CTRL                        0x30
#define MX6SX_USBPHY_STATUS                      0x40
#define MX6SX_USBPHY_DEBUG                       0x50
#define MX6SX_USBPHY_DEBUG0_STATUS               0x60
#define MX6SX_USBPHY_DEBUG1_STATUS               0x70
#define MX6SX_USBPHY_VERSION                     0x80
#define MX6SX_USBPHY_IP                          0x90

/*
 * SNVS Secure Non Voltatile Storage
 */
#define MX6SX_SNVS_BASE							0x020CC000
#define MX6SX_SNVS_SIZE							0x4000
#define MX6SX_SNVS_SECURE_IRQ					51		/* Non TrustZone    */
#define MX6SX_SNVS_NON_SECURE_IRQ				52		/* TrustZone        */

#define MX6SX_SNVS_HPSR							0x14    /* HP Status Register */
#define MX6SX_SNVS_LPCR							0x38    /* LP Control Register */
#define MX6SX_SNVS_LPSR							0x4C    /* LP Status Register */
#define MX6SX_SNVS_LPSRTCMR						0x50    /* LP Secure Real Time Counter MSB Register */
#define MX6SX_SNVS_LPSRTCLR						0x54    /* LP Secure Real Time Counter LSB Register */
#define MX6SX_SNVS_LPTAR							0x58    /* LP Time Alarm Register */
#define MX6SX_SNVS_LPSMCMR						0x5C    /* LP Secure Monotonic Counter MSB Register */
#define MX6SX_SNVS_LPSMCLR						0x60    /* LP Secure Monotonic Counter LSB Register */
#define MX6SX_SNVS_LPPGDR						0x64    /* LP Power Glitch Detector Register */
#define MX6SX_SNVS_LPGPR							0x68    /* LP General Purpose Register */


/*
 * Enhanced Configurable Serial Peripheral Interfaces
 */

#define MX6SX_ECSPI_SIZE                         0x4000

#define MX6SX_ECSPI1_BASE                        0x02008000
#define MX6SX_ECSPI2_BASE                        0x0200C000
#define MX6SX_ECSPI3_BASE                        0x02010000
#define MX6SX_ECSPI4_BASE                        0x02014000
#define MX6SX_ECSPI5_BASE                        0x0228C000

#define MX6SX_ECSPI1_IRQ                         63
#define MX6SX_ECSPI2_IRQ                         64
#define MX6SX_ECSPI3_IRQ                         65
#define MX6SX_ECSPI4_IRQ                         66
#define MX6SX_ECSPI5_IRQ                         18


/*
 * Audio MUX
 */
#define MX6SX_AUDMUX_BASE                        0x021D8000
#define MX6SX_AUDMUX_SIZE                        0x4000

/* Port offset */
#define MX6SX_AUDMUX_PTCR1                       0x00
#define MX6SX_AUDMUX_PDCR1                       0x04
#define MX6SX_AUDMUX_PTCR2                       0x08
#define MX6SX_AUDMUX_PDCR2                       0x0C
#define MX6SX_AUDMUX_PTCR3                       0x10
#define MX6SX_AUDMUX_PDCR3                       0x14
#define MX6SX_AUDMUX_PTCR4                       0x18
#define MX6SX_AUDMUX_PDCR4                       0x1C
#define MX6SX_AUDMUX_PTCR5                       0x20
#define MX6SX_AUDMUX_PDCR5                       0x24
#define MX6SX_AUDMUX_PTCR6                       0x28
#define MX6SX_AUDMUX_PDCR6                       0x2C
#define MX6SX_AUDMUX_PTCR7                       0x30
#define MX6SX_AUDMUX_PDCR7                       0x34
#define MX6SX_AUDMUX_CNMCR                       0x38
#define MX6SX_AUDMUX_CNMCR                       0x38

/* DIGITAL AUDIO MULTIPLEXER (AUDMUX) */
#define AUDMUX_SYNC                             (1<<11)
#define AUDMUX_TFS_DIROUT                       (1<<31)
#define AUDMUX_TFS_PORT4                        (3<<27)
#define AUDMUX_TFS_PORT5                        (4<<27)
#define AUDMUX_TCLK_DIROUT                      (1<<26)
#define AUDMUX_TCLK_PORT4                       (3<<22)
#define AUDMUX_TCLK_PORT5                       (4<<22)
#define AUDMUX_RFS_DIROUT                       (1<<21)
#define AUDMUX_RFS_PORT4                        (3<<17)
#define AUDMUX_RFS_PORT5                        (4<<17)
#define AUDMUX_RCLK_DIROUT                      (1<<16)
#define AUDMUX_RCLK_PORT4                       (3<<12)
#define AUDMUX_RCLK_PORT5                       (4<<12)
#define AUDMUX_RXDSEL_PORT1                     (0<<13)
#define AUDMUX_RXDSEL_PORT2                     (1<<13)
#define AUDMUX_RXDSEL_PORT4                     (3<<13)
#define AUDMUX_RXDSEL_PORT5                     (4<<13)
#define AUDMUX_RXDSEL_MSK                       (7<<13)

#define MX6SX_CCM_BASE                           0x020C4000
#define MX6SX_CCM_SIZE                           0x4000

/* CCM Registers, offset from base address */
#define MX6SX_CCM_CCR                            0x00
#define MX6SX_CCM_CCDR                           0x04
#define MX6SX_CCM_CSR                            0x08
#define MX6SX_CCM_CCSR                           0x0C
#define MX6SX_CCM_CACCR                          0x10
#define MX6SX_CCM_CBCDR                          0x14
	#define MX6SX_CCM_CBCDR_PERIPH_CLK2_PODF_MASK     	(0x7 << 27)
	#define MX6SX_CCM_CBCDR_PERIPH_CLK2_PODF_OFFSET   	(27)
	#define MX6SX_CCM_CBCDR_PERIPH2_CLK2_SEL          	(1 << 26)
	#define MX6SX_CCM_CBCDR_PERIPH_CLK_SEL            	(1 << 25)
	#define MX6SX_CCM_CBCDR_MMDC_CH0_PODF_MASK        	(0x7 << 19)
	#define MX6SX_CCM_CBCDR_MMDC_CH0_PODF_OFFSET      	(19)
	#define MX6SX_CCM_CBCDR_AXI_PODF_MASK         		(0x7 << 16)
	#define MX6SX_CCM_CBCDR_AXI_PODF_OFFSET           	(16)
	#define MX6SX_CCM_CBCDR_AHB_PODF_MASK        	 	(0x7 << 10)
	#define MX6SX_CCM_CBCDR_AHB_PODF_OFFSET           	(10)
	#define MX6SX_CCM_CBCDR_IPG_PODF_MASK         		(0x3 << 8)
	#define MX6SX_CCM_CBCDR_IPG_PODF_OFFSET           	(8)
	#define MX6SX_CCM_CBCDR_AXI_ALT_SEL           		(1 << 7)
	#define MX6SX_CCM_CBCDR_AXI_SEL               		(1 << 6)
	#define MX6SX_CCM_CBCDR_MMDC_CH1_PODF_MASK        	(0x7 << 3)
	#define MX6SX_CCM_CBCDR_MMDC_CH1_PODF_OFFSET      	(3)
	#define MX6SX_CCM_CBCDR_PERIPH2_CLK2_PODF_MASK   	(0x7 << 0)
	#define MX6SX_CCM_CBCDR_PERIPH2_CLK2_PODF_OFFSET		(0)
#define MX6SX_CCM_CBCMR                          0x18
	#define MX6SX_CCM_CBCMR_GPU3D_SHADER_PODF_MASK       (0x7 << 29)
	#define MX6SX_CCM_CBCMR_GPU3D_SHADER_PODF_OFFSET     (29)
	#define MX6SX_CCM_CBCMR_GPU3D_CORE_PODF_MASK      	(0x7 << 26)
	#define MX6SX_CCM_CBCMR_GPU3D_CORE_PODF_OFFSET       (26)
	#define MX6SX_CCM_CBCMR_GPU2D_CORE_PODF_MASK      	(0x7 << 23)
	#define MX6SX_CCM_CBCMR_GPU2D_CORE_PODF_OFFSET       (23)
	#define MX6SX_CCM_CBCMR_PRE_PERIPH2_CLK_SEL_MASK     (0x3 << 21)
	#define MX6SX_CCM_CBCMR_PRE_PERIPH2_CLK_SEL_OFFSET   (21)
	#define MX6SX_CCM_CBCMR_PRE_PERIPH2_CLK2_SEL     	(1 << 20)
	#define MX6SX_CCM_CBCMR_PRE_PERIPH_CLK_SEL_MASK      (0x3 << 18)
	#define MX6SX_CCM_CBCMR_PRE_PERIPH_CLK_SEL_OFFSET    (18)
	#define MX6SX_CCM_CBCMR_GPU2D_CLK_SEL_MASK        	(0x3 << 16)
	#define MX6SX_CCM_CBCMR_GPU2D_CLK_SEL_OFFSET      	(16)
	#define MX6SX_CCM_CBCMR_VPU_AXI_CLK_SEL_MASK     	(0x3 << 14)
	#define MX6SX_CCM_CBCMR_VPU_AXI_CLK_SEL_OFFSET       (14)
	#define MX6SX_CCM_CBCMR_PERIPH_CLK2_SEL_MASK      	(0x3 << 12)
	#define MX6SX_CCM_CBCMR_PERIPH_CLK2_SEL_OFFSET       (12)
	#define MX6SX_CCM_CBCMR_VDOAXI_CLK_SEL            	(1 << 11)
	#define MX6SX_CCM_CBCMR_PCIE_AXI_CLK_SEL          	(1 << 10)
	#define MX6SX_CCM_CBCMR_GPU3D_SHADER_CLK_SEL_MASK    (0x3 << 8)
	#define MX6SX_CCM_CBCMR_GPU3D_SHADER_CLK_SEL_OFFSET  (8)
	#define MX6SX_CCM_CBCMR_GPU3D_CORE_CLK_SEL_MASK      (0x3 << 4)
	#define MX6SX_CCM_CBCMR_GPU3D_CORE_CLK_SEL_OFFSET    (4)
	#define MX6SX_CCM_CBCMR_GPU3D_AXI_CLK_SEL_MASK       (0x1 << 1)
 	#define MX6SX_CCM_CBCMR_GPU3D_AXI_CLK_SEL_OFFSET     (1)
	#define MX6SX_CCM_CBCMR_GPU2D_AXI_CLK_SEL_MASK       (0x1 << 0)
	#define MX6SX_CCM_CBCMR_GPU2D_AXI_CLK_SEL_OFFSET     (0)
#define MX6SX_CCM_CSCMR1                         0x1c
	#define MX6SX_CCM_CSCMR1_ACLK_EMI_SLOW_MASK       	(0x3 << 29)
	#define MX6SX_CCM_CSCMR1_ACLK_EMI_SLOW_OFFSET     	(29)
	#define MX6SX_CCM_CSCMR1_ACLK_EMI_MASK           	(0x3 << 27)
	#define MX6SX_CCM_CSCMR1_ACLK_EMI_OFFSET          	(27)
	#define MX6SX_CCM_CSCMR1_ACLK_EMI_SLOW_PODF_MASK     (0x7 << 23)
	#define MX6SX_CCM_CSCMR1_ACLK_EMI_SLOW_PODF_OFFSET   (23)
	#define MX6SX_CCM_CSCMR1_ACLK_EMI_PODF_MASK       	(0x7 << 20)
	#define MX6SX_CCM_CSCMR1_ACLK_EMI_PODF_OFFSET     	(20)
	#define MX6SX_CCM_CSCMR1_USDHC4_CLK_SEL           	(1 << 19)
	#define MX6SX_CCM_CSCMR1_USDHC3_CLK_SEL           	(1 << 18)
	#define MX6SX_CCM_CSCMR1_USDHC2_CLK_SEL           	(1 << 17)
	#define MX6SX_CCM_CSCMR1_USDHC1_CLK_SEL           	(1 << 16)
	#define MX6SX_CCM_CSCMR1_SSI3_CLK_SEL_MASK        	(0x3 << 14)
	#define MX6SX_CCM_CSCMR1_SSI3_CLK_SEL_OFFSET      	(14)
	#define MX6SX_CCM_CSCMR1_SSI2_CLK_SEL_MASK        	(0x3 << 12)
	#define MX6SX_CCM_CSCMR1_SSI2_CLK_SEL_OFFSET      	(12)
	#define MX6SX_CCM_CSCMR1_SSI1_CLK_SEL_MASK        	(0x3 << 10)
	#define MX6SX_CCM_CSCMR1_SSI1_CLK_SEL_OFFSET      	(10)
	#define MX6SX_CCM_CSCMR1_PERCLK_PODF_MASK         	(0x3F)
#define MX6SX_CCM_CSCMR2                         0x20
#define MX6SX_CCM_CSCDR1                         0x24
    #define MX6SX_CCM_CSCDR1_VPU_AXI_PODF_MASK        	(0x7 << 25)
    #define MX6SX_CCM_CSCDR1_VPU_AXI_PODF_OFFSET      	(25)
    #define MX6SX_CCM_CSCDR1_USDHC4_PODF_DIV_2         	(0x1 << 22)
    #define MX6SX_CCM_CSCDR1_USDHC4_PODF_MASK         	(0x7 << 22)
    #define MX6SX_CCM_CSCDR1_USDHC4_PODF_OFFSET       	(22)
    #define MX6SX_CCM_CSCDR1_USDHC3_PODF_DIV_2         	(0x1 << 19)
    #define MX6SX_CCM_CSCDR1_USDHC3_PODF_MASK         	(0x7 << 19)
    #define MX6SX_CCM_CSCDR1_USDHC3_PODF_OFFSET       	(19)
    #define MX6SX_CCM_CSCDR1_USDHC2_PODF_DIV_2         	(0x1 << 16)
    #define MX6SX_CCM_CSCDR1_USDHC2_PODF_MASK         	(0x7 << 16)
    #define MX6SX_CCM_CSCDR1_USDHC2_PODF_OFFSET       	(16)
    #define MX6SX_CCM_CSCDR1_USDHC1_PODF_DIV_2         	(0x1 << 11)
    #define MX6SX_CCM_CSCDR1_USDHC1_PODF_MASK         	(0x7 << 11)
    #define MX6SX_CCM_CSCDR1_USDHC1_PODF_OFFSET       	(11)
    #define MX6SX_CCM_CSCDR1_USBOH3_CLK_PRED_OFFSET   	(8)
    #define MX6SX_CCM_CSCDR1_USBOH3_CLK_PRED_MASK     	(0x7 << 8)
    #define MX6SX_CCM_CSCDR1_USBOH3_CLK_PODF_OFFSET   	(6)
    #define MX6SX_CCM_CSCDR1_USBOH3_CLK_PODF_MASK     	(0x3 << 6)
    #define MX6SX_CCM_CSCDR1_UART_CLK_PODF_MASK       	(0x3F)
    #define MX6SX_CCM_CSCDR1_UART_CLK_PODF_OFFSET     	(0)
#define MX6SX_CCM_CS1CDR                         0x28
#define MX6SX_CCM_CS2CDR                         0x2c
    #define MX6SX_CCM_CS2CDR_ENFC_CLK_PODF_MASK       	(0x3F << 21)
    #define MX6SX_CCM_CS2CDR_ENFC_CLK_PODF_OFFSET     	(21)
    #define MX6SX_CCM_CS2CDR_ENFC_CLK_PRED_MASK       	(0x7 << 18)
    #define MX6SX_CCM_CS2CDR_ENFC_CLK_PRED_OFFSET     	(18)
    #define MX6SX_CCM_CS2CDR_ENFC_CLK_SEL_MASK        	(0x3 << 16)
    #define MX6SX_CCM_CS2CDR_ENFC_CLK_SEL_OFFSET      	(16)
    #define MX6SX_CCM_CS2CDR_LDB_DI1_CLK_SEL_MASK     	(0x7 << 12)
    #define MX6SX_CCM_CS2CDR_LDB_DI1_CLK_SEL_OFFSET   	(12)
    #define MX6SX_CCM_CS2CDR_LDB_DI0_CLK_SEL_MASK     	(0x7 << 9)
    #define MX6SX_CCM_CS2CDR_LDB_DI0_CLK_SEL_OFFSET   	(9)
    #define MX6SX_CCM_CS2CDR_SSI2_CLK_PRED_MASK       	(0x7 << 6)
    #define MX6SX_CCM_CS2CDR_SSI2_CLK_PRED_OFFSET     	(6)
    #define MX6SX_CCM_CS2CDR_SSI2_CLK_PODF_MASK       	(0x3F)
    #define MX6SX_CCM_CS2CDR_SSI2_CLK_PODF_OFFSET     	(0)
#define MX6SX_CCM_CDCDR                          0x30
#define MX6SX_CCM_CHSCCDR                        0x34
    #define MX6SX_CCM_CHSCCDR_ENET_PRE_CLK_SEL_MASK     (0x7 << 15)
    #define MX6SX_CCM_CHSCCDR_ENET_PRE_CLK_SEL_OFFSET   (15)
    #define MX6SX_CCM_CHSCCDR_ENET_PODF_MASK            (0x7 << 12)
    #define MX6SX_CCM_CHSCCDR_ENET_PODF_OFFSET          (12)
    #define MX6SX_CCM_CHSCCDR_ENET_CLK_SEL_MASK         (0x7 << 9)
    #define MX6SX_CCM_CHSCCDR_ENET_CLK_SEL_OFFSET       (9)
    #define MX6SX_CCM_CHSCCDR_M4_PRE_CLK_SEL_MASK       (0x7 << 6)
    #define MX6SX_CCM_CHSCCDR_M4_PRE_CLK_SEL_OFFSET     (6)
    #define MX6SX_CCM_CHSCCDR_M4_PODF_MASK              (0x7 << 3)
    #define MX6SX_CCM_CHSCCDR_M4_PODF_OFFSET            (3)
    #define MX6SX_CCM_CHSCCDR_M4_CLK_SEL_MASK           (0x7 << 0)
    #define MX6SX_CCM_CHSCCDR_M4_CLK_SEL_OFFSET         (0)
#define MX6SX_CCM_CSCDR2                         0x38
#define MX6SX_CCM_CSCDR2_ECSPI_CLK_PODF_MASK        (0x3F << 19)
#define MX6SX_CCM_CSCDR2_ECSPI_CLK_PODF_OFFSET      (19)
#define MX6SX_CCM_CSCDR3                         0x3c
	// IPU2 registers only available on i.MX6 Dual, i.MX6 Quad
	#define MX6SX_CCM_CSCDR3_EPDC_PODF_MASK              (0x7 << 16)
	#define MX6SX_CCM_CSCDR3_EPDC_PODF_OFFSET            (16)
	#define MX6SX_CCM_CSCDR3_EPDC_CLK_SEL_MASK           (0x3 << 14)
	#define MX6SX_CCM_CSCDR3_EPDC_CLK_SEL_OFFSET         (14)
	#define MX6SX_CCM_CSCDR3_IPU1_HSP_PODF_MASK          (0x7 << 11)
	#define MX6SX_CCM_CSCDR3_IPU1_HSP_PODF_OFFSET        (11)
	#define MX6SX_CCM_CSCDR3_IPU1_HSP_CLK_SEL_MASK       (0x3 << 9)
	#define MX6SX_CCM_CSCDR3_IPU1_HSP_CLK_SEL_OFFSET     (9)
#define MX6SX_CCM_CSCDR4                         0x40
#define MX6SX_CCM_CWDR                           0x44
#define MX6SX_CCM_CDHIPR                         0x48
#define MX6SX_CCM_CDCR                           0x4c
#define MX6SX_CCM_CTOR                           0x50
#define MX6SX_CCM_CLPCR                          0x54
#define MX6SX_CCM_CISR                           0x58
#define MX6SX_CCM_CIMR                           0x5c
#define MX6SX_CCM_CCOSR                          0x60
#define MX6SX_CCM_CGPR                           0x64
#define MX6SX_CCM_CCGR0                          0x68
#define MX6SX_CCM_CCGR1                          0x6c
	#define MX6SX_CCGR1_GPU3D_ENABLE                  (0x3 << 26)
#define MX6SX_CCM_CCGR2                          0x70
#define MX6SX_CCM_CCGR3                          0x74
#define MX6SX_CCM_CCGR4                          0x78
#define MX6SX_CCM_CCGR5                          0x7c
#define MX6SX_CCM_CCGR6                          0x80
#define MX6SX_CCM_CCGR7                          0x84
#define MX6SX_CCM_CMEOR                          0x88

/* CCM Clock Gating Reset Values */
#define MX6SX_CCM_CCGR0_RESET                    0xFFFFFFFF
#define MX6SX_CCM_CCGR1_RESET                    0xFFFFFFFF
#define MX6SX_CCM_CCGR2_RESET                    0xFFFFFFFF
#define MX6SX_CCM_CCGR3_RESET                    0xFFFFFFFF
#define MX6SX_CCM_CCGR4_RESET                    0xFFFFFFFF
#define MX6SX_CCM_CCGR5_RESET                    0xFFFFFFFF
#define MX6SX_CCM_CCGR6_RESET                    0xFFFFFFFF
#define MX6SX_CCM_CCGR7_RESET                    0xFFFFFFFF

/* CCM Clock Gating Values */
#define CCGR0_CG15_AIPS_TZ3                     (0x3 << 30
#define CCGR0_CG13_DCIC2                        (0x3 << 26)
#define CCGR0_CG12_DCIC1                        (0x3 << 24)
#define CCGR0_CG11_CHEETAH_DBG                  (0x3 << 22)
#define CCGR0_CG10_CAN2_SERIAL                  (0x3 << 20)
#define CCGR0_CG9_CAN2_CLK                      (0x3 << 18)
#define CCGR0_CG8_CAN1_SERIAL                   (0x3 << 16)
#define CCGR0_CG7_CAN1_CLK                      (0x3 << 14)
#define CCGR0_CG6_CAAM_IPG                      (0x3 << 12)
#define CCGR0_CG5_CAAM_ACLK                     (0x3 << 10)
#define CCGR0_CG4_CAAM_SECURE_MEM               (0x3 << 8)
#define CCGR0_CG3_ASRC                          (0x3 << 6)
#define CCGR0_CG2_APBH_DMA_HCLK                 (0x3 << 4)
#define CCGR0_CG1_AIPS_TZ2                      (0x3 << 2)
#define CCGR0_CG0_AIPS_TZ1                      (0x3 << 0)

#define CCGR1_CG15_CANDFD                       (0x3 << 30)
#define CCGR1_CG14_OCRAM_S                      (0x3 << 28)
#define CCGR1_CG13_GPU                          (0x3 << 26)
#define CCGR1_CG11_GPT_SERIAL                   (0x3 << 22)
#define CCGR1_CG10_GPT                          (0x3 << 20)
#define CCGR1_CG9_WAKEUP                        (0x3 << 18)
#define CCGR1_CG8_ESAI                          (0x3 << 16)
#define CCGR1_CG7_EPIT2                         (0x3 << 14)
#define CCGR1_CG6_EPIT1                         (0x3 << 12)
#define CCGR1_CG4_ECSPI5                        (0x3 << 8)
#define CCGR1_CG3_ECSPI4                        (0x3 << 6)
#define CCGR1_CG2_ECSPI3                        (0x3 << 4)
#define CCGR1_CG1_ECSPI2                        (0x3 << 2)
#define CCGR1_CG0_ECSPI1                        (0x3 << 0)

#define CCGR2_CG15_PXP                          (0x3 << 30)
#define CCGR2_CG14_LCD                          (0x3 << 28)
#define CCGR2_CG11_IPSYNC                       (0x3 << 22)
#define CCGR2_CG10_IPMUX3                       (0x3 << 20)
#define CCGR2_CG9_IPMUX2                        (0x3 << 18)
#define CCGR2_CG8_IPMUX1                        (0x3 << 16)
#define CCGR2_CG7_IOMUX_IPT                     (0x3 << 14)
#define CCGR2_CG6_OCOTP                         (0x3 << 12)
#define CCGR2_CG5_I2C3                          (0x3 << 10)
#define CCGR2_CG4_I2C2                          (0x3 << 8)
#define CCGR2_CG3_I2C1                          (0x3 << 6)
#define CCGR2_CG1_CSI                           (0x3 << 2)

#define CCGR3_CG14_OCRAM                        (0x3 << 28)
#define CCGR3_CG13_MMDC_CORE_IPG_P1             (0x3 << 26)
#define CCGR3_CG12_MMDC_CORE_IPG_P0             (0x3 << 24)
#define CCGR3_CG10_MMDC_CORE_ACLK_FAST_P0       (0x3 << 20)
#define CCGR3_CG9_MLB                           (0x3 << 18)
#define CCGR3_CG7_QSPI1                         (0x3 << 14)
#define CCGR3_CG6_LDB_DI0                       (0x3 << 12)
#define CCGR3_CG5_LCDIF1                        (0x3 << 10)
#define CCGR3_CG4_LCDIF2                        (0x3 << 8)
#define CCGR3_CG3_DISP_AXI                      (0x3 << 6)
#define CCGR3_CG2_ENET                          (0x3 << 4)
#define CCGR3_CG1_M4                            (0x3 << 2)

#define CCGR4_CG15_RAWNAND_GPMI_INPUT_APB       (0x3 << 30)
#define CCGR4_CG14_RAWNAND_GPMI_BCH_INPUT       (0x3 << 28)
#define CCGR4_CG13_RAWNAND_GPMI_BCH_INPUT       (0x3 << 26)
#define CCGR4_CG12_RAWNAND_BCH_INPUT_APB        (0x3 << 24)
#define CCGR4_CG11_PWM4                         (0x3 << 22)
#define CCGR4_CG10_PWM3                         (0x3 << 20)
#define CCGR4_CG9_PWM2                          (0x3 << 18)
#define CCGR4_CG8_PWM1                          (0x3 << 16)
#define CCGR4_CG7_PL301_PER2_MAINCLK            (0x3 << 14)
#define CCGR4_CG6_PL301_PER1_BCH                (0x3 << 12)
#define CCGR4_CG5_QSPI2                         (0x3 << 10)
#define CCGR4_CG0_PCIE                          (0x3 << 0)

#define CCGR5_CG15_SAI2                         (0x3 << 30)
#define CCGR5_CG14_SAI1                         (0x3 << 28)
#define CCGR5_CG13_UART_SERIAL_CLK              (0x3 << 26)
#define CCGR5_CG12_UART_CLK                     (0x3 << 24)
#define CCGR5_CG11_SSI3                         (0x3 << 22)
#define CCGR5_CG10_SSI2                         (0x3 << 20)
#define CCGR5_CG9_SSI1                          (0x3 << 18)
#define CCGR5_CG7_SPDIF                         (0x3 << 14)
#define CCGR5_CG6_SPBA                          (0x3 << 12)
#define CCGR5_CG3_SDMA                          (0x3 << 6)
#define CCGR5_CG0_ROM                           (0x3 << 0)


#define CCGR6_CG15_PWM7                         (0x3 << 30)
#define CCGR6_CG14_PWM6                         (0x3 << 28)
#define CCGR6_CG13_PWM5                         (0x3 << 26)
#define CCGR6_CG12_I2C4                         (0x3 << 24)
#define CCGR6_CG11_GIS                          (0x3 << 22)
#define CCGR6_CG10_VADC                         (0x3 << 20)
#define CCGR6_CG8_PWM8                          (0x3 << 16)
#define CCGR6_CG5_EMI_SLOW                      (0x3 << 10)
#define CCGR6_CG4_USDHC4                        (0x3 << 8)
#define CCGR6_CG3_USDHC3                        (0x3 << 6)
#define CCGR6_CG2_USDHC2                        (0x3 << 4)
#define CCGR6_CG1_USDHC1                        (0x3 << 2)
#define CCGR6_CG0_USBOH3                        (0x3 << 0)

#define CSCMR1_SSI1_CLK_SEL_MASK                (0x3<<10)
#define CSCMR1_SSI1_CLK_SEL_PLL4                (0x2<<10)
#define CSCMR1_SSI1_CLK_SEL_454PFD              (0x1<<10)

#define CSCMR1_SSI2_CLK_SEL_MASK                (0x3<<12)
#define CSCMR1_SSI2_CLK_SEL_PLL4                (0x2<<12)
#define CSCMR1_SSI2_CLK_SEL_454PFD              (0x1<<12)

#define CSCMR1_SSI3_CLK_SEL_MASK                (0x3<<14)
#define CSCMR1_SSI3_CLK_SEL_PLL4                (0x2<<14)

/*
 * Values passed to SSI clock divisor macros should be the exact
 * divisor value, not the register value.  E.g. SSI_CLK_PRED(1) will
 * divide by 1, which will set the ssiX_clk_pred pre-dividor to bit value 0
 * which corresponds to divide by 1.
 */
#define SSI1_CLK_DIV_MASK                       (0x1ff)
#define SSI1_CLK_PRED(x)                        ((x - 1) << 6)
#define SSI1_CLK_PODF(x)                        ((x - 1) << 0)
#define SSI2_CLK_DIV_MASK                       (0x1ff)
#define SSI2_CLK_PRED(x)                        ((x - 1) << 6)
#define SSI2_CLK_PODF(x)                        ((x - 1) << 0)
#define SSI3_CLK_DIV_MASK                       (0x1ff << 16)
#define SSI3_CLK_PRED(x)                        ((x - 1) << 22)
#define SSI3_CLK_PODF(x)                        ((x - 1) << 16)

/*
 * Synchronous Serial Interfaces
 */
#define MX6SX_SSI1_BASE                          0x02028000
#define MX6SX_SSI2_BASE                          0x0202C000
#define MX6SX_SSI3_BASE                          0x02030000

#define MX6SX_SSI_SIZE                           0x4000

#define MX6SX_SSI1_IRQ                           78
#define MX6SX_SSI2_IRQ                           79
#define MX6SX_SSI3_IRQ                           80

#define MX6SX_SSI_STCCR                          0x24
#define MX6SX_SSI_DIV2                           (1 << 18)
#define MX6SX_SSI_WORD_LENGTH_16                 (7 << 13)
#define MX6SX_SSI_DC(x)                          ((x & 0x1f) << 8)
#define MX6SX_SSI_PM(x)                          ((x & 0xff) << 0)



/*
 * Sony/Philips Digital Interface Transmitter
 */
#define MX6SX_SPDIF_BASE                         0x02004000
#define MX6SX_SPDIF_SIZE                         0x4000

#define MX6SX_SPDIF_IRQ                          84
/*
 * I2C
 */
#define MX6SX_I2C1_BASE                          0x021A0000
#define MX6SX_I2C2_BASE                          0x021A4000
#define MX6SX_I2C3_BASE                          0x021A8000
#define MX6SX_I2C4_BASE                          0x021F8000

#define MX6SX_I2C_SIZE                           0x4000

#define MX6SX_I2C1_IRQ                           68
#define MX6SX_I2C2_IRQ                           69
#define MX6SX_I2C3_IRQ                           70
#define MX6SX_I2C4_IRQ                           67

/*
 * Ethernet MAC-NET
 */
#define MX6SX_ENET1_BASE                         0x02188000
#define MX6SX_ENET2_BASE                         0x021B4000
#define MX6SX_ENET_SIZE                          0x4000
#define MX6SX_ENET_PALR                          0xE4
#define MX6SX_ENET_PAUR                          0xE8


#define MX6SX_ENET1_IRQ                          150
#define MX6SX_ENET2_IRQ                          134

#define MX6SX_ENET1_TIMER_IRQ                    151
#define MX6SX_ENET2_TIMER_IRQ                    135

/*
 * External Interface Module (EIM)
 */
#define MX6SX_EIM_BASE                           0x021B8000
#define MX6SX_EIM_CS0GCR1                        0x00
#define MX6SX_EIM_CS0GCR2                        0x04
#define MX6SX_EIM_CS0RCR1                        0x08
#define MX6SX_EIM_CS0RCR2                        0x0C
#define MX6SX_EIM_CS0WCR1                        0x10
#define MX6SX_EIM_CS0WCR2                        0x14
#define MX6SX_EIM_CS1GCR1                        0x18
#define MX6SX_EIM_CS1GCR2                        0x1C
#define MX6SX_EIM_CS1RCR1                        0x20
#define MX6SX_EIM_CS1RCR2                        0x24
#define MX6SX_EIM_CS1WCR1                        0x28
#define MX6SX_EIM_CS1WCR2                        0x2C
#define MX6SX_EIM_CS2GCR1                        0x30
#define MX6SX_EIM_CS2GCR2                        0x34
#define MX6SX_EIM_CS2RCR1                        0x38
#define MX6SX_EIM_CS2RCR2                        0x3C
#define MX6SX_EIM_CS2WCR1                        0x40
#define MX6SX_EIM_CS2WCR2                        0x44
#define MX6SX_EIM_CS3GCR1                        0x48
#define MX6SX_EIM_CS3GCR2                        0x4C
#define MX6SX_EIM_CS3RCR1                        0x50
#define MX6SX_EIM_CS3RCR2                        0x54
#define MX6SX_EIM_CS3WCR1                        0x58
#define MX6SX_EIM_CS3WCR2                        0x5C
#define MX6SX_EIM_CS4GCR1                        0x60
#define MX6SX_EIM_CS4GCR2                        0x64
#define MX6SX_EIM_CS4RCR1                        0x68
#define MX6SX_EIM_CS4RCR2                        0x6C
#define MX6SX_EIM_CS4WCR1                        0x70
#define MX6SX_EIM_CS4WCR2                        0x74
#define MX6SX_EIM_CS5GCR1                        0x78
#define MX6SX_EIM_CS5GCR2                        0x7C
#define MX6SX_EIM_CS5RCR1                        0x80
#define MX6SX_EIM_CS5RCR2                        0x84
#define MX6SX_EIM_CS5WCR1                        0x88
#define MX6SX_EIM_CS5WCR2                        0x8C
#define MX6SX_EIM_WCR                            0x90
#define MX6SX_EIM_WIAR                           0x94
#define MX6SX_EIM_EAR                            0x98

/*
 * CAN
 */
#define MX6SX_CAN1_PORT                          0x02090000
#define MX6SX_CAN1_MEM                           0x02090080
#define MX6SX_CAN1_IRQ                           142

#define MX6SX_CAN2_PORT                          0x02094000
#define MX6SX_CAN2_MEM                           0x02094080
#define MX6SX_CAN2_IRQ                           143

#define MX6SX_CAN_SIZE                           0x4000

/*
 * GPIOs
 */
#define MX6SX_GPIO1_BASE                         0x0209C000
#define MX6SX_GPIO2_BASE                         0x020A0000
#define MX6SX_GPIO3_BASE                         0x020A4000
#define MX6SX_GPIO4_BASE                         0x020A8000
#define MX6SX_GPIO5_BASE                         0x020AC000
#define MX6SX_GPIO6_BASE                         0x020B0000
#define MX6SX_GPIO7_BASE                         0x020B4000
#define MX6SX_GPIO_SIZE                          0x4000

/* GPIO Registers, offset from base address */
#define MX6SX_GPIO_DR                            0x00
#define MX6SX_GPIO_GDIR                          0x04
#define MX6SX_GPIO_PSR                           0x08
#define MX6SX_GPIO_ICR1                          0x0C
#define MX6SX_GPIO_ICR2                          0x10
#define MX6SX_GPIO_IMR                           0x14
#define MX6SX_GPIO_ISR                           0x18
#define MX6SX_GPIO_EDR                           0x1C

/*
 * SDMA Base Address
 */
#define MX6SX_SDMA_BASE                          0x020EC000
#define MX6SX_SDMA_IRQ                           34
#define MX6SX_SDMA_SIZE                          0x4000

/* SDMA Registers, offset from base address */
#define MX6SX_SDMA_MC0PTR                        0x00	/* AP (MCU) Channel 0 Pointer */
#define MX6SX_SDMA_INTR                          0x04	/* Channel Interrupts */
#define MX6SX_SDMA_STOP_STAT                     0x08	/* Channel Stop / Channel Status */
#define MX6SX_SDMA_HSTART                        0x0C	/* Channel Start */
#define MX6SX_SDMA_EVTOVR                        0x10	/* Channel Event Override */
#define MX6SX_SDMA_DSPOVR                        0x14	/* Channel DSP (BP) Override */
#define MX6SX_SDMA_HOSTOVR                       0x18	/* Channel AP Override */
#define MX6SX_SDMA_EVTPEND                       0x1C	/* Channel Event Pending */
#define MX6SX_SDMA_RESET                         0x24	/* Reset Register */
#define MX6SX_SDMA_EVTERR                        0x28	/* DMA Request Error Register */
#define MX6SX_SDMA_INTRMASK                      0x2C	/* Channel AP Interrupt Mask */
#define MX6SX_SDMA_PSW                           0x30	/* Schedule Status */
#define MX6SX_SDMA_EVTERRDBG                     0x34	/* DMA Request Error Register */
#define MX6SX_SDMA_CONFIG                        0x38	/* Configuration Register */
#define MX6SX_SDMA_ONCE_ENB                      0x40	/* OnCE Enable */
#define MX6SX_SDMA_ONCE_DATA                     0x44	/* OnCE Data Register */
#define MX6SX_SDMA_ONCE_INSTR                    0x48	/* OnCE Instruction Register */
#define MX6SX_SDMA_ONCE_STAT                     0x4C	/* OnCE Status Register */
#define MX6SX_SDMA_ONCE_CMD                      0x50	/* OnCE Command Register */
#define MX6SX_SDMA_EVT_MIRROT                    0x54	/* DMA Request */
#define MX6SX_SDMA_ILLINSTADDR                   0x58	/* Illegal Instruction Trap Address */
#define MX6SX_SDMA_CHN0ADDR                      0x5C	/* Channel 0 Boot Address */
#define MX6SX_SDMA_XTRIG_CONF1                   0x70	/* Cross-Trigger Events Configuration Register 1 */
#define MX6SX_SDMA_XTRIG_CONF2                   0x74	/* Cross-Trigger Events Configuration Register 2 */
#define MX6SX_SDMA_CHNPRI(n)                     (0x100 + ((n)<<2))   /* Channel Priority n = 0 to 31 */
#define MX6SX_SDMA_CHNENBL(n)                    (0x200 + ((n)<<2))   /* Channel Enable n = 0 to 47 */

/*
 * Watchdogs
 */
#define MX6SX_WDOG1_BASE                         0x020BC000
#define MX6SX_WDOG2_BASE                         0x020C0000
#define MX6SX_WDOG_SIZE                          0x4000

/*
 * Watchdog Timer Registers, offset from base address
 */
#define MX6SX_WDOG_CR                            0x00
// Set default time out value to 30 seconds
#define WT_MASK                                 (0xff << 8)
#define WT                                      (0x3B << 8)
#define WDT                                     (0x01 << 3)
#define WDE                                     (0x01 << 2)
#define WDZST                                   (0x01)
#define MX6SX_WDOG_SR                            0x02
#define MX6SX_WDOG_RSR                           0x04
#define MX6SX_WDOG_ICR                           0x06
#define MX6SX_WDOG_MCR                           0x08

/*
 * General Purpose Timer
 */
#define MX6SX_GPT_BASE                           0x02098000
#define MX6SX_GPT_SIZE                           0x4000

/*
 * Enhanced Serial Audio Interface (ESAI)
 */
#define MX6SX_ESAI_BASE                           0x02024000
#define MX6SX_ESAI_SIZE                           0x4000

/*
 * I/O MUX Controller
 */
#define MX6SX_IOMUXC_BASE                        0x020E0000
#define MX6SX_IOMUXC_GPR_BASE                    0x020E4000
#define MX6SX_IOMUXC_SIZE                        0x4000
#define MX6SX_IOMUXC_GPR_SIZE                    0x4000

/*
 * IOMUX GPR Registers, offset from GPR base address
 */
#define MX6SX_IOMUX_GPR0                         0x0000
#define MX6SX_IOMUX_GPR1                         0x0004
#define MX6SX_IOMUX_GPR2                         0x0008
#define MX6SX_IOMUX_GPR3                         0x000C
#define MX6SX_IOMUX_GPR4                         0x0010
#define MX6SX_IOMUX_GPR5                         0x0014
#define MX6SX_IOMUX_GPR6                         0x0018
#define MX6SX_IOMUX_GPR7                         0x001C
#define MX6SX_IOMUX_GPR8                         0x0020
#define MX6SX_IOMUX_GPR9                         0x0024
#define MX6SX_IOMUX_GPR10                        0x0028
#define MX6SX_IOMUX_GPR11                        0x002C
#define MX6SX_IOMUX_GPR12                        0x0030
#define MX6SX_IOMUX_GPR13                        0x0034

/* TODO: RAM Can't find these in the old docs or the new docs. Not sure what they are used for, and if they are in the GPR or the normal IOMUXC */
#define MX6SX_IOMUXC_OBSERVE_MUX_0               0x0038
#define MX6SX_IOMUXC_OBSERVE_MUX_1               0x003C
#define MX6SX_IOMUXC_OBSERVE_MUX_2               0x0040
#define MX6SX_IOMUXC_OBSERVE_MUX_3               0x0044
#define MX6SX_IOMUXC_OBSERVE_MUX_4               0x0048

/*
 * IOMUX Registers, offset from base address
 */
#define MX6SX_IOMUX_SWMUX                        0x0014
#define MX6SX_IOMUX_SWPAD                        0x02AC
#define MX6SX_IOMUX_SWINPUT                      0x0624


/*
 * AHB to IP Bridge (Trust Zone) (AIPSTZ)
 */
#define MX6SX_AIPS1_BASE                        0x02000000
#define MX6SX_AIPS2_BASE                        0x02100000
#define MX6SX_AIPS3_BASE                        0x02200000
#define MX6SX_AIPS1_CONFIG                      (MX6SX_AIPS1_BASE + 0x7c000)
#define MX6SX_AIPS2_CONFIG                      (MX6SX_AIPS2_BASE + 0x7c000)
#define MX6SX_AIPS3_CONFIG                      (MX6SX_AIPS3_BASE + 0x7c000)
#define MX6SX_AIPS_MPR0                         0x00
#define MX6SX_AIPS_MPR1                         0x04
#define MX6SX_AIPS_OPACR0                       0x40
#define MX6SX_AIPS_OPACR1                       0x44
#define MX6SX_AIPS_OPACR2                       0x48
#define MX6SX_AIPS_OPACR3                       0x4C
#define MX6SX_AIPS_OPACR4                       0x50

/*
 * Enhanced Periodic Timers
 */
#define MX6SX_EPIT1_BASE                         0x020D0000
#define MX6SX_EPIT2_BASE                         0x020D4000
#define MX6SX_EPIT_SIZE                          0x4000

#define MX6SX_EPIT1_IRQ                          88
#define MX6SX_EPIT2_IRQ                          89

/*
 * EPIT Registers, off set from base address
 */
#define MX6SX_EPIT_CR                            0x00    /* Control Register */
#define MX6SX_EPIT_SR                            0x04    /* Status Register */
#define MX6SX_EPIT_LR                            0x08    /* Load Register */
#define MX6SX_EPIT_CMPR                          0x0C    /* Compare Register */
#define MX6SX_EPIT_CNR                           0x10    /* Counter Register */

/*
 * SDRAM
 */
#define MX6SX_SDRAM_DUAL_FIXED_BASE              0x80000000

/*
 * MLB
 */
#define MX6SX_MLB_BASE                           0x0218C000
#define MX6SX_MLB_SIZE                           0x4000
#define MX6SX_MLB_IRQ                            85

/*
 * Multimedia Peripherals
 */

/* Document doesn't have GPU stuff in memory map */
#if 0
/* Graphics */

#define MX6SX_GPU2D_BASE                         0x00134000
#define MX6SX_GPU2D_SIZE                         0x4000
#define MX6SX_GPU3D_BASE                         0x00130000
#define MX6SX_GPU3D_SIZE                         0x4000

#define MX6SX_GPU3D_IRQ                          41
#define MX6SX_GPU2D_IRQ                          42
#define MX6SX_OPENVG_XAQ2                        43


/* Image Processing: IPUv3M */
#define MX6SX_IPU1_BASE                          0x02600000

#define MX6SX_IPU1_ERR_IRQ                       37
#define MX6SX_IPU1_SYNC_IRQ                      38
#define MX6SX_IPU2_BASE                          0x02800000

#define MX6SX_IPU_SIZE                           0x400000

#define MX6SX_IPU2_ERR_IRQ                       39
#define MX6SX_IPU2_SYNC_IRQ                      40

/* Video Processing: VPU */
#define MX6SX_VPU_BASE                           0x02040000
#define MX6SX_VPU_SIZE                           0x3C000
#define MX6SX_VPU_IRQ                            44

#endif

/*
  * TODO, waiting for next TRM
  * IC Identification Module (IIM)
  */
#define MX6SX_IIM_BASE
#define MX6SX_IIM_STAT                           0x0
#define MX6SX_IIM_STATM                          0x4
#define MX6SX_IIM_ERR                            0x8
#define MX6SX_IIM_EMASK                          0xC
#define MX6SX_IIM_FCTL                           0x10
#define MX6SX_IIM_UA                             0x14
#define MX6SX_IIM_LA                             0x18
#define MX6SX_IIM_SDAT                           0x1C
#define MX6SX_IIM_PREV                           0x20
#define MX6SX_IIM_SREV                           0x24
#define MX6SX_IIM_PREG_P                         0x28
#define MX6SX_IIM_SCS0                           0x2C
#define MX6SX_IIM_SCS2                           0x34
#define MX6SX_IIM_SCS3                           0x38

/*
 * GPC
 */
#define MX6SX_GPC_BASE                           0x020DC000

#define MX6SX_GPC_CNTR                           0x00
#define MX6SX_GPC_PGR                            0x04
#define MX6SX_GPC_IMR1                           0x08
#define MX6SX_GPC_IMR2                           0x0C
#define MX6SX_GPC_IMR3                           0x10
#define MX6SX_GPC_IMR4                           0x14
#define MX6SX_GPC_ISR1                           0x18
#define MX6SX_GPC_ISR2                           0x1C
#define MX6SX_GPC_ISR3                           0x20
#define MX6SX_GPC_ISR4                           0x24
#define MX6SX_GPC_GPU_CTRL                       0x260

/*
 * On-Chip eFuse OTP Controller
 */
#define MX6SX_OCOTP_BASE                         0x021BC000
#define MX6SX_OCOTP_MAC0                         0x620
#define MX6SX_OCOTP_MAC1                         0x630
/*
  * System Reset Controller (SRC)
  */
#define MX6SX_SRC_BASE                           0x020D8000
#define MX6SX_SRC_SCR                            0x000
#define MX6SX_SRC_SBMR                           0x004
#define MX6SX_SRC_SRSR                           0x008
#define MX6SX_SRC_SAIAR                          0x00c
#define MX6SX_SRC_SAIRAR                         0x010
#define MX6SX_SRC_SISR                           0x014
#define MX6SX_SRC_SIMR                           0x018
#define MX6SX_SRC_SBMR2                          0x01c
#define MX6SX_SRC_GPR1                           0x020
#define MX6SX_SRC_GPR2                           0x024
#define MX6SX_SRC_GPR3                           0x028
#define MX6SX_SRC_GPR4                           0x02c
#define MX6SX_SRC_GPR5                           0x030
#define MX6SX_SRC_GPR6                           0x034
#define MX6SX_SRC_GPR7                           0x038
#define MX6SX_SRC_GPR8                           0x03c
#define MX6SX_SRC_GPR9                           0x040
#define MX6SX_SRC_GPR10                          0x044

#define BP_SRC_SCR_CORE0_RST                     13
#define BP_SRC_SCR_CORES_DBG_RST                 21
#define BP_SRC_SCR_CORE1_ENABLE                  22
#define SCR_WARM_RESET_ENABLE                     1


/*
 * QSPI
 */
#define MX6SX_QSPI1_BASE    (0x21E0000)
#define MX6SX_QSPI2_BASE    (0x21E4000)


/* Trust Zone Address Space Controller (TZASC) */
#define MX6SX_TZASC                             0x021D0000

#define TZASC_CONFIGURATION                    0x000
#define TZASC_ACTION                           0x004
#define TZASC_LOCKDOWN_RANGE                   0x008
#define TZASC_LOCKDOWN_SELECT                  0x00C
#define TZASC_INT_STATUS                       0x010
#define TZASC_INT_CLEAR                        0x014
#define TZASC_FAIL_ADDRESS_LOW                 0x020
#define TZASC_FAIL_ADDRESS_HIGH                0x024
#define TZASC_FAIL_CONTROL                     0x028
#define TZASC_FAIL_ID                          0x02C
#define TZASC_SPECULATION_CONTROL              0x030
#define TZASC_SECURITY_INVERSION_EN            0x034
#define TZASC_REGION_SETUP_LOW(n)              (0x100 + (n*0x10))
#define TZASC_REGION_SETUP_HIGH(n)             (0x104 + (n*0x10))
#define TZASC_REGION_ATTRIBUTES(n)             (0x108 + (n*0x10))
	#define TZASC_REGION_PERM_ENABLEALL    (0xf << 28)
	#define TZASC_REGION_SIZE_64MB         (0x19 << 1)
	#define TZASC_REGION_SIZE_128MB        (0x1a << 1)
	#define TZASC_REGION_SIZE_256MB        (0x1b << 1)
	#define TZASC_REGION_SIZE_512MB        (0x1c << 1)
	#define TZASC_REGION_SIZE_1GB          (0x1d << 1)
	#define TZASC_REGION_SIZE_2GB          (0x1f << 1)
	#define TZASC_REGION_ATTR_ENABLE       (1 << 0)

#define CCM_PLL_ENET__ENET2_ENABLE            (1 << 21)
#define CCM_PLL_ENET__ENET1_ENABLE            (1 << 20)
#define CCM_PLL_ENET__ENABLE_125M             (1 << 19)
#define CCM_PLL_ENET__ENET0_ENABLE            (1 << 13)
#define CCM_PLL_ENET__ENET_1_DIV_125M         (3 << 2)
#define CCM_PLL_ENET__ENET_2_DIV_125M         (3 << 0)

/* ICR macros */
#define ICR_LOW_LEVEL                         0x0
#define ICR_HIGH_LEVEL                        0x1
#define ICR_RISING_EDGE                       0x2
#define ICR_FALLING_EDGE                      0x3

#define _ICR_MASK                             0x3
#define _ICR1(intr, type)                     ((type) << ((intr) * 2))
#define _ICR2(intr, type)                     ((type) << (((intr) - 16) * 2))
#define ICR_MASK(intr)                        (((intr) < 16) ? _ICR1((intr), _ICR_MASK) : _ICR2((intr), _ICR_MASK))
#define ICR_SET_INTR(intr, type)              (((intr) < 16) ? _ICR1((intr), (type))    : _ICR2((intr), (type)))
                                             
                                             
#endif  /* __ARM_MX6SX_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/arm/mx6sx.h $ $Rev: 762955 $")
#endif
