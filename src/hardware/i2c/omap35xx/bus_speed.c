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

// these defines taken from OMAP44xx and OMAP35xx TRM
#define SCLL_BIAS 7
#define SCLH_BIAS 5

int
omap_set_bus_speed(void *hdl, unsigned int speed, unsigned int *ospeed)
{
    omap_dev_t      *dev = hdl;
    unsigned long   iclk;
    unsigned        scll_plus_sclh;
    int             scll_delta;
    int             high_adjust;
    int             low_adjust;
    int             scll_to_write;
    int             sclh_to_write;

    /* This driver support bus speed range from 8KHz to 400KHz
     * limit the low bus speed to 8KHz to protect SCLL/SCLH from overflow(large than 0xff)
     * if speed=8KHz, iclk=4MHz, then SCLL=0xf3, SCLH=0xf5
     */
    if (speed > 400000 || speed < 8000) {
        fprintf(stderr, "i2c-omap35xx:  Invalid bus speed(%d)\n", speed);
        errno = EINVAL;
        return -1;
    }

    omap_clock_enable(dev);

    /* Set the I2C prescaler register to obtain the maximum I2C bit rates
     * and the maximum period of the filtered spikes in F/S mode:
     * Stander Mode: I2Ci_INTERNAL_CLK = 4 MHz
     * Fast Mode:    I2Ci_INTERNAL_CLK = 9.6 MHz
     */
    if (speed <= 100000) {
#ifdef VARIANT_j5
        out16(dev->regbase + OMAP_I2C_PSC, 11);     // The sysclk is 48 MHz in J5 and 96 MHz in OMAP
#else
        out16(dev->regbase + OMAP_I2C_PSC, 23);     // I2Ci_INTERNAL_CLK = 4 MHz
#endif
        iclk = OMAP_I2C_ICLK;
        high_adjust = dev->high_adjust_slow;
        low_adjust = dev->low_adjust_slow;
        // in standard mode, scll is smaller than (scll_plus_sclh>>1) by 1
        scll_delta = -1;
    } else {
#ifdef VARIANT_j5
        out16(dev->regbase + OMAP_I2C_PSC, 4);      // The sysclk is 48 MHz in J5 and 96 MHz in OMAP
#else
        out16(dev->regbase + OMAP_I2C_PSC, 9);      // I2Ci_INTERNAL_CLK = 9.6 MHz
#endif
        iclk = OMAP_I2C_ICLK_9600K;
        high_adjust = dev->high_adjust_fast;
        low_adjust = dev->low_adjust_fast;
#ifdef VARIANT_omap4
        // in fast mode, scll is larger than (scll_plus_sclh>>1) by 1
        scll_delta = 1;
#else
        // At this point it is unclear whether omap3 also has this relationship
        // between SCLL and SCLH in fast mode, so we leave it alone for now to
        // avoid breakage.
        scll_delta = -1;
#endif
    }

    /* Set clock based on "speed" bps */
    scll_plus_sclh = (iclk/speed - (SCLL_BIAS + SCLH_BIAS));

    scll_to_write = (scll_plus_sclh>>1) + scll_delta;
    sclh_to_write = scll_plus_sclh - scll_to_write;
    scll_to_write += low_adjust;
    sclh_to_write += high_adjust;

    if (scll_to_write < 0) scll_to_write = 0;
    if (sclh_to_write <= 2) sclh_to_write = 3;

    out16(dev->regbase + OMAP_I2C_SCLL, scll_to_write);
    out16(dev->regbase + OMAP_I2C_SCLH, sclh_to_write);
    dev->speed = iclk / (scll_to_write + SCLL_BIAS + sclh_to_write + SCLH_BIAS);
    
    if (ospeed)
        *ospeed = dev->speed;

    omap_clock_disable(dev);
    return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/i2c/omap35xx/bus_speed.c $ $Rev: 698026 $")
#endif
