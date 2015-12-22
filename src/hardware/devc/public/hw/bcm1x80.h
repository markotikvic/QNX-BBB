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
 * Definitions for Broadcom BCM1X80 DUART serial device
 *
 */

#if defined (__LITTLEENDIAN__)
	#define __OFFSET(x)	(x)
#elif defined (__BIGENDIAN__)
	#define __OFFSET(x)	((x)+7)
#else
    #error ENDIAN Not defined for system
#endif


#define BCM1x80_DUART_REGSIZE	0x300

/* __OFFSET definitions for registers of DUART A/B */
#define	BCM1x80_DUART_MODE1		__OFFSET(0x00)	/* RW  	Mode reg1 */
#define BCM1x80_DUART_MODE2		__OFFSET(0x10) 	/* RW  	Mode reg2 */
#define BCM1x80_DUART_STATUS	__OFFSET(0x20) 	/* R	Status reg */
#define BCM1x80_DUART_FULLCTL	__OFFSET(0x40)	/* RW	Full control */
#define BCM1x80_DUART_CMD		__OFFSET(0x50)	/* RW	Command reg */
#define BCM1x80_DUART_RX		__OFFSET(0x60)	/* R	RX holding reg */
#define BCM1x80_DUART_TX		__OFFSET(0x70)	/* W	TX holding reg */

/*** special case ***/
/* the baud rate clock reg is implemented 12 bits, and needs to be writen as a word */
#if defined (__LITTLEENDIAN__)
 #define BCM1x80_DUART_CLKSEL	__OFFSET(0x30)	/* RW	Clock select reg */
#elif defined (__BIGENDIAN__)
 #define BCM1x80_DUART_CLKSEL	__OFFSET(0x30-1)
#endif

/* __OFFSET relative to DUART A base */
#define BCM1x80_DUART_ISRA		__OFFSET(0x220)	/* R	Interrupt Status Reg port A */
#define BCM1x80_DUART_IMRA		__OFFSET(0x230)	/* RW	Interrupt Mask Reg  port A */
#define BCM1x80_DUART_OPA       __OFFSET(0x2B0) /* W    Output Port 0 Set Register A/B (DTR/RTS control) */
/* __OFFSET relative to DUART B base */
#define BCM1x80_DUART_ISRB		__OFFSET(0x140)	/* R 	Interrupt Status REG port B */
#define BCM1x80_DUART_IMRB		__OFFSET(0x150)	/* RW	Interrupt Mask Reg  port B */
#define BCM1x80_DUART_OPB       __OFFSET(0x1B0) /* W    Output Port 0 Clear Register A/B (DTR/RTS control) */
/* Offset relative to OP Register set */
#define BCM1x80_DUART_OP_SET    0x0             /* W    Offset of OP SET Register (relative to OP Register set) */
#define BCM1x80_DUART_OP_CLR    0x10            /* W    Offset of OP CLR Register (relative to OP Register set) */

/* Bit definitions for Mode1 Reg */
#define BCM1x80_MODE1_BITS_PER_CHAR(x)	((x) & 0x3)
#define BCM1x80_MODE1_PARITY_ODD		(1 << 2)
#define BCM1x80_MODE1_PARITY_EVEN 0x00
#define BCM1x80_MODE1_PARITY_NONE (1 << 4)
#define BCM1x80_MODE1_RX_IRQSEL_RXFULL	(1 << 6)
#define BCM1x80_MODE1_RX_RTSENA			(1 << 7)

/* Bit definitions for Mode2 Reg */
#define BCM1x80_MODE2_STOPBIT_LEN2		(1 << 3)
#define BCM1x80_MODE2_TX_CTSENA			(1 << 4)
#define BCM1x80_MODE2_CHAN_MODE(x)		((x) & 0xc0)


/* Bit definitions for Command reg */
#define BCM1x80_CMD_RX_EN			0x01
#define BCM1x80_CMD_RX_DIS			0x02
#define BCM1x80_CMD_TX_EN			0x04
#define BCM1x80_CMD_TX_DIS			0x08
#define BCM1x80_CMD_RX_RESET		0x20
#define BCM1x80_CMD_TX_RESET		0x30
#define BCM1x80_CMD_BRKIRQ_RESET	0x50
#define BCM1x80_CMD_BRK_START		0x60
#define BCM1x80_CMD_BRK_STOP		0x70


/* Bit definitions for Status reg */
#define BCM1x80_STATUS_RX_RDY		0x01
#define BCM1x80_STATUS_RX_FFUL		0x02
#define BCM1x80_STATUS_TX_RDY		0x04
#define BCM1x80_STATUS_TX_EMT		0x08
#define BCM1x80_STATUS_OVRUN_ERR	0x10
#define BCM1x80_STATUS_PARITY_ERR	0x20
#define BCM1x80_STATUS_FRM_ERR		0x40
#define BCM1x80_STATUS_RCVD_BRK		0x80


/* Bit definitions for interrupt status reg */
#define BCM1x80_ISR_TX_RDY			0x01
#define BCM1x80_ISR_RX_RDY			0x02
#define BCM1x80_ISR_BRK				0x04
#define BCM1x80_ISR_INPUT			0x08

/* Bit definitions for Output Port Regs */
#define BCM1x80_RTSA            0x1
#define BCM1x80_RTSB            0x2
#define BCM1x80_DTRA            0x4
#define BCM1x80_DTRB            0x8

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/public/hw/bcm1x80.h $ $Rev: 680332 $")
#endif
