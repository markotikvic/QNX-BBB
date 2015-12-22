/*
 * $QNXLicenseC: 
 * Copyright 2007, 2008, QNX Software Systems.  
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
#include <arm/dm816x.h>

/*
 * Clock setup for the RTC on the TI dm816xEVM (ARMv7 Cortex-A8 core).
 * Registers are defined in dm816x.h.
 */


int
RTCFUNC(init,dm816x)(struct chip_loc *chip, char *argv[]) {

    if (chip->phys == NIL_PADDR) {
        chip->phys = DM816X_RTC_BASE;
    }

    if (chip->access_type == NONE) {
        chip->access_type = MEMMAPPED;
    }
    return DM816X_RTC_SIZE;
}

int
RTCFUNC(get,dm816x)(struct tm *tm, int cent_reg) {
	unsigned	sec;
	unsigned	min;
	unsigned	hour;
	unsigned	mday;
	unsigned	mon;
	unsigned	year;

	// convert BCD to binary 
	sec  = chip_read(DM816X_RTC_SECONDS, 32) & 0xff;
	min  = chip_read(DM816X_RTC_MINUTES, 32) & 0xff;
	hour = chip_read(DM816X_RTC_HOURS, 32) & 0xff;	// 00-23 (24hr time)
	mday = chip_read(DM816X_RTC_DAYS, 32) & 0xff;	// 01-31
	mon  = chip_read(DM816X_RTC_MONTHS, 32) & 0xff;	// 01-12
	year = chip_read(DM816X_RTC_YEARS, 32) & 0xff;

	tm->tm_sec  = BCD2BIN(sec);
	tm->tm_min  = BCD2BIN(min);
	tm->tm_hour = BCD2BIN(hour);
	tm->tm_mday = BCD2BIN(mday);
	tm->tm_mon  = BCD2BIN(mon) - 1;
	tm->tm_year = BCD2BIN(year) + 100;

	return(0);
}

int
RTCFUNC(set,dm816x)(struct tm *tm, int cent_reg) {
	unsigned	seconds;
	unsigned	minutes;
	unsigned	hours;
	unsigned	day;
	unsigned	month;
	unsigned	year;
	unsigned	ctrl;

	// convert binary to BCD
	seconds	= BIN2BCD(tm->tm_sec);
	minutes	= BIN2BCD(tm->tm_min);
	hours	= BIN2BCD(tm->tm_hour);
	day 	= BIN2BCD(tm->tm_mday);
	month	= BIN2BCD(tm->tm_mon + 1);
	year	= BIN2BCD(tm->tm_year % 100);

	// unlock the RTC registers
	chip_write(DM816X_RTC_KICK0R, 0x83e70b13,32);
	chip_write(DM816X_RTC_KICK1R, 0x95a4f1e0,32);

	// disable RTC first to avoid TC registers changing
	ctrl = chip_read(DM816X_RTC_CTRL, 32) & 0xff;
	ctrl &= ~DM816X_RTC_CTRL_STOP_RTC;
	chip_write(DM816X_RTC_CTRL, ctrl, 32);
	while ((chip_read(DM816X_RTC_STATUS, 32)) & DM816X_RTC_STATUS_RUN);

	chip_write(DM816X_RTC_SECONDS, seconds, 32);
	chip_write(DM816X_RTC_MINUTES, minutes, 32);
	chip_write(DM816X_RTC_HOURS, hours, 32);		// 00-23 (24hr time)
	chip_write(DM816X_RTC_WEEKS, tm->tm_wday, 32);	// 0 to 6 BCD is same as binary, so no conversion needed
	chip_write(DM816X_RTC_DAYS, day, 32);			// 01-31 (day of month)
	chip_write(DM816X_RTC_MONTHS, month, 32);		// 01-12
	chip_write(DM816X_RTC_YEARS, year, 32);

	// Enable RTC again
	ctrl = chip_read(DM816X_RTC_CTRL, 32) & 0xff;
	ctrl |= DM816X_RTC_CTRL_STOP_RTC;
	chip_write(DM816X_RTC_CTRL, ctrl, 32);

	// Lock RTC again
	chip_write(DM816X_RTC_KICK0R, 0x0, 32);

	return(0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/utils/r/rtc/nto/arm/clk_dm816x.c $ $Rev: 712947 $")
#endif

