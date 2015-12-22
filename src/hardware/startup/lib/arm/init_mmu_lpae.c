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

#include "startup.h"




/*
 * Create section mappings in the 1-1 mapping area for the largest RAM bank
 */
static void
lpae_map_1to1_ram()
{
	unsigned	as_off = AS_NULL_OFF;
	paddr32_t   base = 0;
	unsigned	size = 0;
	uintptr_t   vaddr;

	while ((as_off = as_find(as_off, "ram", 0)) != AS_NULL_OFF) {
		struct asinfo_entry *as = (struct asinfo_entry *)((uintptr_t)lsp.asinfo.p + as_off);
		unsigned			sz  = as->end - as->start + 1;

		if (as->start >= ARM_LPAE_4G_BOUNDARY) {
			// 1-to-1 has to be below 4GB; ignore this region
			continue;
		}
		
		if (sz > size) {
			base = (paddr32_t)as->start;
			size = sz;
		}
	}
	if (size == 0) {
		crash("no ram entry in asinfo?");
	}
	if (base & ARM_LPAE_BMASK) {
		if (debug_flag) {
			kprintf("WARNING: RAM base at %x - not using 1to1 area\n", base);
		}
		return;
	}

	/*
	 * Limit mapping to 256MB
	 */
	if (size > ARM_1TO1_SIZE) {
		size = ARM_1TO1_SIZE;
	}

	// With LPAE we map in block-sized chunks
	size = (size + ARM_LPAE_BMASK) & ~ARM_LPAE_BMASK;
	as_add(base, base + size - 1, AS_ATTR_RAM, "1to1", as_default());
	
	for (vaddr = ARM_1TO1_BASE; size; vaddr += ARM_LPAE_BSIZE, base += ARM_LPAE_BSIZE, size -= ARM_LPAE_BSIZE) {
		arm_lpae_bmap(vaddr, base, ARM_PTE_RW | ARM_PTE_CB);  // Block map
	}
}


void dump_ptbl(void);

/*
 * Initialize page tables for the lpae case
 */
void
init_mmu_lpae(void)
{
	unsigned	base;
	unsigned	ncpu = lsp.syspage.p->num_cpu;
	unsigned	l1_size = ncpu * ARM_LPAE_L1_SIZE;

	// Get the CPU-specific PTE descriptors - Not implemented for A15 under LPAE...
	arm_pte_setup();

	// As paddr == vaddr in startup, we require the L1/L2 tables to be allocated from memory < 4G
	// However, if calloc_ram ever starts handing out high memory, things will promptly stop working.
	// It will not be a subtle failure.

	// LPAE L1 table is only 32 bytes per cpu.  (So we can only get 512 on a single page!)
	L1_paddr = calloc_ram(l1_size, __PAGESIZE);

	if (debug_flag) {
		kprintf("LPAE: %s:%d allocated 0x%x bytes at 0x%x for L1 page table\n", __func__, __LINE__, l1_size, L1_paddr);
	}
	
	// One page per cpu for kernel L2; three pages for user L2, shared between cpus
	// We allocate user and kernel l2 pages together so we can pass both to procnto with one param
	L2_size = (ncpu * ARM_LPAE_L2_SIZE) + ARM_LPAE_USER_L2_SIZE;
	L2_paddr = calloc_ram(L2_size, __PAGESIZE);

	if (debug_flag) {
		kprintf("LPAE: %s:%d allocated 0x%x bytes at 0x%x for L2 page tables\n", __func__, __LINE__, L2_size, L2_paddr);
	}
	
	// Init the L1/L2 tables and make the kernel L2/L3 tables accessible at the specified vaddrs
	// This function uses the above globals directly; doesn't need them as params.
	arm_lpae_table_init(ARM_LPAE_KERN_L2, ARM_LPAE_KERN_L3);
	
	// Map the L1 table into virtual space
	L1_vaddr = arm_map(~0L, L1_paddr, l1_size, ARM_MAP_NOEXEC | ARM_PTE_RW | armv_chip->pte_attr);

	// Block map startup code to allow transition to virtual addresses.
	// This 1-1 mapping is also used by kdebug to access the imagefs.
	// procnto uses syspage->un.arm.startup_base/startup_size to unmap it.
	// Under LPAE we use 2MB blocks instead of v6 1MB sections

	startup_base = shdr->ram_paddr & ~ARM_LPAE_BMASK;
	startup_size = shdr->ram_size;
	for (base = startup_base; base < startup_base + startup_size; base += ARM_LPAE_BSIZE) {
		arm_lpae_bmap(base, base, ARM_PTE_RO);
	}

	/*
	 * Map RAM into the 1-1 mapping area
	 */
	lpae_map_1to1_ram();

	if (debug_flag>2) {
		kprintf("%s dump table\n", __func__);
	    dump_ptbl();
		kprintf("%s dump table done\n", __func__);
	}
		
}



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/init_mmu_lpae.c $ $Rev: 759590 $")
#endif

#endif
