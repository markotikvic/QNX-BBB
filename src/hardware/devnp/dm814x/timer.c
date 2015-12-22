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

#include "ti814x.h"
#include <avb.h>
#include <sys/syslog.h>

#define TICK_TIME	50 /* 20MHz Oscillator gives 50ns ticks */

pthread_t		tid;
int			intid;
struct sigevent		event;

void ti814x_timer_init (ti814x_dev_t *ti814x)
{
    /* Remove from the old thread */
    if (intid) {
	InterruptDetach(intid);
    }

    /* Ensure timer is stopped */
    out32(ti814x->timer_base + TIMER_TCLR, 0);

    /* Setup IRQ registers */
    out32(ti814x->timer_base + TIMER_IRQSTATUS, INT_MAT | INT_OVF | INT_CAP);
    out32(ti814x->timer_base + TIMER_IRQ_EOI, 0);
    out32(ti814x->timer_base + TIMER_IRQENABLE_CLR, INT_MAT | INT_CAP);
    out32(ti814x->timer_base + TIMER_IRQENABLE_SET, INT_OVF);

    /* Attach to this thread */
    event.sigev_notify = SIGEV_INTR;
    intid = InterruptAttachEvent(TINT2, &event, 0);
    tid = pthread_self();
}

int ti814x_timer_delay (ti814x_dev_t *ti814x, struct ifdrv *ifd)
{
    precise_timer_delay_t td;
    uint32_t val;

    if (ISSTACK) {
	log(LOG_ERR, "dm814x precise delay called from stack context");
	return EWOULDBLOCK;
    }
    memcpy(&td, (((uint8_t *)ifd) + sizeof(*ifd)), sizeof(td));

    val = (td.ns / TICK_TIME);
    if (val == 0) {
	return EOK;
    }

    /* Ensure this thread is the one attached */
    if (!pthread_equal(tid, pthread_self())) {
	ti814x_timer_init(ti814x);
    }

    out32(ti814x->timer_base + TIMER_TCLR, 0);
    out32(ti814x->timer_base + TIMER_TCRR, (UINT32_MAX - val) + 1);
    out32(ti814x->timer_base + TIMER_TCLR, 1);

    InterruptWait(0, NULL);
    out32(ti814x->timer_base + TIMER_IRQSTATUS, INT_MAT | INT_OVF | INT_CAP);
    out32(ti814x->timer_base + TIMER_IRQ_EOI, 0);
    InterruptUnmask(TINT2, intid);

    return EOK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/io-pkt/sys/dev_qnx/dm814x/timer.c $ $Rev: 706942 $")
#endif
