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


/*
 * DM814X specific WDT support.
 * WDT2 is used.
 */

#include "startup.h"
#include "arm/dm816x.h"

/*************************************************/
/* WDTIMER0 */
/*************************************************/
#define DM814X_WDTIMER0_BASE 0x44E35000

#define WDTIMER_WDT_WIDR	0x0
#define WDTIMER0_WDT_WDSC 	0x10
#define WDTIMER0_WDT_WDST 	0x14
#define WDTIMER0_WDT_WISR 	0x18
#define WDTIMER0_WDT_WIER 	0x1C
#define WDTIMER0_WDT_WCLR 	0x24
#define WDTIMER0_WDT_WCRR 	0x28
#define WDTIMER0_WDT_WLDR 	0x2c
#define WDTIMER0_WDT_WTGR 	0x30
#define WDTIMER0_WDT_WWPS 	0x34
#define WDTIMER0_WDT_WDLY 	0x44
#define WDTIMER0_WDT_WSPR 	0x48
#define WDTIMER0_WDT_WIRQSTATRAW 	0x54
#define WDTIMER0_WDT_WIRQSTAT 		0x58
#define WDTIMER0_WDT_WIRQENSET 	0x5c
#define WDTIMER0_WDT_WIRQENCLR 	0x60
   
#define DISABLE_SEQ_1 0xaaaa
#define DISABLE_SEQ_2 0x5555
#define ENABLE_SEQ_1  0xbbbb
#define ENABLE_SEQ_2  0x4444

#define WATCHDOG_TIMEOUT_SECONDS 60
#define WATCHDOG_FREQ 32000

void
wait_for_wdt_write_complete(void)
{
  while (in32( DM814X_WDTIMER0_BASE + WDTIMER0_WDT_WWPS));
}

void
wdt_disable(void)
{
  out32( DM814X_WDTIMER0_BASE + WDTIMER0_WDT_WSPR, DISABLE_SEQ_1);
  wait_for_wdt_write_complete();
  out32( DM814X_WDTIMER0_BASE + WDTIMER0_WDT_WSPR, DISABLE_SEQ_2);
  wait_for_wdt_write_complete();
}

void
wdt_enable(void)
{
  uint32_t wldr;

  // disable watchdog
  wdt_disable();

  // no prescalar
  out32( DM814X_WDTIMER0_BASE + WDTIMER0_WDT_WCLR, 0);
  wait_for_wdt_write_complete();

  // set watchdog timeout
  wldr = 0xffffffff - WATCHDOG_TIMEOUT_SECONDS * WATCHDOG_FREQ + 1;
  out32( DM814X_WDTIMER0_BASE + WDTIMER0_WDT_WLDR, wldr);
  wait_for_wdt_write_complete();

  // no interrupt generation -- reset only
  out32( DM814X_WDTIMER0_BASE + WDTIMER0_WDT_WDLY, 0xffffffff);
  wait_for_wdt_write_complete();

  // load timer counter value:  WDT_WCRR
  out32( DM814X_WDTIMER0_BASE + WDTIMER0_WDT_WCRR, wldr);
  wait_for_wdt_write_complete();

  // enable watchdog (woof)
  out32( DM814X_WDTIMER0_BASE + WDTIMER0_WDT_WSPR, ENABLE_SEQ_1);
  wait_for_wdt_write_complete();
  out32( DM814X_WDTIMER0_BASE + WDTIMER0_WDT_WSPR, ENABLE_SEQ_2);
  wait_for_wdt_write_complete();

}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/ti-am335x/init_wdt.c $ $Rev: 712793 $")
#endif
