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
	.globl arm_a8_jacinto_enable_cache
	.globl arm_a8_jacinto_setup_auxcr

arm_a8_jacinto_enable_cache:
	stmfd	r13!, {r4, r12, r14}

	/* invalidation i-cache and d-cache before enabling them */
	bl		arm_v7_icache_invalidate
	bl		arm_v7_dcache_flush

	 /* Enable L2 d-cache through AUXCR */
	mrc		p15, 0, r2, c1, c0, 1
	orr		r0, r2, #(1 << 1)	// set L2EN

	/* SMI call function ID 0x100: "write ACR by R0 value" */
	mov		r12, #0x100
	.word	0xe1600070

	/*
	 * Enable I/D cache
	 */
	mrc		p15, 0, r0, c1, c0, 0
	orr		r0, r0, #(1 << 12)		// enable I Cache
	orr		r0, r0, #(1 <<	2)		// enable D Cache
	mcr		p15, 0, r0, c1, c0, 0
	dsb
	isb

	ldmfd	r13!, {r4, r12, pc}

arm_a8_jacinto_setup_auxcr:
	stmfd	r13!, {r4, r12, r14}

	mrc		p15, 0, r2, c1, c0, 1

	/* Set the generic A8 AUXCR bits */
	bic		r0, r2, #(1 << 4)	// clear ASA bit

	/*
	 * Clearing IBE makes branch predictor operations behave as NOPs
	 * This significantly improves context switch performance.
	 *
	* The errata workarounds requiring IBE are related to Thumb/Thumb2
	* code execution which is not supported by QNX.
	*/
	bic		r0, r0, #(1 << 6)

	/* SMI call function ID 0x100: "write ACR by R0 value" */
	mov		r12, #0x100
	.word	0xe1600070

	ldmfd	r13!, {r4, r12, pc}

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/arm_a8_jacinto_cache.S $ $Rev: 723524 $";
.previous
#endif
#endif
