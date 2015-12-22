/*
 * $QNXLicenseC:
 * Copyright 2007, QNX Software Systems.
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
 * Xilinx Z7000 processor with Dual ARMv7 Cortex-A9 core
 */

#ifndef	__ARM_XZYNQ_H_INCLUDED
#define	__ARM_XZYNQ_H_INCLUDED

#include <arm/xzynq_irq.h>

/* -------------------------------------------------------------------------
 * General Information
 * -------------------------------------------------------------------------
 */
#define XZYNQ_PS_CLOCK		33333333 /* 33.33.. Mhz */

/* -------------------------------------------------------------------------
 * Devices
 * -------------------------------------------------------------------------
 */
#define XZYNQ_HWI_UART						"uart"
#define XZYNQ_HWI_WDT						"wdt"
#define XZYNQ_HWI_DMA						"dma"

/* -------------------------------------------------------------------------
 * SDRAM
 * -------------------------------------------------------------------------
 */
#define XZYNQ_SDRAM_64_BIT_BASE                 0x00100000

/* -------------------------------------
 * EMAC
 * -------------------------------------
 */

#define XZYNQ_EMAC0_BASE		0xE000B000
#define XZYNQ_EMAC1_BASE		0xE000C000
#define XZYNQ_EMAC_SIZE			0x1000

/* -------------------------------------
 * DMA
 * -------------------------------------
 */

#define XZYNQ_DMA_NS_BASE_ADDR		0xF8004000
#define XZYNQ_DMA_S_BASE_ADDR		0xF8003000
#define XZYNQ_DMA_REG_SIZE			0x1000

/* -------------------------------------
 * I2C
 * -------------------------------------
 */

#define XZYNQ_XIICPS1_BASE_ADDR		0xE0004000
#define XZYNQ_XIICPS2_BASE_ADDR		0xE0005000
#define XZYNQ_XIICPS_REG_SIZE		0x2C

/* -------------------------------------
 * GPIO
 * -------------------------------------
 */

#define XZYNQ_GPIO_BASE		0xE000A000
#define XZYNQ_GPIO_SIZE		0x300

/** @name Register offsets for the GPIO. Each register is 32 bits.
 *  @{
 */
#define XZYNQ_GPIOPS_DATA_LSW_OFFSET  0x000  /* Mask and Data Register LSW, WO */
#define XZYNQ_GPIOPS_DATA_MSW_OFFSET  0x004  /* Mask and Data Register MSW, WO */
#define XZYNQ_GPIOPS_DATA_OFFSET	 0x040  /* Data Register, RW */
#define XZYNQ_GPIOPS_DIRM_OFFSET	 0x204  /* Direction Mode Register, RW */
#define XZYNQ_GPIOPS_OUTEN_OFFSET	 0x208  /* Output Enable Register, RW */
#define XZYNQ_GPIOPS_INTMASK_OFFSET	 0x20C  /* Interrupt Mask Register, RO */
#define XZYNQ_GPIOPS_INTEN_OFFSET	 0x210  /* Interrupt Enable Register, WO */
#define XZYNQ_GPIOPS_INTDIS_OFFSET	 0x214  /* Interrupt Disable Register, WO*/
#define XZYNQ_GPIOPS_INTSTS_OFFSET	 0x218  /* Interrupt Status Register, RO */
#define XZYNQ_GPIOPS_INTTYPE_OFFSET	 0x21C  /* Interrupt Type Register, RW */
#define XZYNQ_GPIOPS_INTPOL_OFFSET	 0x220  /* Interrupt Polarity Register, RW */
#define XZYNQ_GPIOPS_INTANY_OFFSET	 0x224  /* Interrupt On Any Register, RW */
/* @} */


/* -------------------------------------------------------------------------
 * ARM / SMP
 * -------------------------------------------------------------------------
 */
#define XZYNQ_CPU1_START_ADDRESS	0xFFFFFFF0
#define XZYNQ_PL310_BASE	0xF8F02000

/* -------------------------------------------------------------------------
 * Clocks
 * -------------------------------------------------------------------------
 */
#define XZYNQ_PERIPH_CLOCK	50000000

/* -------------------------------------------------------------------------
 * Timers
 * -------------------------------------------------------------------------
 */
/* -------------------------------------------------------------------------
 * CPU Private timers
 * -------------------------------------------------------------------------
 */

#define XZYNQ_PRIVATE_TIMER_BASEADDR	0xF8F00600
#define XZYNQ_PRIVATE_TIMER_SIZE		0x10

/* Timer Load register */
#define XZYNQ_PRIVATE_TIMER_LOAD		0x00

/* Timer Counter register */
#define XZYNQ_PRIVATE_TIMER_COUNTER		0x04

/* Timer Control register */
#define XZYNQ_PRIVATE_TIMER_CTRL				0x08
#define XZYNQ_PRIVATE_TIMER_CTRL_EN				(1 << 0)
#define XZYNQ_PRIVATE_TIMER_CTRL_AUTO_RELOAD	(1 << 1)
#define XZYNQ_PRIVATE_TIMER_CTRL_IRQ			(1 << 2)
#define XZYNQ_PRIVATE_TIMER_CTRL_PRESCALER_MSK	(0xFF << 8)

/* Timer Interrupt Status register */
#define XZYNQ_PRIVATE_TIMER_INT_STS 	0x0C

/* -------------------------------------------------------------------------
 * MPCORE
 * -------------------------------------------------------------------------
 */
#define XZYNQ_MPCORE_BASE			0xF8F00000
#define XZYNQ_MPCORE_LEN			0x00002000
#define XZYNQ_MPCORE_FILTER_START_ADDR		0x00000040
#define XZYNQ_MPCORE_FILTER_END_ADDR		0x00000044
#define XZYNQ_MPCORE_FILTER_ADDR_MSK		0xFFF00000


/* -------------------------------------
 * Global timer
 * -------------------------------------
 */

#define XZYNQ_GLOBAL_TIMER_COUNTER_BASE				0xF8F00200
/* Size of the register area (used for mapping) */
#define XZYNQ_GLOBAL_TIMER_COUNTER_BASE_MAP_SIZE	0x610

#define XZYNQ_GLOBAL_TIMER_COUNTER_REG0				0x00
#define XZYNQ_GLOBAL_TIMER_COUNTER_REG1				0x04
#define XZYNQ_GLOBAL_TIMER_CONTROL_REG				0x08
#define XZYNQ_GLOBAL_TIMER_INT_STS				0x0C
#define XZYNQ_GLOBAL_TIMER_CMP_REG0				0x10
#define XZYNQ_GLOBAL_TIMER_CMP_REG1				0x14

#define XZYNQ_GLOBAL_TIMER_CONTROL_TIMER_EN			(1 << 0)
#define XZYNQ_GLOBAL_TIMER_CONTROL_CMP_EN			(1 << 1)
#define XZYNQ_GLOBAL_TIMER_CONTROL_IRQ				(1 << 2)
#define XZYNQ_GLOBAL_TIMER_CONTROL_AE				(1 << 3)
#define XZYNQ_GLOBAL_TIMER_CONTROL_PRESCALER_MSK		(0xFF << 8)

/* -------------------------------------
 * TTC timers
 * -------------------------------------
 */

#define XZYNQ_TTC0_BASE(timer)		(unsigned int)(0xF8001000 + 4 * timer)
#define XZYNQ_TTC1_BASE(timer)		(unsigned int)(0xF8002000 + 4 * timer)
#define XZYNQ_TTC_MAP_SIZE			0x84

/* Address used in the timer callout
 * Make sure it is consistent with the C code (cannot use the C definition
 * in assembly code).
 */
#define XZYNQ_TTC0_BASE_CALLOUT		0xF8001000


#define XZYNQ_TTC_CLOCK_CTRL		0x00
#define XZYNQ_TTC_COUNTER_CTRL		0x0C
#define XZYNQ_TTC_COUNTER_VAL		0x18
#define XZYNQ_TTC_INTR_VAL			0x24
#define XZYNQ_TTC_MATCH_1			0x30
#define XZYNQ_TTC_MATCH_2			0x3C
#define XZYNQ_TTC_MATCH_3			0x48
#define XZYNQ_TTC_ISR				0x54
#define XZYNQ_TTC_IER				0x60

#define XZYNQ_TTC_COUNTER_CTRL_DIS		(1 << 0)
#define XZYNQ_TTC_COUNTER_CTRL_INT		(1 << 1)
#define XZYNQ_TTC_COUNTER_CTRL_DEC		(1 << 2)
#define XZYNQ_TTC_COUNTER_CTRL_MATCH	(1 << 3)
#define XZYNQ_TTC_COUNTER_CTRL_RST		(1 << 4)
#define XZYNQ_TTC_COUNTER_CTRL_EN_WAVE	(1 << 5)
#define XZYNQ_TTC_COUNTER_CTRL_POL_WAVE	(1 << 6)

#define XZYNQ_TTC_CLOCK_CTRL_PS_EN		(1 << 0)

#define XZYNQ_TTC_IER_IXR_INTERVAL		(1 << 0)
#define XZYNQ_TTC_IER_IXR_MATCH_0		(1 << 1)
#define XZYNQ_TTC_IER_IXR_MATCH_1		(1 << 2)
#define XZYNQ_TTC_IER_IXR_MATCH_2		(1 << 3)
#define XZYNQ_TTC_IER_IXR_CNT_OVR		(1 << 4)



/* -------------------------------------------------------------------------
 * System level control registers (SLCR)
 * -------------------------------------------------------------------------
 */

#define XZYNQ_SLCR_BASE					0xF8000000
#define XZYNQ_SLCR_LEN					0x00001000
#define XZYNQ_SLCR_LOCK_OFFSET			0x00000004
#define XZYNQ_SLCR_LOCK_KEY				0x0000767B
#define XZYNQ_SLCR_UNLOCK_OFFSET		0x00000008
#define XZYNQ_SLCR_UNLOCK_KEY			0x0000DF0D
#define XZYNQ_SLCR_ARM_PLL_CTRL_REG		0x00000100
#define XZYNQ_SLCR_DDR_PLL_CTRL_REG		0x00000104
#define XZYNQ_SLCR_IO_PLL_CTRL_REG		0x00000108
#define XZYNQ_SLCR_ARM_CLK_CTRL_REG		0x00000120
#define XZYNQ_SLCR_APER_CLK_CTRL_REG	0x0000012C
#define XZYNQ_SLCR_GEM0_RCLK_CTRL_REG	0x00000138
#define XZYNQ_SLCR_GEM0_CLK_CTRL_REG	0x00000140
#define XZYNQ_SLCR_GEM1_RCLK_CTRL_REG	0x0000013C
#define XZYNQ_SLCR_GEM1_CLK_CTRL_REG	0x00000144
#define XZYNQ_SLCR_LQSPI_CLK_CTRL_REG	0x0000014C
#define XZYNQ_SLCR_SDIO_CLK_CTRL_REG	0x00000150
#define XZYNQ_SLCR_UART_CLK_CTRL_REG	0x00000154
#define XZYNQ_SLCR_SPI_CLK_CTRL_REG		0x00000158
#define XZYNQ_SLCR_CAN_CLK_CTRL_REG		0x0000015C
#define XZYNQ_SLCR_CAN_MIOCLK_CTRL_REG	0x00000160
#define XZYNQ_SLCR_CLK_621_TRUE_REG		0x000001C4
#define XZYNQ_SLCR_CAN_RST_CTRL_REG		0x00000220
#define XZYNQ_SLCR_FPGA_RST_CTRL_REG		0x00000240
#define XZYNQ_SLCR_LVL_SHFTR_EN_REG		0x00000900
#define XZYNQ_SLCR_OCM_CFG_REG				0x00000910

#define XZYNQ_SLCR_ARM_PLL_CTRL_PLL_FDIV_MASK		(0x7F << 12)
#define XZYNQ_SLCR_ARM_PLL_CTRL_PLL_FDIV_OFFSET		(12)

#define XZYNQ_SLCR_DDR_PLL_CTRL_PLL_FDIV_MASK		(0x7F << 12)
#define XZYNQ_SLCR_DDR_PLL_CTRL_PLL_FDIV_OFFSET		(12)

#define XZYNQ_SLCR_IO_PLL_CTRL_PLL_FDIV_MASK		(0x7F << 12)
#define XZYNQ_SLCR_IO_PLL_CTRL_PLL_FDIV_OFFSET		(12)

/* ARM CTRL register */
#define XZYNQ_SLCR_ARM_CLK_CTRL_SRCSEL_MASK			(0x3 << 4)
#define XZYNQ_SLCR_ARM_CLK_CTRL_SRCSEL_CPU_PLL1 	(0x0 << 4)
#define XZYNQ_SLCR_ARM_CLK_CTRL_SRCSEL_CPU_PLL2 	(0x1 << 4)
#define XZYNQ_SLCR_ARM_CLK_CTRL_SRCSEL_DDR_PLL		(0x2 << 4)
#define XZYNQ_SLCR_ARM_CLK_CTRL_SRCSEL_IO_PLL		(0x3 << 4)
#define XZYNQ_SLCR_ARM_CLK_CTRL_DIVISOR_OFFSET 		(8)
#define XZYNQ_SLCR_ARM_CLK_CTRL_DIVISOR_MASK 		(0x3F << 8)

/* Peripheral CTRL register */
#define XZYNQ_SLCR_PERIPH_CLK_CTRL_SRCSEL_MASK		(0x3 << 4)
#define XZYNQ_SLCR_PERIPH_CLK_CTRL_SRCSEL_DDR_PLL 	(0x3 << 4)
#define XZYNQ_SLCR_PERIPH_CLK_CTRL_SRCSEL_ARM_PLL	(0x2 << 4)
#define XZYNQ_SLCR_PERIPH_CLK_CTRL_SRCSEL_IO_PLL	(0x0 << 4)
#define XZYNQ_SLCR_PERIPH_CLK_CTRL_DIVISOR_OFFSET 	(8)
#define XZYNQ_SLCR_PERIPH_CLK_CTRL_DIVISOR_MASK 	(0x3F << 8)

/* CAN CTRL register */
#define XZYNQ_SLCR_CAN_CLK_CTRL_DIVISOR0_OFFSET 	(8)
#define XZYNQ_SLCR_CAN_CLK_CTRL_DIVISOR1_OFFSET 	(20)
#define XZYNQ_SLCR_CAN_CLK_CTRL_DIVISOR0_MASK 		(0x3F << 8)
#define XZYNQ_SLCR_CAN_CLK_CTRL_DIVISOR1_MASK 		(0x3F << 20)

#define XZYNQ_SLCR_CLK_CTRL_CLKACT_CAN0_OFFSET	  (0)
#define XZYNQ_SLCR_CLK_CTRL_CLKACT_CAN1_OFFSET	  (1)
#define XZYNQ_SLCR_CLK_CTRL_CLKACT_GEM0_OFFSET	  (0)
#define XZYNQ_SLCR_CLK_CTRL_CLKACT_GEM1_OFFSET	  (0)
#define XZYNQ_SLCR_RCLK_CTRL_CLKACT_GEM0_OFFSET	  (0)
#define XZYNQ_SLCR_RCLK_CTRL_CLKACT_GEM1_OFFSET	  (0)
#define XZYNQ_SLCR_CLK_CTRL_CLKACT_LQSPI_OFFSET  (0)
#define XZYNQ_SLCR_CLK_CTRL_CLKACT_SPI0_OFFSET   (0)
#define XZYNQ_SLCR_CLK_CTRL_CLKACT_SPI1_OFFSET   (1)

#define XZYNQ_SLCR_APER_CLK_CTRL_DMA	(1<<0)
#define XZYNQ_SLCR_APER_CLK_CTRL_USB0	(1<<2)
#define XZYNQ_SLCR_APER_CLK_CTRL_USB1	(1<<3)
#define XZYNQ_SLCR_APER_CLK_CTRL_GEM0	(1<<6)
#define XZYNQ_SLCR_APER_CLK_CTRL_GEM1	(1<<7)
#define XZYNQ_SLCR_APER_CLK_CTRL_SDI0	(1<<10)
#define XZYNQ_SLCR_APER_CLK_CTRL_SDI1	(1<<11)
#define XZYNQ_SLCR_APER_CLK_CTRL_SPI0	(1<<14)
#define XZYNQ_SLCR_APER_CLK_CTRL_SPI1	(1<<15)
#define XZYNQ_SLCR_APER_CLK_CTRL_CAN0	(1<<16)
#define XZYNQ_SLCR_APER_CLK_CTRL_CAN1	(1<<17)
#define XZYNQ_SLCR_APER_CLK_CTRL_I2C0	(1<<18)
#define XZYNQ_SLCR_APER_CLK_CTRL_I2C1	(1<<19)
#define XZYNQ_SLCR_APER_CLK_CTRL_UART0	(1<<20)
#define XZYNQ_SLCR_APER_CLK_CTRL_UART1	(1<<21)
#define XZYNQ_SLCR_APER_CLK_CTRL_GPIO	(1<<22)

#define XZYNQ_SLCR_PER_DIV_SHIFT		(8)
#define XZYNQ_SLCR_PER_DIV_MASK			(0x00003F00)
#define XZYNQ_SLCR_PER_DIV1_SHIFT		(20)
#define XZYNQ_SLCR_PER_DIV1_MASK		(0x03F00000)
#define XZYNQ_SLCR_PER_SRC_SHIFT		(4)
#define XZYNQ_SLCR_PER_SRC_IOPLL		(0)
#define XZYNQ_SLCR_PER_SRC_ARMPLL		(2)
#define XZYNQ_SLCR_PER_SRC_DDRPLL		(3)
#define XZYNQ_SLCR_PER_SRC_MASK			(0x00000030)
#define XZYNQ_SLCR_PER_CLK_ACT0			(0x00000001)
#define XZYNQ_SLCR_PER_CLK_ACT1			(0x00000002)

#define XZYNQ_SLCR_CAN0_RST_MASK		(1)
#define XZYNQ_SLCR_CAN1_RST_MASK		(2)

/* -------------------------------------------------------------------------
 * SDIO ports
 * -------------------------------------------------------------------------
 */
#define XZYNQ_SDIO0_BASEADDR		0xE0100000
#define XZYNQ_SDIO1_BASEADDR		0xE0101000

/* -------------------------------------------------------------------------
 * Serial ports
 * -------------------------------------------------------------------------
 */

/* UART base addresses */
#define	XZYNQ_XUARTPS_UART0_BASE		0xE0000000
#define	XZYNQ_XUARTPS_UART1_BASE		0xE0001000

#define	XZYNQ_XUARTPS_UART_SIZE			0x1000

/* UART registers */
#define XZYNQ_XUARTPS_CR_REG          0x00 /* UART Control register */
#define XZYNQ_XUARTPS_MR_REG          0x04 /* UART Mode register */
#define XZYNQ_XUARTPS_IER_REG         0x08 /* Interrupt Enable register */
#define XZYNQ_XUARTPS_IDR_REG         0x0C /* Interrupt Disable register */
#define XZYNQ_XUARTPS_IMR_REG         0x10 /* Interrupt Mask register */
#define XZYNQ_XUARTPS_ISR_REG         0x14 /* Channel Int Status register*/
#define XZYNQ_XUARTPS_BAUDGEN_REG     0x18 /* Baud rate divider register */
#define XZYNQ_XUARTPS_RXTOUT_REG      0x1C /* Receiver timeout register */
#define XZYNQ_XUARTPS_RXWM_REG        0x20 /* Receiver FIFO trigger level */
#define XZYNQ_XUARTPS_MODEMCR_REG     0x24 /* Modem control register */
#define XZYNQ_XUARTPS_MODEMSR_REG     0x28 /* Modem status register */
#define XZYNQ_XUARTPS_SR_REG          0x2C /* Channel status register */
#define XZYNQ_XUARTPS_FIFO_REG        0x30 /* Transmit and Receive FIFO */
#define XZYNQ_XUARTPS_BAUDDIV_REG     0x34 /* Baud Rate Count Register */
#define XZYNQ_XUARTPS_FLOWDEL_REG     0x38 /* Flow Control delay register */
#define XZYNQ_XUARTPS_TXWM_REG        0x44 /* Transmitter timeout register */

/* UART Control register */
#define XZYNQ_XUARTPS_CR_STOPBRK     0x00000100  /* Stop transmission of break */
#define XZYNQ_XUARTPS_CR_STARTBRK    0x00000080  /* Set break */
#define XZYNQ_XUARTPS_CR_TORST       0x00000040  /* RX timeout counter restart */
#define XZYNQ_XUARTPS_CR_TX_DIS      0x00000020  /* TX disabled. */
#define XZYNQ_XUARTPS_CR_TX_EN       0x00000010  /* TX enabled */
#define XZYNQ_XUARTPS_CR_RX_DIS      0x00000008  /* RX disabled. */
#define XZYNQ_XUARTPS_CR_RX_EN       0x00000004  /* RX enabled */
#define XZYNQ_XUARTPS_CR_EN_DIS_MASK 0x0000003C  /* Enable/disable Mask */
#define XZYNQ_XUARTPS_CR_TXRST       0x00000002  /* TX logic reset */
#define XZYNQ_XUARTPS_CR_RXRST       0x00000001  /* RX logic reset */

/* UART Mode register */
#define XZYNQ_XUARTPS_MR_CCLK             0x00000400  /* Input clock selection */
#define XZYNQ_XUARTPS_MR_CHMODE_R_LOOP    0x00000300  /* Remote loopback mode */
#define XZYNQ_XUARTPS_MR_CHMODE_L_LOOP    0x00000200  /* Local loopback mode */
#define XZYNQ_XUARTPS_MR_CHMODE_ECHO      0x00000100  /* Auto echo mode */
#define XZYNQ_XUARTPS_MR_CHMODE_NORM      0x00000000  /* Normal mode */
#define XZYNQ_XUARTPS_MR_CHMODE_SHIFT              8  /* Mode shift */
#define XZYNQ_XUARTPS_MR_CHMODE_MASK      0x00000300  /* Mode mask */
#define XZYNQ_XUARTPS_MR_STOPMODE_2_BIT   0x00000080  /* 2 stop bits */
#define XZYNQ_XUARTPS_MR_STOPMODE_1_5_BIT 0x00000040  /* 1.5 stop bits */
#define XZYNQ_XUARTPS_MR_STOPMODE_1_BIT   0x00000000  /* 1 stop bit */
#define XZYNQ_XUARTPS_MR_STOPMODE_SHIFT            6  /* Stop bits setting shift */
#define XZYNQ_XUARTPS_MR_STOPMODE_MASK    0x000000A0  /* Stop bits setting mask */
#define XZYNQ_XUARTPS_MR_PARITY_NONE      0x00000020  /* No parity mode */
#define XZYNQ_XUARTPS_MR_PARITY_MARK      0x00000018  /* Mark parity mode */
#define XZYNQ_XUARTPS_MR_PARITY_SPACE     0x00000010  /* Space parity mode */
#define XZYNQ_XUARTPS_MR_PARITY_ODD       0x00000008  /* Odd parity mode */
#define XZYNQ_XUARTPS_MR_PARITY_EVEN      0x00000000  /* Even parity mode */
#define XZYNQ_XUARTPS_MR_PARITY_SHIFT              3  /* Parity setting shift */
#define XZYNQ_XUARTPS_MR_PARITY_MASK      0x00000038  /* Parity mask */
#define XZYNQ_XUARTPS_MR_CHARLEN_6_BIT    0x00000006  /* 6 bits data */
#define XZYNQ_XUARTPS_MR_CHARLEN_7_BIT    0x00000004  /* 7 bits data */
#define XZYNQ_XUARTPS_MR_CHARLEN_8_BIT    0x00000000  /* 8 bits data */
#define XZYNQ_XUARTPS_MR_CHARLEN_SHIFT             1  /* data Length setting shift */
#define XZYNQ_XUARTPS_MR_CHARLEN_MASK     0x00000006  /* Data length mask. */
#define XZYNQ_XUARTPS_MR_CLKSEL           0x00000001  /* Input clock selection */

/* Interrupt registers */
#define XZYNQ_XUARTPS_IXR_TXOVR   0x00001000	/* TX Overrun error interrupt */
#define XZYNQ_XUARTPS_IXR_TXNFULL 0x00000800	/* TX FIFO near full interrupt */
#define XZYNQ_XUARTPS_IXR_TRIG    0x00000400	/* TX FIFO trigger interrupt */
#define XZYNQ_XUARTPS_IXR_DMS     0x00000200	/* Modem status change interrupt */
#define XZYNQ_XUARTPS_IXR_TOUT    0x00000100	/* Timeout error interrupt */
#define XZYNQ_XUARTPS_IXR_PARITY  0x00000080	/* Parity error interrupt */
#define XZYNQ_XUARTPS_IXR_FRAMING 0x00000040	/* Framing error interrupt */
#define XZYNQ_XUARTPS_IXR_RXOVR   0x00000020	/* RX Overrun error interrupt */
#define XZYNQ_XUARTPS_IXR_TXFULL  0x00000010	/* TX FIFO full interrupt. */
#define XZYNQ_XUARTPS_IXR_TXEMPTY 0x00000008	/* TX FIFO empty interrupt. */
#define XZYNQ_XUARTPS_IXR_RXFULL  0x00000004	/* RX FIFO full interrupt. */
#define XZYNQ_XUARTPS_IXR_RXEMPTY 0x00000002	/* RX FIFO empty interrupt. */
#define XZYNQ_XUARTPS_IXR_RTRIG   0x00000001	/* RX FIFO trigger interrupt. */
#define XZYNQ_XUARTPS_IXR_MASK    0x00001FFF	/* Valid bit mask */

/* Baud rate divider register */
#define XZYNQ_XUARTPS_BAUDGEN_DISABLE 0x00000000    /* Disable clock */
#define XZYNQ_XUARTPS_BAUDGEN_MASK    0x0000FFFF    /* Valid bits mask */

/* Receiver timeout register */
#define XZYNQ_XUARTPS_RXTOUT_DISABLE    0x00000000  /* Disable time out */
#define XZYNQ_XUARTPS_RXTOUT_MASK       0x000000FF  /* Valid bits mask */

/* Receiver FIFO trigger level */
#define XZYNQ_XUARTPS_RXWM_DISABLE 0x00000000  /* Disable RX trigger interrupt */
#define XZYNQ_XUARTPS_RXWM_MASK    0x0000001F  /* Valid bits mask */

/* Modem control register */
#define XZYNQ_XUARTPS_MODEMCR_FCM 0x00000010	 /* Flow control mode */
#define XZYNQ_XUARTPS_MODEMCR_RTS 0x00000002	 /* Request to send */
#define XZYNQ_XUARTPS_MODEMCR_DTR 0x00000001	 /* Data terminal ready */

/* Modem status register */
#define XZYNQ_XUARTPS_MODEMSR_FCMS  0x00000100  /* Flow control mode (FCMS) */
#define XZYNQ_XUARTPS_MODEMSR_DCD   0x00000080  /* Complement of DCD input */
#define XZYNQ_XUARTPS_MODEMSR_RI    0x00000040  /* Complement of RI input */
#define XZYNQ_XUARTPS_MODEMSR_DSR   0x00000020  /* Complement of DSR input */
#define XZYNQ_XUARTPS_MODEMSR_CTS   0x00000010  /* Complement of CTS input */
#define XZYNQ_XUARTPS_MODEMSR_DCDX  0x00000008  /* Delta DCD indicator */
#define XZYNQ_XUARTPS_MODEMSR_RIX   0x00000004  /* Change of RI */
#define XZYNQ_XUARTPS_MODEMSR_DSRX  0x00000002  /* Change of DSR */
#define XZYNQ_XUARTPS_MODEMSR_CTSX  0x00000001  /* Change of CTS */

/* Channel status register */
#define XZYNQ_XUARTPS_SR_FLOWDEL  0x00001000	 /* RX FIFO fill over flow delay */
#define XZYNQ_XUARTPS_SR_TACTIVE  0x00000800	 /* TX active */
#define XZYNQ_XUARTPS_SR_RACTIVE  0x00000400	 /* RX active */
#define XZYNQ_XUARTPS_SR_DMS      0x00000200	 /* Delta modem status change */
#define XZYNQ_XUARTPS_SR_TOUT     0x00000100	 /* RX timeout */
#define XZYNQ_XUARTPS_SR_PARITY   0x00000080	 /* RX parity error */
#define XZYNQ_XUARTPS_SR_FRAME    0x00000040	 /* RX frame error */
#define XZYNQ_XUARTPS_SR_OVER     0x00000020	 /* RX overflow error */
#define XZYNQ_XUARTPS_SR_TXFULL   0x00000010	 /* TX FIFO full */
#define XZYNQ_XUARTPS_SR_TXEMPTY  0x00000008	 /* TX FIFO empty */
#define XZYNQ_XUARTPS_SR_RXFULL   0x00000004	 /* RX FIFO full */
#define XZYNQ_XUARTPS_SR_RXEMPTY  0x00000002	 /* RX FIFO empty */
#define XZYNQ_XUARTPS_SR_RXOVR    0x00000001	 /* RX FIFO fill over trigger */

/* Flow Control delay register */
#define XZYNQ_XUARTPS_FLOWDEL_MASK    XZYNQ_XUARTPS_RXWM_MASK    /* Valid bit mask */

/* -------------------------------------------------------------------------
 * CAN
 * -------------------------------------------------------------------------
 */
#define XZYNQ_CAN0_BASEADDR			0xE0008000
#define XZYNQ_CAN1_BASEADDR			0xE0009000

/* -------------------------------------------------------------------------
 * Watchdog
 * -------------------------------------------------------------------------
 */
#define XZYNQ_WDT_BASEADDR			0xF8005000
#define XZYNQ_WDT_SIZE				0x10
#define XZYNQ_WDT_CLOCK_FREQ		133000000

#define XZYNQ_WDT_ZMR_OFFSET		0x0 /**< Zero Mode Register */
#define XZYNQ_WDT_CCR_OFFSET		0x4 /**< Counter Control Register */
#define XZYNQ_WDT_RESTART_OFFSET	0x8 /**< Restart Register */
#define XZYNQ_WDT_SR_OFFSET			0xC /**< Status Register */

/* Register MODE */
#define XZYNQ_WDT_ZMR_WDEN_MASK		0x00000001	/**< enable the WDT */
#define XZYNQ_WDT_ZMR_RSTEN_MASK	0x00000002	/**< enable the reset output */
#define XZYNQ_WDT_ZMR_IRQEN_MASK	0x00000004	/**< enable the IRQ output */

#define XZYNQ_WDT_ZMR_RSTLN_MASK	0x00000070	/**< set length of reset pulse */
#define XZYNQ_WDT_ZMR_RSTLN_SHIFT	4	   		/**< shift for reset pulse */

#define XZYNQ_WDT_ZMR_IRQLN_MASK	0x00000180	/**< set length of interrupt pulse */
#define XZYNQ_WDT_ZMR_IRQLN_SHIFT	7	   		/**< shift for interrupt pulse */

#define XZYNQ_WDT_ZMR_ZKEY_MASK		0x00FFF000	/**< mask for writing access key */
#define XZYNQ_WDT_ZMR_ZKEY			0xABC		/**< access key, 0xABC */
#define XZYNQ_WDT_ZMR_ZKEY_SHIFT	12
#define XZYNQ_WDT_ZMR_ZKEY_VAL		(XZYNQ_WDT_ZMR_ZKEY<<XZYNQ_WDT_ZMR_ZKEY_SHIFT)

/* Register CONTROL */
#define XZYNQ_WDT_CCR_CLKSEL_MASK	0x00000003	/**< counter clock prescale */

#define XZYNQ_WDT_CCR_PSCALE_0008	0x00000000	/**< divide clock by 8 */
#define XZYNQ_WDT_CCR_PSCALE_0064	0x00000001	/**< divide clock by 64 */
#define XZYNQ_WDT_CCR_PSCALE_0512	0x00000002	/**< divide clock by 512 */
#define XZYNQ_WDT_CCR_PSCALE_4096	0x00000003	/**< divide clock by 4096 */

#define XZYNQ_WDT_CCR_CRV_MASK		0x00003FFC	/**< counter reset value */
#define XZYNQ_WDT_CCR_CRV_SHIFT		2			/**< shift for writing value */

#define XZYNQ_WDT_CCR_CKEY_MASK		0x03FFC000	/**< mask for writing access key */
#define XZYNQ_WDT_CCR_CKEY			0x00000248	/**< access key, 0x248 */
#define XZYNQ_WDT_CCR_CKEY_SHIFT	14
#define XZYNQ_WDT_CCR_CKEY_VAL		(XZYNQ_WDT_CCR_CKEY<<XZYNQ_WDT_CCR_CKEY_SHIFT)

/* Register RESTART */
#define XZYNQ_WDT_RESTART_KEY_VAL	0x00001999 /**< valid key */

/* Register STATUS */
#define XZYNQ_WDT_SR_WDZ_MASK		0x00000001 /**< time out occured */

/* -------------------------------------------------------------------------
 * Device Config
 * -------------------------------------------------------------------------
 */
#define XZYNQ_DEVCFG_BASEADDR			0xF8007000
#define XZYNQ_DEVCFG_SIZE				0x1000

/* Register offsets */
#define XZYNQ_DEVCFG_CTRL_OFFSET			0x00		/* Control Register */
#define XZYNQ_DEVCFG_LOCK_OFFSET			0x04		/* Lock Register. Used to lock changes in the control reg */
#define XZYNQ_DEVCFG_UNLOCK_OFFSET			0x34		/* Unlock Register. Write the unlock key to access the devcfg interface */

#define XZYNQ_DEVCFG_UNLOCK_KEY				0x757BDF0D	/*Write to Unlock Register to use the devcfg interface */

/* Return Codes */
#define XZYNQ_SUCCESS 0
#define XZYNQ_FAILURE 1


/* -------------------------------------------------------------------------
 * XADC
 * Part of the Device Config Interface. All Offsets are based on the devcfg
 * base address.
 * -------------------------------------------------------------------------
 */

/* Register offsets */
#define XZYNQ_XADC_CFG_OFFSET			0x100	/* XADC Configuration Register */
#define XZYNQ_XADC_INT_STS_OFFSET		0x104	/* XADC Interrupt Status Register */
#define XZYNQ_XADC_INT_MASK_OFFSET		0x108	/* XADC Interrupt Mask Register */
#define XZYNQ_XADC_MSTS_OFFSET			0x10C	/* XADC Miscellaneous Status Register */
#define XZYNQ_XADC_CMDFIFO_OFFSET		0x110	/* XADC Command FIFO Register */
#define XZYNQ_XADC_RDFIFO_OFFSET		0x114	/* XADC Data FIFO Register */
#define XZYNQ_XADC_MCTL_OFFSET			0x118	/* XADC Miscellaneous Control Register */

/* XADC Config Register definitions */
#define XZYNQ_XADC_CFG_ENABLE_MASK				0x80000000	/* Enable access from the PS */
#define XZYNQ_XADC_CFG_CFIFOTH_MASK				0x00F00000	/* Command FIFO Threshold mask */
#define XZYNQ_XADC_CFG_DFIFOTH_MASK				0x000F0000	/* Data FIFO Threshold mask */
#define XZYNQ_XADC_CFG_WEDGE_MASK				0x00002000	/* Write Edge Mask */
#define XZYNQ_XADC_CFG_REDGE_MASK				0x00001000	/* Read Edge Mask */
#define XZYNQ_XADC_CFG_TCKRATE_MASK				0x00000300  /* Clock freq control */
#define XZYNQ_XADC_CFG_IGAP_MASK				0x0000001F	/* Idle Gap between successive commands */


/* XADC Interrupt Status/Mask Register Bit definitions */
#define XZYNQ_XADC_INTX_ALL_MASK   	   0x000003FF /**< Alarm Signals Mask  */
#define XZYNQ_XADC_INTX_CFIFO_LTH_MASK 0x00000200 /**< CMD FIFO less than threshold */
#define XZYNQ_XADC_INTX_DFIFO_GTH_MASK 0x00000100 /**< Data FIFO greater than threshold */
#define XZYNQ_XADC_INTX_OT_MASK	   0x00000080 /**< Over temperature Alarm Status */
#define XZYNQ_XADC_INTX_ALM_ALL_MASK   0x0000007F /**< Alarm Signals Mask  */
#define XZYNQ_XADC_INTX_ALM6_MASK	   0x00000040 /**< Alarm 6 Mask  */
#define XZYNQ_XADC_INTX_ALM5_MASK	   0x00000020 /**< Alarm 5 Mask  */
#define XZYNQ_XADC_INTX_ALM4_MASK	   0x00000010 /**< Alarm 4 Mask  */
#define XZYNQ_XADC_INTX_ALM3_MASK	   0x00000008 /**< Alarm 3 Mask  */
#define XZYNQ_XADC_INTX_ALM2_MASK	   0x00000004 /**< Alarm 2 Mask  */
#define XZYNQ_XADC_INTX_ALM1_MASK	   0x00000002 /**< Alarm 1 Mask  */
#define XZYNQ_XADC_INTX_ALM0_MASK	   0x00000001 /**< Alarm 0 Mask  */

/* XADC Miscellaneous Register Bit definitions */
#define XZYNQ_XADC_MSTS_CFIFO_LVL_MASK  0x000F0000 /**< Command FIFO Level mask */
#define XZYNQ_XADC_MSTS_DFIFO_LVL_MASK  0x0000F000 /**< Data FIFO Level Mask  */
#define XZYNQ_XADC_MSTS_CFIFOF_MASK     0x00000800 /**< Command FIFO Full Mask  */
#define XZYNQ_XADC_MSTS_CFIFOE_MASK     0x00000400 /**< Command FIFO Empty Mask  */
#define XZYNQ_XADC_MSTS_DFIFOF_MASK     0x00000200 /**< Data FIFO Full Mask  */
#define XZYNQ_XADC_MSTS_DFIFOE_MASK     0x00000100 /**< Data FIFO Empty Mask  */
#define XZYNQ_XADC_MSTS_OT_MASK	    0x00000080 /**< Over Temperature Mask */
#define XZYNQ_XADC_MSTS_ALM_MASK	    0x0000007F /**< Alarms Mask  */

/* XADC Miscellaneous Control Register Bit definitions */
#define XZYNQ_XADC_MCTL_RESET_MASK      0x00000010 /**< Reset XADC */
#define XZYNQ_XADC_MCTL_FLUSH_MASK      0x00000001 /**< Flush the FIFOs */

/* Internal Register offsets of the XADC
 * The following constants provide access to each of the internal registers of
 * the XADC device.
 */

/* XADC Internal Channel Registers */
#define XZYNQ_XADC_TEMP_OFFSET		  0x00 /**< On-chip Temperature Reg */
#define XZYNQ_XADC_VCCINT_OFFSET		  0x01 /**< On-chip VCCINT Data Reg */
#define XZYNQ_XADC_VCCAUX_OFFSET		  0x02 /**< On-chip VCCAUX Data Reg */
#define XZYNQ_XADC_VPVN_OFFSET		  0x03 /**< ADC out of VP/VN	   */
#define XZYNQ_XADC_VREFP_OFFSET		  0x04 /**< On-chip VREFP Data Reg */
#define XZYNQ_XADC_VREFN_OFFSET		  0x05 /**< On-chip VREFN Data Reg */
#define XZYNQ_XADC_VBRAM_OFFSET		  0x06 /**< On-chip VBRAM , 7 Series */
#define XZYNQ_XADC_ADC_A_SUPPLY_CALIB_OFFSET  0x08 /**< ADC A Supply Offset Reg */
#define XZYNQ_XADC_ADC_A_OFFSET_CALIB_OFFSET  0x09 /**< ADC A Offset Data Reg */
#define XZYNQ_XADC_ADC_A_GAINERR_CALIB_OFFSET 0x0A /**< ADC A Gain Error Reg  */
#define XZYNQ_XADC_VCCPINT_OFFSET		  0x0D /**< On-chip VCCPINT Reg, Zynq */
#define XZYNQ_XADC_VCCPAUX_OFFSET		  0x0E /**< On-chip VCCPAUX Reg, Zynq */
#define XZYNQ_XADC_VCCPDRO_OFFSET		  0x0F /**< On-chip VCCPDRO Reg, Zynq */

/* XADC External Channel Registers */
#define XZYNQ_XADC_AUX00_OFFSET	0x10 /**< ADC out of VAUXP0/VAUXN0 */
#define XZYNQ_XADC_AUX01_OFFSET	0x11 /**< ADC out of VAUXP1/VAUXN1 */
#define XZYNQ_XADC_AUX02_OFFSET	0x12 /**< ADC out of VAUXP2/VAUXN2 */
#define XZYNQ_XADC_AUX03_OFFSET	0x13 /**< ADC out of VAUXP3/VAUXN3 */
#define XZYNQ_XADC_AUX04_OFFSET	0x14 /**< ADC out of VAUXP4/VAUXN4 */
#define XZYNQ_XADC_AUX05_OFFSET	0x15 /**< ADC out of VAUXP5/VAUXN5 */
#define XZYNQ_XADC_AUX06_OFFSET	0x16 /**< ADC out of VAUXP6/VAUXN6 */
#define XZYNQ_XADC_AUX07_OFFSET	0x17 /**< ADC out of VAUXP7/VAUXN7 */
#define XZYNQ_XADC_AUX08_OFFSET	0x18 /**< ADC out of VAUXP8/VAUXN8 */
#define XZYNQ_XADC_AUX09_OFFSET	0x19 /**< ADC out of VAUXP9/VAUXN9 */
#define XZYNQ_XADC_AUX10_OFFSET	0x1A /**< ADC out of VAUXP10/VAUXN10 */
#define XZYNQ_XADC_AUX11_OFFSET	0x1B /**< ADC out of VAUXP11/VAUXN11 */
#define XZYNQ_XADC_AUX12_OFFSET	0x1C /**< ADC out of VAUXP12/VAUXN12 */
#define XZYNQ_XADC_AUX13_OFFSET	0x1D /**< ADC out of VAUXP13/VAUXN13 */
#define XZYNQ_XADC_AUX14_OFFSET	0x1E /**< ADC out of VAUXP14/VAUXN14 */
#define XZYNQ_XADC_AUX15_OFFSET	0x1F /**< ADC out of VAUXP15/VAUXN15 */

/*
 * XADC Registers for Maximum/Minimum data captured for the
 * on chip Temperature/VCCINT/VCCAUX data.
 */
#define XZYNQ_XADC_MAX_TEMP_OFFSET		0x20 /**< Max Temperature Reg */
#define XZYNQ_XADC_MAX_VCCINT_OFFSET	0x21 /**< Max VCCINT Register */
#define XZYNQ_XADC_MAX_VCCAUX_OFFSET	0x22 /**< Max VCCAUX Register */
#define XZYNQ_XADC_MAX_VCCBRAM_OFFSET	0x23 /**< Max BRAM Register, 7 series */
#define XZYNQ_XADC_MIN_TEMP_OFFSET		0x24 /**< Min Temperature Reg */
#define XZYNQ_XADC_MIN_VCCINT_OFFSET	0x25 /**< Min VCCINT Register */
#define XZYNQ_XADC_MIN_VCCAUX_OFFSET	0x26 /**< Min VCCAUX Register */
#define XZYNQ_XADC_MIN_VCCBRAM_OFFSET	0x27 /**< Min BRAM Register, 7 series */
#define XZYNQ_XADC_MAX_VCCPINT_OFFSET	0x28 /**< Max VCCPINT Register, Zynq */
#define XZYNQ_XADC_MAX_VCCPAUX_OFFSET	0x29 /**< Max VCCPAUX Register, Zynq */
#define XZYNQ_XADC_MAX_VCCPDRO_OFFSET	0x2A /**< Max VCCPDRO Register, Zynq */
#define XZYNQ_XADC_MIN_VCCPINT_OFFSET	0x2C /**< Min VCCPINT Register, Zynq */
#define XZYNQ_XADC_MIN_VCCPAUX_OFFSET	0x2D /**< Min VCCPAUX Register, Zynq */
#define XZYNQ_XADC_MIN_VCCPDRO_OFFSET	0x2E /**< Min VCCPDRO Register,Zynq */
#define XZYNQ_XADC_FLAG_OFFSET		0x3F /**< Flag Register */

/* XADC Configuration Registers */
#define XZYNQ_XADC_CFR0_OFFSET	0x40	/**< Configuration Register 0 */
#define XZYNQ_XADC_CFR1_OFFSET	0x41	/**< Configuration Register 1 */
#define XZYNQ_XADC_CFR2_OFFSET	0x42	/**< Configuration Register 2 */

/* XADC Sequence Registers */
#define XZYNQ_XADC_SEQ00_OFFSET	0x48 /**< Seq Reg 00 Adc Channel Selection */
#define XZYNQ_XADC_SEQ01_OFFSET	0x49 /**< Seq Reg 01 Adc Channel Selection */
#define XZYNQ_XADC_SEQ02_OFFSET	0x4A /**< Seq Reg 02 Adc Average Enable */
#define XZYNQ_XADC_SEQ03_OFFSET	0x4B /**< Seq Reg 03 Adc Average Enable */
#define XZYNQ_XADC_SEQ04_OFFSET	0x4C /**< Seq Reg 04 Adc Input Mode Select */
#define XZYNQ_XADC_SEQ05_OFFSET	0x4D /**< Seq Reg 05 Adc Input Mode Select */
#define XZYNQ_XADC_SEQ06_OFFSET	0x4E /**< Seq Reg 06 Adc Acquisition Select */
#define XZYNQ_XADC_SEQ07_OFFSET	0x4F /**< Seq Reg 07 Adc Acquisition Select */

/* XADC Alarm Threshold/Limit Registers (ATR) */
#define XZYNQ_XADC_ATR_TEMP_UPPER_OFFSET	0x50 /**< Temp Upper Alarm Register */
#define XZYNQ_XADC_ATR_VCCINT_UPPER_OFFSET	0x51 /**< VCCINT Upper Alarm Reg */
#define XZYNQ_XADC_ATR_VCCAUX_UPPER_OFFSET	0x52 /**< VCCAUX Upper Alarm Reg */
#define XZYNQ_XADC_ATR_OT_UPPER_OFFSET	0x53 /**< Over Temp Upper Alarm Reg */
#define XZYNQ_XADC_ATR_TEMP_LOWER_OFFSET	0x54 /**< Temp Lower Alarm Register */
#define XZYNQ_XADC_ATR_VCCINT_LOWER_OFFSET	0x55 /**< VCCINT Lower Alarm Reg */
#define XZYNQ_XADC_ATR_VCCAUX_LOWER_OFFSET	0x56 /**< VCCAUX Lower Alarm Reg */
#define XZYNQ_XADC_ATR_OT_LOWER_OFFSET	0x57 /**< Over Temp Lower Alarm Reg */
#define XZYNQ_XADC_ATR_VBRAM_UPPER_OFFSET	0x58 /**< VBRAM Upper Alarm, 7 series */
#define XZYNQ_XADC_ATR_VCCPINT_UPPER_OFFSET	0x59 /**< VCCPINT Upper Alarm, Zynq */
#define XZYNQ_XADC_ATR_VCCPAUX_UPPER_OFFSET	0x5A /**< VCCPAUX Upper Alarm, Zynq */
#define XZYNQ_XADC_ATR_VCCPDRO_UPPER_OFFSET	0x5B /**< VCCPDRO Upper Alarm, Zynq */
#define XZYNQ_XADC_ATR_VBRAM_LOWER_OFFSET	0x5C /**< VRBAM Lower Alarm, 7 Series */
#define XZYNQ_XADC_ATR_VCCPINT_LOWER_OFFSET	0x5D /**< VCCPINT Lower Alarm, Zynq */
#define XZYNQ_XADC_ATR_VCCPAUX_LOWER_OFFSET	0x5E /**< VCCPAUX Lower Alarm, Zynq */
#define XZYNQ_XADC_ATR_VCCPDRO_LOWER_OFFSET	0x5F /**< VCCPDRO Lower Alarm, Zynq */

/* Configuration Register 0 (CFR0) mask(s) */
#define XZYNQ_XADC_CFR0_CAL_AVG_MASK	0x8000 /**< Averaging enable Mask */
#define XZYNQ_XADC_CFR0_AVG_VALID_MASK	0x3000 /**< Averaging bit Mask */
#define XZYNQ_XADC_CFR0_AVG1_MASK		0x0000 /**< No Averaging */
#define XZYNQ_XADC_CFR0_AVG16_MASK		0x1000 /**< Average 16 samples */
#define XZYNQ_XADC_CFR0_AVG64_MASK	 	0x2000 /**< Average 64 samples */
#define XZYNQ_XADC_CFR0_AVG256_MASK 	0x3000 /**< Average 256 samples */
#define XZYNQ_XADC_CFR0_AVG_SHIFT	 	12     /**< Averaging bits shift */
#define XZYNQ_XADC_CFR0_MUX_MASK	 	0x0800 /**< External Mask Enable */
#define XZYNQ_XADC_CFR0_DU_MASK	 	0x0400 /**< Bipolar/Unipolar mode */
#define XZYNQ_XADC_CFR0_EC_MASK	 	0x0200 /**< Event driven/
						 *  Continuous mode selection
						 */
#define XZYNQ_XADC_CFR0_ACQ_MASK	 	0x0100 /**< Add acquisition by 6 ADCCLK */
#define XZYNQ_XADC_CFR0_CHANNEL_MASK	0x001F /**< Channel number bit Mask */

/* Configuration Register 1 (CFR1) mask(s) */
#define XZYNQ_XADC_CFR1_SEQ_VALID_MASK	  0xF000 /**< Sequence bit Mask */
#define XZYNQ_XADC_CFR1_SEQ_SAFEMODE_MASK	  0x0000 /**< Default Safe Mode */
#define XZYNQ_XADC_CFR1_SEQ_ONEPASS_MASK	  0x1000 /**< Onepass through Seq */
#define XZYNQ_XADC_CFR1_SEQ_CONTINPASS_MASK	     0x2000 /**< Continuous Cycling Seq */
#define XZYNQ_XADC_CFR1_SEQ_SINGCHAN_MASK	     0x3000 /**< Single channel - No Seq */
#define XZYNQ_XADC_CFR1_SEQ_SIMUL_SAMPLING_MASK  0x4000 /**< Simulataneous Sampling Mask */
#define XZYNQ_XADC_CFR1_SEQ_INDEPENDENT_MASK  0x8000 /**< Independent Mode */
#define XZYNQ_XADC_CFR1_SEQ_SHIFT		  12     /**< Sequence bit shift */
#define XZYNQ_XADC_CFR1_ALM_VCCPDRO_MASK	  0x0800 /**< Alm 6 - VCCPDRO, Zynq  */
#define XZYNQ_XADC_CFR1_ALM_VCCPAUX_MASK	  0x0400 /**< Alm 5 - VCCPAUX, Zynq */
#define XZYNQ_XADC_CFR1_ALM_VCCPINT_MASK	  0x0200 /**< Alm 4 - VCCPINT, Zynq */
#define XZYNQ_XADC_CFR1_ALM_VBRAM_MASK	  0x0100 /**< Alm 3 - VBRAM, 7 series */
#define XZYNQ_XADC_CFR1_CAL_VALID_MASK	  0x00F0 /**< Valid Calibration Mask */
#define XZYNQ_XADC_CFR1_CAL_PS_GAIN_OFFSET_MASK  0x0080 /**< Calibration 3 -Power
							Supply Gain/Offset
							Enable */
#define XZYNQ_XADC_CFR1_CAL_PS_OFFSET_MASK	  0x0040 /**< Calibration 2 -Power
							Supply Offset Enable */
#define XZYNQ_XADC_CFR1_CAL_ADC_GAIN_OFFSET_MASK 0x0020 /**< Calibration 1 -ADC Gain
							Offset Enable */
#define XZYNQ_XADC_CFR1_CAL_ADC_OFFSET_MASK	 0x0010 /**< Calibration 0 -ADC Offset
							Enable */
#define XZYNQ_XADC_CFR1_CAL_DISABLE_MASK	0x0000 /**< No Calibration */
#define XZYNQ_XADC_CFR1_ALM_ALL_MASK	0x0F0F /**< Mask for all alarms */
#define XZYNQ_XADC_CFR1_ALM_VCCAUX_MASK	0x0008 /**< Alarm 2 - VCCAUX Enable */
#define XZYNQ_XADC_CFR1_ALM_VCCINT_MASK	0x0004 /**< Alarm 1 - VCCINT Enable */
#define XZYNQ_XADC_CFR1_ALM_TEMP_MASK	0x0002 /**< Alarm 0 - Temperature */
#define XZYNQ_XADC_CFR1_OT_MASK		0x0001 /**< Over Temperature Enable */

/* Configuration Register 2 (CFR2) mask(s) */
#define XZYNQ_XADC_CFR2_CD_VALID_MASK	0xFF00  /**<Clock Divisor bit Mask   */
#define XZYNQ_XADC_CFR2_CD_SHIFT		8	/**<Num of shift on division */
#define XZYNQ_XADC_CFR2_CD_MIN		8	/**<Minimum value of divisor */
#define XZYNQ_XADC_CFR2_CD_MAX		255	/**<Maximum value of divisor */

#define XZYNQ_XADC_CFR2_CD_MIN		8	/**<Minimum value of divisor */
#define XZYNQ_XADC_CFR2_PD_MASK		0x0030	/**<Power Down Mask */
#define XZYNQ_XADC_CFR2_PD_XADC_MASK	0x0030	/**<Power Down XADC Mask */
#define XZYNQ_XADC_CFR2_PD_ADC1_MASK	0x0020	/**<Power Down ADC1 Mask */
#define XZYNQ_XADC_CFR2_PD_SHIFT		4	/**<Power Down Shift */

/* Sequence Register (SEQ) Bit Definitions */
#define XZYNQ_XADC_SEQ_CH_CALIB	0x00000001 /**< ADC Calibration Channel */
#define XZYNQ_XADC_SEQ_CH_VCCPINT	0x00000020 /**< VCCPINT, Zynq Only */
#define XZYNQ_XADC_SEQ_CH_VCCPAUX	0x00000040 /**< VCCPAUX, Zynq Only */
#define XZYNQ_XADC_SEQ_CH_VCCPDRO	0x00000080 /**< VCCPDRO, Zynq Only */
#define XZYNQ_XADC_SEQ_CH_TEMP	0x00000100 /**< On Chip Temperature Channel */
#define XZYNQ_XADC_SEQ_CH_VCCINT	0x00000200 /**< VCCINT Channel */
#define XZYNQ_XADC_SEQ_CH_VCCAUX	0x00000400 /**< VCCAUX Channel */
#define XZYNQ_XADC_SEQ_CH_VPVN	0x00000800 /**< VP/VN analog inputs Channel */
#define XZYNQ_XADC_SEQ_CH_VREFP	0x00001000 /**< VREFP Channel */
#define XZYNQ_XADC_SEQ_CH_VREFN	0x00002000 /**< VREFN Channel */
#define XZYNQ_XADC_SEQ_CH_VBRAM	0x00004000 /**< VBRAM Channel, 7 series */
#define XZYNQ_XADC_SEQ_CH_AUX00	0x00010000 /**< 1st Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX01	0x00020000 /**< 2nd Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX02	0x00040000 /**< 3rd Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX03	0x00080000 /**< 4th Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX04	0x00100000 /**< 5th Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX05	0x00200000 /**< 6th Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX06	0x00400000 /**< 7th Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX07	0x00800000 /**< 8th Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX08	0x01000000 /**< 9th Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX09	0x02000000 /**< 10th Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX10	0x04000000 /**< 11th Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX11	0x08000000 /**< 12th Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX12	0x10000000 /**< 13th Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX13	0x20000000 /**< 14th Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX14	0x40000000 /**< 15th Aux Channel */
#define XZYNQ_XADC_SEQ_CH_AUX15	0x80000000 /**< 16th Aux Channel */

#define XZYNQ_XADC_SEQ00_CH_VALID_MASK  0x7FE1 /**< Mask for the valid channels */
#define XZYNQ_XADC_SEQ01_CH_VALID_MASK  0xFFFF /**< Mask for the valid channels */

#define XZYNQ_XADC_SEQ02_CH_VALID_MASK  0x7FE0 /**< Mask for the valid channels */
#define XZYNQ_XADC_SEQ03_CH_VALID_MASK  0xFFFF /**< Mask for the valid channels */

#define XZYNQ_XADC_SEQ04_CH_VALID_MASK  0x0800 /**< Mask for the valid channels */
#define XZYNQ_XADC_SEQ05_CH_VALID_MASK  0xFFFF /**< Mask for the valid channels */

#define XZYNQ_XADC_SEQ06_CH_VALID_MASK  0x0800 /**< Mask for the valid channels */
#define XZYNQ_XADC_SEQ07_CH_VALID_MASK  0xFFFF /**< Mask for the valid channels */


#define XZYNQ_XADC_SEQ_CH_AUX_SHIFT	16 /**< Shift for the Aux Channel */

/* OT Upper Alarm Threshold Register Bit Definitions */

#define XZYNQ_XADC_ATR_OT_UPPER_ENB_MASK	0x000F /**< Mask for OT enable */
#define XZYNQ_XADC_ATR_OT_UPPER_VAL_MASK	0xFFF0 /**< Mask for OT value */
#define XZYNQ_XADC_ATR_OT_UPPER_VAL_SHIFT	4      /**< Shift for OT value */
#define XZYNQ_XADC_ATR_OT_UPPER_ENB_VAL	0x0003 /**< Value for OT enable */
#define XZYNQ_XADC_ATR_OT_UPPER_VAL_MAX	0x0FFF /**< Max OT value */

/* JTAG DRP Bit Definitions */
#define XZYNQ_XADC_JTAG_DATA_MASK		0x0000FFFF /**< Mask for the Data */
#define XZYNQ_XADC_JTAG_ADDR_MASK		0x03FF0000 /**< Mask for the Addr */
#define XZYNQ_XADC_JTAG_ADDR_SHIFT		16	   /**< Shift for the Addr */
#define XZYNQ_XADC_JTAG_CMD_MASK		0x3C000000 /**< Mask for the Cmd */
#define XZYNQ_XADC_JTAG_CMD_WRITE_MASK	0x08000000 /**< Mask for CMD Write */
#define XZYNQ_XADC_JTAG_CMD_READ_MASK	0x04000000 /**< Mask for CMD Read */
#define XZYNQ_XADC_JTAG_CMD_SHIFT		26	   /**< Shift for the Cmd */

/* Reset Keys */
#define	XZYNQ_XADC_RESET_EN		0x10 //Reset Enabled
#define XZYNQ_XADC_RESET_DIS	0x00 //Reset Disabled

/* XADC Macros */

/*****************************************************************************/
/**
*
* Formats the data to be written to the the XADC registers.
*
* @param	offset is the offset of the Register
* @param	data is the data to be written to the Register if it is a write.
* @param	readwrite specifies whether it is a Read or a Write.
* 			Use 0 for Read, 1 for Write.
*
* @return	None.
*
******************************************************************************/
#define XADC_FORMAT_WRITE_DATA(offset, data, readwrite)							\
	((readwrite ? XZYNQ_XADC_JTAG_CMD_WRITE_MASK : XZYNQ_XADC_JTAG_CMD_READ_MASK) | 										\
	((offset << XZYNQ_XADC_JTAG_ADDR_SHIFT) & XZYNQ_XADC_JTAG_ADDR_MASK) | 	\
	(data & XZYNQ_XADC_JTAG_DATA_MASK))

/****************************************************************************/
/**
*
* This macro is used for writing to the XADC Registers using the
* command FIFO.
*
* @param	base is the memory mapped address of the devctrl register.
*
* @param	data is the data to be written
*
* @return	None.
*
*****************************************************************************/
#define XADC_WRITE_FIFO(base, data)		\
	out32(base + XZYNQ_XADC_CMDFIFO_OFFSET, data)

/****************************************************************************/
/**
*
* This macro is used for reading from the XADC Registers using the
* data FIFO.
*
* @param	base is the memory mapped address of the devctrl register
*
* @return	Data read from the FIFO
*
*****************************************************************************/
#define XADC_READ_FIFO(base)			\
	in32(base + XZYNQ_XADC_RDFIFO_OFFSET)


/* -------------------------------------------------------------------------
 * FPGA
 * Part of the Device Config Interface. All Offsets are based on the devcfg
 * base address.
 * -------------------------------------------------------------------------
 */

/* Register offsets */
#define XZYNQ_FPGA_CTRL_OFFSET	0x00 /*Control Register*/
#define XZYNQ_FPGA_LOCK_OFFSET		0x04 /* Lock Register */
#define XZYNQ_FPGA_CFG_OFFSET		0x08 /* Configuration Register */
#define XZYNQ_FPGA_INT_STS_OFFSET		0x0C /* Interrupt Status Register */
#define XZYNQ_FPGA_INT_MASK_OFFSET		0x10 /* Interrupt Mask Register */
#define XZYNQ_FPGA_STATUS_OFFSET		0x14 /* Status Register */
#define XZYNQ_FPGA_DMA_SRC_ADDR_OFFSET	0x18 /* DMA Source Address Register */
#define XZYNQ_FPGA_DMA_DEST_ADDR_OFFSET	0x1C /* DMA Destination Address Reg */
#define XZYNQ_FPGA_DMA_SRC_LEN_OFFSET	0x20 /* DMA Source Transfer Length */
#define XZYNQ_FPGA_DMA_DEST_LEN_OFFSET	0x24 /* DMA Destination Transfer */
#define XZYNQ_FPGA_ROM_SHADOW_OFFSET		0x28 /* DMA ROM Shadow Register */
#define XZYNQ_FPGA_MULTIBOOT_ADDR_OFFSET	0x2C /* Multi BootAddress Pointer */
#define XZYNQ_FPGA_SW_ID_OFFSET		0x30 /* Software ID Register */
#define XZYNQ_FPGA_UNLOCK_OFFSET		0x34 /* Unlock Register */
#define XZYNQ_FPGA_MCTRL_OFFSET		0x80 /* Miscellaneous Control Reg */

/* Control Masks */
#define XZYNQ_FPGA_CTRL_FORCE_RST_MASK	0x80000000 /* Force  into secure Reset */
#define XZYNQ_FPGA_CTRL_PCFG_PROG_B_MASK	0x40000000 /* Program signal to reset FPGA */
#define XZYNQ_FPGA_CTRL_PCAP_PR_MASK	  	0x08000000 /* Enable PCAP for PR */
#define XZYNQ_FPGA_CTRL_PCAP_MODE_MASK	0x04000000 /* Enable PCAP */
#define XZYNQ_FPGA_CTRL_PCAP_RATE_EN_MASK	0x02000000 /* Enable PCAP send data to FPGA every 4 PCAP cycles */
#define XZYNQ_FPGA_CTRL_MULTIBOOT_EN_MASK	0x01000000 /* Multiboot Enable */
#define XZYNQ_FPGA_CTRL_JTAG_CHAIN_DIS_MASK	0x00800000 /* JTAG Chain Disable */
#define XZYNQ_FPGA_CTRL_USER_MODE_MASK	0x00008000 /* User Mode Mask */
#define XZYNQ_FPGA_CTRL_PCFG_AES_EN_MASK	0x00000E00 /* AES Enable Mask */
#define XZYNQ_FPGA_CTRL_SEU_EN_MASK		0x00000100 /* SEU Enable Mask */
#define XZYNQ_FPGA_CTRL_SEC_EN_MASK		0x00000080 /* Secure/Non Secure status mask */
#define XZYNQ_FPGA_CTRL_SPNIDEN_MASK		0x00000040 /* Secure Non Invasive debug enable */
#define XZYNQ_FPGA_CTRL_SPIDEN_MASK		0x00000020 /* Secure Invasive Debug Enable */
#define XZYNQ_FPGA_CTRL_NIDEN_MASK		0x00000010 /* Non-Invasive Debug Enable */
#define XZYNQ_FPGA_CTRL_DBGEN_MASK		0x00000008 /* Invasive Debug Enable */
#define XZYNQ_FPGA_CTRL_DAP_EN_MASK		0x00000007 /* DAP Enable Mask */

/* Lock Masks */
#define XZYNQ_FPGA_LOCK_AES_EN_MASK		0x00000008 /* Lock AES_EN update */
#define XZYNQ_FPGA_LOCK_SEU_MASK		0x00000004 /* Lock SEU_En update */
#define XZYNQ_FPGA_LOCK_SEC_MASK		0x00000002 /* Lock SEC_EN and USER_MODE */
#define XZYNQ_FPGA_LOCK_DBG_MASK		0x00000001 /* This bit locks security config including: DAP_En, DBGEN, NIDEN, SPNIEN */

/* Config Masks */
#define XZYNQ_FPGA_CFG_RFIFO_TH_MASK	  	0x00000C00 /* Read FIFO Threshold Mask */
#define XZYNQ_FPGA_CFG_WFIFO_TH_MASK	  	0x00000300 /* Write FIFO Threshold Mask */
#define XZYNQ_FPGA_CFG_RCLK_EDGE_MASK	0x00000080 /* Read data active clock edge */
#define XZYNQ_FPGA_CFG_WCLK_EDGE_MASK	0x00000040 /* Write data active clock edge */
#define XZYNQ_FPGA_CFG_DISABLE_SRC_INC_MASK	0x00000020 /* Disable Source address increment mask */
#define XZYNQ_FPGA_CFG_DISABLE_DST_INC_MASK	0x00000010 /* Disable Destination address increment mask */
#define XZYNQ_FPGA_CFG_RD_ISSU_MASK		0x00000008 /* AXI read issuing capability */
#define XZYNQ_FPGA_CFG_WR_ISSU_MASK		0x00000004 /* AXI write issuing capability */
#define XZYNQ_FPGA_CFG_RDLEN_MASK	  	0x00000002 /* AXI Read burst length  Mask */
#define XZYNQ_FPGA_CFG_WRLEN_MASK	  	0x00000001 /* AXI write burst length  Mask */

/* Status Masks */
#define XZYNQ_FPGA_STATUS_DMA_CMD_Q_F_MASK	0x80000000 /* DMA command queue full */
#define XZYNQ_FPGA_STATUS_DMA_CMD_Q_E_MASK	0x40000000 /* DMA command queue empty */
#define XZYNQ_FPGA_STATUS_DMA_DONE_CNT_MASK	0x30000000 /* Number of completed DMA transfers */
#define XZYNQ_FPGA_STATUS_RX_FIFO_LVL_MASK	0x01F000000 /* Rx FIFO level */
#define XZYNQ_FPGA_STATUS_TX_FIFO_LVL_MASK	0x0007F000  /* Tx FIFO level */
#define XZYNQ_FPGA_STATUS_SECURE_RST_MASK	0x00000080  /* Secure Reset POR Status */
#define XZYNQ_FPGA_STATUS_ILLEGAL_APB_ACCESS_MASK 	0x00000040 /* Illegal APB access */
#define XZYNQ_FPGA_STATUS_PCFG_INIT_MASK		0x00000010 /* FPGA Init Status */
#define XZYNQ_FPGA_STATUS_EFUSE_SW_RESERVE_MASK	0x00000008 /* Efuse Reserved Status */
#define XZYNQ_FPGA_STATUS_EFUSE_SEC_EN_MASK		0x00000004 /* Efuse Security Enable Status */
#define XZYNQ_FPGA_STATUS_EFUSE_JTAG_DIS_MASK	0x00000002 /* EFuse JTAG Disable status */
#define XZYNQ_FPGA_STATUS_SECURE_DIS_MASK		0x00000001 /* Security Disable Status */

#define XZYNQ_FPGA_DMA_LEN_MASK		0x7FFFFFF /* Length Mask */

/* Miscellaneous Control Masks */
#define XZYNQ_FPGA_MCTRL_PCAP_LPBK_MASK	0x00000010 /* PCAP loopback mask */
#define XZYNQ_FPGA_MCTRL_RFIFO_FLUSH_MASK	0x00000002 /* Flush Rx FIFO */
#define XZYNQ_FPGA_MCTRL_WFIFO_FLUSH_MASK	0x00000001 /* Flush Tx FIFO */

/*Interrupt Masks */
#define XZYNQ_FPGA_IXR_AXI_WTO_MASK		0x00800000 /**< AXI Write Address or Data or response timeout */
#define XZYNQ_FPGA_IXR_AXI_WERR_MASK		0x00400000 /**< AXI Write response error */
#define XZYNQ_FPGA_IXR_AXI_RTO_MASK		0x00200000 /**< AXI Read Address or response timeout */
#define XZYNQ_FPGA_IXR_AXI_RERR_MASK		0x00100000 /**< AXI Read response error */
#define XZYNQ_FPGA_IXR_RX_FIFO_OV_MASK	0x00040000 /**< Rx FIFO Overflow */
#define XZYNQ_FPGA_IXR_WR_FIFO_LVL_MASK	0x00020000 /**< Tx FIFO less than threshold */
#define XZYNQ_FPGA_IXR_RD_FIFO_LVL_MASK	0x00010000 /**< Rx FIFO greater than threshold */
#define XZYNQ_FPGA_IXR_DMA_CMD_ERR_MASK	0x00008000 /**< Illegal DMA command */
#define XZYNQ_FPGA_IXR_DMA_Q_OV_MASK		0x00004000 /**< DMA command queue overflow */
#define XZYNQ_FPGA_IXR_DMA_DONE_MASK		0x00002000 /**< DMA Command Done */
#define XZYNQ_FPGA_IXR_D_P_DONE_MASK		0x00001000 /**< DMA and PCAP transfers Done */
#define XZYNQ_FPGA_IXR_P2D_LEN_ERR_MASK	0x00000800 /**< PCAP to DMA transfer length error */
#define XZYNQ_FPGA_IXR_PCFG_HMAC_ERR_MASK	0x00000040 /**< HMAC error mask */
#define XZYNQ_FPGA_IXR_PCFG_SEU_ERR_MASK	0x00000020 /**< SEU Error mask */
#define XZYNQ_FPGA_IXR_PCFG_POR_B_MASK	0x00000010 /**< FPGA POR mask */
#define XZYNQ_FPGA_IXR_PCFG_CFG_RST_MASK	0x00000008 /**< FPGA Reset mask */
#define XZYNQ_FPGA_IXR_PCFG_DONE_MASK	0x00000004 /**< Done Signal  Mask */
#define XZYNQ_FPGA_IXR_PCFG_INIT_PE_MASK	0x00000002 /**< Detect Positive edge of Init Signal */
#define XZYNQ_FPGA_IXR_PCFG_INIT_NE_MASK  	0x00000001 /**< Detect Negative edge of Init Signal */
#define XZYNQ_FPGA_IXR_ERROR_FLAGS_MASK		(XZYNQ_FPGA_IXR_AXI_WTO_MASK | \
						XZYNQ_FPGA_IXR_AXI_WERR_MASK | \
						XZYNQ_FPGA_IXR_AXI_RTO_MASK |  \
						XZYNQ_FPGA_IXR_AXI_RERR_MASK | \
						XZYNQ_FPGA_IXR_RX_FIFO_OV_MASK | \
						XZYNQ_FPGA_IXR_DMA_CMD_ERR_MASK |\
						XZYNQ_FPGA_IXR_DMA_Q_OV_MASK |   \
						XZYNQ_FPGA_IXR_P2D_LEN_ERR_MASK |\
						XZYNQ_FPGA_IXR_PCFG_HMAC_ERR_MASK)
#define XZYNQ_FPGA_IXR_ALL_MASK			0x00F7F8EF


#define XZYNQ_FPGA_DMA_INVALID_ADDRESS 0xFFFFFFFF

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/arm/xzynq.h $ $Rev: 752035 $")
#endif
