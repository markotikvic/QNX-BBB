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

/*
 * J5ECO (DRx62x)
 */

#ifndef	__ARM_DRx62x_H_INCLUDED
#define	__ARM_DRx62x_H_INCLUDED

#define SRAM0_START						0x40300000
#define	DRx62x_EDMA_BASE				0x49000000

#define DRx62x_MMCHS0_BASE				0x48060000
#define DRx62x_MMCHS1_BASE				0x481D8000
#define DRx62x_MMCHS2_BASE				0x47810000

#define DRx62x_UART0_BASE			 	0x48020000
#define DRx62x_UART1_BASE			 	0x48022000
#define DRx62x_UART2_BASE			 	0x48024000
#define DRx62x_UART3_BASE			 	0x481A6000
#define DRx62x_UART4_BASE			 	0x481A8000
#define DRx62x_UART5_BASE				0x481AA000
#define DRx62x_UART6_BASE			 	0x48440000
#define DRx62x_UART7_BASE			 	0x48442000

#define USBSS_BASE_REG					0x47400000
#define USB0_BASE_REG					0x47401000
#define USB1_BASE_REG					0x47401800
#define MENTOR_CORE_OFF			 		0x400

// Timer
#define DRx62x_WDTIMER0_BASE			0x481C7000
#define WDTIMER_SIZE 					0x100

/* DM Timer base addresses */
#define DRx62x_TIMER_SIZE			 	0x1000
#define DRx62x_TIMER0_BASE				0x4802C000
#define DRx62x_TIMER1_BASE				0x4802E000
#define DRx62x_TIMER2_BASE				0x48040000
#define DRx62x_TIMER3_BASE				0x48042000
#define DRx62x_TIMER4_BASE				0x48044000
#define DRx62x_TIMER5_BASE				0x48046000
#define DRx62x_TIMER6_BASE				0x48048000
#define DRx62x_TIMER7_BASE				0x4804A000
#define DRx62x_TIMER8_BASE				0x481c1000

/* GPIO Base address */
#define DRx62x_GPIO0_BASE				0x48032000
#define DRx62x_GPIO1_BASE				0x4804C000
#define DRx62x_GPIO2_BASE				0x481AC000
#define DRx62x_GPIO3_BASE			 	0x481AE000

/* SPI Base address, the additional 0x100 is for historic reason */
#define DRx62x_SPI0_BASE				0x48030100
#define DRx62x_SPI1_BASE				0x481A0100
#define DRx62x_SPI2_BASE				0x481A2100
#define DRx62x_SPI3_BASE				0x481A4100

/* I2C Base address */
#define DRx62x_I2C0_BASE				0x48028000
#define DRx62x_I2C2_BASE				0x4802A000
#define DRx62x_I2C3_BASE				0x4819C000
#define DRx62x_I2C4_BASE				0x4819E000

/* BCH Error Location Module */
#define ELM_BASE						0x48080000

/* EMIF Base address */
#define EMIF4_0_CFG_BASE				0x4C000000
#define EMIF4_1_CFG_BASE				0x4D000000
#define DMM_BASE						0x4E000000

/* GPMC Base address */
#define GPMC_BASE						0x50000000

/* rtc */
#define DRx62x_RTC_BASE					0x480C0000
#define	DRx62x_RTC_SIZE			 		0x1000

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/arm/drx62x.h $ $Rev: 716693 $")
#endif
