/*
 * $QNXLicenseC: 
 * Copyright 2012, QNX Software Systems.  
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

#include "rtc.h"
#include <time.h>
#include <arm/mx6x.h>

/* Bit Definitions */
#define	MX6X_SNVS_LPCR_SRTC_ENV	(1 << 0)

/* Other defines */
#define	CNTR_TO_SECS_SH	15	/* Converts 47-bit counter to 32-bit seconds */

int
RTCFUNC(init,mx6srtc)(struct chip_loc *chip, char *argv[])
{
    if (chip->phys == NIL_PADDR) {
        chip->phys = MX6X_SNVS_BASE;
    }
    if (chip->access_type == NONE) {
        chip->access_type = MEMMAPPED;
    }

    return MX6X_SNVS_SIZE;
}

int
RTCFUNC(get, mx6srtc)(struct tm *tm, int cent_reg) 
{
    uint64_t read1, read2;
    uint32_t counter_sec = MX6X_SNVS_LPSRTCMR;

    do {
        /* MSB */
        read1 = chip_read32(MX6X_SNVS_LPSRTCMR);
        read1 <<= 32; 

        /* LSB */
        read1 |= chip_read32(MX6X_SNVS_LPSRTCLR);

        /* MSB */
        read2 = chip_read32(MX6X_SNVS_LPSRTCMR);
        read2 <<= 32; 

        /* LSB */
        read2 |= chip_read32(MX6X_SNVS_LPSRTCLR); 

    //Loop while time inconsistent
    } while (read1 != read2);

	/* Convert 47-bit counter to 32-bit raw second count */
	counter_sec = (uint32_t) (read1 >> CNTR_TO_SECS_SH);

#ifdef  VERBOSE_SUPPORTED
    if (verbose) {
        printf("rtc read: %d\n", counter_sec);
    }
#endif
  
    gmtime_r(&counter_sec, tm);

    return(0);
}


int
RTCFUNC(set, mx6srtc)(struct tm *tm, int cent_reg) 
{
    uint32_t lp_cr;
    time_t      t;

    t = mktime(tm);

    /*
     *  mktime assumes local time.  We will subtract nd timezmezone
     */
    t -= timezone;

#ifdef  VERBOSE_SUPPORTED
    if (verbose) {
        printf("rtc write: %d\n", t);
    }
#endif

    /* Disable RTC first */
    lp_cr = chip_read32(MX6X_SNVS_LPCR) & ~MX6X_SNVS_LPCR_SRTC_ENV;
    chip_write32(MX6X_SNVS_LPCR, lp_cr);
    while (chip_read32(MX6X_SNVS_LPCR) & MX6X_SNVS_LPCR_SRTC_ENV);

    /* Write 32-bit time to 47-bit timer, leaving 15 LSBs blank */
    chip_write32(MX6X_SNVS_LPSRTCLR, t << CNTR_TO_SECS_SH);
    chip_write32(MX6X_SNVS_LPSRTCMR, t >> (32 - CNTR_TO_SECS_SH));

    /* Enable RTC again */
    chip_write32(MX6X_SNVS_LPCR, lp_cr | MX6X_SNVS_LPCR_SRTC_ENV);
    while (!(chip_read32(MX6X_SNVS_LPCR) & MX6X_SNVS_LPCR_SRTC_ENV));

    return(0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/utils/r/rtc/nto/arm/clk_mx6srtc.c $ $Rev: 681415 $")
#endif
