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


#include "proto.h"

#define OMAP_I2C_STAT_MASK \
            (OMAP_I2C_STAT_XUDF | \
             OMAP_I2C_STAT_RDR  | \
             OMAP_I2C_STAT_XDR  | \
             OMAP_I2C_STAT_ROVR | \
             OMAP_I2C_STAT_AAS  | \
             OMAP_I2C_STAT_GC   | \
             OMAP_I2C_STAT_XRDY | \
             OMAP_I2C_STAT_RRDY | \
             OMAP_I2C_STAT_ARDY | \
             OMAP_I2C_STAT_NACK | \
             OMAP_I2C_STAT_AL)


int
omap_wait_bus_not_busy(omap_dev_t *dev, unsigned int stop)
{
    unsigned        tries = 1000000;

	if(dev->re_start) {
		if (stop){
			dev->re_start = 0;
		}
		return 0;
	}else {
		omap_clock_enable(dev);
		while (in16(dev->regbase + OMAP_I2C_STAT) & OMAP_I2C_STAT_BB){
			if (tries-- == 0){
				/* reset the controller to see if it's able to recover*/
				if (omap_i2c_reset(dev) == -1) {
					goto fail; // CAN NOT recover form bus busy!
				}else{
					break;
				}
			}
		}
		/* The I2C_STAT register should be 0, otherwise reset the I2C interface */
		if (in16(dev->regbase + OMAP_I2C_STAT)) {
			if (omap_i2c_reset(dev) == -1) {
				goto fail;
			}
		}
		if(!stop) {
			dev->re_start = 1;
		}
		omap_clock_disable(dev);
		return 0;
	}

fail:
	omap_clock_disable(dev);
	return -1;
}

const struct sigevent *i2c_intr(void *area, int id)
{
	omap_dev_t *dev = area;
	uint16_t	stat; 
	uint16_t	transmit_stat = (OMAP_I2C_STAT_XRDY | OMAP_I2C_STAT_XDR);
	uint16_t	receive_stat = (OMAP_I2C_STAT_RRDY | OMAP_I2C_STAT_RDR);
	
	// Shouldn't be here if clocks are disabled
	if(dev->clkctrl_disabled){
	    return NULL;
	}

	if (dev->options & OMAP_OPT_ROVR_XUDF_OK) {
		transmit_stat |= OMAP_I2C_STAT_XUDF;
		receive_stat |= OMAP_I2C_STAT_ROVR;
	}
	
	if ((stat = in16(dev->regbase + OMAP_I2C_STAT)) & OMAP_I2C_STAT_MASK) {

		// check errors and transaction done
		if (stat & (OMAP_I2C_STAT_NACK | OMAP_I2C_STAT_AL)) {
			if (stat & OMAP_I2C_STAT_NACK)
				dev->status |= I2C_STATUS_NACK ;
			if (stat & OMAP_I2C_STAT_AL)
				// when we get arbitration lost ARDY is not present, so force DONE
				dev->status |= I2C_STATUS_ARBL | I2C_STATUS_DONE;
			out16(dev->regbase + OMAP_I2C_CON, 
				   in16(dev->regbase + OMAP_I2C_CON) | OMAP_I2C_CON_STP);
			dev->re_start = 0;
		}
		if (0 == (dev->options & OMAP_OPT_ROVR_XUDF_OK)) {
			if (stat & (OMAP_I2C_STAT_ROVR | OMAP_I2C_STAT_XUDF)) {
				dev->status |= I2C_STATUS_ERROR;
				out16(dev->regbase + OMAP_I2C_CON,
					   in16(dev->regbase + OMAP_I2C_CON) | OMAP_I2C_CON_STP);
				dev->re_start = 0;
			}
		}
		if (stat & OMAP_I2C_STAT_ARDY) {
			dev->status |= I2C_STATUS_DONE;
		}

		// check receive interrupt
		if (stat & receive_stat) {
			int num_bytes = 1;
			if (dev->fifo_size) {
				if (stat & OMAP_I2C_STAT_RRDY)
					num_bytes = dev->fifo_size;
				else
					num_bytes = (in16(dev->regbase+OMAP_I2C_BUFSTAT) & OMAP_I2C_BUFSTAT_RXSTAT) >> 8;
			}
			while (num_bytes) {
				num_bytes--;
				if (dev->xlen) {
					*dev->buf++ = (uint8_t)(in16(dev->regbase + OMAP_I2C_DATA) & 0x00FF);
					dev->xlen--;
				} else {
					break;
				}
			}
		}

		// check transmit interrupt
		if (stat & transmit_stat) {
			int num_bytes = 1;
			if (dev->fifo_size) {
				if (stat & OMAP_I2C_STAT_XRDY)
					num_bytes = dev->fifo_size;
				else
					num_bytes = in16(dev->regbase+OMAP_I2C_BUFSTAT) & OMAP_I2C_BUFSTAT_TXSTAT;
			}
			while (num_bytes) {
				num_bytes--;
				if (dev->xlen) {
					out8(dev->regbase + OMAP_I2C_DATA, *dev->buf++);
					dev->xlen--;
				} else {
					break;
				}
			}
		}
	}

	//clear the status
	out16(dev->regbase + OMAP_I2C_STAT, stat);

	// check transaction done
	if ((dev->status & I2C_STATUS_DONE) && dev->intexpected) {
		dev->intexpected = 0;
		return &dev->intrevent;
	}

	return NULL;
}


uint32_t
omap_wait_status(omap_dev_t *dev)
{
	struct _pulse	pulse;
	uint64_t        ntime = 1e9;
	
	while (1) {
		ntime += (dev->xlen + 1) * 1000 * 50;
		TimerTimeout(CLOCK_MONOTONIC, _NTO_TIMEOUT_RECEIVE, NULL, &ntime, NULL);
		if (MsgReceivePulse(dev->chid, &pulse, sizeof(pulse), NULL) == -1){
			fprintf(stderr, "omap_i2c_wait_status: PID_%d %s(%d), dev->status %x, stat reg %x\n",
				getpid(), strerror(errno), errno, dev->status, in16(dev->regbase + OMAP_I2C_STAT));
			omap_i2c_reset(dev);
#ifdef VARIANT_j5
			// this second reset is needed for J5
			omap_i2c_reset(dev);
#endif
			return (I2C_STATUS_DONE | I2C_STATUS_ERROR);
		}
		switch (pulse.code) {
			case OMAP_I2C_EVENT:
			{
				if (dev->status & (I2C_STATUS_ARBL | I2C_STATUS_ERROR)) {
					omap_i2c_reset(dev);
				}
				return (dev->status);
			}
		}
	}

	return (I2C_STATUS_DONE | I2C_STATUS_ERROR);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/i2c/omap35xx/wait.c $ $Rev: 724903 $")
#endif
