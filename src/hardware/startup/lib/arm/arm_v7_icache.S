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
	.global	arm_v7_icache_invalidate

/* Invalidate i-Cache */
arm_v7_icache_invalidate:
	mov     r0, #0
	mcr     p15, 0, r0, c7, c5, 0
	dsb
	isb
	mov     pc, lr

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/arm_v7_icache.S $ $Rev: 723524 $";
.previous
#endif
#endif
