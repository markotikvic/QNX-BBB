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

// The whole file is protected by #ifdef so that nothing here will exist
// in the 32-bit libstartup.

#if (_PADDR_BITS-0) == 64

#include "startup.h"

//#define LPAE_DEBUG 1
//#define LPAE_DEBUG (0)
#define LPAE_DEBUG (debug_flag>3)

void dump_ptbl(void);
void kp64(uint64_t val);
char* ks64(uint64_t val);


/*
 * lpae_pd_t is a page descriptor.
 *
 * The ARM documentation distinguishes between table, block, and page descriptors.
 * Table descriptors point to other tables.      (L1/L2)
 * Block descriptors describe 2MB memory blocks. (L2)
 * Page descriptors describe 4Kb memory pages.   (L3)
 *
 * These are mostly the same; there are differences in what fields are valid.
 * This code uses the lpae_pd_t type for all of these uses.
 * 
 */


// As paddrs are cast to pointers, it is assumed that L1s and L2s are allocated from memory < 4G

// L1 entries are all in a single page
#define	CPU_LPAE_L1_TABLE(cpu)	((uintptr_t)(L1_paddr + (cpu * ARM_LPAE_L1_SIZE)))

// Kernel L2 table is a single page, one per cpu
// The kernel L2 pages start after the user L2 
#define	CPU_LPAE_KERN_L2(cpu)	((uintptr_t)(L2_paddr + ARM_LPAE_USER_L2_SIZE +(cpu * ARM_LPAE_KERN_L2_SIZE)))

// The user L2 is three pages long, shared between cpus
#define	CPU_LPAE_USER_L2	((uintptr_t)(L2_paddr))


static lpae_pd_t 
lpae_set_prot(paddr64_t paddr, unsigned flags)
{
	lpae_pd_t pd;

	pd = paddr;

	pd |= ARM_LPAE_DESC_AF; // No access faults

	// ARM_MAP_SYSPAGE/ARM_MAP_SYSPAGE_RO aren't flags, they are discrete values
	// So we handle these inline and then GTFO
	if (flags == ARM_MAP_SYSPAGE) {
		// When using LPAE, we don't have a kern-rw/user-ro mode
		// So we make one mapping for kernel and one for users
		pd |= ARM_LPAE_DESC_ATTR_NORMAL;        // Currently attr zero, but set it for correctness
		pd |= ARM_LPAE_DESC_SH_INNER_SHAREABLE; // All RAM is inner-shareable on A15/omap5
		pd |= ARM_LPAE_DESC_AP_PRIV_RW;         // kernel read/write; global
		if (LPAE_DEBUG) {
			kprintf("LPAE: %s:%d syspage pd:%s \n", __func__, __LINE__, ks64(pd) );
		}
		return pd;
	} else if (flags == ARM_MAP_SYSPAGE_RO) {
		// Read-only version of syspage/cpupage for user access
		pd |= ARM_LPAE_DESC_ATTR_NORMAL;        // Currently attr zero, but set it for correctness
		pd |= ARM_LPAE_DESC_SH_INNER_SHAREABLE; // All RAM is inner-shareable on A15/omap5
		pd |= ARM_LPAE_DESC_AP_ALL_RO;          // world read-only
		if (LPAE_DEBUG) {
			kprintf("LPAE: %s:%d syspage_RO paddr:%s \n", __func__, __LINE__, ks64(pd) );
		}
		return pd;
	}

	// Set type and shareability
	if (flags & ARM_MAP_DEVICE) {
		pd |= ARM_LPAE_DESC_ATTR_DEVICE;

		// All device memory is treated as outer-shareable under LPAE as per ARM ARM A.3.5.2
		// The SH (shareablility) bits are ignored; but set them anyway
		pd |= ARM_LPAE_DESC_SH_OUTER_SHAREABLE;
		if (LPAE_DEBUG) {
			kprintf("LPAE: %s:%d device map pd:%s \n", __func__, __LINE__, ks64(pd) );
		}
	} else if (flags & ARM_MAP_STORDER) {
		pd |= ARM_LPAE_DESC_ATTR_STORDER;

		// All strongly-ordered memory is treated as outer-shareable under LPAE as per ARM ARM A.3.5.2
		// The SH (shareablility) bits are ignored; but set them anyway
		pd |= ARM_LPAE_DESC_SH_OUTER_SHAREABLE;
		if (LPAE_DEBUG) {
			kprintf("LPAE: %s:%d strongly-ordered map pd:%s \n", __func__, __LINE__, ks64(pd) );
		}
	} else if (flags & ARM_MAP_NOCACHE) {
		pd |= ARM_LPAE_DESC_ATTR_NOCACHE;

		// Normal, uncached memory. Make this inner-shareable.
		pd |= ARM_LPAE_DESC_SH_INNER_SHAREABLE;
		if (LPAE_DEBUG) {
			kprintf("LPAE: %s:%d uncached map pd:%s \n", __func__, __LINE__, ks64(pd) );
		}
	} else {
		pd |= ARM_LPAE_DESC_ATTR_NORMAL;  // Currently attr zero, but set it for correctness

		// All RAM is inner-shareable on A15/omap5
		pd |= ARM_LPAE_DESC_SH_INNER_SHAREABLE;
	}

	if (flags & ARM_PTE_U) {
		if (flags & ARM_PTE_RW) {
			// User r/w
			pd |= ARM_LPAE_DESC_AP_ALL_RW;
		} else {
			// User r/o
			pd |= ARM_LPAE_DESC_AP_ALL_RO;
		}

		// This is a user mapping - should be non global
		pd |= ARM_LPAE_DESC_NG;
		
	} else {
		if (flags & ARM_PTE_RW) {
			// Kernel r/w
			pd |= ARM_LPAE_DESC_AP_PRIV_RW;
		} else {
			// Kernel r/o
			pd |= ARM_LPAE_DESC_AP_PRIV_RO;
		}
	}

	if (flags & ARM_MAP_NOEXEC) {
		pd |= (ARM_LPAE_DESC_XN|ARM_LPAE_DESC_PXN);  // No-execute for user and privileged
	}

	return pd;
}


/*
 * Point the appropriate L2 entry at the provided L3 page table for a given cpu
 */
static void
lpae_map_l3_cpu(unsigned cpu, paddr64_t l3_ptbl, uintptr_t vaddr)
{
	lpae_pd_t *l2;
	lpae_pd_t value;

	// Find the right l2 entry for this vaddr and cpu
	if (ARM_LPAE_USER_SPACE(vaddr)) {
		l2 = (lpae_pd_t *)CPU_LPAE_USER_L2 + ARM_LPAE_VADDR_TO_3GL2_IDX(vaddr);
	} else {
		l2 = (lpae_pd_t *)CPU_LPAE_KERN_L2(cpu) + ARM_LPAE_VADDR_TO_1GL2_IDX(vaddr);
	}

	// l3_ptbl contains the physical address of a new L3 page table
	// We update the appropriate L2 page table entry to point to this new table

	value = lpae_set_prot(l3_ptbl, ARM_PTE_RW);
	value |= ARM_LPAE_DESC_TABLE | ARM_LPAE_DESC_VALID;
	*l2 = value;

	if (LPAE_DEBUG) {
		kprintf("LPAE: %s:%d entry cpu:%d vaddr:%x l2:%x value: ", __func__, __LINE__, cpu, vaddr, l2);
		kp64(value); kprintf("\n");
	}
}	
	


/*
 * Map a 4K L3 page table into L2 table on all cpus
 */
void
lpae_map_ptbl(paddr64_t pt, uintptr_t vaddr)
{
	unsigned cpu;

	if (LPAE_DEBUG) {
		kprintf("LPAE: %s:%d vaddr:%x pt:", __func__, __LINE__, vaddr); kp64(pt); kprintf("\n");
	}

	for (cpu = 0; cpu < lsp.syspage.p->num_cpu; cpu++) {
		lpae_map_l3_cpu(cpu, pt, vaddr); 
	}
}


/*
 * Called once to set things up.
 *
 * Initialize the page tables. 
 * Makes the L2 tables directly accessible at vaddr
 *
 * This function uses the L1/L2 globals directly; doesn't need them as params.
 *
 */
 
void
arm_lpae_table_init(uintptr_t l2_vaddr, uintptr_t l3_vaddr)
{
	unsigned i, cpu;
	paddr_t ker_l2;

	if (LPAE_DEBUG) {
		kprintf("LPAE: %s:%d entry l2_vaddr:%x ", __func__, __LINE__, l2_vaddr);
		kprintf("L1_paddr:%s ", ks64(L1_paddr));
		kprintf("L2_paddr:%s \n", ks64(L2_paddr));
	}

	// First, we will point the L1 table entries at the L2 pages for each cpu
	for (cpu = 0; cpu < lsp.syspage.p->num_cpu; cpu++) {
		lpae_pd_t *l1 = (lpae_pd_t *)CPU_LPAE_L1_TABLE(cpu);
		lpae_pd_t value;

		// User pages for all cpus point to the same place
		value = (lpae_pd_t) CPU_LPAE_USER_L2;
		value |= ARM_LPAE_DESC_TABLE | ARM_LPAE_DESC_VALID;

		// On each cpu, initialize the userspace pagetable entries
		// First three entries point to user l2 pages

		for (i=0; i<3; i++) {
			if (LPAE_DEBUG) {
				kprintf("LPAE: %s:%d cpu:%d user L2 l1:%x val:", __func__, __LINE__, cpu, l1 );
				kp64(value); kprintf("\n");
			}
			*l1 = value;
			
			value += __PAGESIZE;
            l1++;
		}

		// Kernel L1 pages are different for each cpu
		value = (lpae_pd_t) CPU_LPAE_KERN_L2(cpu);
		value |= ARM_LPAE_DESC_TABLE | ARM_LPAE_DESC_VALID;

		if (LPAE_DEBUG) {
			kprintf("LPAE: %s:%d cpu:%d kernel L2 l1:%x val:", __func__, __LINE__, cpu, l1 );
			kp64(value); kprintf("\n");
		}

		*l1++ = value;  // 4th entry point to kernel L2 page

	}

	// L1 is all set, and points to the appropriate L2 pages
	// We assume the L2 contains all zeroes at this point.

	// Next, we're going to arrange for the virtual address 'l2_vaddr' to point at our L2 page table.
	// We do this by arranging for the right L3 entries to point to the L2.
	// Then, when the mmu is turned on, an access to 'l2_vaddr' will result in an access to the page table.
	// This is different on every cpu, so we need an L3 table for each

	for (cpu = 0; cpu < lsp.syspage.p->num_cpu; cpu++) {
		paddr_t l3_paddr;
		lpae_pd_t *l3;
		lpae_pd_t value;

		// Allocate a fresh page for use as an L3 page for this cpu
		l3_paddr = calloc_ram(ARM_LPAE_L3_SIZE, __PAGESIZE);

		// Find the right slot for 'l2_vaddr' within this single L3 page table
		l3 = (lpae_pd_t *)((uintptr_t)l3_paddr);
		l3 += ARM_LPAE_VADDR_TO_L3_IDX(l2_vaddr);

		if (LPAE_DEBUG) {
			kprintf("LPAE: cpu:%d New l3 page paddr:%s l3:%x \n", cpu, ks64(l3_paddr), l3); 
		}

		// Verify that this is a kernel-space vaddr
		if (ARM_LPAE_USER_SPACE(l2_vaddr)) {
			kprintf("LPAE: ERROR! l2_vaddr:%x not in kernel space\n", l2_vaddr);
		}

		ker_l2 = CPU_LPAE_KERN_L2(cpu);
		if (LPAE_DEBUG) {
			kprintf("LPAE: ker_l2: "); kp64(ker_l2); kprintf("\n");
		}

		// Turn the L2 paddr into an L3 page table entry
		value = lpae_set_prot(ker_l2, ARM_PTE_RW);
		value |= ARM_LPAE_DESC_VALID | ARM_LPAE_DESC_TABLE;  // Valid table

		// Now point a kernel l3 table entry at the kernel l2
		if (LPAE_DEBUG) {
			kprintf("LPAE: l3:%x value: ", l3); kp64(value); kprintf("\n");
		}
		*l3 = value;

		// Don't forget to populate the L2 entry to point to the new L3 table
		lpae_map_l3_cpu(cpu, l3_paddr, l2_vaddr);

		// Point an L3 entry at the kernel L2 table
		// This gives the kernel access to its own L3 page table entries at "l3_vaddr"
		lpae_map_l3_cpu(cpu, ker_l2, l3_vaddr);

	}

	// And I think that's it...
	if (LPAE_DEBUG) {
		dump_ptbl();
	}

	if (LPAE_DEBUG) {
		kprintf("LPAE: %s:%d exit\n", __func__, __LINE__);
	}

}


/*
 * Map a 2MB block into the L2 table.
 */
void
arm_lpae_bmap(uintptr_t vaddr, paddr_t paddr, unsigned flags)
{
	lpae_pd_t  value;
	lpae_pd_t *l2;

	if (LPAE_DEBUG) {
		kprintf("LPAE: %s:%d enter vaddr:%x paddr:%s \n", __func__, __LINE__, vaddr, ks64(paddr));
	}

	// 2MB block will be a single entry in an L2
	value = lpae_set_prot(paddr, flags);
	value |= ARM_LPAE_DESC_VALID;  // valid, but not a table

	if (ARM_LPAE_USER_SPACE(vaddr)) {
		// User space address; put it in the user L2 (shared by all CPUs)
		l2 = (lpae_pd_t *)CPU_LPAE_USER_L2 + ARM_LPAE_VADDR_TO_3GL2_IDX(vaddr);

		if (*l2) {
			// LPAE: sanity check for already populated entry
			if (*l2 != value || LPAE_DEBUG ) {
				kprintf("LPAE: %s:%d l2 non-empty! vaddr:%x l2:%x", __func__, __LINE__, vaddr, l2); 
				kprintf(" old: ");  kp64(*l2);
				kprintf(" new: ");  kp64(value);
				kprintf("\n");
			}
		} else {
			if (LPAE_DEBUG) {
				kprintf("LPAE: %s:%d vaddr:%x user l2:%x value:%s\n", __func__, __LINE__, vaddr, l2, ks64(value));
			}
		}
		
		*l2 = value;
		
	} else {
		unsigned	cpu;
		for (cpu = 0; cpu < lsp.syspage.p->num_cpu; cpu++) {
			l2 = (lpae_pd_t *)CPU_LPAE_KERN_L2(cpu) + ARM_LPAE_VADDR_TO_1GL2_IDX(vaddr);

			if (*l2) {
				// LPAE: sanity check for already populated entry
				
				if (*l2 != value || LPAE_DEBUG ) {
					// Log it if it's a different value or if LPAE_DEBUG
					kprintf("LPAE: %s:%d l2 non-empty! vaddr:%x l2:%x", __func__, __LINE__, vaddr, l2); 
					kprintf(" old: ");  kp64(*l2);
					kprintf(" new: ");  kp64(value);
					kprintf("\n");
				}
			} else {
				if (LPAE_DEBUG) {
					kprintf("LPAE: %s:%d cpu:%d vaddr:%x kern l2:%x value:%s\n",
							__func__, __LINE__, cpu, vaddr, l2, ks64(value));
				}
			}

			*l2 = value;
		}
	}
}

/*
 * Unmap block entry
 */
void
arm_lpae_bunmap(uintptr_t vaddr)
{
	lpae_pd_t *l2;

	if (LPAE_DEBUG) {
		kprintf("LPAE: %s:%d enter\n", __func__, __LINE__, vaddr);
	}

	if (ARM_LPAE_USER_SPACE(vaddr)) {
		l2 = (lpae_pd_t *)CPU_LPAE_USER_L2 + ARM_LPAE_VADDR_TO_3GL2_IDX(vaddr);
		*l2 = 0;
	} else {
		unsigned cpu;
		for (cpu = 0; cpu < lsp.syspage.p->num_cpu; cpu++) {
			l2 = (lpae_pd_t *)CPU_LPAE_KERN_L2(cpu) + ARM_LPAE_VADDR_TO_1GL2_IDX(vaddr);

			if (LPAE_DEBUG) {
				kprintf("LPAE: %s:%d wiping td:%x value: ", __func__, __LINE__, l2); kp64(*l2); kprintf("\n");
			}

			*l2 = 0;  // Do we need to check that this is actually a block map?
		}
	}
}


/*
 * Map [paddr, paddr+size) with ARM specific attributes in flags.
 * If vaddr is ~0, we assign the address, otherwise, the mapping
 * will be made at the specified virtual address.
 *
 * Map the same address on all CPUs
 */
uintptr_t
arm_lpae_map(uintptr_t vaddr, paddr_t paddr, size_t size, unsigned flags)
{
	static uintptr_t next_addr = ARM_LPAE_STARTUP_BASE;
	paddr64_t	     off = paddr & PGMASK;
	unsigned         cpu;

	if (LPAE_DEBUG) {
		kprintf("LPAE: %s:%d enter vaddr:%x size:%x paddr:", __func__, __LINE__, vaddr, size); kp64(paddr); kprintf("\n");
	}

	if (!(shdr->flags1 & STARTUP_HDR_FLAGS1_VIRTUAL)) {
		return (uintptr_t)paddr;  // May truncate
	}

	paddr &= ~PGMASK;
	size = ROUNDPG(size + off);
	if (vaddr == ~(uintptr_t)0) {
		vaddr = next_addr;
		next_addr += size;
	}

	if (ARM_LPAE_USER_SPACE(vaddr)) {
		size_t		sz = size;
		uintptr_t   va = vaddr;
		paddr_t		pa = paddr;

		while (sz) {
			unsigned     i;
			lpae_pd_t	*l2pd = (lpae_pd_t *)CPU_LPAE_USER_L2 + ARM_LPAE_VADDR_TO_3GL2_IDX(va);
			lpae_pd_t	*l3pd;
			
			if ((*l2pd & ARM_LPAE_DESC_VT_MASK) == 0) {
				uintptr_t new;
				lpae_pd_t value;

				// No table or block there.  Need to allocate a page table and slot it in.
				new = calloc_ram(ARM_LPAE_L3_SIZE, __PAGESIZE);
				l3pd = (lpae_pd_t *)new;

				if (LPAE_DEBUG) {
					kprintf("LPAE: %s:%d  Adding L3 page for vaddr:%x l3pd:%x ", __func__, __LINE__, va, l3pd); 
					kprintf("new:%x (paddr_t)new:%s \n", new, ks64((paddr_t)new));
				}

				value = lpae_set_prot(new, ARM_PTE_RW);
				value |= ARM_LPAE_DESC_TABLE | ARM_LPAE_DESC_VALID;
				*l2pd = value;
			}
			
			// Get L3 address from l2 table
			l3pd = (lpae_pd_t *)((uintptr_t)(*l2pd & ARM_LPAE_DESC_ADDR_MASK));   // assumes 32 bit ptr!

			// Find right starting slot in L3 table for vaddr
			l3pd +=  ARM_LPAE_VADDR_TO_L3_IDX(va);

			// Start filling in entries in the L3 table until we've slotted all the memory
			// or we have run out of L3 table
			for (i = ((uintptr_t)va >> 12) & (ARM_LPAE_L3_SIZE-1); sz && i < ARM_LPAE_L3_SIZE; i++) {
				lpae_pd_t td;

				td = lpae_set_prot(pa, flags); 
				td |= ARM_LPAE_DESC_VALID | ARM_LPAE_DESC_TABLE;  // valid table

				if (LPAE_DEBUG) {
					kprintf("LPAE: %s:%d user va:%x paddr:%s ", __func__, __LINE__, va, ks64(pa));
					kprintf("l2:%x l3:%x ", l2pd, l3pd);
					kprintf("pd:%s ", ks64(td)); 
					kprintf("\n");
				}

				*l3pd++ = td;
				pa += __PAGESIZE;
				va += __PAGESIZE;
				sz -= __PAGESIZE;
			}
		}
	} else {
	
		for (cpu = 0; cpu < lsp.syspage.p->num_cpu; cpu++) {
			size_t		sz = size;
			uintptr_t   va = vaddr;
			paddr_t		pa = paddr;
			
			while (sz) {
				unsigned     i;
				lpae_pd_t	*l2pd = (lpae_pd_t *)CPU_LPAE_KERN_L2(cpu) + ARM_LPAE_VADDR_TO_1GL2_IDX(va);
				lpae_pd_t	*l3pd;

				if ((*l2pd & ARM_LPAE_DESC_VT_MASK) == 0) {
					uintptr_t new;

					// No table or block there.  Need to allocate a page table and slot it in.
					new = calloc_ram(ARM_LPAE_L3_SIZE, __PAGESIZE);
					l3pd = (lpae_pd_t *)new;

					if (LPAE_DEBUG) {
						kprintf("LPAE: %s:%d  Adding L3 page for vaddr:%x l3pd:%x ", __func__, __LINE__, va, l3pd); 
						kprintf("new:%x (paddr_t)new:%s \n", new, ks64((paddr_t)new));
					}
					
					// Point L2 of all cpus at same new L3 table
					lpae_map_ptbl((paddr_t)new, va);
					
					// dump_ptbl();
				}

				// Get L3 address from l2 table
				l3pd = (lpae_pd_t *)((uintptr_t)(*l2pd & ARM_LPAE_DESC_ADDR_MASK));   // assumes 32 bit ptr!

				// Find right starting slot in L3 table for vaddr
				l3pd +=  ARM_LPAE_VADDR_TO_L3_IDX(va);

				// Start filling in entries in the L3 table until we've slotted all the memory
				// or we have run out of L3 table
				for (i = ((uintptr_t)va >> 12) & (ARM_LPAE_L3_SIZE-1); sz && i < ARM_LPAE_L3_SIZE; i++) {
					lpae_pd_t td;

					td = lpae_set_prot(pa, flags); 
					td |= ARM_LPAE_DESC_VALID | ARM_LPAE_DESC_TABLE;  // valid table
					
					if (LPAE_DEBUG) {
						kprintf("LPAE: %s:%d cpu:%d va:%x paddr:%s ", __func__, __LINE__, cpu, va, ks64(pa));
						kprintf("l2:%x l3:%x ", l2pd, l3pd);
						kprintf("pd:%s ", ks64(td)); 
						kprintf("\n");
					}
					
					*l3pd++ = td;
					pa += __PAGESIZE;
					va += __PAGESIZE;
					sz -= __PAGESIZE;
				}
			}
		}
	}

	if (LPAE_DEBUG) {
		// dump_ptbl();
		kprintf("LPAE: %s:%d exit\n", __func__, __LINE__);
	}
	
	return ((uintptr_t)(vaddr + off));
}


/*
 * Map a page in a cpu-specific page table to allow the same virtual
 * address to map different physical pages on each cpu.
 */
void
arm_lpae_map_cpu(unsigned cpu, uintptr_t vaddr, paddr64_t paddr, unsigned flags)
{
	// Since its a per-cpu mapping, we know this is a kernel space
	lpae_pd_t	*l2 = (lpae_pd_t *)CPU_LPAE_KERN_L2(cpu) + ARM_LPAE_VADDR_TO_1GL2_IDX(vaddr);
	lpae_pd_t	*pd;
    lpae_pd_t   value;

    if (LPAE_DEBUG) {
		kprintf("LPAE: %s:%d cpu:%d vaddr:%x paddr:%x l2:%x\n", __func__, __LINE__, cpu, vaddr, ks64(paddr), l2);
    }

	value = *l2;

	if ((value & ARM_LPAE_DESC_VT_MASK) == ARM_LPAE_DESC_VALID) {
		// It's valid but not a table; so it's 2MB block mapping... something is wrong
		kprintf("LPAE: ERROR %s:%d value: ", __func__, __LINE__); kp64(value); kprintf("\n");
		return;
	}

	if (!(value & ARM_LPAE_DESC_VALID)) {
		uintptr_t new;

		// Not valid; allocate an L3 page table
		new = calloc_ram(ARM_LPAE_L3_SIZE, __PAGESIZE);
		pd = (lpae_pd_t *)new;

		if (LPAE_DEBUG) {
			kprintf("LPAE: %s:%d cpu:%d alloc L3 page table paddr:%x \n", __func__, __LINE__, cpu, pd);
		}
		lpae_map_l3_cpu(cpu, (paddr64_t)new, vaddr);
		
		// It should be there now
		value = *l2;
	}

	// Extract the address bits to get the start of the L3 page table
	pd = (lpae_pd_t *)((uintptr_t)(value & ARM_LPAE_DESC_ADDR_MASK));  // discards upper byte of ptr - assumes 32 bit ptr!!
	pd += ARM_LPAE_VADDR_TO_L3_IDX(vaddr);                // Index to find the right L3 entry.

	value = lpae_set_prot(paddr, flags);
	value |= ARM_LPAE_DESC_VALID | ARM_LPAE_DESC_TABLE;  // valid table entry
	
	*pd = value;

	//kprintf("LPAE: %s:%d  exit value ", __func__, __LINE__); kp64(value);
}



paddr_t
arm_lpae_elf_vaddr_to_paddr(uintptr_t vaddr) {
	lpae_pd_t	*pd;
	paddr64_t  paddr;
	lpae_pd_t value;

	// CPU zero only?
	if (ARM_LPAE_USER_SPACE(vaddr)) {
		pd = (lpae_pd_t *)CPU_LPAE_USER_L2 + ARM_LPAE_VADDR_TO_3GL2_IDX(vaddr);
	} else {
		pd = (lpae_pd_t *)CPU_LPAE_KERN_L2(0) + ARM_LPAE_VADDR_TO_1GL2_IDX(vaddr);
	}

	value = *pd;
	if (LPAE_DEBUG) kprintf("LPAE: %s:%d pd:%x value:%s \n", __func__, __LINE__, pd, ks64(value));

	if ((value & ARM_LPAE_DESC_VT_MASK) == (ARM_LPAE_DESC_VALID)) { 
		// It is a block mapping
		paddr = ((value & ARM_LPAE_DESC_ADDR_MASK) & ~ARM_LPAE_BMASK) + ((uintptr_t)vaddr & ARM_LPAE_BMASK);
		if (LPAE_DEBUG) {
			kprintf("LPAE: %s:%d block paddr:%s \n", __func__, __LINE__, ks64(paddr));
		}
	} else {
		// It is an L3 table mapping
		pd =  (lpae_pd_t *)((uintptr_t)(value & ARM_LPAE_DESC_ADDR_MASK));  // Get addr of start of L3
		pd += ARM_LPAE_VADDR_TO_L3_IDX(vaddr);           // Find right L3 entry
		paddr = (*pd & ARM_LPAE_DESC_ADDR_MASK) + ((uintptr_t)vaddr & PGMASK);
		if (LPAE_DEBUG) {
			kprintf("LPAE: %s:%d L3 paddr:%s \n", __func__, __LINE__, ks64(paddr));
		}
	}
	return (paddr);
};


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/arm_map_lpae.c $ $Rev: 761639 $")
#endif

#endif
