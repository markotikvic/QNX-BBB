/*
 * $QNXLicenseC:
 * Copyright 2008, QNX Software Systems. 
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

#include "startup.h"


// As paddrs are cast to pointers, it is assumed that L1s and L2s are allocated from memory < 4G

#define	CPU_L1_TABLE(cpu)	((uintptr_t)((L1_paddr + ((cpu) * ARM_L1_SIZE)) & ~ARM_TTBR_ATTR_MASK))
#define	CPU_L2_TABLE(cpu)	((uintptr_t)(L2_paddr + ((cpu) * __PAGESIZE)))

static paddr32_t
set_prot(paddr32_t paddr, int flags)
{
	if (flags == ARM_MAP_SYSPAGE) {
		if (arm_v6_mmu) {
			/*
			 * WARNING: we assume user RW has APX/AP1/AP0=011:
			 *          - by clearing AP0 we can set user RO/priv RW
			 *          - we need to clear nG so it doesn't use an ASID
			 */
			paddr |= armv_pte->upte_rw;
			paddr &= ~(ARM_PTE_V6_AP0 | ARM_PTE_V6_nG);
		}
		else {
			/*
			 * user RO allows privileged RW access on non-ARMv6 cpus
			 */
			paddr |= armv_pte->upte_ro;
		}
		return paddr;
	}

	if (flags & ARM_PTE_U) {
		paddr |= (flags & ARM_PTE_RW) ? armv_pte->upte_rw
									  : armv_pte->upte_ro;
	}
	else {
		paddr |= (flags & ARM_PTE_RW) ? armv_pte->kpte_rw
									  : armv_pte->kpte_ro;
	}
	if ((flags & ARM_PTE_CB) != ARM_PTE_CB) {
		paddr &= ~armv_pte->mask_nc;
	}
	if (arm_v6_mmu) {
		paddr |= (flags & (ARM_PTE_V6_S|ARM_PTE_V6_SP_TEX_MASK|ARM_PTE_CB));
		if (flags & ARM_MAP_NOEXEC) {
			paddr |= ARM_PTE_V6_SP_XN;
		}
	} else {
		paddr |= (flags & (ARM_PTE_V5_SP_TEX_MASK|ARM_PTE_CB));
	}
	return paddr;
}

/*
 * Map a 4K page table into "page directory" and L1 table for specified cpu
 * Each L2 is 1k; this carves one page into 4 L2 tables and points 4 L1 entries appropriately 
 */
static void
map_ptbl_cpu(int cpu, paddr32_t ptbl, uintptr_t vaddr)
{
    
	pte_t	*pte = (pte_t *)CPU_L2_TABLE(cpu) + (vaddr >> 22);          // 1mb per page dir
	ptp_t	*ptp = (ptp_t *)CPU_L1_TABLE(cpu) + ((vaddr >> 20) & ~3);
	int		i;

	/*
	 * Map the page table into the "page directory" page table
	 */
	*pte = ((ptbl | armv_pte->kpte_rw) & ~armv_pte->mask_nc) | armv_chip->pte_attr;

	/*
	 * Map the page table into the L1 table
	 */
	for (i = 0; i < 4; i++, ptbl += ARM_L2_SIZE) {
		*ptp++ = ptbl | armv_pte->l1_pgtable;   // 1k each, so four consecutive l1 entries
	}
}

/*
 * Map a 4K page table into "page directory" and L1 table on all cpus
 */
void
map_ptbl(paddr32_t pt, uintptr_t vaddr)
{
	int		cpu;

	for (cpu = 0; cpu < lsp.syspage.p->num_cpu; cpu++) {
		map_ptbl_cpu(cpu, pt, vaddr);
	}
}

/*
 * Map the "page directory" page for each CPU
 */
void
arm_pdmap(uintptr_t vaddr)
{
	int		cpu;

	for (cpu = 0; cpu < lsp.syspage.p->num_cpu; cpu++) {
		map_ptbl_cpu(cpu, CPU_L2_TABLE(cpu), vaddr);
	}
}

/*
 * Map a 1MB section into the L1 table.
 */
void
arm_scmap(uintptr_t vaddr, paddr32_t paddr, int flags)
{
	unsigned	prot = (flags & ARM_PTE_RW) ? armv_pte->kscn_rw : armv_pte->kscn_ro;
	unsigned	cpu;

	if (flags & ARM_PTE_CB) {
		prot |= armv_pte->kscn_cb;
	}
	for (cpu = 0; cpu < lsp.syspage.p->num_cpu; cpu++) {
		ptp_t	*ptp = (ptp_t *)CPU_L1_TABLE(cpu) + (vaddr >> 20);
		*ptp = paddr | prot;
	}
}

/*
 * Unmap section entry
 */
void
arm_scunmap(uintptr_t vaddr)
{
	unsigned	cpu;

	for (cpu = 0; cpu < lsp.syspage.p->num_cpu; cpu++) {
		ptp_t	*ptp = (ptp_t *)CPU_L1_TABLE(cpu) + (vaddr >> 20);
		*ptp = 0;
	}
}


/*
 * Map [paddr, paddr+size) with ARM specific attributes in flags.
 * If vaddr is ~0, we assign the address, otherwise, the mapping
 * will be made at the specified virtual address.
 */
uintptr_t
arm_32_map(uintptr_t vaddr, paddr32_t paddr, size_t size, int flags)
{
	static uintptr_t	next_addr = ARM_STARTUP_BASE;
	paddr32_t			off = paddr & PGMASK;
	int					cpu;

	if (!(shdr->flags1 & STARTUP_HDR_FLAGS1_VIRTUAL)) {
		return paddr;
	}

	paddr &= ~PGMASK;
	size = ROUNDPG(size + off);
	if (vaddr == ~(uintptr_t)0) {
		vaddr = next_addr;
		next_addr += size;
	}

	paddr = set_prot(paddr, flags);

	for (cpu = 0; cpu < lsp.syspage.p->num_cpu; cpu++) {
		size_t		sz = size;
		uintptr_t	va = vaddr;
		paddr32_t	pa = paddr;
		ptp_t		*ptp = (ptp_t *)CPU_L1_TABLE(cpu) + ((va >> 20) & ~3);

		while (sz) {
			int		i;
			pte_t	*pte = (pte_t *)(*ptp & ~PGMASK);  // point at start of table

			if ((*ptp & ARM_PTP_VALID) == 0) {
				/*
				 * Need to allocate a page table
				 */
				pte = (pte_t *)calloc_ram(__PAGESIZE, __PAGESIZE);
				map_ptbl((uintptr_t)pte, va);   //  puts four in place at a time
			}
			pte += (va >> 12) & (ARM_L2_SIZE-1);   // start at right slot in table
			for (i = (va >> 12) & (ARM_L2_SIZE-1); sz && i < ARM_L2_SIZE; i++) {
				*pte++ = pa;
				pa += __PAGESIZE;
				va += __PAGESIZE;
				sz -= __PAGESIZE;
			}
			ptp += 4;	// move to next 4MB set
		}
	}
	return vaddr + off;
}



void
arm_32_map_cpu(int cpu, uintptr_t vaddr, paddr32_t paddr, int flags)
{
	ptp_t	*ptp = (ptp_t *)CPU_L1_TABLE(cpu) + ((vaddr >> 20) & ~3);
	pte_t	*pte = (pte_t *)(*ptp & ~PGMASK);

	if ((*ptp & ARM_PTP_VALID) == 0) {
		/*
		 * Need to allocate a page table
		 */
		pte = (pte_t *)calloc_ram(__PAGESIZE, __PAGESIZE);
		map_ptbl_cpu(cpu, (uintptr_t)pte, vaddr);
	}
	pte += (vaddr >> 12) & (ARM_L2_SIZE-1);
	*pte = set_prot(paddr, flags);
}


uintptr_t
arm_32_elf_vaddr_to_paddr(uintptr_t vaddr) {
	ptp_t	*ptp;
	pte_t	*pte;

	ptp = (ptp_t *)CPU_L1_TABLE(0) + (vaddr >> 20);
	if ((*ptp & ARM_PTP_VALID) == (ARM_PTP_SC & ARM_PTP_VALID)) {
		return (*ptp & ~ARM_SCMASK) + (vaddr & ARM_SCMASK);
	} else {
		pte = (pte_t *)(*ptp & ~PGMASK);
		pte += (vaddr >> 12) & (ARM_L2_SIZE-1);
		return (*pte & ~PGMASK) + (vaddr & PGMASK);
	}
};


/*
 * Map [paddr, paddr+size) with ARM specific attributes in flags.
 * If vaddr is ~0, we assign the address, otherwise, the mapping
 * will be made at the specified virtual address.
 */
uintptr_t
arm_map(uintptr_t vaddr, paddr_t paddr, size_t size, int flags)
{
#if (_PADDR_BITS-0) == 64
	// Use the LPAE version if enabled in the 64-bit build
	if (paddr_bits != 32) {
		return arm_lpae_map(vaddr, paddr, size, flags);
	}
#endif

	return arm_32_map(vaddr, (paddr32_t)paddr, size, flags);
}


/*
 * Map a page in a cpu-specific page table to allow the same virtual
 * address to map different physical pages on each cpu.
 */
void
arm_map_cpu(int cpu, uintptr_t vaddr, paddr_t paddr, int flags)
{

#if (_PADDR_BITS-0) == 64
	// Use the LPAE version if enabled in the 64-bit build
	if (paddr_bits != 32) {
		arm_lpae_map_cpu(cpu, vaddr, paddr, flags);
		return;
	}
#endif
	
	arm_32_map_cpu(cpu, vaddr, (paddr32_t)paddr, flags);
}



paddr_t
elf_vaddr_to_paddr(uintptr_t vaddr) {

#if (_PADDR_BITS-0) == 64
	// Use the LPAE version if enabled in the 64-bit build
	if (paddr_bits != 32) {
		return arm_lpae_elf_vaddr_to_paddr(vaddr);
	}
#endif

	return arm_32_elf_vaddr_to_paddr(vaddr);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/arm_map.c $ $Rev: 759590 $")
#endif
