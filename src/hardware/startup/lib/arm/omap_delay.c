/*
 * $QNXLicenseC:
 * Copyright 2010, QNX Software Systems.
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
 
#include "startup.h"

#define OMAP44XX_SYNCNT_32K_CR  0x4a304010
#define OMAP54XX_SYNCNT_32K_CR  0x4ae04030

#define USLEEP_FREQ                                 32768ULL
#define USEC_SEC                                    (1000000ULL)

/* Allow system to wait for at least the specify number of us. We'll be using the
   32K sync clock which will provide us with a ~30 uS resolution. We'll use a lasy
   approach to do it and not bother with roll over of the sync timer. 
*/
void
omap_usleep(unsigned long int us, unsigned int cr_reg_addr)
{
    uint32_t tick;
	
    /* Convert desire number of uSec into desired resolution and round it up to
       make sure we wait at least the specify time + 30uS.
    */
    us = ((us * USLEEP_FREQ) / USEC_SEC) + 2;
    tick = in32(cr_reg_addr);
    
    while (us > 0){
        /* Decrement count base on next 32K sysclk running. In this loop we
           don't care if sync clk did increase by more then one tick as we
           will catch up eventually and this is purely to make sure we wait
           at least the specify amount of time by the count variable.
         */
        if (tick != in32(cr_reg_addr)) {
            tick++;
            us--;
        }
    }
}


void
omap_delay(unsigned delay, unsigned int cr_reg_addr){
    omap_usleep((unsigned long)delay * 1000UL, cr_reg_addr);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/omap_delay.c $ $Rev: 733817 $")
#endif
