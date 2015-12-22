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


#include <sys/srcversion.h>






/*
 *  hw/dsiu.h
 *
 *  Register definitions for a DSIU (DEBUG SERIAL INTERFACE UNIT)
 *
 */
#ifndef _HW_DSIU_INCLUDED
#define _HW_DSIU_INCLUDED

#define  DSIU_PORTREG		0	/*RW Port Change Register 			(0x0B0001A0)*/
#define  DSIU_MODEMREG		1	/*R	 Modem Control Register			(0x0B0001A2)*/
#define  DSIU_ASIM00REG		2	/*RW Asynchronous Mode 0 Register	(0x0B0001A4)*/
#define  DSIU_ASIM01REG		3	/*RW Asynchronous Mode 1 Register	(0x0B0001A6)*/
#define  DSIU_RXB0RREG		4	/*R	 Receive Buffer Register (Extented)	(0x0B0001A8)*/
#define  DSIU_RXB0LREG		5	/*R	 Receive Buffer Register		(0x0B0001AA)*/
#define  DSIU_TXS0RREG		6	/*RW Transmit Data Register (Extended)	(0x0B0001AC)*/
#define  DSIU_TXS0LREG		7	/*RW Transmit Data Register			(0x0B0001AE)*/
#define  DSIU_ASIS0REG		8	/*R	 Status Register				(0x0B0001B0)*/
#define  DSIU_INTR0REG		9	/*RW Debug SIU Interrupt Register	(0x0B0001B2)*/
#define  DSIU_RFU1			10	/*	Reserved for Future Use			(0x0B0001B4)*/
#define  DSIU_BPRM0REG		11	/*RW Baud rate Generator Prescaler Mode Register (0x0B0001B6)*/
#define  DSIU_DSIURESETREG	12	/*RW Debug SIU Reset Register		(0x0B0001B8)*/
#define  DSIU_RFU2			13	/*	Reserved for Future Use			(0x0B0001BA)*/
#define  DSIU_RFU3			14	/*	Reserved for Future Use			(0x0B0001BC)*/
#define  DSIU_RFU4			15	/*	Reserved for Future Use 		(0x0B0001BE)*/

#define  CMU_CMUCLKMSK		16	/* Clock Mask Register (Clock Mask Unit) */
#define  ICU_MSYSINT2REG	17	/* Level 1 mask system interrupt register 2 (ICU) */
#define  ICU_MDSIUINTREG	18	/* Level 2 mask DSIU interrupt register (ICU) */

#define  DSIU_MEM_SIZE		16

/*
 * Port Change Register bit definitions
 */

#define DSIU_PORTREG_CDCTS	1
#define DSIU_PORTREG_CDRTS	2
#define DSIU_PORTREG_CDDOUT	4
#define DSIU_PORTREG_CDDIN	8



/*
 * Modem Control Register bit definitions
 */

#define DSIU_MODEMREG_DCTS	1
#define DSIU_MODEMREG_DRTS	2

/*
 * Asynchronous Mode 0 Register bit definitions
 */

#define DSIU_ASIM00REG_SL0		 4	/* stop bit 1: 2 bits 0: 1 bit */
#define DSIU_ASIM00REG_CL0	 	 8	/* character length 1: 8 bits 0: 7 bits */
#define DSIU_ASIM00REG_PS0		48	/* parity (00: no parity. Set to 00 for extended-bit operations) */
#define DSIU_ASIM00REG_PS0_ZERO	16	/*	01: Zero parity bits during transmit No parity during receive */
#define DSIU_ASIM00REG_PS0_ODD	32	/*	10: odd */
#define DSIU_ASIM00REG_PS0_EVEN	48	/*	11: even 10 */
#define DSIU_ASIM00REG_RXE0		64	/* receive enable */
#define DSIU_ASIM00REG_BIT7	   128	/* should be 1 */

/*
 * Asynchronous Mode 1 Register bit definitions
 */

#define DSIU_ASIM01REG_EBS0	1	/* extended bit operation enable */

/*
 * Receive Buffer Register (Extented) bit definitions
 */

#define DSIU_RXB0RREG_RXB0	0x1ff

/*
 * Receive Buffer Register bit definitions
 */

#define DSIU_RXB0LREG_RXB0L	0xff

/*
 * Transmit Data Register (Extended) bit definitions
 */

#define DSIU_TXS0RREG_TXS0	0x1ff

/*
 * Transmit Data Register bit definitions
 */

#define DSIU_TXS0LREG_TXS0	0xff

/*
 * Status Register bit definitions
 */

#define DSIU_ASIS0REG_OVE0	1
#define DSIU_ASIS0REG_FE0	2
#define DSIU_ASIS0REG_PE0	4
#define DSIU_ASIS0REG_SOT0	0x80

/*
 * Debug SIU Interrupt Register bit definitions
 */

#define DSIU_INTR0REG_INTST0	1
#define DSIU_INTR0REG_INTSR0	2
#define DSIU_INTR0REG_INTSER0	4
#define DSIU_INTR0REG_INTDCD	8

/*
 * Baud rate Generator Prescaler Mode Register bit definitions
 */

#define DSIU_BPRM0REG_BPR0	 0x7	/* baud rate setting */
#define DSIU_BPRM0REG_BRCE0	0x80	/* baud rate generator count enable */

/*
 * Debug SIU Reset Register bit definitions
 */

#define DSIU_DSIURESETREG_DSIURST	1

/*
 * Clock Mask Register bit definitions
 */

#define CMU_CMUCLKMSK_MSKDSIU	0x20

/*
 * Level 1 mask system interrupt register 2 (ICU) bit definitions
 */

#define ICU_MSYSINT2REG_DSIUINTR	0x10

/*
 * Level 2 mask DSIU interrupt register (ICU) bit definitions
 */

#define ICU_MDSIUINTREG_INTDCTS		0x800
#define ICU_MDSIUINTREG_INTSER0		0x400
#define ICU_MDSIUINTREG_INTSR0		0x200
#define ICU_MDSIUINTREG_INTST0		0x100

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/public/hw/dsiu.h $ $Rev: 680332 $")
#endif
