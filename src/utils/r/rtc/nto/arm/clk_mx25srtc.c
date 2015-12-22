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





#include "rtc.h"
#include <time.h>
#include <arm/mx25.h>

int
RTCFUNC(init,mx25srtc)(struct chip_loc *chip, char *argv[])
{
 	if (chip->phys == NIL_PADDR)
    	/* Set the base address to the MX25_DRYICE_BASE that way we can use simple
        register offsets allready defined. */
     	chip->phys = MX25_DRYICE_BASE; 
 	
    	if (chip->century_reg == UNSET)
       	/* There is no century register avaliable year is stored in 12 bits */
        chip->century_reg = -1;     

    	if (chip->access_type == NONE)
        	chip->access_type = MEMMAPPED;
   
    	chip->reg_shift = 0;
  
  	/* return size of memory to be mapped */
 	 return(MX25_DRYICE_SIZE);
 }

int
RTCFUNC(get,mx25srtc)(struct tm *tm, int cent_reg)
{
	time_t		t ;
 	
     	/* read RTC counter value */
	t = chip_read(MX25_DRYICE_DTCMR, 32);

#ifdef	VERBOSE_SUPPORTED
	if (verbose) {
		printf("rtc read: %x\n", t);
	}
#endif
	    
    	gmtime_r(&t,tm);	
	
	return 0;
}

int
RTCFUNC(set,mx25srtc)(struct tm *tm, int cent_reg)
{
	time_t		t;
	
	t = mktime(tm);

	/* mktime assumes local time.  We will subtract timezone */
	t -= timezone;

#ifdef	VERBOSE_SUPPORTED
	if (verbose) {
		printf("rtc write: %d\n", t);
	}
#endif
	
	chip_write(MX25_DRYICE_DTCMR, t, 32);

	return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/utils/r/rtc/nto/arm/clk_mx25srtc.c $ $Rev: 680331 $")
#endif
