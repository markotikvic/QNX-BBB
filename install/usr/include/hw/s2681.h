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
 *  hw/s2681.h
 *
 *  Register definitions for a Dual Asynchronous Receiver/Transmitter scn2681
 *
 */
#ifndef _HW_S2681_INCLUDED
#define _HW_S2681_INCLUDED

#define  S2681_REG_MR1A		0	/*RW   modeA,         00     04   */
#define  S2681_REG_MR2A		0	/*RW   modeA,         00     04   */
#define  S2681_REG_SRA		1   /*R   statusA,        01     0C   */
#define  S2681_REG_CSRA		1   /*W   statusA,        01     0C   */
#define  S2681_REG_CRA		2   /*W   commandA,       02     14   */
#define  S2681_REG_RHRA		3   /*R   dataA,          03     1C   */
#define  S2681_REG_THRA		3   /*W   dataA,          03     1C   */
#define  S2681_REG_IPCR		4   /*R   portchange,     04     24   */
#define  S2681_REG_ACR		4   /*W   portchange,     04     24   */
#define  S2681_REG_ISR		5   /*R   interrupt,      05     2C   */
#define  S2681_REG_IMR		5   /*W   interrupt,      05     2C   */
#define  S2681_REG_CTU		6   /*R   CTUpper,        06     34   */
#define  S2681_REG_CTL		7   /*R   CTLower,        07     3C   */
#define  S2681_REG_CTUR		6   /*W   CTUpper,        06     34   */
#define  S2681_REG_CTLR		7   /*W   CTLower,        07     3C   */
#define  S2681_REG_MR1B		8   /*RW   modeB,         08     44   */
#define  S2681_REG_MR2B		8   /*RW   modeB,         08     44   */
#define  S2681_REG_SRB		9   /*R   statusB,        09     4C   */
#define  S2681_REG_CSRB		9   /*W   statusB,        09     4C   */
#define  S2681_REG_CRB		10  /*W   commandB,       0A     54   */
#define  S2681_REG_RHRB		11  /*R   dataB,          0B     5C   */
#define  S2681_REG_THRB		11  /*W   dataB,          0B     5C   */
#define  S2681_REG_OPCR		13  /*W   portstatus,     0D     6C   */
#define  S2681_INPUTPORT	13  /*R   portstatus,     0D     6C   */
#define  S2681_SETPORT_CMD	14  /*R   setport,        0E     74   */
#define  S2681_RESETPORT_CMD 15	/*R   resetport;      0F     7C   */
#define  S2681_START_COUNTER_CMD 14 /*W   setport,    0E     74   */
#define  S2681_STOP_COUNTER_CMD	15 /*W   resetport;   0F     7C   */
#define  S2681_MEM_SIZE		16

/*
 * Mode register #1 bit definitions
 */
#define S2681_MR1_DBITS5		0
#define S2681_MR1_DBITS6		1
#define S2681_MR1_DBITS7		2
#define S2681_MR1_DBITS8		3

#define S2681_MR1_PRTYODD		4		/* Only applies when parity is on */
#define S2681_MR1_PRTYEVEN	0

#define S2681_MR1_PRTMODE	24
#define S2681_MR1_PRTYON		0
#define S2681_MR1_PRTYFORCE	8
#define S2681_MR1_PRTYOFF		16

#define S2681_MR1_MULTIDROP	24		/* weird operating mode - ignore  */

#define S2681_MR1_CHARERROR	0		/* received character error mode  */
#define S2681_MR1_BLOCKERROR	32

#define S2681_MR1_RXRDYINTR	0		/* interrupt on RxRdy		  */
#define S2681_MR1_RXFULLINTR	64		/* interrupt on Rx Fifo Full	  */

#define S2681_MR1_RXRTSCONTROL	128		/* RTS will drop if the Fifo fills*/


/*
 * Mode register #2 bit definitions
 */

#define S2681_MR2_STOP9X16	0		/* Stop bit(s) range from:	   */
#define S2681_MR2_STOP10X16	1		/* 9/16 to 1, and 25/16 to 2	   */
#define S2681_MR2_STOP11X16	2
#define S2681_MR2_STOP12X16	3
#define S2681_MR2_STOP13X16	4
#define S2681_MR2_STOP14X16	5
#define S2681_MR2_STOP15X16	6
#define S2681_MR2_STOP16X16	7
#define S2681_MR2_STOP25X16	8
#define S2681_MR2_STOP26X16	9
#define S2681_MR2_STOP27X16	10
#define S2681_MR2_STOP28X16	11
#define S2681_MR2_STOP29X16	12
#define S2681_MR2_STOP30X16	13
#define S2681_MR2_STOP31X16	14
#define S2681_MR2_STOP32X16	15

#define S2681_MR2_TXCTSCONTROL	16		/* To transmit, CTS must be high  */
#define S2681_MR2_TXRTSCONTROL	32		/* RTS will drop if Tx is disabled */

#define S2681_MR2_MODENORMAL	0
#define S2681_MR2_MODEECHO	64
#define S2681_MR2_MODELOCALLOOP	128
#define S2681_MR2_MODEREMOTELOOP	192


/*
 * Clock Select register bit definitions
 */

#define S2681_CSR_TXCLOCK		1	/* use these as multipliers:		   */
#define S2681_CSR_RXCLOCK		16	/* txclock*S0baud50 + rxclock*S0baud50	   */

#define S2681_CSR_S0BAUD50	0
#define S2681_CSR_S0BAUD110	1
#define S2681_CSR_S0BAUD134	2		/* The S0 or S1 refers to the baud */
#define S2681_CSR_S0BAUD200	3		/* rate generator set select bit,  */
#define S2681_CSR_S0BAUD300	4		/* ACR bit 07.			   */
#define S2681_CSR_S0BAUD600	5
#define S2681_CSR_S0BAUD1200	6
#define S2681_CSR_S0BAUD1050	7
#define S2681_CSR_S0BAUD2400	8
#define S2681_CSR_S0BAUD4800	9
#define S2681_CSR_S0BAUD7200	10
#define S2681_CSR_S0BAUD9600	11
#define S2681_CSR_S0BAUD38400	12
#define S2681_CSR_S0BAUDTIMER     13
#define S2681_CSR_S0BAUDEXT16X    14
#define S2681_CSR_S0BAUDEXT1X     15

#define S2681_CSR_S1BAUD75	0
#define S2681_CSR_S1BAUD110	1
#define S2681_CSR_S1BAUD134	2
#define S2681_CSR_S1BAUD150	3
#define S2681_CSR_S1BAUD300	4
#define S2681_CSR_S1BAUD600	5
#define S2681_CSR_S1BAUD1200	6
#define S2681_CSR_S1BAUD2000	7
#define S2681_CSR_S1BAUD2400	8
#define S2681_CSR_S1BAUD4800	9
#define S2681_CSR_S1BAUD1800	10
#define S2681_CSR_S1BAUD9600	11
#define S2681_CSR_S1BAUD19200	12
#define S2681_CSR_S1BAUDTIMER     13
#define S2681_CSR_S1BAUDEXT16X    14
#define S2681_CSR_S1BAUDEXT1X     15

/*
 * Status register bit definitions
 */
#define S2681_SR_RXREADY		1	/* Rx and Tx status	   */
#define S2681_SR_FIFOFULL	2
#define S2681_SR_TXREADY		4
#define S2681_SR_TXEMPTY		8
#define S2681_SR_OVRRNERROR	16
#define S2681_SR_PRTYERROR	32	/* Fifo character status   */
#define S2681_SR_FRMERROR	64
#define S2681_SR_RCVBREAK	128


/*
 * Command register bit definitions
 */

#define S2681_CR_STP_RXENABLE	1		/* Rx and Tx enable and disable	*/
#define S2681_CR_RXDISABLE	2
#define S2681_CR_STP_TXENABLE	4
#define S2681_CR_TXDISABLE	8
#define S2681_CR_RESETMRPTR	16		/* channel commands */
#define S2681_CR_RESETRX		32
#define S2681_CR_RESETTX		48
#define S2681_CR_RESETERROR	64
#define S2681_CR_RESETDBREAK	80
#define S2681_CR_STARTBREAK	96
#define S2681_CR_STOPBREAK	112

/*
 * Interrupt Status and Mask register
 */
#define S2681_ISMR_TXRDYA		1
#define S2681_ISMR_RXRDYA		2
#define S2681_ISMR_BREAKXA		4
#define S2681_ISMR_CNTRREADY	8
#define S2681_ISMR_TXRDYB		16
#define S2681_ISMR_RXRDYB		32	/* Fifo character status   */
#define S2681_ISMR_BREAKXB		64
#define S2681_ISMR_MODEMX		128

/*
 * OPR bits:
 */
#define S2681_CDA		0x01	/* Port A Carrier detect */

/*
 * ACR register (write to a.aux1):
 */

#define S2681_ACR_CTEXT	    	0	/* External clock */
#define S2681_ACR_CTTXCA	    	0x10	/* TxCA 1X A clock */
#define S2681_ACR_CTTXCB	    	0x20	/* TxCB 1X B clock */
#define S2681_ACR_CTXTAL	    	0x30	/* Crystal or External clock divided by 16 */
#define S2681_ACR_TIMEXT	    	0x40	/* External IP2 1X */
#define S2681_ACR_TIMEXTDIV16 	0x50	/* External IP2 16X */
#define S2681_ACR_TIMXTAL	    	0x60	/* Crystal or External clock (X1/CLK) */
#define S2681_ACR_TIMXTALDIV16	0x70	/* Crystal or External clock (X1/CLK) / 16 */
#define S2681_ACR_BRGSET0	    	0	/* Baud rate set 0 */
#define S2681_ACR_BRGSET1	    	0x80	/* Baud rate set 1 */

/*
 * Input port register
 */
#define S2681_IPCR_RINGA           0x01    /* Port A RING (hardware name is IDTRA) */
#define S2681_IPCR_CTSB            0x02    /* Port B CTS */
#define S2681_IPCR_RINGB           0x08    /* Port B Ring indication */
#define S2681_IPCR_DCDB            0x10    /* Port B DCD */
#define S2681_IPCR_TXCLKB          0x20    /* Port B TX clock input */
#define S2681_IPCR_RXCLKB          0x40    /* Port B RX clock input */

/*
 * Output port set/reset registers
 */
#define S2681_DTRA            0x01    /* Port A DTR (hardware name is CDA) */
#define S2681_RTSB            0x02    /* Port B RTS */
#define S2681_TXCLKA          0x04    /* Port A TX clock */
#define S2681_DTRB            0x08    /* Port B DTR */

#endif /* _S2681_H */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/public/hw/s2681.h $ $Rev: 680332 $")
#endif
