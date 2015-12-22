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


/*
 * TI DM816x general-purpose timer support
 */

#include "callout.ah"
#include <arm/am335x.h>


/*
 * --------------------------------------------------------------------------
 * Routine to patch callout code
 *
 * On entry:
 *  r0 - physical address of syspage
 *  r1 - virtual  address of syspage
 *  r2 - offset from start of syspage to start of the callout routine
 *  r3 - offset from start of syspage to read/write data used by callout
 * --------------------------------------------------------------------------
 */
patch_timer_am335x:
    stmdb   sp!, {r4, lr}
    add     r4, r0, r2                  // address of callout routine

    ldr     r0, Lvaddr
    cmp     r0, #-1
    bne     1f

    /*
     * Map registers
     */
    mov     r0, #AM335X_TIMER_SIZE      // size of registers
    ldr     r1, Lpaddr
    bl      callout_io_map
    str     r0, Lvaddr

1:
    /*
     * Patch the callout routine
     */
    CALLOUT_PATCH   r4, r0, r1, r2, ip

    ldmia   sp!, {r4, pc}

Lpaddr: .word   AM335X_TIMER2_BASE
Lvaddr: .word   -1


/*
 * --------------------------------------------------------------------------
 * Set clock resolution, and enable interrupt triggering as a side effect.
 * The interrupt remains masked until it is enabled via intr_unmask_omap
 *
 * On entry:
 *  r0 - pointer to syspage_entry
 *  r1 - pointer to qtime_entry
 * --------------------------------------------------------------------------
 */
CALLOUT_START(timer_load_am335x, 0, patch_timer_am335x)
    /*
     * Get the address of the timer registers (patched)
     */
    mov     ip,     #0x000000ff
    orr     ip, ip, #0x0000ff00
    orr     ip, ip, #0x00ff0000
    orr     ip, ip, #0xff000000

    /*
     *  Stop the timer
     */
    ldr     r0, [ip, #AM335X_TIMER_TCLR]
    bic     r0, r0, #AM335X_TIMER_TCLR_ST
    str     r0, [ip, #AM335X_TIMER_TCLR]

    /*
     * Get the load value 
     */
    ldr     r0, [r1, #QT_TIMER_LOAD]

    /*
     * Adjust the load value to suit the OMAP's formula
     * for computing Tint
     */
    rsb     r0, r0, #0

    /*
     * Load counter value
     */
    str     r0, [ip, #AM335X_TIMER_TCRR]
    str     r0, [ip, #AM335X_TIMER_TLDR]

    /*
     * Enable overflow interrupt
     */
    mov     r0, #AM335X_TIMER_OVF_EN_FLAG
    str     r0, [ip, #AM335X_TIMER_IRQENABLE_SET]

    /*
     *  Start the timer
     */
    ldr     r0, [ip, #AM335X_TIMER_TCLR]
    orr     r0, r0, #AM335X_TIMER_TCLR_ST
    str     r0, [ip, #AM335X_TIMER_TCLR]

    mov     pc, lr
CALLOUT_END(timer_load_am335x)


/*
 * --------------------------------------------------------------------------
 * Read the current timer value, relative to the last clock tick
 *
 * On entry:
 *  r0 - pointer to syspage_entry
 *  r1 - pointer to qtime_entry
 *
 * NOTE: this doesn't deal with the counter wrapping, eg. ClockCycles just
 *        at the point where the clock interrupt is triggerred.
 * --------------------------------------------------------------------------
 */
CALLOUT_START(timer_value_am335x, 0, patch_timer_am335x)
    /*
     * Get the address of the timer registers (patched)
     */
    mov     ip,     #0x000000ff
    orr     ip, ip, #0x0000ff00
    orr     ip, ip, #0x00ff0000
    orr     ip, ip, #0xff000000

    /*
     * Latch the current timer value and return qtp->timer_load - value
     */
    ldr     r0, [ip, #AM335X_TIMER_TCRR]
    ldr     r1, [r1, #QT_TIMER_LOAD]
    add     r0, r1, r0

    mov     pc, lr
CALLOUT_END(timer_value_am335x)


/*
 * --------------------------------------------------------------------------
 * Clear timer interrupt.
 *
 * On entry:
 *  r0 - pointer to syspage_entry
 *  r1 - pointer to qtime_entry
 * --------------------------------------------------------------------------
 */
CALLOUT_START(timer_reload_am335x, 0, patch_timer_am335x)
    /*
     * Get the address of the timer registers (patched)
     */
    mov     ip,     #0x000000ff
    orr     ip, ip, #0x0000ff00
    orr     ip, ip, #0x00ff0000
    orr     ip, ip, #0xff000000

    /*
     * Clear overflow interrupt status
     */
    mov     r0, #AM335X_TIMER_OVF_EN_FLAG
    str     r0, [ip, #AM335X_TIMER_IRQSTATUS]

    mov     pc, lr
CALLOUT_END(timer_reload_am335x)

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/ti-am335x/callout_timer_am335x.S $ $Rev: 735852 $";
.previous
#endif
#endif 
