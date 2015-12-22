/*
 * $QNXLicenseC:
 * Copyright 2012, QNX Software Systems. 
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




/*
 * TI Sitara AM335x (based on ARM Cortex A-8 core)
 */

#ifndef    __ARM_AM335X_H_INCLUDED
#define    __ARM_AM335X_H_INCLUDED

/* GPIO Base address */
#define AM335X_GPIO0_BASE          0x44E07000
#define AM335X_GPIO1_BASE          0x4804C000
#define AM335X_GPIO2_BASE          0x481AC000
#define AM335X_GPIO3_BASE          0x481AE000
#define AM335X_GPIO_SIZE           0x00001000

#define GPIO_REVISION              0x000
#define GPIO_SYSCONFIG             0x010
#define GPIO_EOI                   0x020
#define GPIO_IRQSTATUS_RAW_0       0x024
#define GPIO_IRQSTATUS_RAW_1       0x028
#define GPIO_IRQSTATUS_0           0x02C
#define GPIO_IRQSTATUS_1           0x030
#define GPIO_IRQSTATUS_SET_0       0x034
#define GPIO_IRQSTATUS_SET_1       0x038
#define GPIO_IRQSTATUS_CLR_0       0x03C
#define GPIO_IRQSTATUS_CLR_1       0x040
#define GPIO_SYSSTATUS             0x114
#define GPIO_CTRL                  0x130
#define GPIO_OE                    0x134
#define GPIO_DATAIN                0x138
#define GPIO_DATAOUT               0x13C
#define GPIO_LEVELDETECT0          0x140
#define GPIO_LEVELDETECT1          0x144
#define GPIO_RISINGDETECT          0x148
#define GPIO_FALLINGDETECT         0x14C
#define GPIO_DEBOUNCENABLE         0x150
#define GPIO_DEBOUNCINGTIME        0x154
#define GPIO_CLEARDATAOUT          0x190
#define GPIO_SETDATAOUT            0x194

/*
 * Interrupt Controller
 */
#define AM335X_INTC_BASE           0x48200000
#define AM335X_INTC_SIZE           0x1000

#define AM335X_I2C0_BASE           0x44E0B000
#define AM335X_I2C1_BASE           0x4802A000
#define AM335X_I2C2_BASE           0x4819C000

#define AM335X_I2C0_IRQ            70
#define AM335X_I2C1_IRQ            71
#define AM335X_I2C2_IRQ            30

/* PRCM Base Address */
#define PRCM_BASE                  0x44E00000
#define CM_WKUP_OFFSET             0x00000400
#define CM_DPLL_OFFSET             0x00000500
#define CM_MPU_OFFSET              0x00000600
#define CM_DEVICE_OFFSET           0x00000700
#define CM_RTC_OFFSET              0x00000800
#define CM_GFX_OFFSET              0x00000900
#define PRM_PER                    0x00000C00
#define PRM_WKUP                   0x00000D00
#define PRM_MPU                    0x00000E00
#define PRM_DEVICE                 0x00000F00
    // PRM_RSTCTRL Register (offset = 0h)
    #define RST_GLOBAL_WARM_SW               (1<<0)
    #define RST_GLOBAL_COLD_SW               (1<<1)
#define PRM_RTC                    0x00001000
#define PRM_GFX                    0x00001100
#define PRM_CEFUSE                 0x00001200
#define CM_PRCM_SIZE               0x00003000

#define AM335X_CM_PER_L4LS_CLKSTCTRL    (PRCM_BASE + 0x00)            /* This register enables the domain power state transition.                       */
                                                                      /* It controls the SW supervised clock domain state                               */
                                                                      /* transition between ON-PER and ON-INPER states. It                              */
                                                                      /* also hold one status bit per clock input of the domain.                        */
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_TIMER6      (1<<28)        /* This field indicates the state of the TIMER6 CLKTIMER clock in the GCLK domain.*/
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_TIMER5      (1<<27)        /* This field indicates the state of the TIMER5 CLKTIMER clock in the GCLK domain.*/
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_GPIO_5      (1<<26)        /* This field indicates the state of the GPIO DBCLK clock in the GDBCLK domain.   */
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_SPI_GCL     (1<<25)        /* This field indicates the state of the SPI_GCLK clock in the domain.            */
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_I2C_FCLK    (1<<24)        /* This field indicates the state of the I2C _FCLK clock in the domain.           */
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_GPIO_4      (1<<22)        /* This field indicates the state of the GPIO4_GDBCLK clock in the GDBCLK domain. */
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_GPIO_3      (1<<21)        /* This field indicates the state of the GPIO3_GDBCLK clock in the GDBCLK domain. */
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_GPIO_2      (1<<20)        /* This field indicates the state of the GPIO2_ GDBCLK clock in the GDBCLK domain.*/
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_GPIO_1      (1<<19)        /* This field indicates the state of the GPIO1_GDBCLK clock in the GDBCLK domain. */
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_GPIO_6      (1<<18)        /* This field indicates the state of the GPIO6_GDBCLK clock in the GDBCLK domain. */
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_LCDC_GC     (1<<17)        /* This field indicates the state of the LCD clock in the domain.                 */
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_TIMER4      (1<<16)        /* This field indicates the state of the TIMER4 CLKTIMER clock in the GCLK domain.*/
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_TIMER3      (1<<15)        /* This field indicates the state of the TIMER3 CLKTIMER clock in the GCLK domain.*/
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_TIMER2      (1<<14)        /* This field indicates the state of the TIMER2 CLKTIMER clock in the GCLK domain.*/
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_TIMER7      (1<<13)        /* This field indicates the state of the TIMER7 CLKTIMER clock in the GCLK domain.*/
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_CAN_CLK     (1<<11)        /* This field indicates the state of the CAN_CLK clock in the domain.             */
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_UART_GF     (1<<10)        /* This field indicates the state of the UART_GFCLK clock in the CLK domain.      */
    #define AM335X_CM_PER_L4LS_CLKACTIVITY_L4LS_GC     (1<< 8)        /* This field indicates the state of the L4LS_GCLK clock in the domain.           */

#define AM335X_CM_PER_L3S_CLKSTCTRL        (PRCM_BASE + 0x04)         /* This register enables the domain power state transition. Section 8.1.2.1.2     */
                                                                      /* It controls the SW supervised clock domain state                               */
                                                                      /* transition between ON-ACTIVE and ON-INACTIVE                                   */
                                                                      /* states. It also hold one status bit per clock input of the domain.             */
#define AM335X_CM_PER_L4FW_CLKSTCTRL       (PRCM_BASE + 0x08)         /* This register enables the domain power state transition. Section 8.1.2.1.3     */
                                                                      /* It controls the SW supervised clock domain state                               */
                                                                      /* transition between ON-ACTIVE and ON-INACTIVE states.                           */
                                                                      /* It also hold one status bit per clock input of the domain.                     */
                                                                      /* transition between ON-ACTIVE and ON-INACTIVE                                   */
                                                                      /* states. It also hold one status bit per clock input of the domain.             */
#define AM335X_CM_PER_L3_CLKSTCTRL         (PRCM_BASE + 0x0C)         /* This register enables the domain power state transition.                       */
                                                                      /* It controls the SW supervised clock domain state                               */
                                                                      /* transition between ON-ACTIVE and ON-INACTIVE states.                           */
                                                                      /* It also hold one status bit per clock input of the domain.                     */
#define AM335X_CM_PER_PCIE_CLKCTRL         (PRCM_BASE + 0x10)
#define AM335X_CM_PER_CPGMAC0_CLKCTRL      (PRCM_BASE + 0x14)
#define AM335X_CM_PER_LCDC_CLKCTRL         (PRCM_BASE + 0x18)
#define AM335X_CM_PER_USB0_CLKCTRL         (PRCM_BASE + 0x1C)
#define AM335X_CM_PER_MLB_CLKCTRL          (PRCM_BASE + 0x20)
#define AM335X_CM_PER_TPTC0_CLKCTRL        (PRCM_BASE + 0x24)
#define AM335X_CM_PER_EMIF_CLKCTRL         (PRCM_BASE + 0x28)
#define AM335X_CM_PER_OCMCRAM_CLKCTRL      (PRCM_BASE + 0x2C)
#define AM335X_CM_PER_GPMC_CLKCTRL         (PRCM_BASE + 0x30)
#define AM335X_CM_PER_MCASP0_CLKCTRL       (PRCM_BASE + 0x34)
#define AM335X_CM_PER_UART5_CLKCTRL        (PRCM_BASE + 0x38)
#define AM335X_CM_PER_MMC0_CLKCTRL         (PRCM_BASE + 0x3C)
#define AM335X_CM_PER_ELM_CLKCTRL          (PRCM_BASE + 0x40)
#define AM335X_CM_PER_I2C2_CLKCTRL         (PRCM_BASE + 0x44)
#define AM335X_CM_PER_I2C1_CLKCTRL         (PRCM_BASE + 0x48)
#define AM335X_CM_PER_SPI0_CLKCTRL         (PRCM_BASE + 0x4C)
#define AM335X_CM_PER_SPI1_CLKCTRL         (PRCM_BASE + 0x50)
#define AM335X_CM_PER_L4LS_CLKCTRL         (PRCM_BASE + 0x60)
#define AM335X_CM_PER_L4FW_CLKCTRL         (PRCM_BASE + 0x64)
#define AM335X_CM_PER_MCASP1_CLKCTRL       (PRCM_BASE + 0x68)
#define AM335X_CM_PER_UART1_CLKCTRL        (PRCM_BASE + 0x6C)
#define AM335X_CM_PER_UART2_CLKCTRL        (PRCM_BASE + 0x70)
#define AM335X_CM_PER_UART3_CLKCTRL        (PRCM_BASE + 0x74)
#define AM335X_CM_PER_UART4_CLKCTRL        (PRCM_BASE + 0x78)
#define AM335X_CM_PER_TIMER7_CLKCTRL       (PRCM_BASE + 0x7C)
#define AM335X_CM_PER_TIMER2_CLKCTRL       (PRCM_BASE + 0x80)
#define AM335X_CM_PER_TIMER3_CLKCTRL       (PRCM_BASE + 0x84)
#define AM335X_CM_PER_TIMER4_CLKCTRL       (PRCM_BASE + 0x88)
#define AM335X_CM_PER_MCASP2_CLKCTRL       (PRCM_BASE + 0x8C)
#define AM335X_CM_PER_RNG_CLKCTRL          (PRCM_BASE + 0x90)
#define AM335X_CM_PER_AES0_CLKCTRL         (PRCM_BASE + 0x94)
#define AM335X_CM_PER_AES1_CLKCTRL         (PRCM_BASE + 0x98)
#define AM335X_CM_PER_DES_CLKCTRL          (PRCM_BASE + 0x9C)
#define AM335X_CM_PER_SHA0_CLKCTRL         (PRCM_BASE + 0xA0)
#define AM335X_CM_PER_PKA_CLKCTRL          (PRCM_BASE + 0xA4)
#define AM335X_CM_PER_GPIO6_CLKCTRL        (PRCM_BASE + 0xA8)
#define AM335X_CM_PER_GPIO1_CLKCTRL        (PRCM_BASE + 0xAC)
#define AM335X_CM_PER_GPIO2_CLKCTRL        (PRCM_BASE + 0xB0)
#define AM335X_CM_PER_GPIO3_CLKCTRL        (PRCM_BASE + 0xB4)
#define AM335X_CM_PER_GPIO4_CLKCTRL        (PRCM_BASE + 0xB8)
#define AM335X_CM_PER_TPCC_CLKCTRL         (PRCM_BASE + 0xBC)
#define AM335X_CM_PER_DCAN0_CLKCTRL        (PRCM_BASE + 0xC0)
#define AM335X_CM_PER_DCAN1_CLKCTRL        (PRCM_BASE + 0xC4)
#define AM335X_CM_PER_EPWMSS1_CLKCTRL      (PRCM_BASE + 0xCC)
#define AM335X_CM_PER_EMIF_FW_CLKCTRL      (PRCM_BASE + 0xD0)
#define AM335X_CM_PER_EPWMSS0_CLKCTRL      (PRCM_BASE + 0xD4)
#define AM335X_CM_PER_EPWMSS2_CLKCTRL      (PRCM_BASE + 0xD8)
#define AM335X_CM_PER_L3_INSTR_CLKCTRL     (PRCM_BASE + 0xDC)
#define AM335X_CM_PER_L3_CLKCTRL           (PRCM_BASE + 0xE0)
#define AM335X_CM_PER_IEEE5000_CLKCTRL     (PRCM_BASE + 0xE4)
#define AM335X_CM_PER_ICSS_CLKCTRL         (PRCM_BASE + 0xE8)
#define AM335X_CM_PER_TIMER5_CLKCTRL       (PRCM_BASE + 0xEC)
#define AM335X_CM_PER_TIMER6_CLKCTRL       (PRCM_BASE + 0xF0)
#define AM335X_CM_PER_MMC1_CLKCTRL         (PRCM_BASE + 0xF4)
#define AM335X_CM_PER_MMC2_CLKCTRL         (PRCM_BASE + 0xF8)
#define AM335X_CM_PER_TPTC1_CLKCTRL        (PRCM_BASE + 0xFC)
#define AM335X_CM_PER_TPTC2_CLKCTRL        (PRCM_BASE + 0x100)
#define AM335X_CM_PER_GPIO5_CLKCTRL        (PRCM_BASE + 0x104)
#define AM335X_CM_PER_SPINLOCK_CLKCTRL     (PRCM_BASE + 0x10C)
#define AM335X_CM_PER_MAILBOX0_CLKCTRL     (PRCM_BASE + 0x110)
#define AM335X_CM_PER_L4HS_CLKSTCTRL       (PRCM_BASE + 0x11C)       /* This register enables the domain power state transition.           */
                                                                     /* It controls the SW supervised clock domain state                   */
                                                                     /* transition between ON-ACTIVE and ON-INACTIVE                       */
                                                                     /* states. It also hold one status bit per clock input of the domain. */
#define AM335X_CM_PER_L4HS_CLKCTRL         (PRCM_BASE + 0x120)       /* This register manages the L4 Fast clocks.                          */
#define AM335X_CM_PER_OCPWP_L3_CLKSTCT     (PRCM_BASE + 0x12C)       /* This register enables the domain power state transition.           */
                                                                     /* RL It controls the SW supervised clock domain state                */
                                                                     /* transition between ON-ACTIVE and ON-INACTIVE                       */
                                                                     /* states. It also hold one status bit per clock input of the domain. */
                                                                    
#define AM335X_CM_PER_OCPWP_CLKCTRL        (PRCM_BASE + 0x130)       /* This register manages the OCPWP clocks.                            */
#define AM335X_CM_PER_MAILBOX1_CLKCTRL     (PRCM_BASE + 0x134)
#define AM335X_CM_PER_ICSS_CLKSTCTRL       (PRCM_BASE + 0x140)       /* This register enables the clock domain state transition.           */
                                                                     /* controls the SW supervised clock domain state transition           */
                                                                     /* between ON-ACTIVE and ON-INACTIVE states. It also                  */
                                                                     /* hold one status bit per clock input of the domain.                 */
#define AM335X_CM_PER_CPSW_CLKSTCTRL       (PRCM_BASE + 0x144)       /* This register enables the clock domain state transition.           */
                                                                     /* controls the SW supervised clock domain state transition           */
                                                                     /* between ON-ACTIVE and ON-INACTIVE states. It also                  */
                                                                     /* hold one status bit per clock input of the domain.                 */

#define AM335X_CM_PER_LCDC_CLKSTCTRL       (PRCM_BASE + 0x148)       /* This register enables the clock domain state transition.           */
                                                                     /* controls the SW supervised clock domain state transition           */
                                                                     /* between ON-ACTIVE and ON-INACTIVE states. It also                  */
                                                                     /* hold one status bit per clock input of the domain.                 */
#define AM335X_CM_PER_CLKDIV32K_CLKCTRL    (PRCM_BASE + 0x14C)       /* This register manages the CLKDIV32K clocks.                        */
#define AM335X_CM_PER_CLK_24MHZ_CLKSTCT    (PRCM_BASE + 0x150)       /* This register enables the clock domain state transition.           */
                                                                     /* RL controls the SW supervised clock domain state transition        */
                                                                     /* between ON-ACTIVE and ON-INACTIVE states. It also                  */
                                                                     /* hold one status bit per clock input of the domain.                 */


#define CM_WKUP_ADC_TSC_CLKCTRL            (PRCM_BASE + CM_WKUP_OFFSET + 0xBC)

/* AM335X_CM_CLKSEL_DPLL_PER bits: */
#define DPLL_MULT(x)                       x << 8
#define DPLL_DIV(x)                        x
#define DPLL_MULT_MASK                     0x7FFFF

/* AM335X_CM_CLKMODE_DPLL_PER bits: */
#define DPLL_MN_BYP_MODE                   0x00000004
#define DPLL_LOCK_MODE                     0x00000007
/* AM335X_CM_DIV_M2_DPLL_PER bits: */
#define DPLL_CLKOUT_DIV_MASK               0x7F
/* AM335X_CM_IDLEST_DPLL_PER bits: */
#define ST_MN_BYPASS                       0x00000100

#define AM335X_CM_CLKDCOLDO_DPLL_PER       (PRCM_BASE + CM_WKUP_OFFSET + 0x7C)
/* AM335X_CM_CLKDCOLDO_DPLL_PER bits: */
#define DPLL_CLKDCOLDO_GATE_CTRL           (1 << 8)

/* Control Module Base Address */
#define AM335X_CTRL_BASE                   0x44E10000
#define AM335X_CTRL_BASE_SIZE              0x00002000
 
#define AM335X_MAC_ID0_LO                  0x00000630
#define AM335X_MAC_ID0_HI                  0x00000634
#define AM335X_MAC_ID1_LO                  0x00000638
#define AM335X_MAC_ID1_HI                  0x0000063c

/* AM335x USB setup */
#define AM335X_USBCTRL0                    (AM335X_CTRL_BASE + 0x0620)
#define AM335X_USBSTS0                     (AM335X_CTRL_BASE + 0x0624)
#define AM335X_USBCTRL1                    (AM335X_CTRL_BASE + 0x0628)
#define AM335X_USBSTS1                     (AM335X_CTRL_BASE + 0x062c)

/* AM335X_USBCTRLx bits */
#define USBPHY_CM_PWRDN                    (1 << 0)
#define USBPHY_OTG_PWRDN                   (1 << 1)
#define USBPHY_CHGDET_DIS                  (1 << 2)
#define USBPHY_CHGDET_RSTRT                (1 << 3)
#define USBPHY_SRCONDM                     (1 << 4)
#define USBPHY_SINKONDP                    (1 << 5)
#define USBPHY_CHGISINK_EN                 (1 << 6)
#define USBPHY_CHGVSRC_EN                  (1 << 7)
#define USBPHY_DMPULLUP                    (1 << 8)
#define USBPHY_DPPULLUP                    (1 << 9)
#define USBPHY_CDET_EXTCTL                 (1 << 10)
#define USBPHY_GPIO_MODE                   (1 << 12)
#define USBPHY_DPOPBUFCTL                  (1 << 13)
#define USBPHY_DMOPBUFCTL                  (1 << 14)
#define USBPHY_DPGPIO_PD                   (1 << 17)
#define USBPHY_DMGPIO_PD                   (1 << 18)
#define USBPHY_OTGVDET_EN                  (1 << 19)
#define USBPHY_OTGSESSEND_EN               (1 << 20)
#define USBPHY_DATA_POLARITY               (1 << 23)

#define CM_WKUP_BASE                       (PRCM_BASE + 0x400)

#define AM335X_CM_WKUP_CLKSTCTRL           (CM_WKUP_BASE + 0x00)    
#define AM335X_CM_WKUP_CONTROL_CLKCTRL     (CM_WKUP_BASE + 0x04)
#define AM335X_CM_WKUP_GPIO0_CLKCTRL       (CM_WKUP_BASE + 0x08)
#define AM335X_CM_WKUP_L4WKUP_CLKCTRL      (CM_WKUP_BASE + 0x0C)
#define AM335X_CM_WKUP_TIMER0_CLKCTRL      (CM_WKUP_BASE + 0x10)
#define AM335X_CM_WKUP_DEBUGSS_CLKCTRL     (CM_WKUP_BASE + 0x14)
#define AM335X_CM_L3_AON_CLKSTCTRL         (CM_WKUP_BASE + 0x18)
#define AM335X_CM_AUTOIDLE_DPLL_MPU        (CM_WKUP_BASE + 0x1C)
#define AM335X_CM_IDLEST_DPLL_MPU          (CM_WKUP_BASE + 0x20)
#define AM335X_CM_SSC_DELTAMSTEP_DPLL_MPU  (CM_WKUP_BASE + 0x24)
#define AM335X_CM_SSC_MODFREQDIV_DPLL_MPU  (CM_WKUP_BASE + 0x28)
#define AM335X_CM_CLKSEL_DPLL_MPU          (CM_WKUP_BASE + 0x2C)
#define AM335X_CM_AUTOIDLE_DPLL_DDR        (CM_WKUP_BASE + 0x30)
#define AM335X_CM_IDLEST_DPLL_DDR          (CM_WKUP_BASE + 0x34)
#define AM335X_CM_SSC_DELTAMSTEP_DPLL_DDR  (CM_WKUP_BASE + 0x38)
#define AM335X_CM_SSC_MODFREQDIV_DPLL_DDR  (CM_WKUP_BASE + 0x3C)
#define AM335X_CM_CLKSEL_DPLL_DDR          (CM_WKUP_BASE + 0x40)
#define AM335X_CM_AUTOIDLE_DPLL_DISP       (CM_WKUP_BASE + 0x44)
#define AM335X_CM_IDLEST_DPLL_DISP         (CM_WKUP_BASE + 0x48)
#define AM335X_CM_SSC_DELTAMSTEP_DPLL_DISP (CM_WKUP_BASE + 0x4C)
#define AM335X_CM_SSC_MODFREQDIV_DPLL_DISP (CM_WKUP_BASE + 0x50)
#define AM335X_CM_CLKSEL_DPLL_DISP         (CM_WKUP_BASE + 0x54)
#define AM335X_CM_AUTOIDLE_DPLL_CORE       (CM_WKUP_BASE + 0x58)
#define AM335X_CM_IDLEST_DPLL_CORE         (CM_WKUP_BASE + 0x5C)
#define AM335X_CM_SSC_DELTAMSTEP_DPLL_C    (CM_WKUP_BASE + 0x60)
#define AM335X_CM_SSC_MODFREQDIV_DPLL_C    (CM_WKUP_BASE + 0x64)
#define AM335X_CM_CLKSEL_DPLL_CORE         (CM_WKUP_BASE + 0x68)
#define AM335X_CM_AUTOIDLE_DPLL_PER        (CM_WKUP_BASE + 0x6C)
#define AM335X_CM_IDLEST_DPLL_PER          (CM_WKUP_BASE + 0x70)
#define AM335X_CM_SSC_DELTAMSTEP_DPLL_P    (CM_WKUP_BASE + 0x74)
#define AM335X_CM_SSC_MODFREQDIV_DPLL_P    (CM_WKUP_BASE + 0x78)
#define AM335X_CM_CLKDCOLDO_DPLL_PERx      (CM_WKUP_BASE + 0x7C)
#define AM335X_CM_DIV_M4_DPLL_CORE         (CM_WKUP_BASE + 0x80)
#define AM335X_CM_DIV_M5_DPLL_CORE         (CM_WKUP_BASE + 0x84)
#define AM335X_CM_CLKMODE_DPLL_MPU         (CM_WKUP_BASE + 0x88)
#define AM335X_CM_CLKMODE_DPLL_PER         (CM_WKUP_BASE + 0x8C)
#define AM335X_CM_CLKMODE_DPLL_CORE        (CM_WKUP_BASE + 0x90)
#define AM335X_CM_CLKMODE_DPLL_DDR         (CM_WKUP_BASE + 0x94)
#define AM335X_CM_CLKMODE_DPLL_DISP        (CM_WKUP_BASE + 0x98)
#define AM335X_CM_CLKSEL_DPLL_PER          (CM_WKUP_BASE + 0x9C)
#define AM335X_CM_DIV_M2_DPLL_DDR          (CM_WKUP_BASE + 0xA0)
#define AM335X_CM_DIV_M2_DPLL_DISP         (CM_WKUP_BASE + 0xA4)
#define AM335X_CM_DIV_M2_DPLL_MPU          (CM_WKUP_BASE + 0xA8)
#define AM335X_CM_DIV_M2_DPLL_PER          (CM_WKUP_BASE + 0xAC)
#define AM335X_CM_WKUP_WKUP_M3_CLKCTRL     (CM_WKUP_BASE + 0xB0)
#define AM335X_CM_WKUP_UART0_CLKCTRL       (CM_WKUP_BASE + 0xB4)
#define AM335X_CM_WKUP_I2C0_CLKCTRL        (CM_WKUP_BASE + 0xB8)
#define AM335X_CM_WKUP_ADC_TSC_CLKCTRL     (CM_WKUP_BASE + 0xBC)
#define AM335X_CM_WKUP_SMARTREFLEX0_CL     (CM_WKUP_BASE + 0xC0)
#define AM335X_CM_WKUP_TIMER1_CLKCTRL      (CM_WKUP_BASE + 0xC4)
#define AM335X_CM_WKUP_SMARTREFLEX1_CL     (CM_WKUP_BASE + 0xC8)
#define AM335X_CM_L4_WKUP_AON_CLKSTCTR     (CM_WKUP_BASE + 0xCC)
#define AM335X_CM_WKUP_WDT1_CLKCTRLx       (CM_WKUP_BASE + 0xD4)
#define AM335X_CM_DIV_M6_DPLL_CORE         (CM_WKUP_BASE + 0xD8)


#define CM_DPLL_BASE                       (PRCM_BASE + 0x500)

#define AM335X_CLKSEL_TIMER7_CLK           (CM_DPLL_BASE+0x04)    /* Selects the Mux select line for TIMER7 clock         */
#define AM335X_CLKSEL_TIMER2_CLK           (CM_DPLL_BASE+0x08)    /* Selects the Mux select line for TIMER2 clock         */
#define AM335X_CLKSEL_TIMER3_CLK           (CM_DPLL_BASE+0x0C)    /* Selects the Mux select line for TIMER3 clock         */
#define AM335X_CLKSEL_TIMER4_CLK           (CM_DPLL_BASE+0x10)    /* Selects the Mux select line for TIMER4 clock         */
#define AM335X_CM_MAC_CLKSEL               (CM_DPLL_BASE+0x14)    /* Selects the clock divide ration for MII clock        */
#define AM335X_CLKSEL_TIMER5_CLK           (CM_DPLL_BASE+0x18)    /* Selects the Mux select line for TIMER5 clock         */
#define AM335X_CLKSEL_TIMER6_CLK           (CM_DPLL_BASE+0x1C)    /* Selects the Mux select line for TIMER6 clock         */
#define AM335X_CM_CPTS_RFT_CLKSEL          (CM_DPLL_BASE+0x20)    /* Selects the Mux select line for CPTS RFT clock       */
#define AM335X_CLKSEL_TIMER1MS_CLK         (CM_DPLL_BASE+0x28)    /* Selects the Mux select line for TIMER1 clock         */
#define AM335X_CLKSEL_GFX_FCLK             (CM_DPLL_BASE+0x2C)    /* Selects the divider value for GFX clock              */
#define AM335X_CLKSEL_ICSS_OCP_CLK         (CM_DPLL_BASE+0x30)    /* Controls the Mux select line for ICSS OCP clock      */
#define AM335X_CLKSEL_LCDC_PIXEL_CLK       (CM_DPLL_BASE+0x34)    /* Controls the Mux select line for LCDC PIXEL clock    */
#define AM335X_CLKSEL_WDT1_CLK             (CM_DPLL_BASE+0x38)    /* Selects the Mux select line for Watchdog1 clock      */
#define AM335X_CLKSEL_GPIO0_DBCLK          (CM_DPLL_BASE+0x3C)    /* Selects the Mux select line for GPIO0 debounce clock */


/*
 * EDMA Channel controllers
 */
#define AM335X_EDMA0_CC_BASE          0x49000000
#define AM335X_EDMA_CC_SIZE           0x5000

/*
 * EDMA Transfer controllers
 */
#define AM335X_EDMA0_TC0_BASE         0x49800000
#define AM335X_EDMA0_TC1_BASE         0x49900000
#define AM335X_EDMA0_TC2_BASE         0x49A00000
#define AM335X_EDMA_TC_SIZE           0x100000

/*
 * Registers, Offset from EDMA base
 */
#define AM335X_EDMA_PID               0x00
#define AM335X_EDMA_CCCFG             0x04

/* Global registers, offset from EDMA base */
#define AM335X_EDMA_QCHMAP(c)         (0x200 + (c) * 4)
#define AM335X_EDMA_DMAQNUM(c)        (0x240 + (c) * 4)
#define AM335X_EDMA_QDMAQNUM          0x260
#define AM335X_EDMA_QUEPRI            0x284
#define AM335X_EDMA_EMR               0x300
#define AM335X_EDMA_EMRH              0x304
#define AM335X_EDMA_EMCR              0x308
#define AM335X_EDMA_EMCRH             0x30C
#define AM335X_EDMA_QEMR              0x310
#define AM335X_EDMA_QEMCR             0x314
#define AM335X_EDMA_CCERR             0x318
#define AM335X_EDMA_CCERRCLR          0x31C
#define AM335X_EDMA_EEVAL             0x320
#define AM335X_EDMA_DRAE(c)           (0x340 + (c) * 8)
#define AM335X_EDMA_DRAEH(c)          (0x344 + (c) * 8)
#define AM335X_EDMA_QRAE(c)           (0x380 + (c) * 4)
#define AM335X_EDMA_QRAE(c)           (0x380 + (c) * 4)
#define AM335X_EDMA_Q0E(c)            (0x400 + (c) * 4)
#define AM335X_EDMA_Q1E(c)            (0x440 + (c) * 4)
#define AM335X_EDMA_QSTAT0            0x600
#define AM335X_EDMA_QSTAT1            0x604
#define AM335X_EDMA_QWMTHRA           0x620
#define AM335X_EDMA_CCSTAT            0x640

/* Channel registers, offset from EDMA base */
#define AM335X_EDMA_GLOBAL            0x1000
#define AM335X_EDMA_REGION0           0x2000
#define AM335X_EDMA_REGION1           0x2200
#define AM335X_EDMA_REGION2           0x2400
#define AM335X_EDMA_REGION3           0x2600
#define AM335X_EDMA_ER                0x00
#define AM335X_EDMA_ERH               0x04
#define AM335X_EDMA_ECR               0x08
#define AM335X_EDMA_ECRH              0x0C
#define AM335X_EDMA_ESR               0x10
#define AM335X_EDMA_ESRH              0x14
#define AM335X_EDMA_CER               0x18
#define AM335X_EDMA_CERH              0x1C
#define AM335X_EDMA_EER               0x20
#define AM335X_EDMA_EERH              0x24
#define AM335X_EDMA_EECR              0x28
#define AM335X_EDMA_EECRH             0x2C
#define AM335X_EDMA_EESR              0x30
#define AM335X_EDMA_EESRH             0x34
#define AM335X_EDMA_SER               0x38
#define AM335X_EDMA_SERH              0x3C
#define AM335X_EDMA_SECR              0x40
#define AM335X_EDMA_SECRH             0x44
#define AM335X_EDMA_IER               0x50
#define AM335X_EDMA_IERH              0x54
#define AM335X_EDMA_IECR              0x58
#define AM335X_EDMA_IECRH             0x5C
#define AM335X_EDMA_IESR              0x60
#define AM335X_EDMA_IESRH             0x64
#define AM335X_EDMA_IPR               0x68
#define AM335X_EDMA_IPRH              0x6C
#define AM335X_EDMA_ICR               0x70
#define AM335X_EDMA_ICRH              0x74
#define AM335X_EDMA_IEVAL             0x78
#define AM335X_EDMA_QER               0x80
#define AM335X_EDMA_QEER              0x84
#define AM335X_EDMA_QEECR             0x88
#define AM335X_EDMA_QEESR             0x8C
#define AM335X_EDMA_QSER              0x90
#define AM335X_EDMA_QSECR             0x94

/* Parameter RAM base, offser from EDMA base */
#define AM335X_EDMA_PARAM_BASE        0x4000


#define AM335X_SRAM0_START            0x402F0400

#define AM335X_UART0_BASE             0x44E09000
#define AM335X_UART1_BASE             0x48022000
#define AM335X_UART2_BASE             0x48024000
#define AM335X_UART3_BASE             0x481A6000

/* USB */
#define AM335X_USBSS                  0x47400000
#define AM335X_USB0                   0x47401000
#define AM335X_USB0_PHY               0x47401300
#define AM335X_USB0_CORE              0x47401400
#define AM335X_USB1                   0x47401800
#define AM335X_USB1_PHY               0x47401B00
#define AM335X_USB1_CORE              0x47401C00
#define AM335X_USB_CPPI_CONTROLLER    0x47402000
#define AM335X_USB_CPPI_SCHEDULER     0x47403000
#define AM335X_USB_QUEUE_MANAGER      0x47404000

/* MMC (SD/MMC) */
#define AM335X_MMCHS0_BASE            0x48060000    
#define AM335X_MMC1_BASE              0x481D8000
#define AM335X_MMCHS2_BASE            0x47810000

#define AM335X_MMC_SIZE               512

/* EDMA event numbers */
#define EDMA_MMCHS1_SDTXEVT1          2
#define EDMA_MMCHS1_SDRXEVT1          3
#define EDMA_MMCHS0_SDTXEVT0          24
#define EDMA_MMCHS0_SDRXEVT0          25

/* BCH Error Location Module */
#define ELM_BASE                      0x48080000

/* Watchdog Timer */
#define AM335X_WDT_BASE              0x44E35000
#define AM335X_WDT_SIZE              0x64

#define WDT_WIDR                     0x00
#define WDT_WDSC                     0x10
#define WDT_WDST                     0x14
#define WDT_WISR                     0x18
#define WDT_WIER                     0x1C
#define WDT_WWER                     0x20
#define WDT_WCLR                     0x24
#define WDT_WCRR                     0x28
#define WDT_WLDR                     0x2C
#define WDT_WTGR                     0x30
#define WDT_WWPS                     0x34
#define WDT_WDLY                     0x44
#define WDT_WSPR                     0x48
#define WDT_WIRQSTATRAW              0x54
#define WDT_WIRQSTAT                 0x58
#define WDT_WIRQENSET                0x5C
#define WDT_WIRQENCLR                0x60

#define WDT_WSPR_STOPVAL1            0x0000AAAA
#define WDT_WSPR_STOPVAL2            0x00005555
#define WDT_WSPR_STARTVAL1           0x0000BBBB
#define WDT_WSPR_STARTVAL2           0x00004444

#define WDTI_FCLK                    32768              /* 32KHz */

#define PRCM_CM_PER                  0x0000
#define PRCM_CM_WKUP                 0x0400

/* EMIF Base address */
#define EMIF4_0_CFG_BASE             0x4C000000

/* GPMC Registers */
#define GPMC_BASE                    0x50000000
#define GPMC_SYSCONFIG               (GPMC_BASE + 0x10)
#define GPMC_IRQSTATUS               (GPMC_BASE + 0x18)
#define GPMC_IRQENABLE               (GPMC_BASE + 0x1C)
#define GPMC_TIMEOUT_CONTROL         (GPMC_BASE + 0x40)
#define GPMC_CONFIG                  (GPMC_BASE + 0x50)
#define GPMC_CONFIG1_0               (GPMC_BASE + 0x60)
#define GPMC_CONFIG2_0               (GPMC_BASE + 0x64)
#define GPMC_CONFIG3_0               (GPMC_BASE + 0x68)
#define GPMC_CONFIG4_0               (GPMC_BASE + 0x6C)
#define GPMC_CONFIG5_0               (GPMC_BASE + 0x70)
#define GPMC_CONFIG6_0               (GPMC_BASE + 0x74)
#define GPMC_CONFIG7_0               (GPMC_BASE + 0x78)

#define BIT(x)                       (1 << x)
#define CL_BIT(x)                    (0 << x)

/* Timer base addresses */
#define AM335X_TIMER_SIZE            0x1000
#define AM335X_TIMER0_BASE           0x44E05000
#define AM335X_TIMER1_BASE           0x44E06000

#define DDR_PHY_BASE_ADDR            0x44E12000
#define AM335X_TIMER1_1MS_BASE       0x44E31000
#define AM335X_TIMER2_BASE           0x48040000
#define AM335X_TIMER3_BASE           0x48042000
#define AM335X_TIMER4_BASE           0x48044000
#define AM335X_TIMER5_BASE           0x48046000
#define AM335X_TIMER6_BASE           0x48048000
#define AM335X_TIMER7_BASE           0x4804A000

/* Timer registers */
#define AM335X_TIMER_TIDR            0x00            /* Timer Identification Register */
#define AM335X_TIMER_THWINFO         0x04
#define AM335X_TIMER_TIOCP_CFG       0x10            /* Timer OCP Configuration Register */
#define AM335X_TIMER_IRQ_EOI         0x20            /* Timer IRQ EOI Register */
#define AM335X_TIMER_IRQSTATUS_RAW   0x24            /* Timer IRQ STATUS RAW Register */
#define AM335X_TIMER_IRQSTATUS       0x28            /* Timer IRQ STATUS Register */
#define AM335X_TIMER_IRQENABLE_SET   0x2C            /* Timer IRQ ENABLE SET Register*/
#define AM335X_TIMER_IRQENABLE_CLR   0x30            /* Timer IRQ ENABLE CLR Register*/
#define AM335X_TIMER_IRQWAKEEN       0x34            /* Timer IRQ WAKEUP ENABLE Register*/
#define AM335X_TIMER_TCLR            0x38            /* Timer Control Register */
#define AM335X_TIMER_TCRR            0x3C            /* Timer Counter Register */
#define AM335X_TIMER_TLDR            0x40            /* Timer Load Value Register*/
#define AM335X_TIMER_TTGR            0x44            /* Timer Trigger Register */
#define AM335X_TIMER_TWPS            0x48            /* Timer Write Posted Status Register */
#define AM335X_TIMER_TMAR            0x4C            /* Timer Match Register*/
#define AM335X_TIMER_TCAR1           0x50            /* Timer Capture Register 1 */
#define AM335X_TIMER_TSICR           0x54            /* Timer Synchronous Interface Control Register */
#define AM335X_TIMER_TCAR2           0x58            /* Timer Capture Register 2 */

/* Timer register bits */
#define AM335X_TIMER_MAT_EN_FLAG       BIT(0)        /* IRQ bit for Match */
#define AM335X_TIMER_OVF_EN_FLAG       BIT(1)        /* IRQ bit for Overflow */
#define AM335X_TIMER_TCAR_EN_FLAG      BIT(2)        /* IRQ bit for Compare */

#define AM335X_TIMER_TCLR_ST           BIT(0)        /* Start=1 Stop=0 */
#define AM335X_TIMER_TCLR_AR           BIT(1)        /* Auto reload */
#define AM335X_TIMER_TCLR_PTV_SHIFT       (2)        /* Pre-scaler shift value */
#define AM335X_TIMER_TCLR_PRE          BIT(5)        /* Pre-scaler enable for the timer input clk */
#define AM335X_TIMER_TCLR_PRE_DISABLE  CL_BIT(5)     /* Pre-scalar disable for the timer input clk */


#define EMIF4_0_SDRAM_CONFIG           (EMIF4_0_CFG_BASE + 0x08)
#define EMIF4_0_SDRAM_CONFIG2          (EMIF4_0_CFG_BASE + 0x0C)
#define EMIF4_0_SDRAM_REF_CTRL         (EMIF4_0_CFG_BASE + 0x10)
#define EMIF4_0_SDRAM_REF_CTRL_SHADOW  (EMIF4_0_CFG_BASE + 0x14)
#define EMIF4_0_SDRAM_TIM_1            (EMIF4_0_CFG_BASE + 0x18)
#define EMIF4_0_SDRAM_TIM_1_SHADOW     (EMIF4_0_CFG_BASE + 0x1C)
#define EMIF4_0_SDRAM_TIM_2            (EMIF4_0_CFG_BASE + 0x20)
#define EMIF4_0_SDRAM_TIM_2_SHADOW     (EMIF4_0_CFG_BASE + 0x24)
#define EMIF4_0_SDRAM_TIM_3            (EMIF4_0_CFG_BASE + 0x28)
#define EMIF4_0_SDRAM_TIM_3_SHADOW     (EMIF4_0_CFG_BASE + 0x2C)
#define EMIF4_0_DDR_PHY_CTRL_1         (EMIF4_0_CFG_BASE + 0xE4)
#define EMIF4_0_DDR_PHY_CTRL_1_SHADOW  (EMIF4_0_CFG_BASE + 0xE8)

#define EMIF4_1_SDRAM_CONFIG           (EMIF4_1_CFG_BASE + 0x08)
#define EMIF4_1_SDRAM_CONFIG2          (EMIF4_1_CFG_BASE + 0x0C)
#define EMIF4_1_SDRAM_REF_CTRL         (EMIF4_1_CFG_BASE + 0x10)
#define EMIF4_1_SDRAM_REF_CTRL_SHADOW  (EMIF4_1_CFG_BASE + 0x14)
#define EMIF4_1_SDRAM_TIM_1            (EMIF4_1_CFG_BASE + 0x18)
#define EMIF4_1_SDRAM_TIM_1_SHADOW     (EMIF4_1_CFG_BASE + 0x1C)
#define EMIF4_1_SDRAM_TIM_2            (EMIF4_1_CFG_BASE + 0x20)
#define EMIF4_1_SDRAM_TIM_2_SHADOW     (EMIF4_1_CFG_BASE + 0x24)
#define EMIF4_1_SDRAM_TIM_3            (EMIF4_1_CFG_BASE + 0x28)
#define EMIF4_1_SDRAM_TIM_3_SHADOW     (EMIF4_1_CFG_BASE + 0x2C)
#define EMIF4_1_DDR_PHY_CTRL_1         (EMIF4_1_CFG_BASE + 0xE4)
#define EMIF4_1_DDR_PHY_CTRL_1_SHADOW  (EMIF4_1_CFG_BASE + 0xE8)

#define CMD0_CTRL_SLAVE_RATIO_0        (DDR_PHY_BASE_ADDR + 0x1C)
#define CMD0_DLL_LOCK_DIFF_0           (DDR_PHY_BASE_ADDR + 0x28)
#define CMD0_INVERT_CLKOUT_0           (DDR_PHY_BASE_ADDR + 0x2C)

#define CMD1_CTRL_SLAVE_RATIO_0        (DDR_PHY_BASE_ADDR + 0x50)
#define CMD1_DLL_LOCK_DIFF_0           (DDR_PHY_BASE_ADDR + 0x5C)
#define CMD1_INVERT_CLKOUT_0           (DDR_PHY_BASE_ADDR + 0x60)

#define CMD2_CTRL_SLAVE_RATIO_0        (DDR_PHY_BASE_ADDR + 0x84)
#define CMD2_DLL_LOCK_DIFF_0           (DDR_PHY_BASE_ADDR + 0x90)
#define CMD2_INVERT_CLKOUT_0           (DDR_PHY_BASE_ADDR + 0x94)

#define DATA0_RD_DQS_SLAVE_RATIO_0     (DDR_PHY_BASE_ADDR + 0xC8)
#define DATA0_WR_DQS_SLAVE_RATIO_0     (DDR_PHY_BASE_ADDR + 0xDC)
#define DATA0_WRLVL_INIT_RATIO_0       (DDR_PHY_BASE_ADDR + 0xF0)
#define DATA0_GATELVL_INIT_RATIO_0     (DDR_PHY_BASE_ADDR + 0xFC)
#define DATA0_FIFO_WE_SLAVE_RATIO_0    (DDR_PHY_BASE_ADDR + 0x108)
#define DATA0_WR_DATA_SLAVE_RATIO_0    (DDR_PHY_BASE_ADDR + 0x120)
#define DATA0_RANK0_DELAYS_0           (DDR_PHY_BASE_ADDR + 0x134)
#define DATA0_DLL_LOCK_DIFF_0          (DDR_PHY_BASE_ADDR + 0x138)
#define DATA1_RANK0_DELAYS_0           (DDR_PHY_BASE_ADDR + 0x1D8)



/* RTC Clock register defines */
#define AM335X_RTC_BASE                0x44e3e000
#define AM335X_RTC_SIZE                0xA0

#define AM335X_RTC_SECONDS             0x00
#define AM335X_RTC_MINUTES             0x04
#define AM335X_RTC_HOURS               0x08
#define AM335X_RTC_DAYS                0x0c
#define AM335X_RTC_MONTHS              0x10
#define AM335X_RTC_YEARS               0x14
#define AM335X_RTC_WEEKS               0x18
#define AM335X_RTC_ALARM_SECONDS       0x20
#define AM335X_RTC_ALARM_MINUTES       0x24
#define AM335X_RTC_ALARM_HOURS         0x28
#define AM335X_RTC_ALARM_DAYS          0x2c
#define AM335X_RTC_ALARM_MONTHS        0x30
#define AM335X_RTC_ALARM_YEARS         0x34
#define AM335X_RTC_CTRL                0x40
#define		AM335X_RTC_CTRL_RTC_DISABLE       0x40
#define		AM335X_RTC_CTRL_SET_32_CNTR       0x20
#define		AM335X_RTC_CTRL_TEST_MODE         0x10
#define		AM335X_RTC_CTRL_MODE_12_MODE      0x08
#define		AM335X_RTC_CTRL_AUTO_COMP         0x04
#define		AM335X_RTC_CTRL_ROUND_30S         0x02
#define		AM335X_RTC_CTRL_RUN_RTC           0x01
#define AM335X_RTC_STATUS              0x44
#define		AM335X_RTC_STATUS_ALARM2          0x80
#define 	AM335X_RTC_STATUS_ALARM			  0x40
#define 	AM335X_RTC_STATUS_1D_EVENT		  0x20
#define 	AM335X_RTC_STATUS_1H_EVENT		  0x10
#define 	AM335X_RTC_STATUS_1M_EVENT		  0x08
#define 	AM335X_RTC_STATUS_1S_EVENT		  0x04
#define 	AM335X_RTC_STATUS_RUN			  0x02
#define 	AM335X_RTC_STATUS_BUSY			  0x01
#define AM335X_RTC_INTERRUPTS          0x48
#define AM335X_RTC_COMP_LSB            0x4c
#define AM335X_RTC_COMP_MSB            0x50
#define AM335X_RTC_OSC                 0x54
#define		AM335X_RTC_OSC_32KCLK_EN          0x40
#define		AM335X_RTC_OSC_OSC32K_GZ_DISABLE  0x10
#define		AM335X_RTC_OSC_32KCLK_SEL_EXT     0x08
#define		AM335X_RTC_OSC_RES_SEL_EXT        0x04
#define		AM335X_RTC_OSC_SW2                0x02
#define		AM335X_RTC_OSC_SW1                0x01
#define AM335X_RTC_SCRATCH0            0x60
#define AM335X_RTC_SCRATCH1            0x64
#define AM335X_RTC_SCRATCH2            0x68
#define AM335X_RTC_KICK0R              0x6C
#define AM335X_RTC_KICK1R              0x70
#define AM335X_RTC_REVISION            0x74
#define AM335X_RTC_SYSCONFIG           0x78
#define AM335X_RTC_IRQWAKEEN_0         0x7A

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/arm/am335x.h $ $Rev: 719628 $")
#endif
