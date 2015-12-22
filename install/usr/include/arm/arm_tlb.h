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

#ifndef __ARM_TLB_H__
#define __ARM_TLB_H__

/* To make it easy, we use 1M section in IPL/startup */
#define PAGE_SHIFT			20
#define PAGE_SIZE			(1 << PAGE_SHIFT)	/* 1M */
#define TLB_SIZE			0x1000				/* 4096 1M sections to cover addr 0 -- 0xFFFFFFFF */

/**
 * TLB first level descriptor format for sections:
 *	Bits[1:0]: type, 2 - section
 *	Bits[2]: Bufferable
 *	Bits[3]: Cacheable
 *	Bits[8:5]: domain, 15 is used
 *	Bits[11:10]: Access Permission, 3 - R/W
 *	Bits[18]: 0 - 1M section; 1 - 16M supersection
 */
#define SECTION			(2 << 0)
#define DOMAIN_15		(15 << 5)
#define AP_RW			(3 << 10)
#define B_ENA			(1 << 2)
#define B_DIS			(0 << 2)
#define C_ENA			(1 << 3)
#define C_DIS			(0 << 3)
#define SEC_1M			(0 << 18)
#define BA_MASK			(0xFFF00000)

typedef struct arm_tlb {
	unsigned long start;
	unsigned long len;
	unsigned long attr;
} arm_tlb_t;

/* 
 * Construct a translation table to cover the whole 32 bit address space
 * Please NOTE that the tlb needs to assured that it's 16K aligned (0x4000)
 */
static __inline__ void arm_setup_tlb(arm_tlb_t *table, unsigned long *tlb)
{
	unsigned base;
	unsigned offset = 0;

	while (table && (table->start != -1L)) {
		base = table->start;

		/* Fill the holes: cache/buff disabled  */
		for (; offset < (base >> PAGE_SHIFT); offset++) {
			tlb[offset] = (offset << PAGE_SHIFT) | SECTION | AP_RW | DOMAIN_15 | B_DIS | C_DIS;
		}

		for (; (base >= table->start) && (table->len > base - table->start) && (offset < TLB_SIZE); base += PAGE_SIZE) {
			tlb[offset++] = (base & BA_MASK)				/* 12 bits section base address  */
						| table->attr 						/* Normally cacheable/bufferable */
						| (SECTION | AP_RW | DOMAIN_15);	/* common section attributes */
		}
		table += 1;
	}

	/* Complete the TLB table */
	for (; offset < TLB_SIZE; offset++) {
		tlb[offset] = (offset << PAGE_SHIFT) | SECTION | AP_RW | DOMAIN_15 | B_DIS | C_DIS;
	}
}
#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/arm/arm_tlb.h $ $Rev: 724099 $")
#endif
