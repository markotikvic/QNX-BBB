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

#ifndef __OMAP_I2C_H__
#define __OMAP_I2C_H__

#include <hw/inout.h>
#include <inttypes.h>

/* Can be overwritten in startup/boards */
#define I2C_FUNCTION_CLK				48000000

/* I2C registers */
#define I2C_SYSC						0x10
#define I2C_IE							0x2C
#define I2C_STAT						0x28
	#define I2C_STAT_SBD				(1 << 15) /* Single byte data */
	#define I2C_STAT_XDR				(1 << 14) /* Transmit draining ready */
	#define I2C_STAT_RDR				(1 << 13) /* Receive draining ready */
	#define I2C_STAT_BB				 	(1 << 12) /* Bus busy */
	#define I2C_STAT_ROVR				(1 << 11) /* Receive overrun */
	#define I2C_STAT_XUDF				(1 << 10) /* Transmit underflow */
	#define I2C_STAT_AAS				(1 << 9)	/* Address as slave */
	#define I2C_STAT_GC				 	(1 << 5)
	#define I2C_STAT_XRDY				(1 << 4)	/* Transmit data ready */
	#define I2C_STAT_RRDY				(1 << 3)	/* Receive data ready */
	#define I2C_STAT_ARDY				(1 << 2)	/* Register access ready */
	#define I2C_STAT_NACK				(1 << 1)	/* No acknowledgment */
	#define I2C_STAT_AL				 	(1 << 0)	/* Arbitration lost */
#define I2C_WE							0x34
#define I2C_SYSS						0x90
	#define I2C_SYSS_RDONE				(1 << 0)	/* Reset Done */
#define I2C_BUF						 	0x94
	#define I2C_BUF_RXFIFO_CLR			(1 << 14)	
	#define I2C_BUF_TXFIFO_CLR			(1 << 6)	
	#define I2C_BUF_RXTRSH_OFF			8	
	#define I2C_BUF_TXTRSH_OFF			0	
#define I2C_CNT						 	0x98
#define I2C_DATA						0x9C
#define I2C_CON						 	0xA4
	#define I2C_CON_EN					(1 << 15)	/* I2C module enable */
	#define I2C_CON_BE					(1 << 14)	/* Big endian mode */
	#define I2C_CON_STB				 	(1 << 11)	/* Start byte mode (master mode only) */
	#define I2C_CON_MST				 	(1 << 10)	/* Master/slave mode */
	#define I2C_CON_TRX				 	(1 << 9)	/* Transmitter/receiver mode */
	#define I2C_CON_XA					(1 << 8)	/* Expand address */
	#define I2C_CON_STP				 	(1 << 1)	/* Stop condition (master mode only) */
	#define I2C_CON_STT				 	(1 << 0)	/* Start condition (master mode only) */
#define I2C_OA0						 	0xA8
#define I2C_SA							0xAC
#define I2C_PSC						 	0xB0
#define I2C_SCLL						0xB4
#define I2C_SCLH						0xB8
#define I2C_SYSTEST					 	0xBC
#define I2C_BUFSTAT					 	0xC0
	#define I2C_BUFSTAT_RXSTAT			(0x3F00)
	#define I2C_BUFSTAT_TXSTAT			(0x003F)
	#define I2C_BUFSTAT_FIFODEPTH_OFF	14
#define I2C_OA1						 	0xC4
#define I2C_OA2						 	0xC8
#define I2C_OA3						 	0xCC
#define I2C_ACTOA						0xD0
#define I2C_SBLOCK						0xD4

typedef struct {
	unsigned int base;
	unsigned int clock;
	unsigned short slave;
} omap_i2c_dev_t;

int omap_i2c_read (omap_i2c_dev_t *dev, uint32_t reg, uint8_t alen, uint8_t* buffer, int len);
int omap_i2c_write(omap_i2c_dev_t *dev, uint8_t *buf, int len);
int omap_i2c_init(omap_i2c_dev_t *dev);
void omap_i2c_deinit(omap_i2c_dev_t *dev);
void omap_i2c_set_debug(int dbg);
#endif /*__OMAP_I2C_H__*/



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/hw/omap_i2c.h $ $Rev: 734927 $")
#endif
