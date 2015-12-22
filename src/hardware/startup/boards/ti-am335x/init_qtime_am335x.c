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


/*
 * TI AM335X specific timer support.
 * TIMER2 is used.
 */

#include "startup.h"
#include <arm/am335x.h>

#define AM335X_CLOCK_FREQ   24000000UL
#define AM335X_CLOCK_RATE   41666666UL
#define AM335X_CLOCK_SCALE  -15

extern struct callout_rtn   timer_load_am335x;
extern struct callout_rtn   timer_value_am335x;
extern struct callout_rtn   timer_reload_am335x;
static uintptr_t    timer_base;

static const struct callout_slot    timer_callouts[] = {
    { CALLOUT_SLOT(timer_load, _am335x) },
    { CALLOUT_SLOT(timer_value, _am335x) },
    { CALLOUT_SLOT(timer_reload, _am335x) },
};

static unsigned
timer_start_am335x()
{
    out32(timer_base + AM335X_TIMER_TCLR, 
        in32(timer_base + AM335X_TIMER_TCLR) | AM335X_TIMER_TCLR_ST);
    return in32(timer_base + AM335X_TIMER_TCRR);
}

static unsigned
timer_diff_am335x(unsigned start)
{
    unsigned now = in32(timer_base + AM335X_TIMER_TCRR);
    return (now - start);
}

void
init_qtime_am335x()
{
    struct qtime_entry  *qtime = alloc_qtime();

    /*
     * Map timer registers, Uboot only enabled timer2, so use timer2 for now
     */
    timer_base = startup_io_map(AM335X_TIMER_SIZE, AM335X_TIMER2_BASE);

    /* Clear timer count and reload count */
    out32(timer_base + AM335X_TIMER_TLDR, 0);
    out32(timer_base + AM335X_TIMER_TCRR, 0);

    /*
     * Setup Timer0
     * Auto-reload enable
     * Prescaler disable
     * Stop timer, timer_load will enable it
     */
    out32(timer_base + AM335X_TIMER_TCLR, (AM335X_TIMER_TCLR_PRE_DISABLE | AM335X_TIMER_TCLR_AR));

    timer_start = timer_start_am335x;
    timer_diff  = timer_diff_am335x;

    qtime->intr = 68;   /* Timer2 irq */

    qtime->timer_scale = AM335X_CLOCK_SCALE;
	qtime->timer_rate = AM335X_CLOCK_RATE;
	qtime->cycles_per_sec = (uint64_t)AM335X_CLOCK_FREQ;

    add_callout_array(timer_callouts, sizeof(timer_callouts));
}




#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/ti-am335x/init_qtime_am335x.c $ $Rev: 716637 $")
#endif
