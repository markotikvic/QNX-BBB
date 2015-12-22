#
# Copyright 2014, QNX Software Systems. 
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

		
	.text
	.align 2
	.globl vstart_lpae

/*
 * void	vstart_lpae(uintptr_t syspageptr, unsigned entry_point, unsigned cpunum)
 *
 * based on vstart_v7, extended to support lpae mode
 *
 * Enable the mmu and jump to the next program's entry point
 * The next program is responsible for tearing down the 1-1 section
 * mapping of the startup program set up by init_mmu().
 */
		
/*
 * r0 = syspageptr, r1=entry_point, r2 = cpunum
 * Called once per cpu
*/		
		
vstart_lpae:
	mov		r4, r1     // Save r1/r2 in r4/r5
	mov		r5, r2

    mov     r6, #0                      // high 32-bits = 0

	// TTBR0 gets pointed at L1 
	ldr		ip, =L1_paddr
	ldr		ip, [ip]
	add		ip, ip, r5, lsl #5			// low value = L1_paddr + (cpu * 32 bytes)
	mcrr	p15, 0, ip, r6, c2		    // move 64-bit value from r6/ip into TTBR0

	// TTBR1 gets pointed directly at kernel L2 for this cpu, skipping the L1 table
	ldr		ip, =L2_paddr
	ldr		ip, [ip]
	mov     r1, #(4096*3)
	add		ip, ip, r1        			// kernel tables start after 3 pages of user table
	add		ip, ip, r5, lsl #12			// low value = L2_paddr + (cpu * __PAGESIZE (2^12))
	mcrr	p15, 1, ip, r6, c2  		// move 64-bit value into TTBR1

		
	// TTBCR bit assignments	   mask
	// EAE   = LPAE in use       = 0x80000000
	// SH1   = Sharability       = 0x30000000 
	// ORGN1 = Outer cachability = 0x0c000000
	// IRGN1 = Inner cachability = 0x03000000
	// T1SZ  = TTBR1 size        = 0x00070000
	// SH0   = sharability       = 0x00003000
	// ORGN0 = Outer cachability = 0x00000c00
	// IRGN0 = Inner cachability = 0x00000300
	// t0SZ  = TTBR0 size        = 0x00000007
	// 
	// Shareability of translation table memory:	
	//   00=non-sharable; 01 = unpredictable; 10 = outer-sharable; 11 = inner-sharable
	//
	// Cachability of translation table memory:	
	//   00=non-cachable; 01 = WB/WA cacheable; 10 = WT cachable; 11 = WB no WA cacheable
	//	
	// 0x80010001 worked for bringup, but it's slowww.
	// 
	// 0x80000000 = LPAE; EPD1=0; EPD0=0; A1=0
	// 0x30000000 = sh1   ttbr1 shareability - inner sharable 11
	// 0x00000000 = orgn1 ttbr1 outer memory - non cacheable  00
	// 0x01000000 = irgn1 ttbr1 inner memory - wb/wa cacheable  01
	// 0x00020000 = t1sz 1G
	// 0x00003000 = sh0   ttbr0 shareability - inner sharable 11
	// 0x00000000 = orgn0 ttbr0 outer memory - non cacheable  00
	// 0x00000100 = irgn0 ttbr0 inner memory - wb/wa cacheable  01
	// 0x00000000 = t0sz 3G
	// ----------	
	// 0xb1023100 = TTBCR value
				
	mov		ip,#0xb1000000
	add ip, #0x23000		
	add ip, #0x100		
	mcr		p15, 0, ip, c2, c0, 2		
		

	// From here down is the same as vstart_v7
	/*
	 * Enable the MMU, using read-modify-write to preserve reserved bits.
	 */
	ldr		r2, =mmu_cr_clr
	ldr		r3, =mmu_cr_set
	ldr		r2, [r2]
	ldr		r3, [r3]
	mrc		p15, 0, lr, c1, c0, 0   // read from SCTLR
	bic		ip, lr, r2
	orr		ip, ip, r3
	dsb
	isb
	mcr		p15, 0, ip, c1, c0, 0   // Write to SCTLR
	dsb
	isb
	mov		ip, #0

	/*
	 * Invalidate the caches and TLBs
	 */
	.align 5
	mcr		p15, 0, ip, c7, c5, 0	// invalidate instruction caches
	mcr		p15, 0, ip, c8, c7, 0	// invalidate TLBs
	dsb
	isb

	/*
	 * Call entry_point(_syspage_ptr, cpunum)
	 */
	mov		r1, r5
	mov		pc, r4

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL:$ $Rev:$";
previous
#endif
#endif
