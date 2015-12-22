/*
 * $QNXLicenseC:
 * Copyright 2011, QNX Software Systems.
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
  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/resmgr.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <hw/i2c.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <errno.h>
#include <sys/procmgr.h>
#include <drvr/hwinfo.h>
#include <time.h>

#define WDTIMER0_WDT_WTGR 	0x30
#define WDTIMER0_WDT_WSPR 	0x48

#define DM814X_WDT_WSPR_MASK         0xffff
#define DM814X_WDT_WSPR_ENABLED      0x4444

#define ENABLE_SEQ_1  0xbbbb
#define ENABLE_SEQ_2  0x4444

int main(int argc, char *argv[]) 
{
    int         opt;
    int         priority = 10;  // default priority:default 10
    int         time = -1;      // default time in mS for watchdog timer kick
    int         verbose =0;  
    int         exit = 0;                     
    uintptr_t   regbase = MAP_DEVICE_FAILED;
    size_t      len = -1;
    uint32_t    physbase = 0;
    hwiattr_timer_t wdt_attr;
    unsigned    hwi_off ;
    
    
    /* Getting the WDOG Base addresss from the Hwinfo Section if available */ 
    hwi_off = hwi_find_device("wdt", 0);
    if (HWI_NULL_OFF != hwi_off)
    {
        if (EOK == hwiattr_get_timer(hwi_off, &wdt_attr)) {
            if (wdt_attr.common.location.len > 0) {
                len = wdt_attr.common.location.len;
            }
            if (wdt_attr.common.location.base > 0) {
                physbase = wdt_attr.common.location.base;
            }
            time = hwitag_find_clkfreq(hwi_off, NULL);
        }
    }
        
    /* Process command line options */
    while ((opt = getopt(argc, argv, "a:l:p:t:ev")) != -1) {    
        switch (opt) {
            case 'a':   // WDT register physical base address
                physbase = strtoul(optarg, NULL, 0);
                break;
			case 'l': 	                    
				len = strtoul(optarg, NULL, 0);
				break;
   			case 'p':   // priority
                priority = strtoul(optarg, NULL, 0) ;
                break;
            case 't':   // kick interval                        
                time = strtoul(optarg, NULL, 0);
                break;
            case 'e':   // exit flag    
                exit = 1;
                break;
            case 'v':   // verbose flag    
                verbose++;
                break;
        }
    }

    /*check if the params are valid*/ 
    if (0 == physbase) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "DM814X-wdtkick:  Invalid WDT register physics based address. Please check the command line or HWInfo default setting.");
        return EXIT_FAILURE;
    }
    if (0 == len) 
    {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "DM814X-wdtkick:  Invalid  WDT registers size. Please check the command line or HWInfo default setting.");
        return EXIT_FAILURE;
    }
    if (-1 == time)
    {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "DM814X-wdtkick:  Invalid default time for watchdog timer kick. Please check the command line or HWInfo default setting.");
        return EXIT_FAILURE;
    }
    
    // Enable IO capability.
    if (-1 == ThreadCtl( _NTO_TCTL_IO_PRIV, NULL )) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "DM814X-wdtkick:  ThreadCtl failed");
        return EXIT_FAILURE;
    }

    //run in the background
	if ( procmgr_daemon( EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE | PROCMGR_DAEMON_NODEVNULL ) == -1 ) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"%s:  procmgr_daemon",argv[0]);
		return EXIT_FAILURE;
	}

    // configure information 
    if (verbose) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "DM814X-wdtkick:  Paddr = 0x%x, size = 0x%x, kick = %d, Priority = %d, exit = %d",
            physbase, len, time, priority, exit);
    }

    // If requested: Change priority.
    if (getprio(0) != priority) {
        if (-1 == setprio(0, priority)) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"DM814X-wdtkick:  can't change priority");
        }
    }

    // mmap io memory for WDT
    regbase = mmap_device_io(len, physbase);
    if (MAP_DEVICE_FAILED == regbase) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "DM814X-wdtkick:  Failed to map WDT registers");
        return EXIT_FAILURE;
    }

    if ((in32(regbase + WDTIMER0_WDT_WSPR) & DM814X_WDT_WSPR_MASK) != DM814X_WDT_WSPR_ENABLED)
    {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "DM814X-wdtkick:  WDT is disabled now");
        if(exit)
        {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "DM814X-wdtkick:  Terminate WDT Module.");
            goto done;
        }   
    }
    
    while (1) {
		out32(regbase + WDTIMER0_WDT_WTGR, in32(regbase + WDTIMER0_WDT_WTGR) ^ 0xffffffff);
        delay(time);
    }
        
done:  
    munmap_device_io(regbase,len);
    return EXIT_SUCCESS;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/support/dm814x-wdtkick/main.c $ $Rev: 748793 $")
#endif
