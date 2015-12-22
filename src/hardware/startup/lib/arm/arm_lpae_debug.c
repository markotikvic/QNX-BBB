/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems. 
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

#if (_PADDR_BITS-0) == 64

// Debug/dump code is placed in it's own file
// It will not be linked into the image unless LPAE_DEBUG is appropriately defined

#include "startup.h"

// L1 entries are all in a single page
#define	CPU_LPAE_L1_TABLE(cpu)	((uintptr_t)(L1_paddr + (cpu * ARM_LPAE_L1_SIZE)))

void kp64(uint64_t val) {
	kprintf("%x:%x",
			(uint32_t)((val>>32) & 0xffffffff),
			(uint32_t)(val & 0xffffffff));
	return;
}

char* ks64(uint64_t val) {
	static char buf[35];
	
	ksprintf(buf,
			 "(%x:%x)",
			(uint32_t)((val>>32) & 0xffffffff),
			(uint32_t)(val & 0xffffffff));
	return buf;
}


static void decode_attrs(lpae_pd_t value)
{
	kprintf(" ");

	if ((value & ARM_LPAE_DESC_VALID) != ARM_LPAE_DESC_VALID) {
		kprintf("INVALID\n");
		return;
	} else {
		kprintf("VALID ");
	}

	if ((value & ARM_LPAE_DESC_TABLE) == ARM_LPAE_DESC_TABLE) {
		kprintf("TBL ");
	} else {
		kprintf("BLK ");
	}

	if ((value & ARM_LPAE_DESC_XN) == ARM_LPAE_DESC_XN) {
		kprintf("XN ");
	}
	if ((value & ARM_LPAE_DESC_PXN) == ARM_LPAE_DESC_PXN) {
		kprintf("PXN ");
	}

	if ((value & ARM_LPAE_DESC_NG) == ARM_LPAE_DESC_NG) {
		kprintf("NG ");
	}
	if ((value & ARM_LPAE_DESC_AF) == ARM_LPAE_DESC_AF) {
		kprintf("AF ");
	}

	// Can only be one of these
	if ((value & ARM_LPAE_DESC_SH_MASK) == ARM_LPAE_DESC_SH_INNER_SHAREABLE) {
		kprintf("InSh ");
	}
	if ((value & ARM_LPAE_DESC_SH_MASK) == ARM_LPAE_DESC_SH_OUTER_SHAREABLE) {
		kprintf("OutSh ");
	}
	if ((value & ARM_LPAE_DESC_SH_MASK) == ARM_LPAE_DESC_SH_NON_SHAREABLE) {
		kprintf("NonSh ");
	}

	// Can only be one of these
	if ((value & ARM_LPAE_DESC_AP_MASK) == ARM_LPAE_DESC_AP_ALL_RO) {
		kprintf("ALL-RO ");
	}
	if ((value & ARM_LPAE_DESC_AP_MASK) == ARM_LPAE_DESC_AP_PRIV_RW) {
		kprintf("PRIV-RW ");
	}
	if ((value & ARM_LPAE_DESC_AP_MASK) == ARM_LPAE_DESC_AP_PRIV_RO) {
		kprintf("PRIV-RO ");
	}
	if ((value & ARM_LPAE_DESC_AP_MASK) == ARM_LPAE_DESC_AP_ALL_RW) {
		kprintf("ALL-RW ");
	}

	kprintf("AttrIdx:%d ", (uint32_t)((value & ARM_LPAE_DESC_ATTRINDX_MASK) >> 2) );

	kprintf("\n");
	return;
}


static void dump_ptbl_cpu(unsigned cpu)
{
	lpae_pd_t *l1;
	lpae_pd_t *l2;
	lpae_pd_t *l3;
	unsigned   i,j,k;
	uintptr_t  basevaddr;
	lpae_pd_t  value;
	
	// Start of L1 table from global value
	l1 = (lpae_pd_t*) ((uintptr_t)CPU_LPAE_L1_TABLE(cpu));

	kprintf("PTBL_START cpu:%d\n", cpu);
	kprintf("L1_paddr:%s \n", ks64(L1_paddr));
	
	// Check each L1 entry
	for (i=0; i<4; i++) {

		if (*l1) {
			// Something in the L1
			basevaddr = i<<30;
			kprintf("L1 idx %d addr:%x vaddr:%x val:", i, l1, basevaddr);
			kp64(*l1);
			kprintf("\n");

			// Sanity check l2 value against global 

			l2 = (lpae_pd_t*) ((uintptr_t)(*l1 & ARM_LPAE_DESC_ADDR_MASK));  // assumes addr is 32-bit!
			basevaddr = i<<30;
			kprintf("     L2 base addr:%x covers:%x-%x \n", l2, basevaddr, basevaddr+0x40000000-1);

			// Look at each L2 entry
			for (j=0; j<512; j++) {
				if (*l2) {
					// non-zero entry in L2
					basevaddr = i<<30 | j<<21;
					kprintf("     L2 idx %d addr:%x covers:%x val:", j, l2, basevaddr);
					kp64(*l2);

					value = *l2;

					decode_attrs(value);

					if ((value & ARM_LPAE_DESC_VT_MASK) == ARM_LPAE_DESC_VALID) {
						// It's valid but not a table... skip
						l2++;
						continue;
					}

					l3 = (lpae_pd_t*)((uintptr_t)(*l2 & ARM_LPAE_DESC_ADDR_MASK)); // Assumes addr fits in 32 bits
					basevaddr = i<<30 | j<<21;
					kprintf("       L3 base addr:%x covers:%x-%x\n", l3, basevaddr, basevaddr+0x200000-1 );

					// Look at each L3 entry
					for (k=0; k<512; k++) {
						if (*l3) {
							// non-zero entry in L3
							basevaddr = i<<30 | j<<21 | k<<12;
							kprintf("       L3 idx %d addr:%x covers:%x val:", k, l3, basevaddr);

							value = *l3;
							kp64(value); 
							decode_attrs(value);
						}
						l3++;
					} // each l3
				} // valid l2
				l2++;
			} // each l2
		} else {
			kprintf("L1 idx %d vaddr:%x empty\n", i, l1);
		} // valid L1
		l1++;
	} // For each L1

	kprintf("PTBL_END\n");
}


void dump_ptbl(void) {
	unsigned cpu;
	kprintf("LPAE: %s:%d\n", __func__, __LINE__);
	for (cpu = 0; cpu < lsp.syspage.p->num_cpu; cpu++) {
		dump_ptbl_cpu(cpu);
	}
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/arm_lpae_debug.c $ $Rev: 759590 $")
#endif

#endif
