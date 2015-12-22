
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

/******************************************************************************
*   Copyright (C) 2010-2011 Renesas Electronics Corporation. All rights reserved.
******************************************************************************/
#define _BITFIELD32L(__start_bit,__value)	((__value) << (__start_bit))
#define _BITFIELD16L(__start_bit,__value)	((__value) << (__start_bit))
#define _BITFIELD8L(__start_bit,__value)	((__value) << (__start_bit))

/* registers addresses */
#define	SH_SCIF_BASE			0xffe41000
#define	SH_SCIF_SCSMR_OFF		0x0
#define	SH_SCIF_SCBRR_OFF		0x4
#define	SH_SCIF_SCSCR_OFF		0x8
#define	SH_SCIF_SCFTDR_OFF		0xC
#define	SH_SCIF_SCFSR_OFF		0x10
#define	SH_SCIF_SCFRDR_OFF		0x14
#define	SH_SCIF_SCFCR_OFF		0x18
#define	SH_SCIF_SCFDR_OFF		0x1c
#define	SH_SCIF_SCSPTR_OFF		0x20
#define	SH_SCIF_SCLSR_OFF		0x24

#define	SH_SCIF_HSSRR_OFF		0x40				//Sampling rate register
#define	SH_SCIF_HSRER_OFF		0x44				//Serial error register
#define	SH_SCIF_HSRTGR_OFF		0x50				//RTS output active trigger count register
#define	SH_SCIF_HSRTRGR_OFF		0x54				//Receive FIFO data count trigger register
#define	SH_SCIF_HSTTRGR_OFF		0x58				//Transmit FIFO data count trigger register
/* SCSMR */
#define	SH_SCIF_SCSMR_CHR		(1<<6)
#define	SH_SCIF_SCSMR_PE		(1<<5)
#define	SH_SCIF_SCSMR_OE		(1<<4)
#define	SH_SCIF_SCSMR_STOP		(1<<3)
#define	SH_SCIF_SCSMR_CKS_M		_BITFIELD16L(0,3)
#define	SH_SCIF_SCSMR_CKS_0		_BITFIELD16L(0,0)
#define	SH_SCIF_SCSMR_CKS_4		_BITFIELD16L(0,1)
#define	SH_SCIF_SCSMR_CKS_16	        _BITFIELD16L(0,2)
#define	SH_SCIF_SCSMR_CKS_64	        _BITFIELD16L(0,3)

/* SCSCR */

#define	SH_SCIF_SCSCR_TEIE		(1<<11)
#define	SH_SCIF_SCSCR_TIE		(1<<7)
#define	SH_SCIF_SCSCR_RIE		(1<<6)
#define	SH_SCIF_SCSCR_TE		(1<<5)
#define	SH_SCIF_SCSCR_RE		(1<<4)
#define	SH_SCIF_SCSCR_REIE		(1<<3)
#define	SH_SCIF_SCSCR_CKE1		(1<<1)

/* SCFSR */
#define	SH_SCIF_SCFSR_PERF(x)	((x>>12)&0xf)
#define	SH_SCIF_SCFSR_FERF(x)	((x>>8)&0xf)
#define	SH_SCIF_SCFSR_ER		(1<<7)
#define	SH_SCIF_SCFSR_TEND		(1<<6)
#define	SH_SCIF_SCFSR_TDFE		(1<<5)
#define	SH_SCIF_SCFSR_BRK		(1<<4)
#define	SH_SCIF_SCFSR_FER		(1<<3)
#define	SH_SCIF_SCFSR_PER		(1<<2)
#define	SH_SCIF_SCFSR_RDF		(1<<1)
#define	SH_SCIF_SCFSR_DR		(1<<0)

/* SCFDR */
#define	SH_SCIF_SCFDR_TX(x)		((x>>8)&0x1f)
#define	SH_SCIF_SCFDR_RX(x)		(x&0x1f)

#define	SH_SCIF_HSCFDR_TX(x)	((x>>8)&0xff)
#define	SH_SCIF_HSCFDR_RX(x)	(x&0xff)

/* SCSPTR */
#define	SH_SCIF_SCSPTR_RTSIO	(1<<7)
#define	SH_SCIF_SCSPTR_RTSDT	(1<<6)
#define	SH_SCIF_SCSPTR_CTSIO	(1<<5)
#define	SH_SCIF_SCSPTR_CTSDT	(1<<4)
#define	SH_SCIF_SCSPTR_SPB2IO	(1<<1)
#define	SH_SCIF_SCSPTR_SPB2DT	(1<<0)

/* SCLSR */
#define	SH_SCIF_SCLSR_ORER		(1<<0)

/* SCFCR */
#define	SH_SCIF_SCFCR_RTRG_M	_BITFIELD16L(6,3)
#define	SH_SCIF_SCFCR_RTRG_1	_BITFIELD16L(6,0)
#define	SH_SCIF_SCFCR_RTRG_4	_BITFIELD16L(6,1)
#define	SH_SCIF_SCFCR_RTRG_8	_BITFIELD16L(6,2)
#define	SH_SCIF_SCFCR_RTRG_E	_BITFIELD16L(6,3)
#define	SH_SCIF_SCFCR_TTRG_E	_BITFIELD16L(4,3)
#define SH_SCIF_SCFCR_TTRG_M    _BITFIELD16L(4,3)
#define SH_SCIF_SCFCR_TTRG_1    _BITFIELD16L(4,3)
#define SH_SCIF_SCFCR_TTRG_2    _BITFIELD16L(4,2)
#define SH_SCIF_SCFCR_TTRG_4    _BITFIELD16L(4,1)
#define SH_SCIF_SCFCR_TTRG_8    _BITFIELD16L(4,0)
#define	SH_SCIF_SCFCR_MCE		(1<<3)
#define	SH_SCIF_SCFCR_TFRST		(1<<2)
#define	SH_SCIF_SCFCR_RFRST		(1<<1)
#define	SH_SCIF_SCFCR_LOOP		(1<<0)

#define	SH_SCIF_FIFO_LEN		16


/* Different SH7761 base addresses */
#define SH7761_SCIF_BASE0		0xFFE40000
#define SH7761_SCIF_BASE1		0xFFE41000
#define SH7761_SCIF_BASE2		0xFFE42000
#define SH7761_SCIF_BASE3		0xFFE43000
#define SH7761_SCIF_BASE4		0xFFE44000
#define SH7761_SCIF_BASE5		0xFFE45000
#define SH7761_SCIF_BASE6		0xFFE46000
#define SH7761_SCIF_BASE7		0xFFE47000


/* Different R_CAR_H1  base addresses */
#define R_CAR_H1_SCIF_BASE0		0xFFE40000
#define R_CAR_H1_SCIF_BASE1		0xFFE41000
#define R_CAR_H1_SCIF_BASE2		0xFFE42000
#define R_CAR_H1_SCIF_BASE3		0xFFE43000
#define R_CAR_H1_SCIF_BASE4		0xFFE44000
#define R_CAR_H1_SCIF_BASE5		0xFFE45000

/* Different R_CAR_H2  base addresses */
#define R_CAR_H2_SCIF_BASE0		0xE6E60000
#define R_CAR_H2_SCIF_BASE1		0xE6E68000
#define R_CAR_H2_SCIF_BASE2		0xE6E56000

/* Different SH7760 base addresses */
#define SH7760_SCIF_BASE0 		0xfe600000
#define SH7760_SCIF_BASE1 		0xfe610000
#define SH7760_SCIF_BASE2 		0xfe620000

/* Different SH7760 register offsets */
#define SH7760_SCIF_SCTFDR_OFF 	0x1c
#define SH7760_SCIF_SCRFDR_OFF 	0x20
#define SH7760_SCIF_SCSPTR_OFF	0x24
#define SH7760_SCIF_SCLSR_OFF 	0x28

/* New SH7760 registers */
#define SH7760_SCIF_SCRER_OFF 	0x2C

/* New SH7760 bits */
#define SH7760_SCIF_SCSMR_CA        (1<<7)
#define SH7760_SCIF_SCSCR_CKE0      (1<<0)
#define SH7760_SCIF_SCSPTR_SCKIO    (1<<3)
#define SH7760_SCIF_SCSPTR_SCKDT    (1<<2)
#define	SH7760_SCIF_SCFCR_RSTRG_127	_BITFIELD16L(8,0)
#define SH7760_SCIF_SCFCR_RSTRG_1	_BITFIELD16L(8,1)
#define SH7760_SCIF_SCFCR_RSTRG_16	_BITFIELD16L(8,2)
#define SH7760_SCIF_SCFCR_RSTRG_32	_BITFIELD16L(8,3)
#define SH7760_SCIF_SCFCR_RSTRG_64	_BITFIELD16L(8,4)
#define SH7760_SCIF_SCFCR_RSTRG_96	_BITFIELD16L(8,5)
#define SH7760_SCIF_SCFCR_RSTRG_108	_BITFIELD16L(8,6)
#define SH7760_SCIF_SCFCR_RSTRG_120	_BITFIELD16L(8,7)

/* Different SH7760 FIFO Length */
#define SH7760_SCIF_FIFO_LEN        128

/* Different SH7770 base addresses */
#define SH7770_SCIF_BASE0 		0xff923000
#define SH7770_SCIF_BASE1 		0xff924000
#define SH7770_SCIF_BASE2 		0xff925000
#define SH7770_SCIF_BASE3 		0xff926000
#define SH7770_SCIF_BASE4 		0xff927000
#define SH7770_SCIF_BASE5 		0xff928000
#define SH7770_SCIF_BASE6 		0xff929000
#define SH7770_SCIF_BASE7 		0xff92a000
#define SH7770_SCIF_BASE8 		0xff92b000
#define SH7770_SCIF_BASE9 		0xff92c000

/* New SH7770 bits */
#define	SH7770_SCIF_SCFCR_RSTRG_15	_BITFIELD16L(8, 0)
#define	SH7770_SCIF_SCFCR_RSTRG_1	_BITFIELD16L(8, 1)
#define	SH7770_SCIF_SCFCR_RSTRG_4	_BITFIELD16L(8, 2)
#define	SH7770_SCIF_SCFCR_RSTRG_6	_BITFIELD16L(8, 3)
#define	SH7770_SCIF_SCFCR_RSTRG_8	_BITFIELD16L(8, 4)
#define	SH7770_SCIF_SCFCR_RSTRG_10	_BITFIELD16L(8, 5)
#define	SH7770_SCIF_SCFCR_RSTRG_12	_BITFIELD16L(8, 6)
#define	SH7770_SCIF_SCFCR_RSTRG_14	_BITFIELD16L(8, 7)

/* New SH7780 bits */
#define  SH7780_SCIF_SCFCR_RSTRG_63 _BITFIELD16L(8, 0)
#define  SH7780_SCIF_SCFCR_RSTRG_1  _BITFIELD16L(8, 1)
#define  SH7780_SCIF_SCFCR_RSTRG_8  _BITFIELD16L(8, 2)
#define  SH7780_SCIF_SCFCR_RSTRG_16 _BITFIELD16L(8, 3)
#define  SH7780_SCIF_SCFCR_RSTRG_32 _BITFIELD16L(8, 4)
#define  SH7780_SCIF_SCFCR_RSTRG_48 _BITFIELD16L(8, 5)
#define  SH7780_SCIF_SCFCR_RSTRG_54 _BITFIELD16L(8, 6)
#define  SH7780_SCIF_SCFCR_RSTRG_60 _BITFIELD16L(8, 7)

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/arm/scif.h $ $Rev: 764516 $")
#endif
