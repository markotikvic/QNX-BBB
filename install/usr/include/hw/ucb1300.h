/*
 * $QNXLicenseC:
 * Copyright 2008, QNX Software Systems. 
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
 * Philips UCB1300 codec/touchscreen interface
 *
 */

#ifndef	__HW_UCB1300_H_INCLUDED
#define	__HW_UCB1300_H_INCLUDED

/*
 * Register numbers
 */
#define	UCB1300_IO_DATA		0			/* I/O port data */
#define	UCB1300_IO_DIR		1			/* I/O port direction */
#define	UCB1300_REI			2			/* rising edge interrupt enable */
#define	UCB1300_FEI			3			/* falling edge interrupt enable */
#define	UCB1300_ISC			4			/* interrupt status/clear */
#define	UCB1300_TCRA		5			/* telecom control register A */
#define	UCB1300_TCRB		6			/* telecom control register B */
#define	UCB1300_ACRA		7			/* audio control register A */
#define	UCB1300_ACRB		8			/* audio control register B */
#define	UCB1300_TSCR		9			/* touchscreen control register */
#define	UCB1300_ADCR		10			/* ADC control register */
#define	UCB1300_ADDR		11			/* ADC data register */
#define	UCB1300_ID			12			/* ID register */
#define	UCB1300_MODE		13			/* mode register */

/*
 * Bits in IO_DATA/IO_DIR registers
 */
#define	UCB1300_IO_0		0x0001
#define	UCB1300_IO_1		0x0002
#define	UCB1300_IO_2		0x0004
#define	UCB1300_IO_3		0x0008
#define	UCB1300_IO_4		0x0010
#define	UCB1300_IO_5		0x0020
#define	UCB1300_IO_6		0x0040
#define	UCB1300_IO_7		0x0080
#define	UCB1300_IO_8		0x0100
#define	UCB1300_IO_9		0x0200
#define	UCB1300_IO_SIB		0x8000		/* force SIBDOUT to 0 (IO_DIR only) */

/*
 * Bits in REI/FEI/ISC registers
 */
#define	UCB1300_INT_IO_0	0x0001
#define	UCB1300_INT_IO_1	0x0002
#define	UCB1300_INT_IO_2	0x0004
#define	UCB1300_INT_IO_3	0x0008
#define	UCB1300_INT_IO_4	0x0010
#define	UCB1300_INT_IO_5	0x0020
#define	UCB1300_INT_IO_6	0x0040
#define	UCB1300_INT_IO_7	0x0080
#define	UCB1300_INT_IO_8	0x0100
#define	UCB1300_INT_IO_9	0x0200
#define	UCB1300_INT_ADC		0x0800
#define	UCB1300_INT_TSPX	0x1000
#define	UCB1300_INT_TSMX	0x2000
#define	UCB1300_INT_TCLIP	0x4000
#define	UCB1300_INT_ACLIP	0x8000

/*
 * Bits in TSCR register
 */
#define	UCB1300_TSCR_TSMX_POW	0x0001	/* power TSMX pin */
#define	UCB1300_TSCR_TSPX_POW	0x0002	/* power TSPX pin */
#define	UCB1300_TSCR_TSMY_POW	0x0004	/* power TSMY pin */
#define	UCB1300_TSCR_TSPY_POW	0x0008	/* power TSPY pin */
#define	UCB1300_TSCR_TSMX_GND	0x0010	/* ground TSMX pin */
#define	UCB1300_TSCR_TSPX_GND	0x0020	/* ground TSPX pin */
#define	UCB1300_TSCR_TSMY_GND	0x0040	/* ground TSMY pin */
#define	UCB1300_TSCR_TSPY_GND	0x0080	/* ground TSPY pin */
#define	UCB1300_TSCR_PRESSURE	0x0100	/* pressure measurement mode */
#define	UCB1300_TSCR_POSITION	0x0200	/* position measurement mode */
#define	UCB1300_TSCR_BIAS		0x0800	/* activate bias */
#define	UCB1300_TSCR_TSPX_LOW	0x1000	/* inverted TSPX pin state */
#define	UCB1300_TSCR_TSMX_LOW	0x2000	/* inverted TSMX pin state */

/*
 * Bits in ADCR register
 */
#define	UCB1300_ADCR_SYNC		0x0001	/* activate sync mode */
#define	UCB1300_ADCR_VREFBYP	0x0002	/* connect reference voltage to VREFBYP */
#define	UCB1300_ADCR_TSPX		0x0000	/* select TSPX input */
#define	UCB1300_ADCR_TSMX		0x0004	/* select TSMX input */
#define	UCB1300_ADCR_TSPY		0x0008	/* select TSPY input */
#define	UCB1300_ADCR_TSMY		0x000c	/* select TSMY input */
#define	UCB1300_ADCR_AD0		0x0010	/* select AD0 input */
#define	UCB1300_ADCR_AD1		0x0014	/* select AD1 input */
#define	UCB1300_ADCR_AD2		0x0018	/* select AD2 input */
#define	UCB1300_ADCR_AD3		0x001c	/* select AD3 input */
#define	UCB1300_ADCR_EXT		0x0020	/* external voltage reference on VREFBYP */
#define	UCB1300_ADCR_START		0x0080	/* start ADC conversion */
#define	UCB1300_ADCR_ENA		0x8000	/* enable ADC */

/*
 * Bits in ADDR register
 */
#define	UCB1300_ADDR_VAL		0x8000		/* data is valid */
#define	UCB1300_ADC_DATA(x)		(((x) & ~UCB1300_ADDR_VAL) >> 5)

#endif	/* __HW_UCB1300_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/public/hw/ucb1300.h $ $Rev: 680332 $")
#endif
