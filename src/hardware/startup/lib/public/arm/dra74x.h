/*
 * $QNXLicenseC:
 * Copyright 2013-2014, QNX Software Systems.
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
 * TI DRA74X processor with ARMv7 Cortex-A15 core
 */

#ifndef __ARM_DRA74X_H_INCLUDED
#define __ARM_DRA74X_H_INCLUDED

/* Memory address global mapping */
#define DRA74x_MPU_ROM                          0x40038000
#define DRA74x_OCMC_RAM1                        0x40300000
    #define DRA74x_OCMC_RAM1_SIZE               (512 * 1024)
#define DRA74x_OCMC_RAM2                        0x40400000
    #define DRA74x_OCMC_RAM2_SIZE               (1024 * 1024)
#define DRA74x_OCMC_RAM3                        0x40500000
    #define DRA74x_OCMC_RAM3_SIZE               (1024 * 1024)
#define DRA74X_QSPI_SFI_MM_BASE                 0x5C000000
#define DRA74X_QSPI_SFI_MM_SIZE                 (64 * 1024 * 1024)
#define DRA74x_EMIF_CS0_BASE                    0x80000000

#define DRA74X_QSPI_CORE_BASE                   0x4B300000
#define DRA74X_QSPI_CORE_SIZE                   0x1000

/*************************************************/
/* Interrupts */
/*************************************************/

// Software generated interrupts, internal to ARMv7 core
#define DRA74X_SGI0_IRQ                         0
#define DRA74X_SGI1_IRQ                         1
#define DRA74X_SGI2_IRQ                         2
#define DRA74X_SGI3_IRQ                         3
#define DRA74X_SGI4_IRQ                         4
#define DRA74X_SGI5_IRQ                         5
#define DRA74X_SGI6_IRQ                         6
#define DRA74X_SGI7_IRQ                         7
#define DRA74X_SGI8_IRQ                         8
#define DRA74X_SGI9_IRQ                         9
#define DRA74X_SGI10_IRQ                        10
#define DRA74X_SGI11_IRQ                        11
#define DRA74X_SGI12_IRQ                        12
#define DRA74X_SGI13_IRQ                        13
#define DRA74X_SGI14_IRQ                        14
#define DRA74X_SGI15_IRQ                        15

#define DRA74X_GLOBAL_TIMER_IRQ                 27
#define DRA74X_LEGACY_NFIQ_IRQ                  28
#define DRA74X_PRIVATE_TIMER_IRQ                29
#define DRA74X_PRIVATE_WDTIMER_IRQ              30
#define DRA74X_LEGACY_NIRQ_IRQ                  31

// IRQs defined in the DRA7xx TRM are Shared Peripheral Interrupts that
// start 32 entries into the interrupt of the GIC
#define DRA74X_NUM_INTERNAL_IRQS                32

#define DRA74X_CLUSTER_IRQ_INTERR               (0 + DRA74X_NUM_INTERNAL_IRQS)
#define DRA74X_CTI_IRQ_0                        (1 + DRA74X_NUM_INTERNAL_IRQS)
#define DRA74X_CTI_IRQ_1                        (2 + DRA74X_NUM_INTERNAL_IRQS)
#define DRA74X_CLUSTER_IRQ_AXIERR               (3 + DRA74X_NUM_INTERNAL_IRQS)
#define DRA74X_ELM_IRQ                          (4 + DRA74X_NUM_INTERNAL_IRQS)
#define DRA74X_WD_TIMER_MPU_C0_IRQ_WARN         (5 + DRA74X_NUM_INTERNAL_IRQS)
#define DRA74X_WD_TIMER_MPU_C1_IRQ_WARN         (6 + DRA74X_NUM_INTERNAL_IRQS)
//External External interrupt 1 (active low)
#define DRA74X_SYS_NIRQ1                        (7 + DRA74X_NUM_INTERNAL_IRQS)
// L3 interconnect debug error
#define DRA74X_L3_DBG_IRQ                       (9 + DRA74X_NUM_INTERNAL_IRQS)
// L3 interconnect application error
#define DRA74X_L3_APP_IRQ                       (10 + DRA74X_NUM_INTERNAL_IRQS)
// PRCM interrupt
#define DRA74X_PRCM_MPU_IRQ                     (11 + DRA74X_NUM_INTERNAL_IRQS)
// SDMA SDMA interrupt 0
#define DRA74X_SDMA_IRQ_0                       (12 + DRA74X_NUM_INTERNAL_IRQS)
// SDMA SDMA interrupt 1
#define DRA74X_SDMA_IRQ_1                       (13 + DRA74X_NUM_INTERNAL_IRQS)
// SDMA SDMA interrupt 2
#define DRA74X_SDMA_IRQ_2                       (14 + DRA74X_NUM_INTERNAL_IRQS)
// SDMA SDMA interrupt 3
#define DRA74X_SDMA_IRQ_3                       (15 + DRA74X_NUM_INTERNAL_IRQS)
// L3_MAIN_IRQ_STAT interrupt
#define DRA74X_L3_MAIN_IRQ_STAT_ALARM           (16 + DRA74X_NUM_INTERNAL_IRQS)
// SmartReflex MPU interrupt
#define DRA74X_SR_MPU_IRQ                       (18 + DRA74X_NUM_INTERNAL_IRQS)
// SmartReflex Core interrupt
#define DRA74X_SR_CORE_IRQ                      (19 + DRA74X_NUM_INTERNAL_IRQS)
// GPMC interrupt
#define DRA74X_GPMC_IRQ                         (20 + DRA74X_NUM_INTERNAL_IRQS)
// 2D/3D graphics module interrupt
#define DRA74X_SGX_IRQ                          (21 + DRA74X_NUM_INTERNAL_IRQS)
// Display controller Display controller interrupt
#define DRA74X_DSS_DISPC_IRQ                    (25 + DRA74X_NUM_INTERNAL_IRQS)
// Mailbox user 0 interrupt
#define DRA74X_MAIL_U0_MPU_IRQ                  (26 + DRA74X_NUM_INTERNAL_IRQS)
// DSP MMU interrupt
#define DRA74X_DSP_MMU_IRQ                      (28 + DRA74X_NUM_INTERNAL_IRQS)
// GPIO1 MPU interrupt
#define DRA74X_GPIO1_MPU_IRQ                    (29 + DRA74X_NUM_INTERNAL_IRQS)
// GPIO2 MPU interrupt
#define DRA74X_GPIO2_MPU_IRQ                    (30 + DRA74X_NUM_INTERNAL_IRQS)
// GPIO3 MPU interrupt
#define DRA74X_GPIO3_MPU_IRQ                    (31 + DRA74X_NUM_INTERNAL_IRQS)
// GPIO4 MPU interrupt
#define DRA74X_GPIO4_MPU_IRQ                    (32 + DRA74X_NUM_INTERNAL_IRQS)
// GPIO5 MPU interrupt
#define DRA74X_GPIO5_MPU_IRQ                    (33 + DRA74X_NUM_INTERNAL_IRQS)
// GPIO6 MPU interrupt
#define DRA74X_GPIO6_MPU_IRQ                    (34 + DRA74X_NUM_INTERNAL_IRQS)
// GPIO7 MPU interrupt
#define DRA74X_GPIO7_MPU_IRQ                    (35 + DRA74X_NUM_INTERNAL_IRQS)
// GPTIMER1 interrupt
#define DRA74X_GPT1_IRQ                         (37 + DRA74X_NUM_INTERNAL_IRQS)
// GPTIMER2 interrupt
#define DRA74X_GPT2_IRQ                         (38 + DRA74X_NUM_INTERNAL_IRQS)
// GPTIMER3 interrupt
#define DRA74X_GPT3_IRQ                         (39 + DRA74X_NUM_INTERNAL_IRQS)
// GPTIMER4 interrupt
#define DRA74X_GPT4_IRQ                         (40 + DRA74X_NUM_INTERNAL_IRQS)
// GPTIMER5 interrupt
#define DRA74X_GPT5_IRQ                         (41 + DRA74X_NUM_INTERNAL_IRQS)
// GPTIMER6 interrupt
#define DRA74X_GPT6_IRQ                         (42 + DRA74X_NUM_INTERNAL_IRQS)
// GPTIMER7 interrupt
#define DRA74X_GPT7_IRQ                         (43 + DRA74X_NUM_INTERNAL_IRQS)
// GPTIMER8 interrupt
#define DRA74X_GPT8_IRQ                         (44 + DRA74X_NUM_INTERNAL_IRQS)
// GPTIMER9 interrupt
#define DRA74X_GPT9_IRQ                         (45 + DRA74X_NUM_INTERNAL_IRQS)
// GPTIMER10 interrupt
#define DRA74X_GPT10_IRQ                        (46 + DRA74X_NUM_INTERNAL_IRQS)
// GPTIMER11 interrupt
#define DRA74X_GPT11_IRQ                        (47 + DRA74X_NUM_INTERNAL_IRQS)
// MCSPI4 interrupt
#define DRA74X_MCSPI4_IRQ                       (48 + DRA74X_NUM_INTERNAL_IRQS)
// EDMA interrupt
#define DRA74X_EDMA_IRQ                         (49 + DRA74X_NUM_INTERNAL_IRQS)
// SATA interrupt
#define DRA74X_SATA_IRQ                         (54 + DRA74X_NUM_INTERNAL_IRQS)
// I2C1 interrupt
#define DRA74X_I2C1_IRQ                         (56 + DRA74X_NUM_INTERNAL_IRQS)
// I2C2 interrupt
#define DRA74X_I2C2_IRQ                         (57 + DRA74X_NUM_INTERNAL_IRQS)
// HDQ/1wire interrupt
#define DRA74X_HDQ_IRQ                          (58 + DRA74X_NUM_INTERNAL_IRQS)
// I2C5 interrupt
#define DRA74X_I2C5_IRQ                         (60 + DRA74X_NUM_INTERNAL_IRQS)
// I2C3 interrupt
#define DRA74X_I2C3_IRQ                         (61 + DRA74X_NUM_INTERNAL_IRQS)
// I2C4 interrupt
#define DRA74X_I2C4_IRQ                         (62 + DRA74X_NUM_INTERNAL_IRQS)
// MCSPI1 interrupt
#define DRA74X_MCSPI1_IRQ                       (65 + DRA74X_NUM_INTERNAL_IRQS)
// MCSPI2 interrupt
#define DRA74X_MCSPI2_IRQ                       (66 + DRA74X_NUM_INTERNAL_IRQS)
// UART module 4 interrupt
#define DRA74X_UART4_IRQ                        (70 + DRA74X_NUM_INTERNAL_IRQS)
// UART1 interrupt
#define DRA74X_UART1_IRQ                        (72 + DRA74X_NUM_INTERNAL_IRQS)
// UART2 interrupt
#define DRA74X_UART2_IRQ                        (73 + DRA74X_NUM_INTERNAL_IRQS)
// UART3 interrupt
#define DRA74X_UART3_IRQ                        (74 + DRA74X_NUM_INTERNAL_IRQS)
// Merged interrupt for PBIASlite1 and 2
#define DRA74X_PBIAS_IRQ                        (75 + DRA74X_NUM_INTERNAL_IRQS)
// HSUSB MP host interrupt OHCI controller
#define DRA74X_HSUSB_OHCI_IRQ                   (76 + DRA74X_NUM_INTERNAL_IRQS)
// HSUSB MP host interrupt EHCI controller
#define DRA74X_HSUSB_EHCI_IRQ                   (77 + DRA74X_NUM_INTERNAL_IRQS)
// HSUSB MP TLL interrupt
#define DRA74X_HSUSB_TLL_IRQ                    (78 + DRA74X_NUM_INTERNAL_IRQS)
// WDTIMER2 interrupt
#define DRA74X_WDT2_IRQ                         (80 + DRA74X_NUM_INTERNAL_IRQS)
// MMC1 interrupt
#define DRA74X_MMC1_IRQ                         (83 + DRA74X_NUM_INTERNAL_IRQS)
// MMC2 interrupt
#define DRA74X_MMC2_IRQ                         (86 + DRA74X_NUM_INTERNAL_IRQS)
// CT_UART interrupt generated when data ready on RX or when TX empty
#define DRA74X_DEBUGSS_IRQ_CT_UART              (90 + DRA74X_NUM_INTERNAL_IRQS)
// MCSPI3 interrupt
#define DRA74X_MCSPI3_IRQ                       (91 + DRA74X_NUM_INTERNAL_IRQS)
// HSUSB OTG controller interrupt
#define DRA74X_USB2_INTR1_IRQ                   (92 + DRA74X_NUM_INTERNAL_IRQS)
// HSUSB OTG DMA interrupt
#define DRA74X_USB3_INTR0_IRQ                   (93 + DRA74X_NUM_INTERNAL_IRQS)
// MMC3 interrupt
#define DRA74X_MMC3_IRQ                         (94 + DRA74X_NUM_INTERNAL_IRQS)
// GPTIMER12 interrupt
#define DRA74X_GPT12_IRQ                        (95 + DRA74X_NUM_INTERNAL_IRQS)
// MMC4 interrupt
#define DRA74X_MMC4_IRQ                         (96 + DRA74X_NUM_INTERNAL_IRQS)
// IPU1 MMU interrupt
#define DRA74X_IPU1_MMU_IRQ                     (100 + DRA74X_NUM_INTERNAL_IRQS)
// HDMI interrupt
#define DRA74X_HDMI_IRQ                         (101 + DRA74X_NUM_INTERNAL_IRQS)
// Sync interrupt from ICONT2 (vDMA)
#define DRA74X_IVA_SYNC_1_IRQ                   (103 + DRA74X_NUM_INTERNAL_IRQS)
// Sync interrupt from ICONT1
#define DRA74X_IVA_SYNC_0_IRQ                   (104 + DRA74X_NUM_INTERNAL_IRQS)
// UART5 interrupt
#define DRA74X_UART5_IRQ                        (105 + DRA74X_NUM_INTERNAL_IRQS)
// UART6 interrupt
#define DRA74X_UART6_IRQ                        (106 + DRA74X_NUM_INTERNAL_IRQS)
// IVAHD mailbox interrupt 0
#define DRA74X_IVAHD_MAILBOX_IRQ_0              (107 + DRA74X_NUM_INTERNAL_IRQS)
// McASP1 receive interrupt
#define DRA74X_MCASP1_ARINT                     (108 + DRA74X_NUM_INTERNAL_IRQS)
// McASP1 transmit interrupt
#define DRA74X_MCASP1_AXINT                     (109 + DRA74X_NUM_INTERNAL_IRQS)
// EMIF1 interrupt
#define DRA74X_EMIF1_IRQ                        (110 + DRA74X_NUM_INTERNAL_IRQS)
// EMIF2 interrupt
#define DRA74X_EMIF2_IRQ                        (111 + DRA74X_NUM_INTERNAL_IRQS)
// DMM interrupt
#define DRA74X_DMM_IRQ                          (113 + DRA74X_NUM_INTERNAL_IRQS)
// External interrupt 2 (active low)
#define DRA74X_SYS_NIRQ2                        (119 + DRA74X_NUM_INTERNAL_IRQS)
// Keyboard controller interrupt
#define DRA74X_KBD_CTL_IRQ                      (120 + DRA74X_NUM_INTERNAL_IRQS)
// GPIO8 MPU interrupt
#define DRA74X_GPIO8_MPU_IRQ                    (121 + DRA74X_NUM_INTERNAL_IRQS)
// CAL (CSI2) Interrupt
#define DRA72X_CAL_IRQ                          (124 + DRA72X_NUM_INTERNAL_IRQS
// BB2D interrupt
#define DRA74X_BB2D_IRQ                         (125 + DRA74X_NUM_INTERNAL_IRQS)
// Control module thermal Alert interrupt
#define DRA74X_CTRL_MOD_THERMAL_IRQ             (126 + DRA74X_NUM_INTERNAL_IRQS)
// MPU core 0 PMU interrupt
#define DRA74X_MPU_0_PMU_IRQ                    (131 + DRA74X_NUM_INTERNAL_IRQS)
// MPU core 1 PMU interrupt
#define DRA74X_MPU_1_PMU_IRQ                    (132 + DRA74X_NUM_INTERNAL_IRQS)
// NMI pin interrupt (enabled via Control Module)
#define DRA74X_NMI_IRQ                          (133 + DRA74X_NUM_INTERNAL_IRQS)
// MPU_WD_TIMER channel 0 timeout interrupt (watchdog reset)
#define DRA74X_WD_TIMER_MPU_C0_IRQ              (139 + DRA74X_NUM_INTERNAL_IRQS)
// MPU_WD_TIMER channel 1 timeout interrupt (watchdog reset)
#define DRA74X_WD_TIMER_MPU_C1_IRQ              (140 + DRA74X_NUM_INTERNAL_IRQS)

// Device interrupts that are mapped to IRQ_CROSSBAR input
/*************************************************/
#define DRA72X_EDMA_IRQ                         (49 + DRA72X_NUM_INTERNAL_IRQS)
#define DRA74X_PCIE_SS1_IRQ_INT0                (50 + DRA74X_NUM_INTERNAL_IRQS)
#define DRA74X_PCIE_SS1_IRQ_INT1                (51 + DRA74X_NUM_INTERNAL_IRQS)
#define DRA74X_PCIE_SS2_IRQ_INT0                (52 + DRA74X_NUM_INTERNAL_IRQS)
#define DRA74X_PCIE_SS2_IRQ_INT1                (53 + DRA74X_NUM_INTERNAL_IRQS)

/*************************************************/
/* MPU_WUGEN Register                           */
/*************************************************/
#define DRA74X_MPU_WUGEN                        0x48281000
    #define WKG_CONTROL_0                       0x000
    #define WKG_ENB_A_0                         0x010
    #define WKG_ENB_B_0                         0x014
    #define WKG_ENB_C_0                         0x018
    #define WKG_ENB_D_0                         0x01C
    #define WKG_ENB_E_0                         0x020
    #define WKG_CONTROL_1                       0x400
    #define WKG_ENB_A_1                         0x410
    #define WKG_ENB_B_1                         0x414
    #define WKG_ENB_C_1                         0x418
    #define WKG_ENB_D_1                         0x41C
    #define WKG_ENB_E_1                         0x420
    #define AUX_CORE_BOOT_0                     0x800
    #define AUX_CORE_BOOT_1                     0x804

/*************************************************/
/* GPIO */
/*************************************************/
#define DRA74X_GPIO1_BASE                       0x4ae10000  // 0-31
#define DRA74X_GPIO2_BASE                       0x48055000  // 32-63
#define DRA74X_GPIO3_BASE                       0x48057000  // 64-95
#define DRA74X_GPIO4_BASE                       0x48059000  // 96-127
#define DRA74X_GPIO5_BASE                       0x4805b000  // 128-159
#define DRA74X_GPIO6_BASE                       0x4805d000  // 160-191
#define DRA74X_GPIO7_BASE                       0x48051000  // 192-223
#define DRA74X_GPIO8_BASE                       0x48053000  // 224-255

#define GPIO_REVISION                           0x000
#define GPIO_SYSCONFIG                          0x010
#define GPIO_IRQSTATUS_RAW_0                    0x024
#define GPIO_IRQSTATUS_RAW_1                    0x028
#define GPIO_IRQSTATUS_0                        0x02c
#define GPIO_IRQSTATUS_1                        0x030
#define GPIO_IRQSTATUS_SET_0                    0x034
#define GPIO_IRQSTATUS_SET_1                    0x038
#define GPIO_IRQSTATUS_CLR_0                    0x03c
#define GPIO_IRQSTATUS_CLR_1                    0x040
#define GPIO_IRQWAKEN_0                         0x044
#define GPIO_IRQWAKEN_1                         0x048
#define GPIO_SYSSTATUS                          0x114
#define GPIO_IRQSTATUS1                         0x118
#define GPIO_IRQENABLE1                         0x11c
#define GPIO_WAKEUPENABLE                       0x120
#define GPIO_IRQSTATUS2                         0x128
#define GPIO_IRQENABLE2                         0x12c
#define GPIO_CTRL                               0x130
#define GPIO_OE                                 0x134
#define GPIO_DATAIN                             0x138
#define GPIO_DATAOUT                            0x13c
#define GPIO_LEVELDETECT0                       0x140
#define GPIO_LEVELDETECT1                       0x144
#define GPIO_RISINGDETECT                       0x148
#define GPIO_FALLINGDETECT                      0x14c
#define GPIO_DEBOUNCEENABLE                     0x150
#define GPIO_DEBOUNCINGTIME                     0x154
#define GPIO_CLEARIRQENABLE1                    0x160
#define GPIO_SETIRQENABLE1                      0x164
#define GPIO_CLEARIRQENABLE2                    0x170
#define GPIO_SETIRQENABLE2                      0x174
#define GPIO_CLEARWKUPENA                       0x180
#define GPIO_SETWKUPENA                         0x184
#define GPIO_CLEARDATAOUT                       0x190
#define GPIO_SETDATAOUT                         0x194

/*************************************************/
/* General purpose TIMER    */
/*************************************************/
#define DRA74X_TIMER1_BASE                      0x4AE18000
#define DRA74X_TIMER2_BASE                      0x48032000
#define DRA74X_TIMER3_BASE                      0x48034000
#define DRA74X_TIMER4_BASE                      0x48036000
#define DRA74X_TIMER5_BASE                      0x48820000
#define DRA74X_TIMER6_BASE                      0x48822000
#define DRA74X_TIMER7_BASE                      0x48824000
#define DRA74X_TIMER8_BASE                      0x48826000
#define DRA74X_TIMER9_BASE                      0x4803E000
#define DRA74X_TIMER10_BASE                     0x48086000
#define DRA74X_TIMER11_BASE                     0x48088000
#define DRA74X_TIMER12_BASE                     0x4AE20000
#define DRA74X_TIMER13_BASE                     0x48828000
#define DRA74X_TIMER14_BASE                     0x4882A000
#define DRA74X_TIMER15_BASE                     0x4882C000
#define DRA74X_TIMER16_BASE                     0x4882E000

/*************************************************/
/* UART */
/*************************************************/
#define DRA74X_UART1_BASE                       0x4806a000
#define DRA74X_UART2_BASE                       0x4806c000
#define DRA74X_UART3_BASE                       0x48020000
#define DRA74X_UART4_BASE                       0x4806e000
#define DRA74X_UART_REG_SIZE                    0xFFF

/*************************************************/
/* Shared PHY Component Subsystem               */
/*************************************************/
/* USB3 PHY */
#define DRA74X_USB3_PHY_RX                      0x4A084400
#define DRA74X_USB3_PHY_TX                      0x4A084800
    #define USB3PHYTX_RTRIM_REG                         0x1c
    #define USB3PHYTX_RTRIM_REG_MEM_RTRIM_MASK          0x1f
    #define USB3PHYTX_RTRIM_REG_MEM_RTRIM_SHIFT         27
    #define USB3PHYTX_RTRIM_REG_MEM_OVRD_RTRIM_MASK     0x1
    #define USB3PHYTX_RTRIM_REG_MEM_OVRD_RTRIM_SHIFT    26
    #define USB3PHYTX_TRIM_OBSERVE_REG                  0x24
    #define USB3PHYTX_TRIM_OBSERVE_REG_BGTRIM_MASK      0xffff
    #define USB3PHYTX_TRIM_OBSERVE_REG_BGTRIM_SHIFT     16
    #define USB3PHYTX_TRIM_OBSERVE_REG_RTRIM_MASK       0x1f
    #define USB3PHYTX_TRIM_OBSERVE_REG_RTRIM_SHIFT      11
#define DRA74X_DPLLCTRL_USB_OTG_SS              0x4A084C00
#define DRA74X_OCP2SCP1                         0x4A080000
#define OCP2SCP_SYSCONFIG                       0x10
    #define OCP2SCP_SYSCONFIG_SOFTRESET_OFF     1
#define OCP2SCP_SYSSTATUS                       0x14
    #define OCP2SCP_SYSSTATUS_RESETDONE         1
#define OCP2SCP_TIMING                          0x18
    #define OCP2SCP_TIMING_SYN2_OFF             0
    #define OCP2SCP_TIMING_SYN1_OFF             4

/* PCIe Shared PHY */
#define DRA74X_OCP2SCP3                         0x4a090000  /* OCP2SCP3 is shared by SATA and PCIe */

#define DRA74X_PCIe1_PHY_RX                     0x4a094000
#define DRA74X_PCIe2_PHY_RX                     0x4a095000
#define PCIEPHYRX_ANA_PROGRAMMABILITY_REG1      0x0c
    #define MEM_ANATESTMODE                     27
    #define MEM_ANAMODE                         14
    #define MEM_PLLDIV                          5
#define PCIEPHYRX_TRIM_REG4                     0x1c
    #define MEM_DLL_TRIM_SEL                    30
#define PCIEPHYRX_DLL_REG1                      0x24
    #define MEM_DLL_PHINT_RATE                  30
#define PCIEPHYRX_DIGITAL_MODES_RER1            0x28
    #define MEM_HS_RATE                         (27)
    #define MEM_OVRD_HS_RATE                    (26)
    #define MEM_CDR_FASTLOCK                    (23)
    #define MEM_CDR_LBW                         (21)
    #define MEM_CDR_STEPCNT                     (19)
    #define MEM_CDR_STL                         (16)
    #define MEM_CDR_THR                         (13)
    #define MEM_CDR_THR_MODE                    (12)
    #define MEM_CDR_2NDO_SDM_MODE               (11)
#define PCIEPHYRX_EQUALIZER_REG1                0x38
    #define MEM_EQLEV                           16
    #define MEM_EQFTC                           11
    #define MEM_EQCTL                           7
    #define MEM_OVRD_EQLEV                      2
    #define MEM_OVRD_EQFTC                      1

/* SATA PHY subsystem */
#define DRA74X_DPLLCTRL_SATA_BASE               0x4a096800
#define DPLL_STATUS                             0x04
    #define DPLL_STATUS_LOCK_SHIFT              1
    #define DPLL_STATUS_LOCK                    1
#define DPLL_GO                                 0x08
    #define DPLL_GO_PLL_GO                      0
#define DPLL_CFG1                               0x0c
    #define DPLL_CFG1_M_MASK                    0xfff
    #define DPLL_CFG1_M_SHIFT                   9
    #define DPLL_CFG1_N_MASK                    0xf
    #define DPLL_CFG1_N_SHIFT                   1
#define DPLL_CFG2                               0x10
    #define DPLL_CFG2_SELFREQDCO_MASK           7
    #define DPLL_CFG2_SELFREQDCO_SHIFT          1
    #define DPLL_CFG2_SELFREQDCO                4
#define DPLL_CFG3                               0x14
    #define DPLL_CFG3_SD_MASK                   0xf
    #define DPLL_CFG3_SD_SHIF                   10
#define DPLL_CFG4                               0x20
    #define DPLL_CFG4_REGM_F_MASK               0x1ffff
    #define DPLL_CFG4_REGM_F                    0

#define DRA74X_OCP2SCP3_SATA_PHY_RX             0x4a096000
    #define SATAPHYRX_ANA_PROGRAMMABILITY_REG1  0x0c
    #define SATAPHYRX_DIGITAL_MODES_REG1        0x28
    #define SATAPHYRX_TRIM_REG4                 0x1c
    #define SATAPHYRX_DLL_REG1                  0x24
    #define SATAPHYRX_EQUALIZER_REG1            0x38
    #define SATAPHYRX_IO_AND_A2D_OVERRIDES_REG1 0x44

#define DRA74X_OCP2SCP3_SATA_PHY_TX             0x4a096400
    #define SATAPHYTX_LDO_CONTROL_REG           0x08
    #define SATAPHYTX_LDO_CONTROL_REG_ALDO_MASK      0xffff
    #define SATAPHYTX_LDO_CONTROL_REG_ALDO_SHIFT     16
    #define SATAPHYTX_FUNC_CONFIG_REG           0x0c
    #define SATAPHYTX_TEST_CONFIG_REG           0x2c
    #define SATAPHYTX_PATTGEN_PRELOAD           0x30

/*************************************************/
/* MCSPI */
/*************************************************/
#define DRA74X_MCSPI1_HL_BASE                   0x48098000
#define DRA74X_MCSPI1_BASE                      0x48098100
#define DRA74X_MCSPI2_HL_BASE                   0x4809a000
#define DRA74X_MCSPI2_BASE                      0x4809a100
#define DRA74X_MCSPI3_HL_BASE                   0x480b8000
#define DRA74X_MCSPI3_BASE                      0x480b8100
#define DRA74X_MCSPI4_HL_BASE                   0x480ba000
#define DRA74X_MCSPI4_BASE                      0x480ba100
#define DRA74X_MCSPI_REG_SIZE                   0xFFF

/*************************************************/
/* I2C */
/*************************************************/
#define DRA74X_I2C1_BASE                        0x48070000
#define DRA74X_I2C2_BASE                        0x48072000
#define DRA74X_I2C3_BASE                        0x48060000
#define DRA74X_I2C4_BASE                        0x4807A000
#define DRA74X_I2C5_BASE                        0x4807C000
#define DRA74X_I2C_REG_SIZE                     0xFFF

/*************************************************/
/* MMCSD */
/*************************************************/
#define DRA74X_MMC1_BASE                        0x4809C000
#define DRA74X_MMC2_BASE                        0x480B4000
#define DRA74X_MMC3_BASE                        0x480AD000
#define DRA74X_MMC4_BASE                        0x480D1000
#define DRA74X_MMC_SIZE                         0x18000

/*************************************************/
/* SATA                                         */
/*************************************************/
#define DRA74X_SATA_BASE                        0x4A140000
#define DRA74X_SATA_WRAPPER                     0x4A141100
    #define SATA_CAP                            0x000
    #define SATA_PI                             0x00c
    #define SATA_PxCLB                          0x100
    #define SATA_PxCLBU                         0x104
    #define SATA_PxFB                           0x108
    #define SATA_PxFBU                          0x10C
    #define SATA_PxCMD                          0x118

/*************************************************/
/* USB */
/*************************************************/
#define DRA74X_USB_WRAPPER1                     0x48880000
#define DRA74X_USB_WRAPPER2                     0x488C0000
#define DRA74X_USB_WRAPPER3                     0x48900000
#define DRA74X_USB_WRAPPER4                     0x48940000

#define DRA74X_USB_DWC1                         0x48890000
#define DRA74X_USB_DWC2                         0x488D0000
#define DRA74X_USB_DWC3                         0x48910000
#define DRA74X_USB_DWC4                         0x48950000
    #define USB_OCTL                            0xCC04
    #define USB_GUSB2PHYCFG                     0xc200
    #define USB_GUSB2PHYCFG_ULPI_UTMI_SEL       4
    #define USB_GUSB2PHYCFG_ULPI_UTMI_SEL_UTML  0
    #define USB_GUSB2PHYCFG_ULPI_UTMI_SEL_ULPI  1

#define DRA74X_USB2PHY1                         0x4A084000
#define DRA74X_USB2PHY2                         0x4A085000

/*************************************************/
/* VIP */
/*************************************************/
#define DRA74X_VIP1_BASE                        0x48970000
#define DRA74X_VIP2_BASE                        0x48990000
#define DRA74X_VIP3_BASE                        0x489B0000

/*************************************************/
/* EDMA */
/*************************************************/
#define DRA74X_EDMA_BASE                        0x43300000
#define DRA74X_EDMA_SIZE                        0x5000
#define DRA74X_EDMA_SHADOW0_OFF                 0x2000
#define DRA74X_PARAM_OFF                        0x4000
#define DRA74X_PARAM_SIZE                       0x20
#define DRA74X_PARAM(ch)                        (DRA74X_PARAM_OFF + (DRA74X_PARAM_SIZE * ch))

/*************************************************/
/* SDMA */
/*************************************************/
#define DRA74X_SDMA_BASE                        0x4a056000

#define DRA74X_DMA4_IRQSTATUS_Lj                0x08
#define DRA74X_DMA4_IRQENABLE_Lj                0x18
#define DRA74X_DMA4_OCP_SYSCONFIG               0x2c
    #define DMA4_OCP_SYSCONFIG_AUTOIDLE_FREERUN (0x00 << 0)
    #define DMA4_OCP_SYSCONFIG_AUTOIDLE_AUTO    (0x01 << 0)
    #define DMA4_OCP_SYSCONFIG_SIDLEMODE_FORCE  (0x00 << 3)
    #define DMA4_OCP_SYSCONFIG_SIDLEMODE_NOIDLE (0x01 << 3)
    #define DMA4_OCP_SYSCONFIG_SIDLEMODE_SMART  (0x02 << 3)
    #define DMA4_OCP_SYSCONFIG_MIDLEMODE_FORCE  (0x00 << 12)
    #define DMA4_OCP_SYSCONFIG_MIDLEMODE_NOIDLE (0x01 << 12)
    #define DMA4_OCP_SYSCONFIG_MIDLEMODE_SMART  (0x02 << 12)

#define DRA74X_DMA4_GCR                         0x78
    #define GCR_ARBITRATION_RATE                0xff0000
    #define DMA_DEFAULT_FIFO_DEPTH              0x10
#define DRA74X_DMA4_CCRi                        0x80
#define DRA74X_DMA4_CICRi                       0x88
#define DRA74X_DMA4_CSRi                        0x8C
    #define DRA74X_DMA4_CSR_MSK                 0x5DFE

/*************************************************/
/* DMM */
/*************************************************/
#define DRA74X_DMM_BASE                         0x4e000000
#define DMM_REVISION                            0x00
#define DMM_HWINFO                              0x04
#define DMM_LISA_HWINFO                         0x08
#define DMM_SYSCONFIG                           0x10
#define DMM_LISA_LOCK                           0x1c
#define DMM_LISA_MAP_0                          0x40
#define DMM_LISA_MAP_1                          0x44
#define DMM_LISA_MAP_2                          0x48
#define DMM_LISA_MAP_3                          0x4c
    #define DMM_LISA_MAP_SYS_SIZE_MASK          0x00700000
    #define DMM_LISA_MAP_SYS_SIZE_SHIFT         20
    #define DMM_LISA_MAP_SYS_ADDR_MASK          0xff000000
    #define DMM_LISA_MAP_SDRC_MAP_MASK          0x00000300
    #define DMM_LISA_MAP_SDRC_MAP_SHIFT         8
    #define DMM_LISA_MAP_SDRC_MAP_UNMAPPED      0

#define DRA74X_MA_BASE                          0x482AF000

/*************************************************/
/* WDTIMER2 */
/*************************************************/
#define DRA74X_WDTIMER2_BASE                    0x4ae14000
#define WDT_WIDR                                0x00
#define WDT_WDSC                                0x10
#define WDT_WDST                                0x14
#define WDT_WIER                                0x18
#define WDT_WWER                                0x20
#define WDT_WCLR                                0x24
#define WDT_WCRR                                0x28
#define WDT_WLDR                                0x2c
#define WDT_WTGR                                0x30
#define WDT_WWPS                                0x34
#define WDT_WDLY                                0x44
#define WDT_WSPR                                0x48
    #define DISABLE_SEQ_1                       0xaaaa
    #define DISABLE_SEQ_2                       0x5555
    #define ENABLE_SEQ_1                        0xbbbb
    #define ENABLE_SEQ_2                        0x4444
#define WDT_WIRQSTATRAW                         0x54
#define WDT_WIRQSTAT                            0x58
#define WDT_WIRQENSET                           0x5c
#define WDT_WIRQENCLR                           0x60
#define WDT_WIRQWAKEEN                          0x64

/****************************************************/
/* GPMC module                                      */
/****************************************************/
#define DRA74X_GPMC_BASE                        0x50000000
#define GPMC_SYSCONFIG                          (0x10)
#define GPMC_IRQSTATUS                          (0x18)
#define GPMC_IRQENABLE                          (0x1C)
#define GPMC_TIMEOUT_CONTROL                    (0x40)
#define GPMC_CONFIG                             (0x50)
#define GPMC_STATUS                             (0x54)

#define GPMC_CONFIG1(x)                         (0x60 + 0x30 * (x))
#define GPMC_CONFIG2(x)                         (0x64 + 0x30 * (x))
#define GPMC_CONFIG3(x)                         (0x68 + 0x30 * (x))
#define GPMC_CONFIG4(x)                         (0x6C + 0x30 * (x))
#define GPMC_CONFIG5(x)                         (0x70 + 0x30 * (x))
#define GPMC_CONFIG6(x)                         (0x74 + 0x30 * (x))
#define GPMC_CONFIG7(x)                         (0x78 + 0x30 * (x))

/*************************************************/
/* Power, Reset, and Clock Management           */
/*************************************************/

/*************************************************/
/* L4_WKUP_PRM */
/*************************************************/
#define DRA74X_L4_WKUP_PRM_BASE                 0x4AE06000

/*************************************************/
/* CKGEN_PRM */
/*************************************************/
#define DRA74X_CM_CLKSEL_BASE                   0x4AE06100
#define CM_CLKSEL_SYSCLK1                       0x00
#define CM_CLKSEL_WKUPAON                       0x08
#define CM_CLKSEL_ABE_PLL_REF                   0x0C
#define CM_CLKSEL_SYS                           0x10
    #define SYS_CLKSEL_MASK                     7
#define CM_CLKSEL_ABE_PLL_BYPAS                 0x14
#define CM_CLKSEL_ABE_PLL_SYS                   0x18
#define CM_CLKSEL_ABE_24M                       0x1C
#define CM_CLKSEL_ABE_SYS                       0x20
#define CM_CLKSEL_HDMI_MCASP_AUX                0x24
#define CM_CLKSEL_HDMI_TIMER                    0x28
#define CM_CLKSEL_MCASP_SYS                     0x2C
#define CM_CLKSEL_MLBP_MCASP                    0x30
#define CM_CLKSEL_MLB_MCASP                     0x34
#define CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX    0x38
#define CM_CLKSEL_SYS_CLK1_32K                  0x40
#define CM_CLKSEL_TIMER_SYS                     0x44
#define CM_CLKSEL_VIDEO1_MCASP_AUX              0x48
#define CM_CLKSEL_VIDEO1_TIMER                  0x4C
#define CM_CLKSEL_VIDEO2_MCASP_AUX              0x50
#define CM_CLKSEL_VIDEO2_TIMER                  0x54
#define CM_CLKSEL_CLKOUTMUX0                    0x58
#define CM_CLKSEL_CLKOUTMUX1                    0x5C
#define CM_CLKSEL_CLKOUTMUX2                    0x60
#define CM_CLKSEL_HDMI_PLL_SYS                  0x64
#define CM_CLKSEL_VIDEO1_PLL_SYS                0x68
#define CM_CLKSEL_VIDEO2_PLL_SYS                0x6C
#define CM_CLKSEL_ABE_CLK_DIV                   0x70
#define CM_CLKSEL_ABE_GICLK_DIV                 0x74
#define CM_CLKSEL_AESS_FCLK_DIV                 0x78
#define CM_CLKSEL_EVE_CLK                       0x80
#define CM_CLKSEL_USB_OTG_CLK_CLKOUTMUX         0x84
#define CM_CLKSEL_CORE_DPLL_OUT_CLK_CLKOUTMUX   0x88
#define CM_CLKSEL_DSP_GFCLK_CLKOUTMUX           0x8C
#define CM_CLKSEL_EMIF_PHY_GCLK_CLKOUTMUX       0x90
#define CM_CLKSEL_EMU_CLK_CLKOUTMUX             0x94
#define CM_CLKSEL_FUNC_96M_AON_CLK_CLKOUTMUX    0x98
#define CM_CLKSEL_GMAC_250M_CLK_CLKOUTMUX       0x9C
#define CM_CLKSEL_GPU_GCLK_CLKOUTMUX            0xA0
#define CM_CLKSEL_HDMI_CLK_CLKOUTMUX            0xA4
#define CM_CLKSEL_IVA_GCLK_CLKOUTMUX            0xA8
#define CM_CLKSEL_L3INIT_480M_GFCLK_CLKOUTMUX   0xAC
#define CM_CLKSEL_MPU_GCLK_CLKOUTMUX            0xB0
#define CM_CLKSEL_PCIE1_CLK_CLKOUTMUX           0xB4
#define CM_CLKSEL_PCIE2_CLK_CLKOUTMUX           0xB8
#define CM_CLKSEL_PER_ABE_X1_CLK_CLKOUTMUX      0xBC
#define CM_CLKSEL_SATA_CLK_CLKOUTMUX            0xC0
#define CM_CLKSEL_SECURE_32K_CLK_CLKOUTMUX      0xC4
#define CM_CLKSEL_SYS_CLK1_CLKOUTMUX            0xC8
#define CM_CLKSEL_SYS_CLK2_CLKOUTMUX            0xCC
#define CM_CLKSEL_VIDEO1_CLK_CLKOUTMUX          0xD0
#define CM_CLKSEL_VIDEO2_CLK_CLKOUTMUX          0xD4
#define CM_CLKSEL_ABE_LP_CLK                    0xD8
#define CM_CLKSEL_EVE_GFCLK_CLKOUTMUX           0xE0

// CLKSEL field bits
#define CM_CLKSEL_SYS__MASK                     0x7
#define CM_CLKSEL_SYS__UNINITIALIZED            0
#define CM_CLKSEL_SYS__12M                      1
#define CM_CLKSEL_SYS__20M                      2
#define CM_CLKSEL_SYS__16P8M                    3
#define CM_CLKSEL_SYS__19P2M                    4
#define CM_CLKSEL_SYS__26M                      5
#define CM_CLKSEL_SYS__38P4M                    7

/*************************************************/
/* WKUPAON_CM */
/*************************************************/
#define DRA74X_CM_WKUPAON_BASE                  0x4AE07800
#define CM_WKUPAON_CLKSTCTRL                    0x00
#define CM_WKUPAON_L4_WKUP_CLKCTRL              0x20
#define CM_WKUPAON_WD_TIMER2_CLKCTRL            0x30
#define CM_WKUPAON_GPIO1_CLKCTRL                0x38
#define CM_WKUPAON_TIMER1_CLKCTRL               0x40
#define CM_WKUPAON_TIMER12_CLKCTRL              0x48
#define CM_WKUPAON_COUNTER_32K_CLKCTRL          0x50
#define CM_WKUPAON_KBD_CLKCTRL                  0x78
#define CM_WKUPAON_UART10_CLKCTRL               0x80
#define CM_WKUPAON_DCAN1_CLKCTRL                0x88
#define CM_WKUPAON_SCRM_CLKCTRL                 0x90

#define DRA74X_PRM_BASE                         0x4AE07D00
    #define PRM_RSTCTRL                         0x00
    #define PRM_RSTCTRL_COLDBOOT                (1 << 1)
    #define PRM_RSTCTRL_WARMBOOT                (1 << 0)
    #define PRM_RSTST                           0x04
	#define PRM_RSTST_WARM_RESET_MASK           0x7FEA
    #define PRM_RSTTIME                         0x08
    #define PRM_CLKREQCTRL                      0x0C
    #define PRM_PSCON_COUNT                     0x18
    #define PRM_IO_COUNT                        0x1C
    #define PRM_IO_PMCTRL                       0x20

/*
 * Undocumented registers
 * The only purpose of these registers is to implement errata AVATAR-00056
 */
#define IODELAY_CONFIG_REG_0                    0x4844A00C
#define IODELAY_CONFIG_REG_1                    0x4844A010
#define IODELAY_CONFIG_REG_2                    0x4844A014
#define IODELAY_CONFIG_REG_3                    0x4844A018
#define IODELAY_CONFIG_REG_4                    0x4844A01C
#define IODELAY_CONFIG_REG_5                    0x4844A020
#define IODELAY_CONFIG_REG_6                    0x4844A024
#define IODELAY_CONFIG_REG_7                    0x4844A028
#define IODELAY_CONFIG_REG_8                    0x4844A02C

/*************************************************/
/* PRCM */
/*************************************************/
#define DRA74X_PRCM_BASE                        0x4A005000

/*************************************************/
/* CKGEN_CM_CORE_AON */
/*************************************************/

/* cm1.ckgen */
#define CM_CLKSEL_CORE                          0x100
    #define CLKSEL_SHIFT                        0
    #define CLKSEL_L3_SHIFT                     4
    #define CLKSEL_L4_SHIFT                     8
#define CM_CLKSEL_ABE                           0x108
#define CM_DLL_CTRL                             0x110
#define CM_CLKMODE_DPLL_CORE                    0x120
    #define DPLL_EN_LOW_POWER_BYPASS            5
    #define DPLL_EN_FAST_RELOCK_BYPASS          6
    #define DPLL_EN_LOCK                        7
    #define DPLL_EN_MASK                        7
#define CM_IDLEST_DPLL_CORE                     0x124
    #define ST_DPLL_CLK_MASK                    0x1
#define CM_AUTOIDLE_DPLL_CORE                   0x128
#define CM_CLKSEL_DPLL_CORE                     0x12c
#define CM_DIV_M2_DPLL_CORE                     0x130
#define CM_DIV_M3_DPLL_CORE                     0x134
#define CM_DIV_H11_DPLL_CORE                    0x138
    #define Hnm_DIVHS_MASK                      0x3F
#define CM_DIV_H12_DPLL_CORE                    0x13c
#define CM_DIV_H13_DPLL_CORE                    0x140
#define CM_DIV_H14_DPLL_CORE                    0x144
#define CM_SSC_DELTAMSTEP_DPLL_CORE             0x148
#define CM_SSC_MODFREQDIV_DPLL_CORE             0x14c
#define CM_DIV_H21_DPLL_CORE                    0x150
#define CM_DIV_H22_DPLL_CORE                    0x154
#define CM_DIV_H23_DPLL_CORE                    0x158
#define CM_DIV_H24_DPLL_CORE                    0x15c
#define CM_CLKMODE_DPLL_MPU                     0x160
#define CM_IDLEST_DPLL_MPU                      0x164
#define CM_AUTOIDLE_DPLL_MPU                    0x168
#define CM_CLKSEL_DPLL_MPU                      0x16c
    #define CM_CLKSEL_DCC_EN_SHIFT              22
    #define CM_CLKSEL_DCC_EN_MASK               (1 << CM_CLKSEL_DCC_EN_SHIFT)
    #define CM_CLKSEL_MULT_SHIFT                8
    #define CM_CLKSEL_MULT_MASK                 (0x7ff << CM_CLKSEL_MULT_SHIFT)
    #define CM_CLKSEL_MULT_MAX                  0x7ff
    #define CM_CLKSEL_DPLL_DIV_SHIFT            0
    #define CM_CLKSEL_DPLL_DIV_MASK             (0x7f << CM_CLKSEL_DPLL_DIV_SHIFT)
#define CM_DIV_M2_DPLL_MPU                      0x170
#define CM_SSC_DELTAMSTEP_DPLL_MPU              0x188
#define CM_SSC_MODFREQDIV_DPLL_MPU              0x18C
#define CM_BYPCLK_DPLL_MPU                      0x19C
#define CM_CLKMODE_DPLL_IVA                     0x1A0
#define CM_IDLEST_DPLL_IVA                      0x1A4
#define CM_AUTOIDLE_DPLL_IVA                    0x1A8
#define CM_CLKSEL_DPLL_IVA                      0x1AC
#define CM_DIV_H11_DPLL_IVA                     0x1B0
#define CM_DIV_H12_DPLL_IVA                     0x1B4
#define CM_SSC_DELTAMSTEP_DPLL_IVA              0x1C8
#define CM_SSC_MODFREQDIV_DPLL_IVA              0x1CC
#define CM_BYPCLK_DPLL_IVA                      0x1DC
#define CM_CLKMODE_DPLL_ABE                     0x1E0
    #define DPLL_REGM4XEN                       (1 << 11)
#define CM_IDLEST_DPLL_ABE                      0x1E4
#define CM_AUTOIDLE_DPLL_ABE                    0x1E8
#define CM_CLKSEL_DPLL_ABE                      0x1EC
#define CM_DIV_M2_DPLL_ABE                      0x1F0
    #define DIVHS_MASK                          0x1F

#define CM_DIV_M3_DPLL_ABE                      0x1F4
#define CM_SSC_DELTAMSTEP_DPLL_ABE              0x208
#define CM_SSC_MODFREQDIV_DPLL_ABE              0x20C
#define CM_CLKMODE_DPLL_DDRPHY                  0x210
#define CM_IDLEST_DPLL_DDRPHY                   0x214
#define CM_AUTOIDLE_DPLL_DDRPHY                 0x218
#define CM_CLKSEL_DPLL_DDRPHY                   0x21C
#define CM_DIV_M2_DPLL_DDRPHY                   0x220
#define CM_DIV_H11_DPLL_DDRPHY                  0x228
#define CM_SSC_DELTAMSTEP_DPLL_DDRPHY           0x22C
#define CM_CLKMODE_DPLL_DSP                     0x234
#define CM_SHADOW_FREQ_CONFIG1                  0x260
#define CM_SHADOW_FREQ_CONFIG2                  0x264
#define CM_DYN_DEP_PRESCAL                      0x270
#define CM_RESTORE_ST                           0x280
#define CM_CLKMODE_DPLL_EVE                     0x284
#define CM_CLKMODE_DPLL_GMAC                    0x2A8
#define CM_CLKMODE_DPLL_GPU                     0x2D8

/* cm1.mpu */
#define CM_MPU_MPU_CLKCTRL                      0x320

/* DSP1_CM_CORE_AON */
#define CM_DSP1_CLKSTCTRL                       0x400
#define CM_DSP1_STATICDEP                       0x404
#define CM_DSP1_DYNAMICDEP                      0x408
#define CM_DSP1_DSP1_CLKCTRL                    0x420

/*************************************************/
/* IPU_CM_CORE_AON */
/*************************************************/
#define CM_IPU1_CLKSTCTRL                       0x500
#define CM_IPU1_STATICDEP                       0x504
#define CM_IPU1_DYNAMICDEP                      0x508
#define CM_IPU1_IPU1_CLKCTRL                    0x520
#define CM_IPU_CLKSTCTRL                        0x540
#define CM_IPU_MCASP1_CLKCTRL                   0x550
    #define CM_MCASP_CLKSEL_AHCLKX_OFF          24
    #define CM_MCASP_CLKSEL_AHCLKX_MSK          (0xF << CM_MCASP_CLKSEL_AHCLKX_OFF)
    #define CM_MCASP_CLKSEL_AHCLKX_ATL3         3
    #define CM_MCASP_CLKSEL_AHCLKX_ATL2         4
    #define CM_MCASP_CLKSEL_AHCLKX_ATL1         5
    #define CM_MCASP_CLKSEL_AHCLKX_ATL0         6
    #define CM_MCASP_CLKSEL_AUX_OFF             22
    #define CM_MCASP_CLKSEL_AUX_MSK             (0x3 << CM_MCASP_CLKSEL_AUX_OFF)
    #define CM_MCASP_CLKSEL_AUX_ABE             0x0
    #define CM_MCASP_CLKSEL_AUX_VIDEO1          0x1
    #define CM_MCASP_CLKSEL_AUX_HDMI            0x3
#define CM_IPU_TIMER5_CLKCTRL                   0x558
#define CM_IPU_TIMER6_CLKCTRL                   0x560
#define CM_IPU_TIMER7_CLKCTRL                   0x568
#define CM_IPU_TIMER8_CLKCTRL                   0x570
#define CM_IPU_I2C5_CLKCTRL                     0x578
#define CM_IPU_UART6_CLKCTRL                    0x580

/*************************************************/
/* DSP2_CM_CORE_AON */
/*************************************************/
#define CM_DSP2_CLKSTCTRL                       0x600
#define CM_DSP2_STATICDEP                       0x604
#define CM_DSP2_DYNAMICDEP                      0x608
#define CM_DSP2_DSP2_CLKCTRL                    0x620

/*************************************************/
/* EVE1_CM_CORE_AON */
/*************************************************/
#define CM_EVE1_CLKSTCTRL                       0x640
#define CM_EVE1_STATICDEP                       0x644
#define CM_EVE1_EVE1_CLKCTRL                    0x660

/*************************************************/
/* EVE2_CM_CORE_AON */
/*************************************************/
#define CM_EVE2_CLKSTCTRL                       0x680
#define CM_EVE2_STATICDEP                       0x684
#define CM_EVE2_EVE2_CLKCTRL                    0x6a0

/*************************************************/
/* EVE3_CM_CORE_AON */
/*************************************************/
#define CM_EVE3_CLKSTCTRL                       0x6c0
#define CM_EVE3_STATICDEP                       0x6c4
#define CM_EVE3_EVE3_CLKCTRL                    0x6e0

/* CM_CORE_AON__VPE (offsets from PRCM base 0x4a005000) */
#define CM_VPE_CLKSTCTRL 0x0760
#define CM_VPE_VPE_CLKCTRL 0x0764
#define CM_VPE_STATICDEP 0x0768

/* cm2.ckgen */
#define CM_CLKSEL_USB_60MHZ                     0x3104
#define CM_CLKMODE_DPLL_PER                     0x3140
#define CM_IDLEST_DPLL_PER                      0x3144
#define CM_AUTOIDLE_DPLL_PER                    0x3148
#define CM_CLKSEL_DPLL_PER                      0x314C
#define CM_DIV_M2_DPLL_PER                      0x3150
#define CM_DIV_M3_DPLL_PER                      0x3154
#define CM_DIV_H11_DPLL_PER                     0x3158
#define CM_DIV_H12_DPLL_PER                     0x315C
#define CM_DIV_H14_DPLL_PER                     0x3164
#define CM_SSC_DELTAMSTEP_DPLL_PER              0x3168
#define CM_SSC_MODFREQDIV_DPLL_PER              0x316C
#define CM_CLKMODE_DPLL_USB                     0x3180
#define CM_IDLEST_DPLL_USB                      0x3184
#define CM_AUTOIDLE_DPLL_USB                    0x3188
#define CM_CLKSEL_DPLL_USB                      0x318C
#define CM_DIV_M2_DPLL_USB                      0x3190
#define CM_SSC_DELTAMSTEP_DPLL_USB              0x31A8
#define CM_SSC_MODFREQDIV_DPLL_USB              0x31AC
#define CM_CLKDCOLDO_DPLL_USB                   0x31B4
#define CM_CLKMODE_DPLL_PCIE_REF                0x3200
#define CM_CLKMODE_APLL_PCIE                    0x321C
#define CM_IDLEST_APLL_PCIE                     0x3220
#define CM_DIV_M2_APLL_PCIE                     0x3224
#define CM_CLKVCOLDO_APLL_PCIE                  0x3228

/*************************************************/
/* COREAON_CM_CORE */
/*************************************************/
#define CM_COREAON_CLKSTCTRL                    0x3600
#define CM_COREAON_SMARTREFLEX_MPU_CLKCTRL      0x3628
#define CM_COREAON_SMARTREFLEX_CORE_CLKCTRL     0x3638
#define CM_COREAON_USB_PHY1_CORE_CLKCTRL        0x3640
#define CM_COREAON_SMARTREFLEX_GPU_CLKCTRL      0x3658
#define CM_COREAON_SMARTREFLEX_DSPEVE_CLKCTRL   0x3668
#define CM_COREAON_SMARTREFLEX_IVAHD_CLKCTRL    0x3678
#define CM_COREAON_USB_PHY2_CORE_CLKCTRL        0x3688
#define CM_COREAON_USB_PHY3_CORE_CLKCTRL        0x3698

/*************************************************/
/* L4_CFG__CM_CORE__CORE_CM_CORE registers      */
/*************************************************/
#define DRA74X_CM_CORE_BASE                     0x4A008700
#define CM_L3MAIN1_CLKSTCTRL                    0x3700
#define CM_L3MAIN1_DYNAMICDEP                   0x3708
#define CM_L3MAIN1_L3_MAIN_1_CLKCTRL            0x3720
#define CM_L3MAIN1_GPMC_CLKCTRL                 0x3728
#define CM_L3MAIN1_MMU_EDMA_CLKCTRL             0x3730
#define CM_L3MAIN1_MMU_PCIESS_CLKCTRL           0x3748
#define CM_L3MAIN1_OCMC_RAM1_CLKCTRL            0x3750
#define CM_L3MAIN1_OCMC_RAM2_CLKCTRL            0x3758
#define CM_L3MAIN1_OCMC_RAM3_CLKCTRL            0x3760
#define CM_L3MAIN1_OCMC_ROM_CLKCTRL             0x3768
#define CM_L3MAIN1_TPCC_CLKCTRL                 0x3770
#define CM_L3MAIN1_TPTC1_CLKCTRL                0x3778
#define CM_L3MAIN1_TPTC2_CLKCTRL                0x3780
#define CM_L3MAIN1_VCP1_CLKCTRL                 0x3788
#define CM_L3MAIN1_VCP2_CLKCTRL                 0x3790

#define CM_IPU2_CLKSTCTRL                       0x3900
#define CM_IPU2_STATICDEP                       0x3904
#define CM_IPU2_DYNAMICDEP                      0x3908
#define CM_IPU2_IPU2_CLKCTRL                    0x3920

/*************************************************/
/* CORE_CM_CORE */
/*************************************************/
#define CM_DMA_CLKSTCTRL                        0x3A00
#define CM_DMA_STATICDEP                        0x3A04
#define CM_DMA_DYNAMICDEP                       0x3A08
#define CM_DMA_DMA_SYSTEM_CLKCTRL               0x3A20

#define CM_EMIF_CLKSTCTRL                       0x3B00
#define CM_EMIF_DMM_CLKCTRL                     0x3B20
#define CM_EMIF_EMIF_OCP_FW_CLKCTRL             0x3B28
#define CM_EMIF_EMIF1_CLKCTRL                   0x3B30
#define CM_EMIF_EMIF2_CLKCTRL                   0x3B38
#define CM_EMIF_EMIF_DLL_CLKCTRL                0x3B40

#define CM_ATL_ATL_CLKCTRL                      0x3C00
    #define CLKSEL_SOURCE1_OFF                  24
    #define CLKSEL_SOURCE2_OFF                  26
    #define CLKSEL_SOURCE_WIDTH                 2
#define CM_ATL_CLKSTCTRL                        0x3C20

#define CM_L4CFG_CLKSTCTRL                      0x3D00
#define CM_L4CFG_DYNAMICDEP                     0x3D08
#define CM_L4CFG_L4_CFG_CLKCTRL                 0x3D20
#define CM_L4CFG_HW_SEM_CLKCTRL                 0x3D28
#define CM_L4CFG_MAILBOX_CLKCTRL                0x3D30
#define CM_L4CFG_SAR_ROM_CLKCTRL                0x3D38

//#define DRA74X_CM_L3INSTR_BASE                0x4a008E00
#define CM_L3INSTR_CLKSTCTRL                    0x3E00
#define CM_L3INSTR_L3_MAIN_2_CLKCTRL            0x3E20
#define CM_L3INSTR_L3_INSTR_CLKCTRL             0x3E28
#define CM_L3INSTR_OCP_WP_NOC_CLKCTRL           0x3E40
#define CM_L3INSTR_DLL_AGING_CLKCTRL            0x3E48
#define CM_L3INSTR_CTRL_MODULE_BANDGAP_CLKCTRL  0x3E50

/*************************************************/
/* IVA_CM_CORE */
/*************************************************/
#define CM_IVA_CLKSTCTRL                        0x3F00
#define CM_IVA_STATICDEP                        0x3F04
#define CM_IVA_DYNAMICDEP                       0x3F08
#define CM_IVA_IVAHD_CLKCTRL                    0x3F20
#define CM_IVA_SL2_CLKCTRL                      0x3F28

/*************************************************/
/* CAM_CM_CORE */
/*************************************************/
#define CM_CAM_CLKSTCTRL                        0x4000
#define CM_CAM_STATICDEP                        0x4004
#define CM_CAM_VIP1_CLKCTRL                     0x4020
#define CM_CAM_VIP2_CLKCTRL                     0x4028
#define CM_CAM_VIP3_CLKCTRL                     0x4030

/*************************************************/
/* DSS_CM_CORE */
/*************************************************/
#define CM_DSS_CLKSTCTRL                        0x4100
#define CM_DSS_STATICDEP                        0x4104
#define CM_DSS_DYNAMICDEP                       0x4108
#define CM_DSS_DSS_CLKCTRL                      0x4120
#define CM_DSS_BB2D_CLKCTRL                     0x4130

/*************************************************/
/* GPU_CM_CORE */
/*************************************************/
#define CM_GPU_CLKSTCTRL                        0x4200
#define CM_GPU_STATICDEP                        0x4204
#define CM_GPU_DYNAMICDEP                       0x4208
#define CM_GPU_GPU_CLKCTRL                      0x4220

#define CM_GPU_GPU_CLKCTRL_FCLK_CORE            0x00000000
#define CM_GPU_GPU_CLKCTRL_FCLK_PER             0x01000000

/*************************************************/
/* L3INIT_CM_CORE   */
/*************************************************/
#define CM_L3INIT_CLKSTCTRL                     0x4300
#define CM_L3INIT_STATICDEP                     0x4304
#define CM_L3INIT_DYNAMICDEP                    0x4308
#define CM_L3INIT_MMC1_CLKCTRL                  0x4328
#define CM_L3INIT_MMC2_CLKCTRL                  0x4330
#define CM_L3INIT_USB_OTG_SS2_CLKCTRL           0x4340
#define CM_L3INIT_USB_OTG_SS3_CLKCTRL           0x4348
#define CM_L3INIT_USB_OTG_SS4_CLKCTRL           0x4350
#define CM_L3INIT_MLB_SS_CLKCTRL                0x4358
#define CM_L3INIT_IEEE1500_2_OCP_CLKCTRL        0x4378
#define CM_L3INIT_SATA_CLKCTRL                  0x4388
#define CM_PCIE_CLKSTCTRL                       0x43A0
    #define PCIE_CLKSTCTRL_L3_GICLK             (1 << 8)
    #define PCIE_CLKSTCTRL_L4_GICLK             (1 << 9)
#define CM_PCIE_STATICDEP                       0x43A4
#define CM_PCIE_PCIESS1_CLKCTRL                 0x43B0
#define CM_PCIE_PCIESS2_CLKCTRL                 0x43B8
    #define PCIE_CLKCTRL_OPTFCLKEN              (1 << 9)
#define CM_GMAC_CLKSTCTRL                       0x43C0
#define CM_GMAC_STATICDEP                       0x43C4
#define CM_GMAC_DYNAMICDEP                      0x43C8
#define CM_GMAC_GMAC_CLKCTRL                    0x43D0
#define CM_L3INIT_OCP2SCP1_CLKCTRL              0x43E0
#define CM_L3INIT_OCP2SCP3_CLKCTRL              0x43E8
#define CM_L3INIT_USB_OTG_SS1_CLKCTRL           0x43F0

/*************************************************/
/* L4PER_CM_CORE    */
/*************************************************/
#define CM_L4PER_CLKSTCTRL                      0x4700
#define CM_L4PER_DYNAMICDEP                     0x4708
#define CM_L4PER2_L4_PER2_CLKCTRL               0x470C
#define CM_L4PER3_L4_PER3_CLKCTRL               0x4714
#define CM_L4PER_TIMER10_CLKCTRL                0x4728
#define CM_L4PER_TIMER11_CLKCTRL                0x4730
#define CM_L4PER_TIMER2_CLKCTRL                 0x4738
#define CM_L4PER_TIMER3_CLKCTRL                 0x4740
#define CM_L4PER_TIMER4_CLKCTRL                 0x4748
#define CM_L4PER_TIMER9_CLKCTRL                 0x4750
#define CM_L4PER_ELM_CLKCTRL                    0x4758
#define CM_L4PER_GPIO2_CLKCTRL                  0x4760
#define CM_L4PER_GPIO3_CLKCTRL                  0x4768
#define CM_L4PER_GPIO4_CLKCTRL                  0x4770
#define CM_L4PER_GPIO5_CLKCTRL                  0x4778
#define CM_L4PER_GPIO6_CLKCTRL                  0x4780
#define CM_L4PER_HDQ1W_CLKCTRL                  0x4788
#define CM_L4PER2_PWMSS2_CLKCTRL                0x4790
#define CM_L4PER2_PWMSS3_CLKCTRL                0x4798
#define CM_L4PER_I2C1_CLKCTRL                   0x47a0
#define CM_L4PER_I2C2_CLKCTRL                   0x47a8
#define CM_L4PER_I2C3_CLKCTRL                   0x47b0
#define CM_L4PER_I2C4_CLKCTRL                   0x47b8
#define CM_L4PER_L4_PER1_CLKCTRL                0x47c0
#define CM_L4PER2_PWMSS1_CLKCTRL                0x47C4
#define CM_L4PER3_TIMER13_CLKCTRL               0x47C8
#define CM_L4PER3_TIMER14_CLKCTRL               0x47D0
#define CM_L4PER3_TIMER15_CLKCTRL               0x47D8
#define CM_L4PER_MCSPI1_CLKCTRL                 0x47f0
#define CM_L4PER_MCSPI2_CLKCTRL                 0x47f8
#define CM_L4PER_MCSPI3_CLKCTRL                 0x4800
#define CM_L4PER_MCSPI4_CLKCTRL                 0x4808
#define CM_L4PER_GPIO7_CLKCTRL                  0x4810
#define CM_L4PER_GPIO8_CLKCTRL                  0x4818
#define CM_L4PER_MMC3_CLKCTRL                   0x4820
#define CM_L4PER_MMC4_CLKCTRL                   0x4828
#define CM_L4PER3_TIMER16_CLKCTRL               0x4830
#define CM_L4PER2_QSPI_CLKCTRL                  0x4838
#define CM_L4PER_UART1_CLKCTRL                  0x4840
#define CM_L4PER_UART2_CLKCTRL                  0x4848
#define CM_L4PER_UART3_CLKCTRL                  0x4850
#define CM_L4PER_UART4_CLKCTRL                  0x4858
#define CM_L4PER2_MCASP2_CLKCTRL                0x4860
#define CM_L4PER2_MCASP3_CLKCTRL                0x4868
#define CM_L4PER_UART5_CLKCTRL                  0x4870
#define CM_L4PER2_MCASP5_CLKCTRL                0x4878
#define CM_L4SEC_CLKSTCTRL                      0x4880
#define CM_L4SEC_STATICDEP                      0x4884
#define CM_L4SEC_DYNAMICDEP                     0x4888
#define CM_L4PER2_MCASP8_CLKCTRL                0x4890
#define CM_L4PER2_MCASP4_CLKCTRL                0x4898
#define CM_L4SEC_AES1_CLKCTRL                   0x48A0
#define CM_L4SEC_AES2_CLKCTRL                   0x48A8
#define CM_L4SEC_DES3DES_CLKCTRL                0x48B0
#define CM_L4SEC_FPKA_CLKCTRL                   0x48B8
#define CM_L4SEC_RNG_CLKCTRL                    0x48C0
#define CM_L4SEC_SHA2MD51_CLKCTRL               0x48C8
#define CM_L4PER2_UART7_CLKCTRL                 0x48D0
#define CM_L4SEC_DMA_CRYPTO_CLKCTRL             0x48D8
#define CM_L4PER2_UART8_CLKCTRL                 0x48E0
#define CM_L4PER2_UART9_CLKCTRL                 0x48E8
#define CM_L4PER2_DCAN2_CLKCTRL                 0x48F0
#define CM_L4SEC_SHA2MD52_CLKCTRL               0x48F8
#define CM_L4PER2_CLKSTCTRL                     0x48FC
#define CM_L4PER2_DYNAMICDEP                    0x4900
#define CM_L4PER2_MCASP6_CLKCTRL                0x4904
#define CM_L4PER2_MCASP7_CLKCTRL                0x4908
#define CM_L4PER2_STATICDEP                     0x490C
#define CM_L4PER3_CLKSTCTRL                     0x4910
#define CM_L4PER3_DYNAMICDEP                    0x4914

#define PM_IPU_PWRSTCTRL                        0x4AE06500
    #define PWRSTCTRL_POWERSTATE_OFFSET         0
    #define PWRSTCTRL_POWERSTATE_WIDTH          2
    #define PWRSTCTRL_POWERSTATE_OFF            0x0
    #define PWRSTCTRL_POWERSTATE_RET            0x1
    #define PWRSTCTRL_POWERSTATE_INACTIVE       0x2
    #define PWRSTCTRL_POWERSTATE_ON             0x3

/*************************************************/
/* ATL: Audio Tracking Logic                    */
/*************************************************/
#define ATL_BASE                                0x4843c000

/* ATL instance offsets */
#define ATL_INST(id)                            (0x200 + (id * 0x80))

/* ATL registers */
#define ATL_REVID                               0x000
#define ATL_PPMR(inst)                          (inst + 0x00)
#define ATL_BBSR(inst)                          (inst + 0x04)
#define ATL_ATLCR(inst)                         (inst + 0x08)
#define ATL_SWEN(inst)                          (inst + 0x10)
    #define ATL_SWEN_DISABLE                    0
    #define ATL_SWEN_ENABLE                     1
#define ATL_BWSMUX(inst)                        (inst + 0x14)
#define ATL_AWSMUX(inst)                        (inst + 0x18)
#define ATL_PCLKMUX(inst)                       (inst + 0x1c)
    #define ATL_PCLKMUX_OCP_CLK                 0
    #define ATL_PCLKMUX_ATLPCLK                 1

/*
 * IEN  - Input Enable
 * IDIS - Input Disable
 * PTU  - Pull type Up
 * PTD  - Pull type Down
 * EN   - Pull type selection is active
 * DIS  - Pull type selection is inactive
 * M0   - Mode 0
 */
//Bit Meanings for CONTROL_PADCONF_x Registers
#define FSC                                     (0 << 19)
#define SSC                                     (1 << 19)
#define IEN                                     (1 << 18)
#define IDIS                                    (0 << 18)
#define PTU                                     (1 << 17)
#define PTD                                     (0 << 17)
#define PEN                                     (0 << 16)
#define PDIS                                    (1 << 16)
#define WKEN                                    (1 << 24)
#define WKDIS                                   (0 << 24)

// Mode Selection
#define M0                                      0
#define M1                                      1
#define M2                                      2
#define M3                                      3
#define M4                                      4
#define M5                                      5
#define M6                                      6
#define M7                                      7
#define M8                                      8
#define M9                                      9
#define M10                                     10
#define M11                                     11
#define M12                                     12
#define M13                                     13
#define M14                                     14
#define M15                                     15
#define SAFE_MODE                               M15

/***********************************************************/
/* Common definitions for CLKCTRL and   CLKSTCTRL registers */
/***********************************************************/
/*
 * These are common definitions for those _CLKCTRL registers
 * in different power domain
 * Not all these fields exist in every register
 */
#define CLKCTRL_MODULEMODE_OFFSET               0
#define CLKCTRL_MODULEMODE_WIDTH                2
#define CLKCTRL_MODULEMODE_MASK                 (0x3 << CLKCTRL_MODULEMODE_OFFSET)
#define CLKCTRL_MODULEMODE_DISABLED             (0 << CLKCTRL_MODULEMODE_OFFSET)
#define CLKCTRL_MODULEMODE_AUTO                 (1 << CLKCTRL_MODULEMODE_OFFSET)
#define CLKCTRL_MODULEMODE_ENABLED              (2 << CLKCTRL_MODULEMODE_OFFSET)

#define CLKCTRL_OPTCLKEN_OFFSET                 8
#define CLKCTRL_OPTCLKEN_DISABLED               (0 << CLKCTRL_OPTCLKEN_OFFSET)
#define CLKCTRL_OPTCLKEN_ENABLED                (1 << CLKCTRL_OPTCLKEN_OFFSET)

#define CLKCTRL_IDLEST_OFFSET                   16
#define CLKCTRL_IDLEST_MASK                     (0x3 << CLKCTRL_IDLEST_OFFSET)
#define CLKCTRL_IDLEST_FUNCTIONAL               (0x0 << CLKCTRL_IDLEST_OFFSET)
#define CLKCTRL_IDLEST_TRANSITION               (0x1 << CLKCTRL_IDLEST_OFFSET)
#define CLKCTRL_IDLEST_IDLE                     (0x2 << CLKCTRL_IDLEST_OFFSET)
#define CLKCTRL_IDLEST_DISABLED                 (0x3 << CLKCTRL_IDLEST_OFFSET)

#define CLKCTRL_CLKSEL_OFFSET                   24
#define CLKCTRL_CLKSEL_MSK                      (1 << CLKCTRL_CLKSEL_OFFSET)
#define CLKCTRL_CLKSEL_DIV_OFF                  25
#define CLKCTRL_CLKSEL_DIV_MSK                  (3 << CLKCTRL_CLKSEL_DIV_OFF)

/* These are common definitions for those _CLKSTCTRL registers */
#define CLKSTCTRL_CLKTRCTRL_OFFSET              0
#define CLKSTCTRL_CLKTRCTRL_WIDTH               2
#define CLKSTCTRL_CLKTRCTRL_MASK                (0x3 << CLKSTCTRL_CLKTRCTRL_OFFSET)
#define CLKSTCTRL_CLKTRCTRL_NO_SLEEP            (0x0 << CLKSTCTRL_CLKTRCTRL_OFFSET)
#define CLKSTCTRL_CLKTRCTRL_SW_SEEP             (0x1 << CLKSTCTRL_CLKTRCTRL_OFFSET)
#define CLKSTCTRL_CLKTRCTRL_SW_WKUP             (0x2 << CLKSTCTRL_CLKTRCTRL_OFFSET)
#define CLKSTCTRL_CLKTRCTRL_HW_AUTO             (0x3 << CLKSTCTRL_CLKTRCTRL_OFFSET)

#define CLKSTCTRL_CLKACTIVITY_OFFSET            8
#define CLKSTCTRL_CLKACTIVITY_MASK              (1 << CLKSTCTRL_CLKACTIVITY_OFFSET)
#define CLKSTCTRL_CLKACTIVITY_GATED             (0 << CLKSTCTRL_CLKACTIVITY_OFFSET)
#define CLKSTCTRL_CLKACTIVITY_RUNNING           (1 << CLKSTCTRL_CLKACTIVITY_OFFSET)

/*************************************************/
/* Control Module                               */
/*************************************************/
/*************************************************/
/* SYSCTRL_GENERAL_CORE */
/*************************************************/
#define DRA74X_CTRL_CORE_BASE                   0x4A002000
#define CTRL_CORE_STATUS                        0x134
    #define CTRL_STATUS_DEVICE_TYPE_MASK        0x000001C0
    #define CTRL_STATUS_DEVICE_TYPE_SHIFT       6
    #define CTRL_STATUS_DEVICE_TYPE_UNK         0x00
    #define CTRL_STATUS_DEVICE_TYPE_EMU         0x01
    #define CTRL_STATUS_DEVICE_TYPE_HS          0x02
    #define CTRL_STATUS_DEVICE_TYPE_GP          0x03

#define CTRL_CORE_SEC_ERR_STATUS_FUNC_1         0x148
#define CTRL_CORE_SEC_ERR_STATUS_DEBUG_1        0x150
#define CTRL_CORE_MPU_FORCEWRNP                 0x15c
#define CTRL_CORE_STD_FUSE_OPP_VDD_GPU_0        0x194
#define CTRL_CORE_STD_FUSE_OPP_VDD_GPU_1        0x198
#define CTRL_CORE_STD_FUSE_OPP_VDD_GPU_2        0x19c
#define CTRL_CORE_STD_FUSE_OPP_VDD_GPU_3        0x1a0
#define CTRL_CORE_STD_FUSE_OPP_VDD_GPU_4        0x1a4
#define CTRL_CORE_STD_FUSE_OPP_VDD_GPU_5        0x1a8
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_0        0x1ac
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_1        0x1b0
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_2        0x1b4
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_3        0x1b8
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_4        0x1bc
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_5        0x1c0
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_6        0x1c4
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_7        0x1c8
#define CTRL_CORE_STD_FUSE_OPP_VDD_CORE_0       0x1CC
#define CTRL_CORE_STD_FUSE_OPP_VDD_CORE_1       0x1D0
#define CTRL_CORE_STD_FUSE_OPP_VDD_CORE_2       0x1D4
#define CTRL_CORE_STD_FUSE_OPP_VDD_CORE_3       0x1D8
#define CTRL_CORE_STD_FUSE_OPP_VDD_CORE_4       0x1DC
#define CTRL_CORE_STD_FUSE_OPP_BGAP_GPU         0x1E0
#define CTRL_CORE_STD_FUSE_OPP_BGAP_MPU         0x1E4
#define CTRL_CORE_STD_FUSE_OPP_BGAP_CORE        0x1E8
#define CTRL_CORE_STD_FUSE_OPP_BGAP_MPU23       0x1EC
#define CTRL_CORE_STD_FUSE_MPK_0                0x220
#define CTRL_CORE_STD_FUSE_MPK_1                0x224
#define CTRL_CORE_STD_FUSE_MPK_2                0x228
#define CTRL_CORE_STD_FUSE_MPK_3                0x22C
#define CTRL_CORE_STD_FUSE_MPK_4                0x230
#define CTRL_CORE_STD_FUSE_MPK_5                0x234
#define CTRL_CORE_STD_FUSE_MPK_6                0x238
#define CTRL_CORE_STD_FUSE_MPK_7                0x23C
#define CTRL_CORE_STD_FUSE_OPP_VDD_GPU_LVT_0    0x240
#define CTRL_CORE_STD_FUSE_OPP_VDD_GPU_LVT_1    0x244
#define CTRL_CORE_STD_FUSE_OPP_VDD_GPU_LVT_2    0x248
#define CTRL_CORE_STD_FUSE_OPP_VDD_GPU_LVT_3    0x24C
#define CTRL_CORE_STD_FUSE_OPP_VDD_GPU_LVT_4    0x250
#define CTRL_CORE_STD_FUSE_OPP_VDD_GPU_LVT_5    0x254
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_LVT_0    0x258
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_LVT_1    0x25C
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_LVT_2    0x260
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_LVT_3    0x264
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_LVT_4    0x268
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_LVT_5    0x26C
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_LVT_6    0x270
#define CTRL_CORE_STD_FUSE_OPP_VDD_MPU_LVT_7    0x274
#define CTRL_CORE_CUST_FUSE_SWRV_0              0x2BC
#define CTRL_CORE_CUST_FUSE_SWRV_1              0x2C0
#define CTRL_CORE_CUST_FUSE_SWRV_2              0x2C4
#define CTRL_CORE_CUST_FUSE_SWRV_3              0x2C8
#define CTRL_CORE_CUST_FUSE_SWRV_4              0x2CC
#define CTRL_CORE_CUST_FUSE_SWRV_5              0x2D0
#define CTRL_CORE_CUST_FUSE_SWRV_6              0x2D4
#define CTRL_CORE_BREG_SELECTION                0x2E0
#define CTRL_CORE_DPLL_BCLK                     0x2E4
#define CTRL_CORE_DPLL_BADDR_BDATAW             0x2E8
#define CTRL_CORE_DPLL_BDATAR                   0x2EC
#define CTRL_CORE_TEMP_SENSOR_MPU               0x32C
#define CTRL_DEV_CONF                           0x300
#define CTRL_CORE_TEMP_SENSOR_GPU               0x330
#define CTRL_CORE_TEMP_SENSOR_CORE              0x334
#define CTRL_CORE_CORTEX_M4_MMUADDRTRANSLTR     0x358
#define CTRL_CORE_CORTEX_M4_MMUADDRLOGICTR      0x35C
#define CTRL_CORE_HWOBS_CONTROL                 0x360
#define CTRL_CORE_PCS1                          0x364
#define CTRL_CORE_PCS2                          0x368
#define CTRL_CORE_PCS_REVISION                  0x36C
#define CTRL_CORE_PHY_POWER_USB                 0x370
#define CTRL_CORE_PHY_POWER_SATA                0x374
    #define PWRCTL_CLK_FREQ_SHIFT               22
    #define PWRCTL_CLK_CMD_SHIFT                14
#define CTRL_CORE_BANDGAP_MASK_1                0x380
#define CTRL_CORE_BANDGAP_THRESHOLD_MPU         0x384
#define CTRL_CORE_BANDGAP_THRESHOLD_GPU         0x388
#define CTRL_CORE_BANDGAP_THRESHOLD_CORE        0x38C
#define CTRL_CORE_BANDGAP_TSHUT_MPU             0x390
#define CTRL_CORE_BANDGAP_TSHUT_GPU             0x394
#define CTRL_CORE_BANDGAP_TSHUT_CORE            0x398
#define CTRL_CORE_BANDGAP_CUMUL_DTEMP_MPU       0x39C
#define CTRL_CORE_BANDGAP_CUMUL_DTEMP_GPU       0x3A0
#define CTRL_CORE_BANDGAP_CUMUL_DTEMP_CORE      0x3A4
#define CTRL_CORE_BANDGAP_STATUS_1              0x3A8
#define CTRL_CORE_SATA_EXT_MODE                 0x3AC
    #define SATA_EXTENDED_MODE                  1
#define CTRL_CORE_DTEMP_MPU_0                   0x3C0
#define CTRL_CORE_DTEMP_MPU_1                   0x3C4
#define CTRL_CORE_DTEMP_MPU_2                   0x3C8
#define CTRL_CORE_DTEMP_MPU_3                   0x3CC
#define CTRL_CORE_DTEMP_MPU_4                   0x3D0
#define CTRL_CORE_DTEMP_GPU_0                   0x3D4
#define CTRL_CORE_DTEMP_GPU_1                   0x3D8
#define CTRL_CORE_DTEMP_GPU_2                   0x3DC
#define CTRL_CORE_DTEMP_GPU_3                   0x3E0
#define CTRL_CORE_DTEMP_GPU_4                   0x3E4
#define CTRL_CORE_DTEMP_CORE_0                  0x3E8
#define CTRL_CORE_DTEMP_CORE_1                  0x3EC
#define CTRL_CORE_DTEMP_CORE_2                  0x3F0
#define CTRL_CORE_DTEMP_CORE_3                  0x3F4
#define CTRL_CORE_DTEMP_CORE_4                  0x3F8
#define CTRL_CORE_SMA_SW_0                      0x3FC
#define CTRL_CORE_SEC_ERR_STATUS_FUNC_2         0x414
#define CTRL_CORE_SEC_ERR_STATUS_DEBUG_2        0x41C
#define CTRL_CORE_EMIF_INITIATOR_PRIORITY_1     0x420
#define CTRL_CORE_EMIF_INITIATOR_PRIORITY_2     0x424
#define CTRL_CORE_EMIF_INITIATOR_PRIORITY_3     0x428
#define CTRL_CORE_EMIF_INITIATOR_PRIORITY_4     0x42C
#define CTRL_CORE_EMIF_INITIATOR_PRIORITY_5     0x430
#define CTRL_CORE_EMIF_INITIATOR_PRIORITY_6     0x434
#define CTRL_CORE_L3_INITIATOR_PRESSURE_1       0x43C
#define CTRL_CORE_L3_INITIATOR_PRESSURE_2       0x440
#define CTRL_CORE_L3_INITIATOR_PRESSURE_4       0x448
#define CTRL_CORE_L3_INITIATOR_PRESSURE_5       0x44C
#define CTRL_CORE_L3_INITIATOR_PRESSURE_6       0x450
#define CTRL_CORE_STD_FUSE_OPP_VDD_IVA_0        0x458
#define CTRL_CORE_STD_FUSE_OPP_VDD_IVA_1        0x45C
#define CTRL_CORE_STD_FUSE_OPP_VDD_IVA_2        0x460
#define CTRL_CORE_STD_FUSE_OPP_VDD_IVA_3        0x464
#define CTRL_CORE_STD_FUSE_OPP_VDD_IVA_4        0x468
#define CTRL_CORE_LDOVBB_DSPEVE_VOLTAGE_CTRL    0x46C
#define CTRL_CORE_LDOVBB_IVA_VOLTAGE_CTRL       0x470
#define CTRL_CORE_SMA_SW_1                      0x534
#define CTRL_CORE_DSS_PLL_CONTROL               0x538
#define CTRL_CORE_MMR_LOCK_1                    0x540
#define CTRL_CORE_MMR_LOCK_2                    0x544
#define CTRL_CORE_MMR_LOCK_3                    0x548
#define CTRL_CORE_MMR_LOCK_4                    0x54C
#define CTRL_CORE_MMR_LOCK_5                    0x550
    #define MMR1_UNLOCK_VALUE                   0x2FF1AC2B
    #define MMR1_LOCK_VALUE                     0x1A1C8144
    #define MMR2_UNLOCK_VALUE                   0xF757FDC0
    #define MMR2_LOCK_VALUE                     0xFDF45530
    #define MMR3_UNLOCK_VALUE                   0xE2BC3A6D
    #define MMR3_LOCK_VALUE                     0x1AE6E320
    #define MMR4_UNLOCK_VALUE                   0x1EBF131D
    #define MMR4_LOCK_VALUE                     0x2FFA927C
    #define MMR5_UNLOCK_VALUE                   0x6F361E05
    #define MMR5_LOCK_VALUE                     0x143F832C
#define CTRL_CORE_CONTROL_IO_1                  0x554
#define CTRL_CORE_CONTROL_IO_2                  0x558
#define CTRL_CORE_CONTROL_DSP1_RST_VECT         0x55C
#define CTRL_CORE_CONTROL_DSP2_RST_VECT         0x560
#define CTRL_CORE_STD_FUSE_OPP_BGAP_DSPEVE      0x564
#define CTRL_CORE_STD_FUSE_OPP_BGAP_IVA         0x568
#define CTRL_CORE_LDOSRAM_DSPEVE_VOLTAGE_CTRL   0x56C
#define CTRL_CORE_LDOSRAM_IVA_VOLTAGE_CTRL      0x570
#define CTRL_CORE_TEMP_SENSOR_DSPEVE            0x574
#define CTRL_CORE_TEMP_SENSOR_IVA               0x578
#define CTRL_CORE_BANDGAP_MASK_2                0x57C
#define CTRL_CORE_BANDGAP_THRESHOLD_DSPEVE      0x580
#define CTRL_CORE_BANDGAP_THRESHOLD_IVA         0x584
#define CTRL_CORE_BANDGAP_TSHUT_DSPEVE          0x588
#define CTRL_CORE_BANDGAP_TSHUT_IVA             0x58C
#define CTRL_CORE_BANDGAP_CUMUL_DTEMP_DSPEVE    0x590
#define CTRL_CORE_BANDGAP_CUMUL_DTEMP_IVA       0x594
#define CTRL_CORE_BANDGAP_STATUS_2              0x598
#define CTRL_CORE_DTEMP_DSPEVE_0                0x59C
#define CTRL_CORE_DTEMP_DSPEVE_1                0x5A0
#define CTRL_CORE_DTEMP_DSPEVE_2                0x5A4
#define CTRL_CORE_DTEMP_DSPEVE_3                0x5A8
#define CTRL_CORE_DTEMP_DSPEVE_4                0x5AC
#define CTRL_CORE_DTEMP_IVA_0                   0x5B0
#define CTRL_CORE_DTEMP_IVA_1                   0x5B4
#define CTRL_CORE_DTEMP_IVA_2                   0x5B8
#define CTRL_CORE_DTEMP_IVA_3                   0x5BC
#define CTRL_CORE_DTEMP_IVA_4                   0x5C0
#define CTRL_CORE_STD_FUSE_OPP_VMIN_IVA_0       0x5C4
#define CTRL_CORE_STD_FUSE_OPP_VMIN_IVA_1       0x5C8
#define CTRL_CORE_STD_FUSE_OPP_VMIN_IVA_2       0x5CC
    #define EFUSE_REGBITS                       16
#define CTRL_CORE_STD_FUSE_OPP_VMIN_IVA_3       0x5D0
#define CTRL_CORE_STD_FUSE_OPP_VMIN_IVA_4       0x5D4
#define CTRL_CORE_STD_FUSE_OPP_VMIN_DSPEVE_0    0x5D8
#define CTRL_CORE_STD_FUSE_OPP_VMIN_DSPEVE_1    0x5DC
#define CTRL_CORE_STD_FUSE_OPP_VMIN_DSPEVE_2    0x5E0
#define CTRL_CORE_STD_FUSE_OPP_VMIN_DSPEVE_3    0x5E4
#define CTRL_CORE_STD_FUSE_OPP_VMIN_DSPEVE_4    0x5E8
#define CTRL_CORE_STD_FUSE_OPP_VMIN_CORE_0      0x5EC
#define CTRL_CORE_STD_FUSE_OPP_VMIN_CORE_1      0x5F0
#define CTRL_CORE_STD_FUSE_OPP_VMIN_CORE_2      0x5F4
#define CTRL_CORE_STD_FUSE_OPP_VMIN_CORE_3      0x5F8
#define CTRL_CORE_STD_FUSE_OPP_VMIN_CORE_4      0x5FC
#define CTRL_CORE_LDOSRAM_CORE_2_VOLTAGE_CTRL   0x680
#define CTRL_CORE_LDOSRAM_CORE_3_VOLTAGE_CTRL   0x684
#define CTRL_CORE_NMI_DESTINATION_1             0x68C
#define CTRL_CORE_NMI_DESTINATION_2             0x690
#define CTRL_CORE_IP_PRESSURE                   0x698
#define CTRL_CORE_STD_FUSE_OPP_VDD_DSPEVE_0     0x6A0
#define CTRL_CORE_STD_FUSE_OPP_VDD_DSPEVE_1     0x6A4
#define CTRL_CORE_STD_FUSE_OPP_VDD_DSPEVE_2     0x6A8
#define CTRL_CORE_STD_FUSE_OPP_VDD_DSPEVE_3     0x6AC
#define CTRL_CORE_STD_FUSE_OPP_VDD_DSPEVE_4     0x6B0
#define CTRL_CORE_CUST_FUSE_SWRV_7              0x6B4
#define CTRL_CORE_STD_FUSE_CALIBRATION_OVERRIDE_VALUE_0 0x6B8
#define CTRL_CORE_STD_FUSE_CALIBRATION_OVERRIDE_VALUE_1 0x6BC
#define CTRL_CORE_PCIE_POWER_STATE              0x6C0
#define CTRL_CORE_BOOTSTRAP                     0x6C4
#define CTRL_CORE_MLB_SIG_IO_CTRL               0x6C8
#define CTRL_CORE_MLB_DAT_IO_CTRL               0x6CC
#define CTRL_CORE_MLB_CLK_BG_CTRL               0x6D0
#define CTRL_CORE_EVE1_IRQ_0_1                  0x7A0
#define CTRL_CORE_EVE1_IRQ_2_3                  0x7A4
#define CTRL_CORE_EVE1_IRQ_4_5                  0x7A8
#define CTRL_CORE_EVE1_IRQ_6_7                  0x7AC
#define CTRL_CORE_EVE2_IRQ_0_1                  0x7B0
#define CTRL_CORE_EVE2_IRQ_2_3                  0x7B4
#define CTRL_CORE_EVE2_IRQ_4_5                  0x7B8
#define CTRL_CORE_EVE2_IRQ_6_7                  0x7BC
#define CTRL_CORE_IPU1_IRQ_23_24                0x7E0
#define CTRL_CORE_IPU1_IRQ_25_26                0x7E4
#define CTRL_CORE_IPU1_IRQ_27_28                0x7E8
#define CTRL_CORE_IPU1_IRQ_29_30                0x7EC
#define CTRL_CORE_IPU1_IRQ_31_32                0x7F0
#define CTRL_CORE_IPU1_IRQ_33_34                0x7F4
#define CTRL_CORE_IPU1_IRQ_35_36                0x7F8
#define CTRL_CORE_IPU1_IRQ_37_38                0x7FC
#define CTRL_CORE_IPU1_IRQ_39_40                0x800
#define CTRL_CORE_IPU1_IRQ_41_42                0x804
#define CTRL_CORE_MPU_IRQ_4_5                   0xA48
#define CTRL_CORE_DMA_SYSTEM_DREQ_0_1           0xB78
#define CTRL_CORE_DMA_EDMA_DREQ_0_1             0xC78
#define CTRL_SMART1IO_PADCONF_0                 0xDA8
    #define DS_120_OHM                          0x0
    #define DS_60_OHM                           0x1
    #define DS_45_OHM                           0x2
    #define DS_30_OHM                           0x3
    #define DS_MASK                             0x3
#define CTRL_SMART1IO_PADCONF_1                 0xDAC
#define CTRL_SMART1IO_PADCONF_2                 0xDB0
#define CTRL_SMART2IO_PADCONF_0                 0xDB4
#define CTRL_SMART2IO_PADCONF_1                 0xDB8
#define CTRL_SMART2IO_PADCONF_2                 0xDBC
    #define SC_SLOW                             0x0
    #define SC_MEDIUM                           0x1
    #define SC_FAST                             0x2
    #define SC_NA                               0x3
    #define SC_MASK                             0x3
#define CTRL_SMART3IO_PADCONF_1                 0xDC4
    #define USB_I_MASK                          0x7
#define CTRL_CORE_PBIAS                         0xE00
#define CTRL_DDR3CH1_0                          0xE30
#define CTRL_DDR3CH2_0                          0xE34
#define CTRL_DDRCH1_0                           0xE38
#define CTRL_DDRCH1_1                           0xE3C
#define CTRL_DDRCH2_0                           0xE40
#define CTRL_DDRCH2_1                           0xE44
#define CTRL_LPDDR2CH1_0                        0xE48
#define CTRL_LPDDR2CH1_1                        0xE4C
#define CTRL_DDRIO_0                            0xE50
#define CTRL_DDRIO_1                            0xE54
#define CTRL_DDRIO_2                            0xE58
#define CTRL_CORE_SRCOMP_NORTH_SIDE             0xE74
    #define SRCOMP_NORTH_SIDE_USB2PHY_PD        28
#define CTRL_DDR_CONTROL_EXT_0                  0xE88
#define CTRL_CORE_STD_FUSE_OPP_VMIN_GPU_2       0x1B08
#define CTRL_CORE_STD_FUSE_OPP_VMIN_GPU_3       0x1B0c
#define CTRL_CORE_STD_FUSE_OPP_VMIN_GPU_4       0x1B10
#define CTRL_CORE_STD_FUSE_OPP_VMIN_MPU_2       0x1B20
#define CTRL_CORE_STD_FUSE_OPP_VMIN_MPU_3       0x1B24
#define CTRL_CORE_STD_FUSE_OPP_VMIN_MPU_4       0x1B28
    #define FUSE_VSET_SHIFT                     20
    #define FUSE_VSET_MASK                      (0x1F << FUSE_VSET_SHIFT)
    #define FUSE_ABBEN                          (1 << 25)
#define CTRL_CORE_PHY_POWER_PCIESS1             0x1c40
#define CTRL_CORE_PHY_POWER_PCIESS2             0x1c44

#define DRA74X_CTRL_CORE_CONTROL_PBIAS              0x4A002E00
#define CTRL_CORE_CONTROL_PBIAS_SDCARD_BIAS_3V      (1 << 21)
#define CTRL_CORE_CONTROL_PBIAS_SDCARD_IO_PWRDNZ    (1 << 26)
#define CTRL_CORE_CONTROL_PBIAS_SDCARD_BIAS_PWRDNZ  (1 << 27)

/*************************************************/
/* OCP_SOCKET_PRM                               */
/*************************************************/
#define DRA74X_OCP_SOCKET_PRM_BASE                  0x4AE06000
#define PRM_IRQSTATUS_MPU                           0x010
    #define ABB_IVA_DONE_ST                         (1 << 30)
    #define ABB_DSPEVE_DONE_ST                      (1 << 29)
    #define ABB_GPU_DONE_ST                         (1 << 28)
#define PRM_IRQSTATUS_MPU_2                         0x014
    #define ABB_MPU_DONE_ST                         (1 << 7)

/*************************************************/
/* DEVICE_PRM                                   */
/*************************************************/
#define DRA74X_DEVICE_PRM_BASE                      0x4AE07D00
#define PRM_ABBLDO_MPU_SETUP                        0xDC
#define PRM_ABBLDO_MPU_CTRL                         0xE0
#define PRM_ABBLDO_GPU_SETUP                        0xE4
#define PRM_ABBLDO_GPU_CTRL                         0xE8
#define PRM_ABBLDO_DSPEVE_CTRL                      0x120
#define PRM_ABBLDO_IVA_CTRL                         0x124
#define PRM_ABBLDO_DSPEVE_SETUP                     0x130
#define PRM_ABBLDO_IVA_SETUP                        0x134
    #define OMAP_ABB_NOMINAL_OPP                    0
    #define OMAP_ABB_FAST_OPP                       1
    #define OMAP_ABB_SLOW_OPP                       3
    #define OMAP_ABB_CONTROL_FAST_OPP_SEL_MASK      (0x1 << 0)
    #define OMAP_ABB_CONTROL_SLOW_OPP_SEL_MASK      (0x1 << 1)
    #define OMAP_ABB_CONTROL_OPP_CHANGE_MASK        (0x1 << 2)
    #define OMAP_ABB_CONTROL_SR2_IN_TRANSITION_MASK (0x1 << 6)
    #define OMAP_ABB_SETUP_SR2EN_MASK               (0x1 << 0)
    #define OMAP_ABB_SETUP_ACTIVE_FBB_SEL_MASK      (0x1 << 2)
    #define OMAP_ABB_SETUP_ACTIVE_RBB_SEL_MASK      (0x1 << 1)
    #define OMAP_ABB_SETUP_SR2_WTCNT_VALUE_MASK     (0xff << 8)
    #define OMAP_ABB_SETUP_SR2_WTCNT_VALUE_SHIFT    (8)
    #define OMAP_ABB_SETTLING_TIME                  50
    #define OMAP_ABB_CLOCK_CYCLES                   16


/*************************************************/
/* CTRL_MODULE_WKUP                             */
/*************************************************/
#define DRA74X_CTRL_WKUP_BASE                   0x4AE0C000
#define CTRL_WKUP_PORT_EMIF1_SDRAM_CONFIG       0x110
#define CTRL_WKUP_PORT_EMIF1_LPDDR2_NVM_CONFIG  0x114
#define CTRL_WKUP_PORT_EMIF2_SDRAM_CONFIG       0x118
#define CTRL_WKUP_EMIF1_SDRAM_CONFIG_EXT        0x144
#define CTRL_WKUP_EMIF2_SDRAM_CONFIG_EXT        0x148
#define CTRL_WKUP_EMIF1_SDRAM_CONFIG_EXT_1      0x14C
#define CTRL_WKUP_EMIF2_SDRAM_CONFIG_EXT_2      0x150
#define CTRL_WKUP_LDOVBB_GPU_VOLTAGE_CTRL       0x154
#define CTRL_WKUP_LDOVBB_MPU_VOLTAGE_CTRL       0x158
    #define FBB_MUX_CTRL                        (1 << 10)
    #define VSET_OUT_MASK                       0x1F
    #define VSET_OUT_SHIFT                      0
#define CTRL_WKUP_ID_CODE                       0x204
    #define DRA7xx_ID_CODE_ES1_0                0x0B99002F
    #define DRA7xx_ID_CODE_ES1_1                0x1B99002F
#define CTRL_WKUP_PADCONF_MODE                  0x5A0
#define CTRL_WKUP_XTAL_OSCILLATOR               0x5A4
#define CTRL_WKUP_I2C_2                         0x5A8
#define CTRL_WKUP_CKOBUFFER                     0x5AC
#define CTRL_WKUP_CONTROL_SPARE_RW              0x5B0
#define CTRL_WKUP_CONTROL_SPARE_R               0x5B4
#define CTRL_WKUP_CONTROL_SPARE_R_C0            0x5B8
#define CTRL_WKUP_SRCOMP_EAST_SIDE_WKUP         0x5BC
#define CTRL_WKUP_EFUSE_1                       0x5C0
#define CTRL_WKUP_EFUSE_2                       0x5C4
#define CTRL_WKUP_EFUSE_3                       0x5C8
#define CTRL_WKUP_EFUSE_4                       0x5CC
#define CTRL_WKUP_EFUSE_13                      0x5F0

/*************************************************/
/* register offset for CTRL_CORE_PAD registers  */
/*************************************************/
#define DRA74X_CONTROL_PADCONF_CORE_BASE        0x4A003400
#define GPMC_AD0        0x000
#define GPMC_AD1        0x004
#define GPMC_AD2        0x008
#define GPMC_AD3        0x00C
#define GPMC_AD4        0x010
#define GPMC_AD5        0x014
#define GPMC_AD6        0x018
#define GPMC_AD7        0x01C
#define GPMC_AD8        0x020
#define GPMC_AD9        0x024
#define GPMC_AD10       0x028
#define GPMC_AD11       0x02C
#define GPMC_AD12       0x030
#define GPMC_AD13       0x034
#define GPMC_AD14       0x038
#define GPMC_AD15       0x03C
#define GPMC_A0         0x040
#define GPMC_A1         0x044
#define GPMC_A2         0x048
#define GPMC_A3         0x04C
#define GPMC_A4         0x050
#define GPMC_A5         0x054
#define GPMC_A6         0x058
#define GPMC_A7         0x05C
#define GPMC_A8         0x060
#define GPMC_A9         0x064
#define GPMC_A10        0x068
#define GPMC_A11        0x06C
#define GPMC_A12        0x070
#define GPMC_A13        0x074
#define GPMC_A14        0x078
#define GPMC_A15        0x07C
#define GPMC_A16        0x080
#define GPMC_A17        0x084
#define GPMC_A18        0x088
#define GPMC_A19        0x08C
#define GPMC_A20        0x090
#define GPMC_A21        0x094
#define GPMC_A22        0x098
#define GPMC_A23        0x09C
#define GPMC_A24        0x0A0
#define GPMC_A25        0x0A4
#define GPMC_A26        0x0A8
#define GPMC_A27        0x0AC
#define GPMC_CS1        0x0B0
#define GPMC_CS0        0x0B4
#define GPMC_CS2        0x0B8
#define GPMC_CS3        0x0BC
#define GPMC_CLK        0x0C0
#define GPMC_ADVN_ALE   0x0C4
#define GPMC_OEN_REN    0x0C8
#define GPMC_WEN        0x0CC
#define GPMC_BEN0       0x0D0
#define GPMC_BEN1       0x0D4
#define GPMC_WAIT0      0x0D8
#define VIN1A_CLK0      0x0DC
#define VIN1B_CLK1      0x0E0
#define VIN1A_DE0       0x0E4
#define VIN1A_FLD0      0x0E8
#define VIN1A_HSYNC0    0x0EC
#define VIN1A_VSYNC0    0x0F0
#define VIN1A_D0        0x0F4
#define VIN1A_D1        0x0F8
#define VIN1A_D2        0x0FC
#define VIN1A_D3        0x100
#define VIN1A_D4        0x104
#define VIN1A_D5        0x108
#define VIN1A_D6        0x10C
#define VIN1A_D7        0x110
#define VIN1A_D8        0x114
#define VIN1A_D9        0x118
#define VIN1A_D10       0x11C
#define VIN1A_D11       0x120
#define VIN1A_D12       0x124
#define VIN1A_D13       0x128
#define VIN1A_D14       0x12C
#define VIN1A_D15       0x130
#define VIN1A_D16       0x134
#define VIN1A_D17       0x138
#define VIN1A_D18       0x13C
#define VIN1A_D19       0x140
#define VIN1A_D20       0x144
#define VIN1A_D21       0x148
#define VIN1A_D22       0x14C
#define VIN1A_D23       0x150
#define VIN2A_CLK0      0x154
#define VIN2A_DE0       0x158
#define VIN2A_FLD0      0x15C
#define VIN2A_HSYNC0    0x160
#define VIN2A_VSYNC0    0x164
#define VIN2A_D0        0x168
#define VIN2A_D1        0x16C
#define VIN2A_D2        0x170
#define VIN2A_D3        0x174
#define VIN2A_D4        0x178
#define VIN2A_D5        0x17C
#define VIN2A_D6        0x180
#define VIN2A_D7        0x184
#define VIN2A_D8        0x188
#define VIN2A_D9        0x18C
#define VIN2A_D10       0x190
#define VIN2A_D11       0x194
#define VIN2A_D12       0x198
#define VIN2A_D13       0x19C
#define VIN2A_D14       0x1A0
#define VIN2A_D15       0x1A4
#define VIN2A_D16       0x1A8
#define VIN2A_D17       0x1AC
#define VIN2A_D18       0x1B0
#define VIN2A_D19       0x1B4
#define VIN2A_D20       0x1B8
#define VIN2A_D21       0x1BC
#define VIN2A_D22       0x1C0
#define VIN2A_D23       0x1C4
#define VOUT1_CLK       0x1C8
#define VOUT1_DE        0x1CC
#define VOUT1_FLD       0x1D0
#define VOUT1_HSYNC     0x1D4
#define VOUT1_VSYNC     0x1D8
#define VOUT1_D0        0x1DC
#define VOUT1_D1        0x1E0
#define VOUT1_D2        0x1E4
#define VOUT1_D3        0x1E8
#define VOUT1_D4        0x1EC
#define VOUT1_D5        0x1F0
#define VOUT1_D6        0x1F4
#define VOUT1_D7        0x1F8
#define VOUT1_D8        0x1FC
#define VOUT1_D9        0x200
#define VOUT1_D10       0x204
#define VOUT1_D11       0x208
#define VOUT1_D12       0x20C
#define VOUT1_D13       0x210
#define VOUT1_D14       0x214
#define VOUT1_D15       0x218
#define VOUT1_D16       0x21C
#define VOUT1_D17       0x220
#define VOUT1_D18       0x224
#define VOUT1_D19       0x228
#define VOUT1_D20       0x22C
#define VOUT1_D21       0x230
#define VOUT1_D22       0x234
#define VOUT1_D23       0x238
#define MDIO_MCLK       0x23C
#define MDIO_D          0x240
#define RMII_MHZ_50_CLK 0x244
#define UART3_RXD       0x248
#define UART3_TXD       0x24C
#define RGMII0_TXC      0x250
#define RGMII0_TXCTL    0x254
#define RGMII0_TXD3     0x258
#define RGMII0_TXD2     0x25C
#define RGMII0_TXD1     0x260
#define RGMII0_TXD0     0x264
#define RGMII0_RXC      0x268
#define RGMII0_RXCTL    0x26C
#define RGMII0_RXD3     0x270
#define RGMII0_RXD2     0x274
#define RGMII0_RXD1     0x278
#define RGMII0_RXD0     0x27C
#define USB1_DRVVBUS    0x280
#define USB2_DRVVBUS    0x284
#define GPIO6_14        0x288
#define GPIO6_15        0x28C
#define GPIO6_16        0x290
#define XREF_CLK0       0x294
#define XREF_CLK1       0x298
#define XREF_CLK2       0x29C
#define XREF_CLK3       0x2A0
#define MCASP1_ACLKX    0x2A4
#define MCASP1_FSX      0x2A8
#define MCASP1_ACLKR    0x2AC
#define MCASP1_FSR      0x2B0
#define MCASP1_AXR0     0x2B4
#define MCASP1_AXR1     0x2B8
#define MCASP1_AXR2     0x2BC
#define MCASP1_AXR3     0x2C0
#define MCASP1_AXR4     0x2C4
#define MCASP1_AXR5     0x2C8
#define MCASP1_AXR6     0x2CC
#define MCASP1_AXR7     0x2D0
#define MCASP1_AXR8     0x2D4
#define MCASP1_AXR9     0x2D8
#define MCASP1_AXR10    0x2DC
#define MCASP1_AXR11    0x2E0
#define MCASP1_AXR12    0x2E4
#define MCASP1_AXR13    0x2E8
#define MCASP1_AXR14    0x2EC
#define MCASP1_AXR15    0x2F0
#define MCASP2_ACLKX    0x2F4
#define MCASP2_FSX      0x2F8
#define MCASP2_ACLKR    0x2FC
#define MCASP2_FSR      0x300
#define MCASP2_AXR0     0x304
#define MCASP2_AXR1     0x308
#define MCASP2_AXR2     0x30C
#define MCASP2_AXR3     0x310
#define MCASP2_AXR4     0x314
#define MCASP2_AXR5     0x318
#define MCASP2_AXR6     0x31C
#define MCASP2_AXR7     0x320
#define MCASP3_ACLKX    0x324
#define MCASP3_FSX      0x328
#define MCASP3_AXR0     0x32C
#define MCASP3_AXR1     0x330
#define MCASP4_ACLKX    0x334
#define MCASP4_FSX      0x338
#define MCASP4_AXR0     0x33C
#define MCASP4_AXR1     0x340
#define MCASP5_ACLKX    0x344
#define MCASP5_FSX      0x348
#define MCASP5_AXR0     0x34C
#define MCASP5_AXR1     0x350
#define MMC1_CLK        0x354
#define MMC1_CMD        0x358
#define MMC1_DAT0       0x35C
#define MMC1_DAT1       0x360
#define MMC1_DAT2       0x364
#define MMC1_DAT3       0x368
#define MMC1_SDCD       0x36C
#define MMC1_SDWP       0x370
#define GPIO6_10        0x374
#define GPIO6_11        0x378
#define MMC3_CLK        0x37C
#define MMC3_CMD        0x380
#define MMC3_DAT0       0x384
#define MMC3_DAT1       0x388
#define MMC3_DAT2       0x38C
#define MMC3_DAT3       0x390
#define MMC3_DAT4       0x394
#define MMC3_DAT5       0x398
#define MMC3_DAT6       0x39C
#define MMC3_DAT7       0x3A0
#define SPI1_SCLK       0x3A4
#define SPI1_D1         0x3A8
#define SPI1_D0         0x3AC
#define SPI1_CS0        0x3B0
#define SPI1_CS1        0x3B4
#define SPI1_CS2        0x3B8
#define SPI1_CS3        0x3BC
#define SPI2_SCLK       0x3C0
#define SPI2_D1         0x3C4
#define SPI2_D0         0x3C8
#define SPI2_CS0        0x3CC
#define DCAN1_TX        0x3D0
#define DCAN1_RX        0x3D4
#define DCAN2_TX        0x3D8
#define DCAN2_RX        0x3DC
#define UART1_RXD       0x3E0
#define UART1_TXD       0x3E4
#define UART1_CTSN      0x3E8
#define UART1_RTSN      0x3EC
#define UART2_RXD       0x3F0
#define UART2_TXD       0x3F4
#define UART2_CTSN      0x3F8
#define UART2_RTSN      0x3FC
#define I2C1_SDA        0x400
#define I2C1_SCL        0x404
#define I2C2_SDA        0x408
#define I2C2_SCL        0x40C
#define I2C3_SDA        0x410
#define I2C3_SCL        0x414
#define WAKEUP0         0x418
#define WAKEUP1         0x41C
#define WAKEUP2         0x420
#define WAKEUP3         0x424
#define ON_OFF          0x428
#define RTC_PORZ        0x42C
#define TMS             0x430
#define TDI             0x434
#define TDO             0x438
#define TCLK            0x43C
#define TRSTN           0x440
#define RTCK            0x444
#define EMU0            0x448
#define EMU1            0x44C
#define EMU2            0x450
#define EMU3            0x454
#define EMU4            0x458
#define RESETN          0x45C
#define NMIN            0x460
#define RSTOUTN         0x464

/*************************************************
 * EMIF register sets
*************************************************/
#define DRA74X_EMIF1_BASE                       0x4C000000
#define DRA74X_EMIF2_BASE                       0x4D000000

#define EMIF_EMIF_MOD_ID_REV                    0x00        // EMIF Module ID and Revision Register
#define EMIF_STATUS                             0x04        // SDRAM Status Register
#define EMIF_SDRAM_CONFIG                       0x08        // SDRAM Configuration Register
#define EMIF_SDRAM_CONFIG_2                     0x0C        // SDRAM Configuration 2 Register
#define EMIF_SDRAM_REF_CTRL                     0x10        // SDRAM Refresh Control Register
#define EMIF_SDRAM_REF_CTRL_SHDW                0x14        // SDRAM Refresh Control Shadow Register
#define EMIF_SDRAM_TIM_1                        0x18        // SDRAM Timing 1 Register
#define EMIF_SDRAM_TIM_1_SHDW                   0x1C        // SDRAM Timing 1 Shadow Register
#define EMIF_SDRAM_TIM_2                        0x20        // SDRAM Timing 2 Register
#define EMIF_SDRAM_TIM_2_SHDW                   0x24        // SDRAM Timing 2 Shadow Register
#define EMIF_SDRAM_TIM_3                        0x28        // SDRAM Timing 3 Register
#define EMIF_SDRAM_TIM_3_SHDW                   0x2C        // SDRAM Timing 3 Shadow Register
#define EMIF_LPDDR2_NVM_TIM                     0x30
#define EMIF_LPDDR2_NVM_TIM_SHDW                0x34
#define EMIF_PWR_MGMT_CTRL                      0x38        // Power Management Control Register
#define EMIF_PWR_MGMT_CTRL_SHDW                 0x3C        // Power Management Control Shadow Regist

#define EMIF_LPDDR2_MODE_REG_DATA               0x40        // Mode Reg Data Register
#define EMIF_LPDDR2_MODE_REG_CFG                0x50        // Mode Reg Configuration Register
#define EMIF_OCP_CONFIG                         0x54        // OCP Configuration Register
#define EMIF_OCP_CFG_VAL_1                      0x58        // OCP Configuration Value 1 Register
#define EMIF_OCP_CFG_VAL_2                      0x5C        // OCP Configuration Value 2 Register
#define EMIF_IODFT_TLGC                         0x60        // IODFT Test Logic Global Control Register
#define EMIF_IODFT_CTRL_MISR_RSLT               0x64        // IODFT Test Logic Control MISR Result Register
#define EMIF_IODFT_ADDR_MISR_RSLT               0x68        // IODFT Test Logic Address MISR Result Register
#define EMIF_IODFT_DATA_MISR_RSLT_1             0x6C        // IODFT Test Logic Data MISR Result 1 Register
#define EMIF_IODFT_DATA_MISR_RSLT_2             0x70        // IODFT Test Logic Data MISR Result 2 Register
#define EMIF_IODFT_DATA_MISR_RSLT_3             0x74        // IODFT Test Logic Data MISR Result 3 Register
#define EMIF_PERF_CNT_1                         0x80        // Performance Counter 1 Register
#define EMIF_PERF_CNT_2                         0x84        // Performance Counter 2 Register
#define EMIF_PERF_CNT_CFG                       0x88        // Performance Counter Configuration Register
#define EMIF_PERF_CNT_SEL                       0x8C        // Performance Counter Master Region Select Register
#define EMIF_PERF_CNT_TIM                       0x90        // Performance Counter Time Register
#define EMIF_DLL_CALIB_CTRL                     0x98        // DLL Calibration Control Register
#define EMIF_DLL_CALIB_CTRL_SHDW                0x9C        // DLL Calibration Control Shadow Register
#define EMIF_IRQ_EOI                            0xA0        // End of Interrupt Register
#define EMIF_IRQSTATUS_RAW_SYS                  0xA4        // System OCP Interrupt Raw Status Register
#define EMIF_IRQSTATUS_SYS                      0xAC        // System OCP Interrupt Status Register
#define EMIF_IRQENABLE_SET_SYS                  0xB4        // System OCP Interrupt Enable Set Register
#define EMIF_IRQENABLE_CLR_SYS                  0xBC        // System OCP Interrupt Enable Clear Register
#define EMIF_ZQ_CONFIG                          0xC8        // ZQ Config Register
#define EMIF_TEMP_ALERT_CONFIG                  0xCC        // Temperature Alert Config
#define EMIF_OCP_ERR_LOG                        0xD0        // OCP Error Log Register
#define EMIF_RD_WR_LVL_RMP_WIN                  0xD4
#define EMIF_RD_WR_LVL_RMP_CTL                  0xD8
#define EMIF_RD_WR_LVL_CTL                      0xDC
#define EMIF_DDR_PHY_CTRL_1                     0xE4        // DDR PHY Control 1 Register
#define EMIF_DDR_PHY_CTRL_1_SHDW                0xE8        // DDR PHY Control 1 Shadow Register
#define EMIF_DDR_PHY_CTRL_2                     0xEC        // DDR PHY Control 2 Register
#define EMIF_PRI_COS_MAP                        0x100
#define EMIF_CONNID_COS_1                       0x104
#define EMIF_CONNID_COS_2                       0x108
#define EMIF_RD_WR_EXEC_THRESH                  0x120
#define EMIF_COS_CONFIG                         0x124
#define EMIF_DDR_EXT_PHY_CTRL_X(x)              (0x200 + 8*(x))
#define EMIF_DDR_EXT_PHY_CTRL_X_SHDW(x)         (0x204 + 8*(x))

#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/arm/dra74x.h $ $Rev: 758169 $")
#endif
