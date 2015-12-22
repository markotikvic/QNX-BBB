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




/*
 * TI Sitara AM437X (based on ARM Cortex A-9 core)
 */

#ifndef    __ARM_AM437X_H_INCLUDED
#define    __ARM_AM437X_H_INCLUDED


/*************************/
/*    interrupt          */
/*************************/


/* Interrupt Controller */
#define AM437X_INTC_BASE           0x48240100
#define AM437X_INTC_SIZE           0x100

/* AM437x IRQs are Shared Peripheral Interrupt that
start 32 entries into the interrupt of GIC */
#define AM437X_NUM_INTERNAL_IRQS   32


#define AM437X_PL310_IRQ                32
#define AM437X_CTI_IRQ_0                33
#define AM437X_ELM_IRQ                  36
#define AM437X_NMI_IRQ                  39
#define AM437X_L3_DBG_IRQ               41 // L3 interconnect debug error
#define AM437X_L3_APP_IRQ               42 // L3 interconnect application error
#define AM437X_PRCM_MPU_IRQ             43 // PRCM interrupt
#define AM437X_EDMAACM_IRQ              44 // EDMACOMPINT
#define AM437X_EDMAAMPERR_IRQ           45 // EDMAAMPERR
#define AM437X_EDMAERR_IRQ              46 // EDMAERRINT
#define AM437X_ADC0_GENINT_IRQ          48 // ADC0
#define AM437X_MMCSD1_IRQ               60 // MMCSD1
#define AM437X_MMCSD2_IRQ               61 // MMCSD2
#define AM437X_I2C2_IRQ                 62 // I2C2
#define AM437X_ECAP0INT_IRQ             63 // PWMSS0_ECAP
#define AM437X_GPIO2_A_IRQ              64 // GPIO2
#define AM437X_GPIO2_B_IRQ              65 // GPIO2
#define AM437X_GFX_IRQ                  69 // GFX(SGX530)
#define AM437X_EPWM2_IRQ                71 // PWMSS2_EPWM
#define AM437X_3PGSW_RXTHR_IRQ_0        72 // CPSW(Ethernet)
#define AM437X_3PGSW_RX_IRQ_0           73 // CPSW(Ethernet)
#define AM437X_3PGSW_TX_IRQ_0           74 // CPSW(Ethernet)
#define AM437X_3PGSW_MISC_IRQ_0         75 // CPSW(Ethernet)
#define AM437X_UART3_IRQ                76 // UART3
#define AM437X_UART4_IRQ                77 // UART4
#define AM437X_UART5_IRQ                78 // UART5
#define AM437X_ECAP1INT_IRQ             79 // PWMSS1_ECAP
#define AM437X_CCDC0_IRQ                80 // VPFE0 (Camera0)
#define AM437X_DCAN1_IRQ_0              81 // DCAN1
#define AM437X_CCDC1_IRQ                82 // VPFE1 (Camera0)
#define AM437X_DCAN0_PARITY_IRQ         83 // DCAN0
#define AM437X_DCAN0_IRQ_0              84 // DCAN0
#define AM437X_DCAN0_IRQ_1              85 // DCAN0
#define AM437X_DCAN1_IRQ_1              88 // DCAN1
#define AM437X_DCAN1_PARITY_IRQ         89 // DCAN0
#define AM437X_EPWM0_TZ_IRQ             90 // PWMSS0_EPWM(tz interrupt)
#define AM437X_EPWM1_TZ_IRQ             91 // PWMSS1_EPWM(tz interrupt)
#define AM437X_EPWM2_TZ_IRQ             92 // PWMSS2_EPWM(tz interrupt)
#define AM437X_ECAP2INT_IRQ             93 // PWMSS2_ECAP
#define AM437X_GPIO3_A_IRQ              94 // GPIO3
#define AM437X_GPIO3_B_IRQ              95 // GPIO3
#define AM437X_MMCSD0_IRQ               96 // MMCSD0
#define AM437X_SPI0_IRQ                 97 // McSPI0
#define AM437X_TIMER0_IRQ               98 // DMTimer0
#define AM437X_TIMER1_1MS_IRQ           99 // DMTimer1_1MS
#define AM437X_TIMER2_IRQ               100 // DMTimer2
#define AM437X_TIMER3_IRQ               101 // DMTimer3
#define AM437X_I2C0_IRQ                 102 // I2C0
#define AM437X_I2C1_IRQ                 103 // I2C1
#define AM437X_UART0_IRQ                104 // UART0
#define AM437X_UART1_IRQ                105 // UART1
#define AM437X_UART2_IRQ                106 // UART2
#define AM437X_RTC_IRQ                  107 // RTC
#define AM437X_RTCALARM_IRQ             108 // RTC
#define AM437X_MB0_IRQ                  109 // Mailbox0
#define AM437X_EQEP0_IRQ                111 // PWMSS0_EQEP
#define AM437X_MCA0_TX_IRQ              112 // McASP0
#define AM437X_MCA0_RX_IRQ              113 // McASP0
#define AM437X_MCA1_TX_IRQ              114 // McASP1
#define AM437X_MCA1_RX_IRQ              115 // McASP2
#define AM437X_EPWM0_IRQ                118 // PWMSS0_EPWM(event/interrupt)
#define AM437X_EPWM1_IRQ                119 // PWMSS1_EPWM(event/interrupt)
#define AM437X_EQEP1_IRQ                120 // PWMSS1_EQEP
#define AM437X_EQEP2_IRQ                121 // PWMSS2_EQEP
#define AM437X_EDMA_PIN2_IRQ            122 // EDMA_PIN2
#define AM437X_WDT1_IRQ                 123 // WDT1
#define AM437X_TIMER4_IRQ               124 // DMTimer4
#define AM437X_TIMER5_IRQ               125 // DMTimer5
#define AM437X_TIMER6_IRQ               126 // DMTimer6
#define AM437X_TIMER7_IRQ               127 // DMTimer7
#define AM437X_GPIO0_A_IRQ              128 // GPIO0
#define AM437X_GPIO0_B_IRQ              129 // GPIO0
#define AM437X_GPIO1_A_IRQ              130 // GPIO1
#define AM437X_GPIO1_B_IRQ              131 // GPIO1
#define AM437X_GPMC_IRQ                 132 // GPMC
#define AM437X_DDRERR0_IRQ              133 // DDR EMIF0
#define AM437X_GPIO4_A_IRQ              138 // GPIO4
#define AM437X_GPIO4_B_IRQ              139 // GPIO4
#define AM437X_TCERR0_IRQ               144 // EMA3TC0
#define AM437X_TCERR1_IRQ               145 // EMA3TC1
#define AM437X_TCERR2_IRQ               146 // EMA3TC2
#define AM437X_ADC1_GEN_IRQ             147 // ADC1
#define AM437X_EDMA_PIN0_IRQ            155 // EDMA_PIN0
#define AM437X_EDMA_PIN1_IRQ            156 // EDMA_PIN1
#define AM437X_SPI1_IRQ                 157 // McSPI1
#define AM437X_SPI2_IRQ                 158 // McSPI2
#define AM437X_DSS_IRQ                  159 // DSS(Display SS)
#define AM437X_TIMER8_IRQ               163 // DMTimer8
#define AM437X_TIMER9_IRQ               164 // DMTimer9
#define AM437X_TIMER10_IRQ              165 // DMTimer10
#define AM437X_TIMER11_IRQ              166 // DMTimer11
#define AM437X_SPI3_IRQ                 168 // McSPI3
#define AM437X_SPI4_IRQ                 169 // McSPI4
#define AM437X_QSPI_IRQ                 170 // QSPI
#define AM437X_HDQ_IRQ                  171 // HDQ1W
#define AM437X_EPWM3_IRQ                173 // PWMSS3_EPWM(event/interrupt)
#define AM437X_EPWM3_TZ_IRQ             174 // PWMSS3_EPWM(tz event)
#define AM437X_EPWM4_IRQ                175 // PWMSS4_EPWM(event/interrupt)
#define AM437X_EPWM4_TZ_IRQ             176 // PWMSS4_EPWM(tz event)
#define AM437X_EPWM5_IRQ                177 // PWMSS5_EPWM(event/interrupt)
#define AM437X_EPWM5_TZ_IR              178 // PWMSS5_EPWM(tz event)
#define AM437X_GPIO5_A_IRQ              180 // GPIO5
#define AM437X_GPIO5_B_IRQ              181 // GPIO5
#define AM437X_EDMA_PIN3_IRQ            182 // EDMA_PIN3
#define AM437X_EDMA_PIN4_IRQ            183 // EDMA_PIN4
#define AM437X_EDMA_PIN5_IRQ            184 // EDMA_PIN5
#define AM437X_EDMA_PIN6_IRQ            185 // EDMA_PIN6
#define AM437X_EDMA_PIN7_IRQ            186 // EDMA_PIN7
#define AM437X_EDMA_PIN8_IRQ            187 // EDMA_PIN8
#define AM437X_USB0_MAIN0_IRQ           200 // USB0
#define AM437X_USB0_MAIN1_IRQ           201 // USB0
#define AM437X_USB0_MAIN2_IRQ           202 // USB0
#define AM437X_USB0_MAIN3_IRQ           203 // USB0
#define AM437X_USB0_MISC_IRQ            204 // USB0
#define AM437X_USB1_MAIN0_IRQ           206 // USB1
#define AM437X_USB1_MAIN1_IRQ           207 // USB1
#define AM437X_USB1_MAIN2_IRQ           208 // USB1
#define AM437X_USB1_MAIN3_IRQ           209 // USB1
#define AM437X_USB1_MISC_IRQ            210 // USB1



/*********************/
/* GPIO Base address */
/*********************/
#define AM437X_GPIO0_BASE          0x44E07000
#define AM437X_GPIO1_BASE          0x4804C000
#define AM437X_GPIO2_BASE          0x481AC000
#define AM437X_GPIO3_BASE          0x481AE000
#define AM437X_GPIO4_BASE          0x48320000
#define AM437X_GPIO5_BASE          0x48322000
#define AM437X_GPIO_SIZE           0x00001000

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





/*********************/
/*   I2C             */
/*********************/

#define AM437X_I2C_REG_SIZE        0x1000
#define AM437X_I2C0_BASE           0x44E0B000
#define AM437X_I2C1_BASE           0x4802A000
#define AM437X_I2C2_BASE           0x4819C000


/*********************/
/* PRCM Base Address */
/*********************/
#define AM437X_PRCM_BASE           0x44DF0000
#define PRM_MPU                    0x300
#define PRM_GFX                    0x400
#define PRM_RTC                    0x524
#define PRM_CEFUSE                 0x700
#define PRM_PER                    0x800
#define PRM_WKUP                   0x2000
#define CM_WKUP                    0x2800
#define PRM_DEVICE                 0x4000
#define CM_DEVICE                  0x4100

#define AM437X_CM_DEVICE_BASE      0x44DF4100
#define AM437X_CM_CLKOUT1_CTRL            (AM437X_CM_DEVICE_BASE + 0x00)
#define AM437X_CM_DLL_CTRL                (AM437X_CM_DEVICE_BASE + 0x04)
#define AM437X_CM_CLKOUT2_CTRL            (AM437X_CM_DEVICE_BASE + 0x08)


#define AM437X_CM_WKUP_BASE        0x44DF2800

#define AM437X_CM_L3_AON_CLKSTCTRL            (AM437X_CM_WKUP_BASE + 0x00)
#define AM437X_CM_WKUP_DEBSS_CLKCTRL          (AM437X_CM_WKUP_BASE + 0x20)
#define AM437X_CM_L3S_ADC0_CLKCTRL            (AM437X_CM_WKUP_BASE + 0x100)
#define AM437X_CM_WKUP_ADC0_CLKCTRL           (AM437X_CM_WKUP_BASE + 0x120)
#define AM437X_CM_WKUP_AON_CLKCTRL            (AM437X_CM_WKUP_BASE + 0x200)
#define AM437X_CM_WKUP_L4WKUP_CLKCTRL         (AM437X_CM_WKUP_BASE + 0x220)
#define AM437X_CM_WKUP_PROC_CLKCTRL           (AM437X_CM_WKUP_BASE + 0x228)
#define AM437X_CM_WKUP_SYNCTIMER_CLKCTRL      (AM437X_CM_WKUP_BASE + 0x230)
#define AM437X_CM_WKUP_CLKDIV32K_CLKCTRL      (AM437X_CM_WKUP_BASE + 0x238)
#define AM437X_CM_WKUP_USBPHY0_CLKCTRL        (AM437X_CM_WKUP_BASE + 0x240)
#define AM437X_CM_WKUP_USBPHY1_CLKCTRL        (AM437X_CM_WKUP_BASE + 0x248)
#define AM437X_CM_WKUP_CLKCTRL                (AM437X_CM_WKUP_BASE + 0x300)
#define AM437X_CM_WKUP_TIMER0_CLKCTRL         (AM437X_CM_WKUP_BASE + 0x320)
#define AM437X_CM_WKUP_TIMER1_CLKCTRL         (AM437X_CM_WKUP_BASE + 0x228)
#define AM437X_CM_WKUP_WDT1_CLKCTRL           (AM437X_CM_WKUP_BASE + 0x238)
#define AM437X_CM_WKUP_CLKSTCTRL              (AM437X_CM_WKUP_BASE + 0x300)
#define AM437X_CM_WKUP_I2C0_CLKCTRL           (AM437X_CM_WKUP_BASE + 0x340)
#define AM437X_CM_WKUP_UART0_CLKCTRL          (AM437X_CM_WKUP_BASE + 0x348)
#define AM437X_CM_WKUP_CTRL_CLKCTRL           (AM437X_CM_WKUP_BASE + 0x360)
#define AM437X_CM_WKUP_GPIO0_CLKCTRL          (AM437X_CM_WKUP_BASE + 0x368)

#define AM437X_CM_CLKMODE_DPLL_CORE           (AM437X_CM_WKUP_BASE + 0x520)
#define AM437X_CM_IDLEST_DPLL_CORE            (AM437X_CM_WKUP_BASE + 0x524)
#define AM437X_CM_CLKSEL_DPLL_CORE            (AM437X_CM_WKUP_BASE + 0x52C)
#define AM437X_CM_DIV_M4_DPLL_CORE            (AM437X_CM_WKUP_BASE + 0x538)
#define AM437X_CM_DIV_M5_DPLL_CORE            (AM437X_CM_WKUP_BASE + 0x53C)
#define AM437X_CM_DIV_M6_DPLL_CORE            (AM437X_CM_WKUP_BASE + 0x540)
#define AM437X_CM_SSC_DELTAMSTEP_DPLL_CORE    (AM437X_CM_WKUP_BASE + 0x548)
#define AM437X_CM_SSC_MODFREQDIV_DPLL_CORE    (AM437X_CM_WKUP_BASE + 0x54C)

#define AM437X_CM_CLKMODE_DPLL_MPU            (AM437X_CM_WKUP_BASE + 0x560)
#define AM437X_CM_IDLEST_DPLL_MPU             (AM437X_CM_WKUP_BASE + 0x564)
#define AM437X_CM_CLKSEL_DPLL_MPU             (AM437X_CM_WKUP_BASE + 0x56C)
#define AM437X_CM_DIV_M2_DPLL_MPU             (AM437X_CM_WKUP_BASE + 0x570)
#define AM437X_CM_SSC_DELTAMSTEP_DPLL_MPU     (AM437X_CM_WKUP_BASE + 0x588)
#define AM437X_CM_SSC_MODFREQDIV_DPLL_MPU     (AM437X_CM_WKUP_BASE + 0x58C)

#define AM437X_CM_CLKMODE_DPLL_DDR            (AM437X_CM_WKUP_BASE + 0x5A0)
#define AM437X_CM_IDLEST_DPLL_DDR             (AM437X_CM_WKUP_BASE + 0x5A4)
#define AM437X_CM_CLKSEL_DPLL_DDR             (AM437X_CM_WKUP_BASE + 0x5AC)
#define AM437X_CM_DIV_M2_DPLL_DDR             (AM437X_CM_WKUP_BASE + 0x5B0)
#define AM437X_CM_DIV_M4_DPLL_DDR             (AM437X_CM_WKUP_BASE + 0x5B8)
#define AM437X_CM_DELTAMSTEP_DPLL_DDR         (AM437X_CM_WKUP_BASE + 0x5C8)
#define AM437X_CM_MODFREQDIV_DPLL_DDR         (AM437X_CM_WKUP_BASE + 0x5CC)

#define AM437X_CM_CLKMODE_DPLL_PER            (AM437X_CM_WKUP_BASE + 0x5E0)
#define AM437X_CM_IDLEST_DPLL_PER             (AM437X_CM_WKUP_BASE + 0x5E4)
#define AM437X_CM_CLKSEL_DPLL_PER             (AM437X_CM_WKUP_BASE + 0x5EC)
#define AM437X_CM_DIV_M2_DPLL_PER             (AM437X_CM_WKUP_BASE + 0x5F0)
#define AM437X_CM_CLKSEL2_DPLL_PER            (AM437X_CM_WKUP_BASE + 0x604)
#define AM437X_CM_DELTAMSTEP_DPLL_PER         (AM437X_CM_WKUP_BASE + 0x608)
#define AM437X_CM_MODFREQDIV_DPLL_PER         (AM437X_CM_WKUP_BASE + 0x60C)
#define AM437X_CM_CLKDCOLDO_DPLL_PER          (AM437X_CM_WKUP_BASE + 0x614)

#define AM437X_CM_CLKMODE_DPLL_DISP           (AM437X_CM_WKUP_BASE + 0x620)
#define AM437X_CM_IDLEST_DPLL_DISP            (AM437X_CM_WKUP_BASE + 0x624)
#define AM437X_CM_CLKSEL_DPLL_DISP            (AM437X_CM_WKUP_BASE + 0x62C)
#define AM437X_CM_DIV_M2_DPLL_DISP            (AM437X_CM_WKUP_BASE + 0x630)
#define AM437X_CM_DELTAMSTEP_DPLL_DISP        (AM437X_CM_WKUP_BASE + 0x648)
#define AM437X_CM_MODFREQDIV_DPLL_DISP        (AM437X_CM_WKUP_BASE + 0x64C)

#define AM437X_CM_CLKMODE_DPLL_EXTDEV         (AM437X_CM_WKUP_BASE + 0x660)
#define AM437X_CM_IDLEST_DPLL_EXTDEV          (AM437X_CM_WKUP_BASE + 0x664)
#define AM437X_CM_CLKSEL_DPLL_EXTDEV          (AM437X_CM_WKUP_BASE + 0x66C)
#define AM437X_CM_DIV_M2_DPLL_EXTDEV          (AM437X_CM_WKUP_BASE + 0x670)
#define AM437X_CM_CLKSEL2_DPLL_EXTDEV         (AM437X_CM_WKUP_BASE + 0x684)
#define AM437X_CM_DELTAMSTEP_DPLL_EXTDEV      (AM437X_CM_WKUP_BASE + 0x688)
#define AM437X_CM_MODFREQDIV_DPLL_EXTDEV      (AM437X_CM_WKUP_BASE + 0x68C)

#define AM437X_SHADOW_FREQ_CONFIG1            (AM437X_CM_WKUP_BASE + 0x7A0)
#define AM437X_SHADOW_FREQ_CONFIG2            (AM437X_CM_WKUP_BASE + 0x7A4)




#define CM_DEVICE                  0x4100
#define CM_DPLL                    0x4200
#define CM_DPLL_BASE               0x44DF4200
#define CM_MPU                     0x8300
#define CM_GFX                     0x8400
#define CM_RTC                     0x8500
#define CM_CEFUSE                  0x8700
#define CM_PER                     0x8800
#define CM_PRCM_SIZE               0x10000

#define PRM_DEVICE_BASE            0x44DF4000
    #define PRM_RSTCTRL            0x00
    #define RST_GLOBAL_WARM_SW     (1<<0)
    #define RST_GLOBAL_COLD_SW     (1<<1)
    #define PRM_RSTST              0x04


#define CM_PER_BASE                0x44DF8800
#define AM437X_CM_PER_L3_CLKSTCTRL         (CM_PER_BASE + 0x00)
#define AM437X_CM_PER_L3_CLKCTRL           (CM_PER_BASE + 0x20)
#define AM437X_CM_PER_L3_INSTR_CLKCTRL     (CM_PER_BASE + 0x40)
#define AM437X_CM_PER_OCMCRAM_CLKCTRL      (CM_PER_BASE + 0x50)
#define AM437X_CM_PER_VPFE0_CLKCTRL        (CM_PER_BASE + 0x68)
#define AM437X_CM_PER_VPFE1_CLKCTRL        (CM_PER_BASE + 0x70)
#define AM437X_CM_PER_TPCC_CLKCTRL         (CM_PER_BASE + 0x78)
#define AM437X_CM_PER_TPTC0_CLKCTRL        (CM_PER_BASE + 0x80)
#define AM437X_CM_PER_TPTC1_CLKCTRL        (CM_PER_BASE + 0x88)
#define AM437X_CM_PER_TPTC2_CLKCTRL        (CM_PER_BASE + 0x90)
#define AM437X_CM_PER_DLL_AGING_CLKCTRL    (CM_PER_BASE + 0x98)
#define AM437X_CM_PER_L4HS_CLKCTRL         (CM_PER_BASE + 0xA0)
#define AM437X_CM_PER_L4FW_CLKCTRL         (CM_PER_BASE + 0xA8)
#define AM437X_CM_PER_L3S_CLKSTCTRL        (CM_PER_BASE + 0x200)
#define AM437X_CM_PER_GPMC_CLKCTRL         (CM_PER_BASE + 0x220)
#define AM437X_CM_PER_ADC1_CLKCTRL         (CM_PER_BASE + 0x230)
#define AM437X_CM_PER_MCASP0_CLKCTRL       (CM_PER_BASE + 0x238)
#define AM437X_CM_PER_MCASP1_CLKCTRL       (CM_PER_BASE + 0x240)
#define AM437X_CM_PER_MMC2_CLKCTRL         (CM_PER_BASE + 0x248)
#define AM437X_CM_PER_QSPI_CLKCTRL         (CM_PER_BASE + 0x258)
#define AM437X_CM_PER_USB_OTG_SS0_CLKCTRL  (CM_PER_BASE + 0x260)
#define AM437X_CM_PER_USB_OTG_SS1_CLKCTRL  (CM_PER_BASE + 0x268)
#define AM437X_CM_PER_PRU_ICSS_CLKSTCTRL   (CM_PER_BASE + 0x300)
#define AM437X_CM_PER_PRU_ICSS_CLKCTRL     (CM_PER_BASE + 0x320)
#define AM437X_CM_PER_L4LS_CLKSTCTRL       (CM_PER_BASE + 0x400)      /* This register enables the domain power state transition.*/
                                                                      /* It controls the SW supervised clock domain state */
                                                                      /* transition between ON-PER and ON-INPER states. It */
                                                                      /* also hold one status bit per clock input of the domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_I2C_FCLK    (1<<27)        /* This field indicates the state of the I2C _FCLK clock in the domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_GPIO_5      (1<<26)        /* This field indicates the state of the GPIO4_GDBCLK clock in the GDBCLK domain. */
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_GPIO_4      (1<<25)        /* This field indicates the state of the GPIO4_GDBCLK clock in the GDBCLK domain. */
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_GPIO_3      (1<<24)        /* This field indicates the state of the GPIO3_GDBCLK clock in the GDBCLK domain. */
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_GPIO_2      (1<<23)        /* This field indicates the state of the GPIO2_ GDBCLK clock in the GDBCLK domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_GPIO_1      (1<<22)        /* This field indicates the state of the GPIO1_GDBCLK clock in the GDBCLK domain. */
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_TIMER11      (1<<21)        /* This field indicates the state of the TIMER11 CLKTIMER clock in the GCLK domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_TIMER10      (1<<20)        /* This field indicates the state of the TIMER10 CLKTIMER clock in the GCLK domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_TIMER9      (1<<19)        /* This field indicates the state of the TIMER9 CLKTIMER clock in the GCLK domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_TIMER8      (1<<18)        /* This field indicates the state of the TIMER8 CLKTIMER clock in the GCLK domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_TIMER7      (1<<17)        /* This field indicates the state of the TIMER7 CLKTIMER clock in the GCLK domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_TIMER6      (1<<16)        /* This field indicates the state of the TIMER6 CLKTIMER clock in the GCLK domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_TIMER5      (1<<15)        /* This field indicates the state of the TIMER5 CLKTIMER clock in the GCLK domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_TIMER4      (1<<14)        /* This field indicates the state of the TIMER4 CLKTIMER clock in the GCLK domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_TIMER3      (1<<13)        /* This field indicates the state of the TIMER3 CLKTIMER clock in the GCLK domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_TIMER2      (1<<12)        /* This field indicates the state of the TIMER2 CLKTIMER clock in the GCLK domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_CAN_CLK     (1<<11)        /* This field indicates the state of the CAN_CLK clock in the domain. */
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_UART_GF     (1<<10)        /* This field indicates the state of the UART_GFCLK clock in the CLK domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_SPI_GC     (1<<9)        /* This field indicates the state of the SPI_GCLK clock in the CLK domain.*/
    #define AM437X_CM_PER_L4LS_CLKACTIVITY_L4LS_GC     (1<< 8)        /* This field indicates the state of the L4LS_GCLK clock in the domain. */

#define AM437X_CM_PER_L4LS_CLKCTRL         (CM_PER_BASE + 0x420)
#define AM437X_CM_PER_DCAN0_CLKCTRL        (CM_PER_BASE + 0x428)
#define AM437X_CM_PER_DCAN1_CLKCTRL        (CM_PER_BASE + 0x430)
#define AM437X_CM_PER_PWMSS0_CLKCTRL       (CM_PER_BASE + 0x438)
#define AM437X_CM_PER_PWMSS1_CLKCTRL       (CM_PER_BASE + 0x440)
#define AM437X_CM_PER_PWMSS2_CLKCTRL       (CM_PER_BASE + 0x448)
#define AM437X_CM_PER_PWMSS3_CLKCTRL       (CM_PER_BASE + 0x450)
#define AM437X_CM_PER_PWMSS4_CLKCTRL       (CM_PER_BASE + 0x458)
#define AM437X_CM_PER_PWMSS5_CLKCTRL       (CM_PER_BASE + 0x460)
#define AM437X_CM_PER_ELM_CLKCTRL          (CM_PER_BASE + 0x468)
#define AM437X_CM_PER_GPIO1_CLKCTRL        (CM_PER_BASE + 0x478)
#define AM437X_CM_PER_GPIO2_CLKCTRL        (CM_PER_BASE + 0x480)
#define AM437X_CM_PER_GPIO3_CLKCTRL        (CM_PER_BASE + 0x488)
#define AM437X_CM_PER_GPIO4_CLKCTRL        (CM_PER_BASE + 0x490)
#define AM437X_CM_PER_GPIO5_CLKCTRL        (CM_PER_BASE + 0x498)
#define AM437X_CM_PER_GPIO0_CLKCTRL        (CM_PER_BASE + 0x478)
#define AM437X_CM_PER_HDQ1W_CLKCTRL        (CM_PER_BASE + 0x4A0)
#define AM437X_CM_PER_I2C1_CLKCTRL         (CM_PER_BASE + 0x4A8)
#define AM437X_CM_PER_I2C2_CLKCTRL         (CM_PER_BASE + 0x4B0)
#define AM437X_CM_PER_MAILBOX0_CLKCTRL     (CM_PER_BASE + 0x4B8)
#define AM437X_CM_PER_MMC0_CLKCTRL         (CM_PER_BASE + 0x4C0)
#define AM437X_CM_PER_MMC1_CLKCTRL         (CM_PER_BASE + 0x4C8)
#define AM437X_CM_PER_SPI0_CLKCTRL         (CM_PER_BASE + 0x500)
#define AM437X_CM_PER_SPI1_CLKCTRL         (CM_PER_BASE + 0x508)
#define AM437X_CM_PER_SPI2_CLKCTRL         (CM_PER_BASE + 0x510)
#define AM437X_CM_PER_SPI3_CLKCTRL         (CM_PER_BASE + 0x518)
#define AM437X_CM_PER_SPI4_CLKCTRL         (CM_PER_BASE + 0x520)
#define AM437X_CM_PER_SPINLOCK_CLKCTRL     (CM_PER_BASE + 0x528)
#define AM437X_CM_PER_TIMER2_CLKCTRL       (CM_PER_BASE + 0x530)
#define AM437X_CM_PER_TIMER3_CLKCTRL       (CM_PER_BASE + 0x538)
#define AM437X_CM_PER_TIMER4_CLKCTRL       (CM_PER_BASE + 0x540)
#define AM437X_CM_PER_TIMER5_CLKCTRL       (CM_PER_BASE + 0x548)
#define AM437X_CM_PER_TIMER6_CLKCTRL       (CM_PER_BASE + 0x550)
#define AM437X_CM_PER_TIMER7_CLKCTRL       (CM_PER_BASE + 0x558)
#define AM437X_CM_PER_TIMER8_CLKCTRL       (CM_PER_BASE + 0x560)
#define AM437X_CM_PER_TIMER9_CLKCTRL       (CM_PER_BASE + 0x568)
#define AM437X_CM_PER_TIMER10_CLKCTRL      (CM_PER_BASE + 0x570)
#define AM437X_CM_PER_TIMER11_CLKCTRL      (CM_PER_BASE + 0x578)
#define AM437X_CM_PER_UART1_CLKCTRL        (CM_PER_BASE + 0x580)
#define AM437X_CM_PER_UART2_CLKCTRL        (CM_PER_BASE + 0x588)
#define AM437X_CM_PER_UART3_CLKCTRL        (CM_PER_BASE + 0x590)
#define AM437X_CM_PER_UART4_CLKCTRL        (CM_PER_BASE + 0x598)
#define AM437X_CM_PER_UART5_CLKCTRL        (CM_PER_BASE + 0x5A0)
#define AM437X_CM_PER_USBPHYOCP2SCP0_CLKCTRL        (CM_PER_BASE + 0x5B8)
#define AM437X_CM_PER_USBPHYOCP2SCP1_CLKCTRL        (CM_PER_BASE + 0x5C0)
#define AM437X_CM_PER_EMIF_CLKSTCTRL       (CM_PER_BASE + 0x700)
#define PRCM_EMIF_PHY_ACTIVITY             (1<<10)
#define PRCM_EMIF_L3_GCLK_ACTIVITY         (1<<8)

#define AM437X_CM_PER_EMIF_CLKCTRL         (CM_PER_BASE + 0x720)
#define AM437X_CM_PER_DLL_CLKCTRL          (CM_PER_BASE + 0x728)
#define AM437X_CM_PER_EMIF_FW_CLKCTRL      (CM_PER_BASE + 0x730)
#define AM437X_CM_PER_OTFAEMIF_FW_CLKCTRL  (CM_PER_BASE + 0x738)
#define AM437X_CM_PER_LCDC_CLKSTCTRL       (CM_PER_BASE + 0x800)
#define AM437X_CM_PER_DSS_CLKSTCTRL        (CM_PER_BASE + 0xA00)
#define AM437X_CM_PER_DSS_CLKCTRL          (CM_PER_BASE + 0xA20)
#define AM437X_CM_PER_CPSW_CLKSTCTRL       (CM_PER_BASE + 0xB00)
#define AM437X_CM_PER_CPGMAC0_CLKCTRL      (CM_PER_BASE + 0xB20)
#define AM437X_CM_PER_OCPWP_L3_CLKSTCTRL   (CM_PER_BASE + 0xC00)
#define AM437X_CM_PER_OCPWP_CLKCTRL        (CM_PER_BASE + 0xC20)

/* AM437X_CM_CLKSEL_DPLL_PER bits: */
#define DPLL_MULT(x)                       x << 8
#define DPLL_DIV(x)                        x
#define DPLL_MULT_MASK                     0x7FFFF

/* AM437X_CM_CLKMODE_DPLL_PER bits: */
#define DPLL_MN_BYP_MODE                   0x00000004
#define DPLL_LOCK_MODE                     0x00000007
/* AM437X_CM_DIV_M2_DPLL_PER bits: */
#define DPLL_CLKOUT_DIV_MASK               0x7F
/* AM437X_CM_IDLEST_DPLL_PER bits: */
#define ST_MN_BYPASS                       0x00000100

//#define AM437X_CM_CLKDCOLDO_DPLL_PER       (CM_WKUP_BASE + 0x614)
/* AM437X_CM_CLKDCOLDO_DPLL_PER bits: */
#define DPLL_CLKDCOLDO_GATE_CTRL           (1 << 8)


/*
#define AM437X_CM_L3_AON_CLKSTCTRL         (CM_WKUP_BASE + 0x00)
#define AM437X_CM_WKUP_DEBUGSS_CLKCTRL     (CM_WKUP_BASE + 0x20)
#define AM437X_CM_L3_ADC0_CLKSTCTRL        (CM_WKUP_BASE + 0x100)
#define AM437X_CM_WKUP_ADC0_CLKCTRL        (CM_WKUP_BASE + 0x120)
#define AM437X_CM_L4_WKUP_AON_CLKSTCTRL    (CM_WKUP_BASE + 0x200)
#define AM437X_CM_WKUP_L4WKUP_CLKCTRL      (CM_WKUP_BASE + 0x220)
#define AM437X_CM_WKUP_PROC_CLKCTRL        (CM_WKUP_BASE + 0x220)
*/

#define AM437X_CLKSEL_TIMER2_CLK           (CM_DPLL_BASE+0x04)    /* Selects the Mux select line for TIMER2 clock         */
#define AM437X_CLKSEL_TIMER3_CLK           (CM_DPLL_BASE+0x08)    /* Selects the Mux select line for TIMER3 clock         */
#define AM437X_CLKSEL_TIMER4_CLK           (CM_DPLL_BASE+0x0C)    /* Selects the Mux select line for TIMER4 clock         */
#define AM437X_CLKSEL_TIMER5_CLK           (CM_DPLL_BASE+0x10)    /* Selects the Mux select line for TIMER5 clock         */
#define AM437X_CLKSEL_TIMER6_CLK           (CM_DPLL_BASE+0x14)    /* Selects the Mux select line for TIMER6 clock         */
#define AM437X_CLKSEL_TIMER7_CLK           (CM_DPLL_BASE+0x18)    /* Selects the Mux select line for TIMER7 clock         */
#define AM437X_CLKSEL_TIMER8_CLK           (CM_DPLL_BASE+0x1C)    /* Selects the Mux select line for TIMER8 clock         */
#define AM437X_CLKSEL_TIMER9_CLK           (CM_DPLL_BASE+0x20)    /* Selects the Mux select line for TIMER9 clock         */
#define AM437X_CLKSEL_TIMER10_CLK          (CM_DPLL_BASE+0x24)    /* Selects the Mux select line for TIMER10 clock         */
#define AM437X_CLKSEL_TIMER11_CLK          (CM_DPLL_BASE+0x28)    /* Selects the Mux select line for TIMER11 clock         */
#define AM437X_CLKSEL_WDT_CLK              (CM_DPLL_BASE+0x2C)    /* Selects the Mux select line for WDT1 clock         */
#define AM437X_CLKSEL_SYNCTIMER_CLK        (CM_DPLL_BASE+0x30)    /* Selects the Mux select line for SYNCTIMER clock         */
#define AM437X_CLKSEL_MAC_CLK              (CM_DPLL_BASE+0x34)    /* Selects the Mux select line for MAC clock         */
#define AM437X_CM_CPTS_RFT_CLKSEL          (CM_DPLL_BASE+0x38)    /* Selects the Mux select line for CPTS RFT clock       */
#define AM437X_CLKSEL_GFX_FCLK             (CM_DPLL_BASE+0x3C)    /* Selects the divider value for GFX clock              */
#define AM437X_CLKSEL_GPIO0_DBCLK          (CM_DPLL_BASE+0x40)    /* Selects the Mux select line for GPIO0 debounce clock */
#define AM437X_CLKSEL_ICSS_OCP_CLK         (CM_DPLL_BASE+0x48)    /* Controls the Mux select line for ICSS OCP clock      */
#define AM437X_CLKSEL_ADC1_CLK             (CM_DPLL_BASE+0x4C)    /* Controls the Mux select line for ADC1 clock    */
#define AM437X_CLKSEL_DLL_AGING_CLK        (CM_DPLL_BASE+0x50)    /* Selects the Mux select line for DLL AGING clock      */
#define AM437X_CLKSEL_USBPHY3_2KHZ_CLK     (CM_DPLL_BASE+0x60)    /* Selects the Mux select line for USBPHY3 clock      */






/*******************************/
/* Control Module Base Address */
/*******************************/
#define AM437X_CTRL_BASE                   0x44E10000
#define AM437X_CTRL_BASE_SIZE              0x00010000
#define CTRL_DEV_ATTR                      0x610
    #define MPU_MAX_FREQ_MASK              0x00000FFF
#define CTRL_STS                           0x40

#define AM437X_MAC_ID0_LO                  0x00000630
#define AM437X_MAC_ID0_HI                  0x00000634
#define AM437X_MAC_ID1_LO                  0x00000638
#define AM437X_MAC_ID1_HI                  0x0000063c


/* AM437X USB setup */
#define AM437X_USBCTRL0                    (AM437X_CTRL_BASE + 0x0620)
#define AM437X_USBSTS0                     (AM437X_CTRL_BASE + 0x0624)
#define AM437X_USBCTRL1                    (AM437X_CTRL_BASE + 0x0628)
#define AM437X_USBSTS1                     (AM437X_CTRL_BASE + 0x062c)

/* AM437X_USBCTRLx bits */
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

/* AM437X VTP setup */
#define AM437X_VTP                         (AM437X_CTRL_BASE + 0x0E0C)

#define VTP_CTRL_READY                    (0x1 << 5)
#define VTP_CTRL_ENABLE                   (0x1 << 6)
#define VTP_CTRL_LOCK_EN                  (0x1 << 4)
#define VTP_CTRL_START_EN                 (0x1)


/****************************************/
/*  Cortex-A9 PL310 L2 Cache controller */
/****************************************/
#define AM437X_PL310_BASE          0x48242000


/************************/
/*  L2 Cache controller */
/************************/
#define AM437X_L2_CACHE_BASE       0x45050000
#define AM437X_L2_CACHE_SIZE       0x40000


/*
 * L2 Cache registers
 */
#define AM437X_L2_CACHE_SET_DEBUG_REG              0x100
#define AM437X_L2_CACHE_CLEAN_INVALIDATE_REG       0x101
#define AM437X_L2_CACHE_SET_CONTROL_REG            0x100
#define AM437X_L2_CACHE_SET_AUX_CONTROL_REG        0x109
#define AM437X_L2_CACHE_GET_CONTROL_REG            0x112
#define AM437X_L2_CACHE_SET_PREFETCH_REG           0x113




/*************************************************/
/* MPU_WUGEN Register                           */
/*************************************************/
#define AM437X_MPU_WUGEN                        0x48281000
    #define WKG_CONTROL_0                       0x000
    #define WKG_ENB_A_0                         0x010
    #define WKG_ENB_B_0                         0x014
    #define WKG_ENB_C_0                         0x018
    #define WKG_ENB_D_0                         0x01C
    #define WKG_ENB_E_0                         0x020
    #define WKG_ENB_F_0                         0x024
    #define WKG_ENB_10_0                        0x028
    #define WKG_CONTROL_1                       0x400
    #define WKG_ENB_A_1                         0x410
    #define WKG_ENB_B_1                         0x414
    #define WKG_ENB_C_1                         0x418
    #define WKG_ENB_D_1                         0x41C
    #define WKG_ENB_E_1                         0x420
    #define WKG_ENB_F_1                         0x424
    #define WKG_ENB_10_1                        0x428
    #define AUX_CORE_BOOT_0                     0x800
    #define AUX_CORE_BOOT_1                     0x804


/***************************/
/* EDMA Channel controllers*/
/***************************/
#define AM437X_EDMA0_CC_BASE          0x49000000
#define AM437X_EDMA_CC_SIZE           0x100000
#define AM437X_EDMA0_TC0_BASE         0x49800000
#define AM437X_EDMA0_TC1_BASE         0x49900000
#define AM437X_EDMA0_TC2_BASE         0x49A00000
#define AM437X_EDMA_TC_SIZE           0x100000

#define AM437X_EDMA_INTR_BASE         0x49001000

/* Registers, Offset from EDMA base */
#define AM437X_EDMA_PID               0x00
#define AM437X_EDMA_CCCFG             0x04

/* Global registers, offset from EDMA base */
#define AM437X_EDMA_QCHMAP(c)         (0x200 + (c) * 4)
#define AM437X_EDMA_DMAQNUM(c)        (0x240 + (c) * 4)
#define AM437X_EDMA_QDMAQNUM          0x260
#define AM437X_EDMA_QUEPRI            0x284
#define AM437X_EDMA_EMR               0x300
#define AM437X_EDMA_EMRH              0x304
#define AM437X_EDMA_EMCR              0x308
#define AM437X_EDMA_EMCRH             0x30C
#define AM437X_EDMA_QEMR              0x310
#define AM437X_EDMA_QEMCR             0x314
#define AM437X_EDMA_CCERR             0x318
#define AM437X_EDMA_CCERRCLR          0x31C
#define AM437X_EDMA_EEVAL             0x320
#define AM437X_EDMA_DRAE(c)           (0x340 + (c) * 8)
#define AM437X_EDMA_DRAEH(c)          (0x344 + (c) * 8)
#define AM437X_EDMA_QRAE(c)           (0x380 + (c) * 4)
#define AM437X_EDMA_QRAE(c)           (0x380 + (c) * 4)
#define AM437X_EDMA_Q0E(c)            (0x400 + (c) * 4)
#define AM437X_EDMA_Q1E(c)            (0x440 + (c) * 4)
#define AM437X_EDMA_QSTAT0            0x600
#define AM437X_EDMA_QSTAT1            0x604
#define AM437X_EDMA_QWMTHRA           0x620
#define AM437X_EDMA_CCSTAT            0x640

/* Channel registers, offset from EDMA base */
#define AM437X_EDMA_GLOBAL            0x1000
#define AM437X_EDMA_REGION0           0x2000
#define AM437X_EDMA_REGION1           0x2200
#define AM437X_EDMA_REGION2           0x2400
#define AM437X_EDMA_REGION3           0x2600
#define AM437X_EDMA_ER                0x00
#define AM437X_EDMA_ERH               0x04
#define AM437X_EDMA_ECR               0x08
#define AM437X_EDMA_ECRH              0x0C
#define AM437X_EDMA_ESR               0x10
#define AM437X_EDMA_ESRH              0x14
#define AM437X_EDMA_CER               0x18
#define AM437X_EDMA_CERH              0x1C
#define AM437X_EDMA_EER               0x20
#define AM437X_EDMA_EERH              0x24
#define AM437X_EDMA_EECR              0x28
#define AM437X_EDMA_EECRH             0x2C
#define AM437X_EDMA_EESR              0x30
#define AM437X_EDMA_EESRH             0x34
#define AM437X_EDMA_SER               0x38
#define AM437X_EDMA_SERH              0x3C
#define AM437X_EDMA_SECR              0x40
#define AM437X_EDMA_SECRH             0x44
#define AM437X_EDMA_IER               0x50
#define AM437X_EDMA_IERH              0x54
#define AM437X_EDMA_IECR              0x58
#define AM437X_EDMA_IECRH             0x5C
#define AM437X_EDMA_IESR              0x60
#define AM437X_EDMA_IESRH             0x64
#define AM437X_EDMA_IPR               0x68
#define AM437X_EDMA_IPRH              0x6C
#define AM437X_EDMA_ICR               0x70
#define AM437X_EDMA_ICRH              0x74
#define AM437X_EDMA_IEVAL             0x78
#define AM437X_EDMA_QER               0x80
#define AM437X_EDMA_QEER              0x84
#define AM437X_EDMA_QEECR             0x88
#define AM437X_EDMA_QEESR             0x8C
#define AM437X_EDMA_QSER              0x90
#define AM437X_EDMA_QSECR             0x94

/* Parameter RAM base, offser from EDMA base */
#define AM437X_EDMA_PARAM_BASE        0x4000

/***************/
/*    SRAM     */
/***************/
#define AM437X_SRAM0_START            0x402F0000


/**************/
/*    UART    */
/**************/
#define AM437X_UART_REG_SIZE          0x1000
#define AM437X_UART0_BASE             0x44E09000
#define AM437X_UART1_BASE             0x48022000
#define AM437X_UART2_BASE             0x48024000
#define AM437X_UART3_BASE             0x481A6000
#define AM437X_UART4_BASE             0x481A8000
#define AM437X_UART5_BASE             0x481AA000

/*************/
/*    USB    */
/*************/
#define AM437X_USB0                   0x48380000
#define AM437X_USB0_PHY               0x483A8000
#define AM437X_USB1                   0x483C0000
#define AM437X_USB1_PHY               0x483E8000

/****************/
/* MMC (SD/MMC) */
/****************/
#define AM437X_MMCSD0_BASE            0x48060000
#define AM437X_MMCSD1_BASE            0x481D8000

#define AM437X_MMC_SIZE               0x1000
/* EDMA event numbers */
#define EDMA_MMCHS1_SDTXEVT1          2
#define EDMA_MMCHS1_SDRXEVT1          3
#define EDMA_MMCHS0_SDTXEVT0          24
#define EDMA_MMCHS0_SDRXEVT0          25

/*****************************/
/* BCH Error Location Module */
/*****************************/
#define ELM_BASE                      0x48080000

/******************/
/* Watchdog Timer */
/******************/
#define AM437X_WDT_BASE              0x44E35000
#define AM437X_WDT_SIZE              0x1000
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


/******************/
/* GPMC Registers */
/******************/
#define AM437X_GPMC_BASE             0x50000000
#define GPMC_SYSCONFIG               (AM437X_GPMC_BASE + 0x10)
#define GPMC_IRQSTATUS               (AM437X_GPMC_BASE + 0x18)
#define GPMC_IRQENABLE               (AM437X_GPMC_BASE + 0x1C)
#define GPMC_TIMEOUT_CONTROL         (AM437X_GPMC_BASE + 0x40)
#define GPMC_CONFIG                  (AM437X_GPMC_BASE + 0x50)
#define GPMC_CONFIG1_0               (AM437X_GPMC_BASE + 0x60)
#define GPMC_CONFIG2_0               (AM437X_GPMC_BASE + 0x64)
#define GPMC_CONFIG3_0               (AM437X_GPMC_BASE + 0x68)
#define GPMC_CONFIG4_0               (AM437X_GPMC_BASE + 0x6C)
#define GPMC_CONFIG5_0               (AM437X_GPMC_BASE + 0x70)
#define GPMC_CONFIG6_0               (AM437X_GPMC_BASE + 0x74)
#define GPMC_CONFIG7_0               (AM437X_GPMC_BASE + 0x78)

#define BIT(x)                       (1 << x)
#define CL_BIT(x)                    (0 << x)

/***************************/
/* Timer0~7 base addresses */
/***************************/
#define AM437X_TIMER_SIZE            0x1000
#define AM437X_TIMER0_BASE           0x44E05000
#define AM437X_TIMER1_1MS_BASE       0x44E31000
#define AM437X_TIMER2_BASE           0x48040000
#define AM437X_TIMER3_BASE           0x48042000
#define AM437X_TIMER4_BASE           0x48044000
#define AM437X_TIMER5_BASE           0x48046000
#define AM437X_TIMER6_BASE           0x48048000
#define AM437X_TIMER7_BASE           0x4804A000

/* Timer registers */
#define AM437X_TIMER_TIDR            0x00            /* Timer Identification Register */
#define AM437X_TIMER_THWINFO         0x04
#define AM437X_TIMER_TIOCP_CFG       0x10            /* Timer OCP Configuration Register */
#define AM437X_TIMER_IRQ_EOI         0x20            /* Timer IRQ EOI Register */
#define AM437X_TIMER_IRQSTATUS_RAW   0x24            /* Timer IRQ STATUS RAW Register */
#define AM437X_TIMER_IRQSTATUS       0x28            /* Timer IRQ STATUS Register */
#define AM437X_TIMER_IRQENABLE_SET   0x2C            /* Timer IRQ ENABLE SET Register*/
#define AM437X_TIMER_IRQENABLE_CLR   0x30            /* Timer IRQ ENABLE CLR Register*/
#define AM437X_TIMER_IRQWAKEEN       0x34            /* Timer IRQ WAKEUP ENABLE Register*/
#define AM437X_TIMER_TCLR            0x38            /* Timer Control Register */
#define AM437X_TIMER_TCRR            0x3C            /* Timer Counter Register */
#define AM437X_TIMER_TLDR            0x40            /* Timer Load Value Register*/
#define AM437X_TIMER_TTGR            0x44            /* Timer Trigger Register */
#define AM437X_TIMER_TWPS            0x48            /* Timer Write Posted Status Register */
#define AM437X_TIMER_TMAR            0x4C            /* Timer Match Register*/
#define AM437X_TIMER_TCAR1           0x50            /* Timer Capture Register 1 */
#define AM437X_TIMER_TSICR           0x54            /* Timer Synchronous Interface Control Register */
#define AM437X_TIMER_TCAR2           0x58            /* Timer Capture Register 2 */

/* Timer register bits */
#define AM437X_TIMER_MAT_EN_FLAG       BIT(0)        /* IRQ bit for Match */
#define AM437X_TIMER_OVF_EN_FLAG       BIT(1)        /* IRQ bit for Overflow */
#define AM437X_TIMER_TCAR_EN_FLAG      BIT(2)        /* IRQ bit for Compare */

#define AM437X_TIMER_TCLR_ST           BIT(0)        /* Start=1 Stop=0 */
#define AM437X_TIMER_TCLR_AR           BIT(1)        /* Auto reload */
#define AM437X_TIMER_TCLR_PTV_SHIFT       (2)        /* Pre-scaler shift value */
#define AM437X_TIMER_TCLR_PRE          BIT(5)        /* Pre-scaler enable for the timer input clk */
#define AM437X_TIMER_TCLR_PRE_DISABLE  CL_BIT(5)     /* Pre-scalar disable for the timer input clk */


/*********************/
/* EMIF Base address */
/*********************/

#define EMIF4_0_CFG_BASE             0x4C000000


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

/***************************/
/*  QSPI register defines */
/***************************/
#define AM437X_QSPI_BASE               0x47900000
#define AM437X_QSPI_SIZE               0x100000

/***************************/
/*  MCSPI register defines */
/***************************/
#define AM437X_SPI0_BASE               0x48030000
#define AM437X_SPI1_BASE               0x481A0000
#define AM437X_SPI2_BASE               0x481A2000
#define AM437X_SPI3_BASE               0x481A4000
#define AM437X_SPI_SIZE                0x1000


/***************************/
/*  MCASP register defines */
/***************************/
#define AM437X_MCASP0_BASE              0x48038000
#define AM437X_MCASP1_BASE              0x4803C000
#define AM437X_MCASP_SIZE               0x2000

/***************************/
/*   ADC register defines  */
/***************************/
#define AM437X_ADC0_BASE                0x44E0D000
#define AM437X_ADC1_BASE                0x4834C000
#define AM437X_ADC_SIZE                 0x1000


/***************************/
/*   CAN register defines  */
/***************************/
#define AM437X_CAN0_BASE                0x441CC000
#define AM437X_CAN1_BASE                0x481D0000
#define AM437X_CAN_SIZE                 0x1000



/******************************/
/* RTC Clock register defines */
/******************************/
#define AM437X_RTC_BASE                0x44E3E000
#define AM437X_RTC_SIZE                0x1000

#define AM437X_RTC_SECONDS             0x00
#define AM437X_RTC_MINUTES             0x04
#define AM437X_RTC_HOURS               0x08
#define AM437X_RTC_DAYS                0x0c
#define AM437X_RTC_MONTHS              0x10
#define AM437X_RTC_YEARS               0x14
#define AM437X_RTC_WEEKS               0x18
#define AM437X_RTC_ALARM_SECONDS       0x20
#define AM437X_RTC_ALARM_MINUTES       0x24
#define AM437X_RTC_ALARM_HOURS         0x28
#define AM437X_RTC_ALARM_DAYS          0x2c
#define AM437X_RTC_ALARM_MONTHS        0x30
#define AM437X_RTC_ALARM_YEARS         0x34
#define AM437X_RTC_CTRL                0x40
    #define AM437X_RTC_CTRL_RTC_DISABLE    0x40
    #define AM437X_RTC_CTRL_SET_32_CNTR    0x20
    #define AM437X_RTC_CTRL_TEST_MODE      0x10
    #define AM437X_RTC_CTRL_MODE_12_24   0x08
    #define AM437X_RTC_CTRL_AUTO_COMP      0x04
    #define AM437X_RTC_CTRL_ROUND_30S      0x02
    #define AM437X_RTC_CTRL_RUN_RTC       0x01
#define AM437X_RTC_STATUS              0x44
    #define AM437X_RTC_STATUS_ALARM2       0x80
    #define AM437X_RTC_STATUS_ALARM        0x40
    #define AM437X_RTC_STATUS_1D_EVENT     0x20
    #define AM437X_RTC_STATUS_1H_EVENT     0x10
    #define AM437X_RTC_STATUS_1M_EVENT     0x08
    #define AM437X_RTC_STATUS_1S_EVENT     0x04
    #define AM437X_RTC_STATUS_RUN          0x02
    #define AM437X_RTC_STATUS_BUSY         0x01
#define AM437X_RTC_INTERRUPTS          0x48
#define AM437X_RTC_COMP_LSB            0x4c
#define AM437X_RTC_COMP_MSB            0x50
#define AM437X_RTC_OSC                 0x54
    #define AM437X_RTC_OSC_32KCLK_EN          0x40
    #define AM437X_RTC_OSC_OSC32K_GZ_DISABLE  0x10
    #define AM437X_RTC_OSC_32KCLK_SEL_EXT     0x08
    #define AM437X_RTC_OSC_RES_SEL_EXT        0x04
    #define AM437X_RTC_OSC_SW2                0x02
    #define AM437X_RTC_OSC_SW1                0x01
#define AM437X_RTC_SCRATCH0            0x60
#define AM437X_RTC_SCRATCH1            0x64
#define AM437X_RTC_SCRATCH2            0x68
#define AM437X_RTC_KICK0R              0x6C
#define AM437X_RTC_KICK1R              0x70
#define AM437X_RTC_REVISION            0x74
#define AM437X_RTC_SYSCONFIG           0x78
#define AM437X_RTC_IRQWAKEEN_0         0x7A

/********/
/* CPSW */
/********/
#define AM437X_CPSW_BASE               0x4A100000
    #define CPSW_ALE_IDEVR             0x0
    #define CPSW_ALE_CTRL              0x8
    #define CPSW_ALE_PRESCALE          0x10
    #define CPSW_ALE_UN_VLAN           0x18
    #define CPSW_ALE_TBLCTL            0x20
    #define CPSW_ALE_TBLW2             0x34
    #define CPSW_ALE_TBLW1             0x38
    #define CPSW_ALE_TBLW0             0x3C
    #define CPSW_ALE_PORTCTL_0         0x40
    #define CPSW_ALE_PORTCTL_1         0x44
    #define CPSW_ALE_PORTCTL_2         0x48
    #define CPSW_ALE_PORTCTL_3         0x4C
    #define CPSW_ALE_PORTCTL_4         0x50
    #define CPSW_ALE_PORTCTL_5         0x54

#define AM437X_CPSW_PORT_BASE          0x4A100100
#define AM437X_CPSW_CPDMA_BASE         0x4A100800
#define AM437X_CPSW_STATS_BASE         0x4A100900
#define AM437X_CPSW_STATERAM_BASE      0x4A100A00
#define AM437X_CPSW_CPTS_BASE          0x4A100C00
#define AM437X_CPSW_ALE_BASE           0x4A100D00
#define AM437X_CPSW_SL1_BASE           0x4A100D80
#define AM437X_CPSW_SL2_BASE           0x4A100DC0
#define AM437X_CPSW_MDIO_BASE          0x4A101000
#define AM437X_CPSW_WR_BASE            0x4A101200
#define AM437X_CPSW_CPPI_RAM_BASE      0x4A102000


#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/arm/am437x.h $ $Rev: 765165 $")
#endif
