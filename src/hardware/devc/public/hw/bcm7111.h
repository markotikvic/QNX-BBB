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


#define BCM7111_UARTA_BASE			0xfffe00b0
#define BCM7111_UARTB_BASE			0xfffe00c0
#define BCM7111_UARTC_BASE			0xfffe01a0
#define BCM7111_UARTD_BASE			0xfffe01a8

#define BCM7111_UART_SIZE			0x10

#define BCM7111_RCVSTAT				0x00
#define BCM7111_RCVDATA				0x01
#define BCM7111_CONTROL				0x03
#define BCM7111_BAUDHI				0x04
#define BCM7111_BAUDLO				0x05
#define BCM7111_XMTSTAT				0x06
#define BCM7111_XMTDATA				0x07

#define RCVSTAT_RIE			0x02
#define RCVSTAT_RDA			0x04
#define RCVSTAT_OVRN		0x08
#define RCVSTAT_FE			0x10
#define RCVSTAT_PE			0x20

#define CONTROL_PODD		0x01
#define CONTROL_RXE			0x02
#define CONTROL_TXE			0x04
#define CONTROL_PAREN		0x08
#define CONTROL_BITM8		0x10
#define CONTROL_LOOP		0x20

#define XMTSTAT_TDRE		0x01
#define XMTSTAT_IDLE		0x02
#define XMTSTAT_TIE			0x04

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/public/hw/bcm7111.h $ $Rev: 680332 $")
#endif
