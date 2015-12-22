/*
 * $QNXLicenseC:
 * Copyright 2009, QNX Software Systems.
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

#ifndef __PROTO_H_INCLUDED
#define __PROTO_H_INCLUDED

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <hw/i2c.h>
#include <arm/omap.h>
#include <arm/omap3530.h>
#include "offsets.h"

typedef struct _omap_dev {
    unsigned            reglen;
    uintptr_t           regbase;
    unsigned            physbase;

    uintptr_t           clkctrl_base;
    unsigned            clkctrl_phys;
    uintptr_t           clkstctrl_base;
    unsigned            clkctrl_disabled;

	unsigned			re_start;
    int                 intr;
    int                 iid;
	int					fifo_size;
	int					chid;
	int					coid;
	int					xlen;
	uint8_t				*buf;
	unsigned int		speed;
	volatile int		intexpected;
	volatile uint32_t	status;
    struct sigevent     intrevent;
    int                 intr_priority;

    unsigned            own_addr;
    unsigned            slave_addr;
    i2c_addrfmt_t       slave_addr_fmt;
    unsigned            options;
    int	        high_adjust_fast;
    int	        high_adjust_slow;
    int	        low_adjust_fast;
    int	        low_adjust_slow;
    struct {
        unsigned char   major;
        unsigned char   minor;
    } rev;

#ifdef VARIANT_omap4
	unsigned			soc_version;
	unsigned			no_powmgm;
#endif
	uintptr_t           i2c_context_vaddr;
    unsigned            i2c_context_paddr;
    struct {
        unsigned        captured;
        unsigned        ie;
        unsigned        psc;
        unsigned        scll;
        unsigned        sclh;
        unsigned        buf;
        unsigned        sysc;
        unsigned        we;
    } state;
} omap_dev_t;

#define OMAP_OPT_VERBOSE        0x00000002
#define OMAP_OPT_ROVR_XUDF_OK   0x00000004

#define OMAP_I2C_PCLK           12000000UL  /* peripheral clock */
#define OMAP_I2C_ICLK           4000000UL   /* internal clock for 100K bus speed */
#define OMAP_I2C_ICLK_9600K     9600000UL   /* internal clock for 400K bus speed */
#define OMAP_I2C_ADDRESS        1
#define OMAP_I2C_STATUS_TIMEDOUT 0xFFFF
#define OMAP_I2C_IE_RDR			(1<<13)
#define OMAP_I2C_IE_XDR			(1<<14)
#define OMAP_I2C_STAT_RDR		(1<<13)
#define OMAP_I2C_STAT_XDR		(1<<14)
#define OMAP_I2C_BUF_RXFIF_CLR	(1<<14)
#define OMAP_I2C_BUF_TXFIF_CLR	(1<<6)
#define OMAP_I2C_BUFSTAT_TXSTAT	(0x3f)
#define OMAP_I2C_BUFSTAT_RXSTAT	((0x3f)<<8)
#define OMAP_I2C_IE_MASK		(OMAP_I2C_IE_AL |OMAP_I2C_IE_NACK |OMAP_I2C_IE_ARDY \
								|OMAP_I2C_IE_RRDY |OMAP_I2C_IE_XRDY \
								|OMAP_I2C_IE_RDR | OMAP_I2C_IE_XDR)
#define OMAP_I2C_WE_ALL         0x6F6F
#define OMAP_I2C_CON_XSA        (1<<8)  //1: 10bit, 0:7bit
#define OMAP_I2C_EVENT          1

#ifndef _SLOGC_I2C
#define _SLOGC_I2C              23
#endif

#define TWL4030_AUDIO_SLAVE_ADDRESS 0x49
#define OMAP_I2C_DEF_INTR_PRIORITY  21

void *omap_init(int argc, char *argv[]);
void omap_fini(void *hdl);
int omap_options(omap_dev_t *dev, int argc, char *argv[]);

int omap_set_slave_addr(void *hdl, unsigned int addr, i2c_addrfmt_t fmt);
int omap_set_bus_speed(void *hdl, unsigned int speed, unsigned int *ospeed);
int omap_version_info(i2c_libversion_t *version);
int omap_driver_info(void *hdl, i2c_driver_info_t *info);
int omap_devctl(void *hdl, int cmd, void *msg, int msglen, 
        int *nbytes, int *info);
i2c_status_t omap_recv(void *hdl, void *buf, 
        unsigned int len, unsigned int stop);
i2c_status_t omap_send(void *hdl, void *buf, 
        unsigned int len, unsigned int stop);
int omap_bus_reset(void *dev);

int omap_wait_bus_not_busy(omap_dev_t *dev,  unsigned int stop);
uint32_t omap_wait_status(omap_dev_t *dev);
const struct sigevent *i2c_intr(void *area, int id);
int omap_i2c_reset(omap_dev_t *dev);

void omap_clock_enable(omap_dev_t* dev);
void omap_clock_disable(omap_dev_t* dev);
int omap_clock_toggle_init(omap_dev_t* dev);
int omap_i2c_bus_recover(omap_dev_t *dev);
int omap_reg_map_init(omap_dev_t* dev);

#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/i2c/omap35xx/proto.h $ $Rev: 739181 $")
#endif
