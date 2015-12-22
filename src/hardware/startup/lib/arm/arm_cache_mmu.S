/*
 * $QNXLicenseC:
 * Copyright 2013 QNX Software Systems.
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

	.text
	.align 2
	.globl arm_enable_mmu
	.globl arm_mp_cpuid
	.globl arm_v7_enable_cache
	.globl arm_v7_disable_cache_mmu

/*
 * r0: the address holds the translation table which is supposed
 * to have been filled properly
 * Assume both i-cache and d-cache are clean
 */
arm_enable_mmu:

	mov		r1, #0
	mcr		p15, 0, r1, c8, c7, 0		// invalidate TLBs
	dsb
	isb

	mov		r1, #0
	mcr		p15, 0, r1, c2, c0, 2		// Always use TTBR0

	/*
	 * Set the translation table base
	 */
	orr		r0, r0, #((3 << 3) | 3)
	mcr		p15, 0, r0, c2, c0, 0

	/*
	 * Enable MMU domain 15
	 */
	mov		r0, #(1 << 30)				// Client mode
	mcr		p15, 0, r0, c3, c0, 0

	/* Enable I/D cache, Enable MMU*/
	mrc		p15, 0, r0, c1, c0, 0
	orr		r0, r0, #(1 <<	0)			// enable MMU
	mcr		p15, 0, r0, c1, c0, 0
	dsb
	isb

	mov		pc, lr

/*
 * Supposed to be call if Multiprocessor ID Register is supported
 */
arm_mp_cpuid:

	mrc		p15, 0, r0, c0, c0, 5
	ands	r0, r0, #0x03
	mov		pc, lr

arm_v7_enable_cache:
	stmfd	r13!, {r4, r12, r14}

	/* invalidation i-cache and d-cache before enabling them */
	bl	arm_v7_icache_invalidate
	bl	arm_v7_dcache_flush

	/*
	 * Enable I/D cache
	 */
	mrc	 p15, 0, r0, c1, c0, 0
	orr	 r0, r0, #(1 << 12)		// enable I Cache
	orr	 r0, r0, #(1 << 2)		// enable D Cache
	mcr	 p15, 0, r0, c1, c0, 0
	dsb
	isb

	ldmfd	r13!, {r4, r12, pc}


arm_v7_disable_cache_mmu:

	stmdb	sp!,{lr}
	mrc		p15, 0, lr, c1, c0, 0
	tst		lr, #0x05		// MMU/D-cache enabled?
	ldmeqia	sp!, {pc}

	/*
	 * Flush D-cache
	 */
	bl		arm_v7_dcache_flush

	/*
	 * Turn off MMU and data cache if necessary.
	 * WARNING: assumes we are running with a 1-1 mapping if MMU is enabled.
	 */
	mrc		p15, 0, ip, c1, c0, 0
	bic		ip, ip, #0x00002000		// clear bits 13 (--V-)
	bic		ip, ip, #0x00001000		// clear bit 1 (i-cache)
	bic		ip, ip, #0x00000007		// clear bits 2:0 (-CAM)
	mcr		p15, 0, ip, c1, c0, 0
	dsb
	isb

	/*
	 * Invalidate caches and TLBs.
	 */
	mov		ip, #0
	mcr		p15, 0, ip, c8, c7, 0	// invalidate TLBs
	mcr		p15, 0, ip, c7, c5, 0	// invalidate icache
	dsb
	isb

	ldmia	sp!,{pc}


#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/arm_cache_mmu.S $ $Rev: 738517 $";
.previous
#endif
#endif 
