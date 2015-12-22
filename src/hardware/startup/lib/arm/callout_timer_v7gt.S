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


/*
 * ARMv7 Generic Timer Support
 */

#include "callout.ah"

/*
 * --------------------------------------------------------------------------
 * Set clock resolution, and enable interrupt triggering as a side effect.
 * The interrupt remains masked until it is enabled via intr_unmask_omap
 *
 * On entry:
 *    r0 - pointer to syspage_entry
 *    r1 - pointer to qtime_entry
 * --------------------------------------------------------------------------
 */
CALLOUT_START(timer_load_v7gt, 0, 0)
	/*
	 * Disable event matching.
	 * Any pending event will be de-asserted when we set CNTV_CVAL below
	 */
	mov		r0, #0
	mcr		p15, 0, r0, c14, c3, 1			// CNTV_CTL

	/*
	 * Set CNTV_CVAL to CNTVCT + qtp->timer_load
	 */
	ldr		ip, [r1, #QT_TIMER_LOAD]
	mrrc	p15, 1, r0, r1, c14			// CNTVCT
	adds	r0, r0, ip
	adc		r1, r1, #0					// CNTVCT + qtp->timer_load
	mcrr	p15, 3, r0, r1, c14			// set CNTV_CVAL

	/*
	 * Enable event matching
	 */
	mov		r0, #1
	mcr		p15, 0, r0, c14, c3, 1

	mov		pc, lr
CALLOUT_END(timer_load_v7gt)


/*
 * --------------------------------------------------------------------------
 * Read the current timer value, relative to the last clock tick
 *
 * On entry:
 *    r0 - pointer to syspage_entry
 *    r1 - pointer to qtime_entry
 *
 * --------------------------------------------------------------------------
 */
CALLOUT_START(timer_value_v7gt, 0, 0)
	/*
	 * Elapsed time since last tick = counter + qtp->timer_load - match
	 */
	ldr		ip, [r1, #QT_TIMER_LOAD]
	mrrc	p15, 1, r0, r1, c14			// CNTVCT
	mrrc	p15, 3, r2, r3, c14			// CNTV_CVAL

	/*
	 * Optimise with 32-bit arithmetic since we truncate the result
	 */
	add		r0, r0, ip					// counter + timer_load
	sub		r0, r0, r2					// counter + timer_load - match

	mov		pc, lr
CALLOUT_END(timer_value_v7gt)


/*
 * --------------------------------------------------------------------------
 * Clear timer interrupt.
 *
 * On entry:
 *    r0 - pointer to syspage_entry
 *    r1 - pointer to qtime_entry
 * --------------------------------------------------------------------------
 */
CALLOUT_START(timer_reload_v7gt, 0, 0)
	/*
	 * Advance match by qtp->timer_load
	 * Note that this de-asserts the timer interrupt
	 */
	ldr		ip, [r1, #QT_TIMER_LOAD]
	mrrc	p15, 3, r0, r1, c14
	adds	r0, r0, ip
	adc		r1, r1, #0
	mcrr	p15, 3, r0, r1, c14

	mov		r0, #1
	mov		pc, lr
CALLOUT_END(timer_reload_v7gt)

/*
 * --------------------------------------------------------------------------
 * Return 64-bit counter value:
 * - for LE r0=lo word, r1=hi word
 * - for BE r0=hi word, r1=lo word
 */
CALLOUT_START(clock_cycles_v7gt, 0, 0)
.ifdef VARIANT_le
	mrrc	p15, 1, r0, r1, c14
.else
	mrrc	p15, 1, r1, r0, c14
.endif
	mov		pc, lr
CALLOUT_END(clock_cycles_v7gt)

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/callout_timer_v7gt.S $ $Rev: 701498 $";
.previous
#endif
#endif 
