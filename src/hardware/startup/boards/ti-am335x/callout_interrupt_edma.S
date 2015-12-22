#
# Copyright 2007, 2008, QNX Software Systems. 
# 
# Licensed under the Apache License, Version 2.0 (the "License"). You 
# may not reproduce, modify or distribute this software except in 
# compliance with the License. You may obtain a copy of the License 
# at: http://www.apache.org/licenses/LICENSE-2.0 
# 
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" basis, 
# WITHOUT WARRANTIES OF ANY KIND, either express or implied.
#
# This file may contain contributions from others, either as 
# contributors under the License or as licensors under other terms.  
# Please review this entire file for other proprietary rights or license 
# notices, as well as the QNX Development Suite License Guide at 
# http://licensing.qnx.com/license-guide/ for other information.
#

#include "callout.ah"
#include <arm/am335x.h>

/*
 * -----------------------------------------------------------------------
 * Routines to patch callout code
 *
 * On entry:
 *	r0 - physical address of syspage
 *	r1 - virtual  address of syspage
 *	r2 - offset from start of syspage to start of the callout routine
 *	r3 - offset from start of syspage to read/write data used by callout
 * -----------------------------------------------------------------------
 */
#define IER_OFFSET		0x50
#define IERH_OFFSET		0x54
#define IECR_OFFSET		0x58
#define IECRH_OFFSET	0x5C
#define IESR_OFFSET		0x60
#define IESRH_OFFSET	0x64
#define IPR_OFFSET		0x68
#define IPRH_OFFSET		0x6C
#define ICR_OFFSET		0x70
#define ICRH_OFFSET		0x74
#define IEVAL_OFFSET	0x78

edma_interrupt_patch:
	stmdb	sp!,{r4,lr}
	add		r4, r0, r2			// address of callout routine

	ldr		r0, Lvaddr
	cmp		r0, #-1
	bne		1f

	/*
	 * Map interrupt controller registers
	 */
	mov		r0, #0x1000			// size to map
	ldr		r1, [sp, #8]
	ldr		r1, [r1]
	bl		callout_io_map
	str		r0, Lvaddr

	/*
	 * Patch the callout routine
	 */
1:
	CALLOUT_PATCH	r4, r0, r1, r2, ip
	ldmia	sp!,{r4,pc}

Lvaddr:	.word	-1

/* -----------------------------------------------------------------------
 * Returns interrupt number in r4
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_id_edma, 0, edma_interrupt_patch)

	mov		ip,     #0x000000ff
	orr		ip, ip, #0x0000ff00
	orr		ip, ip, #0x00ff0000
	orr		ip, ip, #0xff000000

	/* Check for HIGH / LOW Channel */
	ldr		r0, [ip, #IPR_OFFSET]
	ldr		r2, [ip, #IER_OFFSET]
	ands	r0, r2, r0
	movne	r3, #0
	moveq	r3, #32
	addeq	ip, ip, #IPRH_OFFSET-IPR_OFFSET
	ldreq	r0, [ip, #IPR_OFFSET]
	ldreq	r2, [ip, #IER_OFFSET]
	andeq	r0, r2, r0
	clz		r4, r0
	rsbs	r4, r4, #31
	blt		1f

	mov		r2, #1
	mov		r2, r2, lsl r4
	str		r2, [ip, #IECR_OFFSET]
	str		r2, [ip, #ICR_OFFSET]

	add		r4, r4, r3
1:
CALLOUT_END(interrupt_id_edma)


/*
 * -----------------------------------------------------------------------
 * Acknowledge specified interrupt
 *
 * On entry:
 *	r4 contains the interrupt number
 *	r7 contains the interrupt mask count
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_eoi_edma, 0, edma_interrupt_patch)

	mov		ip,     #0x000000ff
	orr		ip, ip, #0x0000ff00
	orr		ip, ip, #0x00ff0000
	orr		ip, ip, #0xff000000

	/*
	 * done with this interrupt
	 */
	mov		r2, #1
	str		r2, [ip, #IEVAL_OFFSET]

	/*
	 * Only unmask interrupt if mask count is zero
	 */
	teq     r7, #0
	bne     1f

	cmp		r4, #32
	movlt   r0, r2, lsl r4
	subge	r1, r4, #32
	movge	r0, r2, lsl r1
	addge	ip, ip, #IESRH_OFFSET-IESR_OFFSET
	str		r0, [ip, #IESR_OFFSET]

1:
CALLOUT_END(interrupt_eoi_edma)


/*
 * -----------------------------------------------------------------------
 * Mask specified interrupt
 *
 * On entry:
 *	r0 - syspage_ptr
 *	r1 - interrupt number
 *
 * Returns:
 *	r0 - error status
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_mask_edma, 0, edma_interrupt_patch)
	/*
	 * Get the interrupt controller base address (patched)
	 */
	mov		ip,     #0x000000ff
	orr		ip, ip, #0x0000ff00
	orr		ip, ip, #0x00ff0000
	orr		ip, ip, #0xff000000

	cmp		r1, #32
	subge	r1, r1, #32
	addge	ip, ip, #IECRH_OFFSET-IECR_OFFSET

    mov     r2, #1
    mov     r2, r2, lsl r1

	/*
	 * mask interrupt
	 */
	str		r2, [ip, #IECR_OFFSET]

    mov     r0, #0
    mov     pc, lr
CALLOUT_END(interrupt_mask_edma)


/*
 * -----------------------------------------------------------------------
 * Unmask specified interrupt
 *
 * On entry:
 *	r0 - syspage_ptr
 *	r1 - interrupt number
 *
 * Returns:
 *	r0 - error status
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_unmask_edma, 0, edma_interrupt_patch)
	/*
	 * Get the interrupt controller base address (patched)
	 */
	mov		ip,     #0x000000ff
	orr		ip, ip, #0x0000ff00
	orr		ip, ip, #0x00ff0000
	orr		ip, ip, #0xff000000

	cmp		r1, #32
	subge	r1, r1, #32
	addge	ip, ip, #IESRH_OFFSET-IESR_OFFSET

    mov     r2, #1
    mov     r2, r2, lsl r1

	/*
	 * unmask interrupt
	 */
	str		r2, [ip, #IESR_OFFSET]

    mov     r0, #0
    mov     pc, lr
CALLOUT_END(interrupt_unmask_edma)

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/ti-am335x/callout_interrupt_edma.S $ $Rev: 763679 $";
.previous
#endif
#endif 
