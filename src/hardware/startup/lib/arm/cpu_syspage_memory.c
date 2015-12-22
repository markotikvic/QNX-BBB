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

/*
 * Initialize CPU specific pointers in the system page.
 * This code is hardware independant and should not have to be
 * changed by end users.
 */
void
cpu_init_syspage_memory()
{
	set_syspage_section(&lsp.cpu.arm_cpu, sizeof(*lsp.cpu.arm_cpu.p));
}

struct syspage_entry *
cpu_alloc_syspage_memory(paddr32_t *cpupagep, paddr32_t *syspagep, unsigned spsize)
{
	struct syspage_entry		*sp = lsp.syspage.p;
	struct system_private_entry	*private;
	unsigned					size;
	unsigned					cpsize;
	paddr32_t					syspage_paddr;
	unsigned					spacing;

	#define	SP_OFFSET(field)	PTR_DIFF(lsp.cpu.field.p, sp)
	#define	INIT_ENTRY(_cpu, _field) \
		sp->un._cpu._field.entry_size = lsp.cpu._cpu##_##_field.size; \
		sp->un._cpu._field.entry_off  = SP_OFFSET(_cpu##_##_field)

	spsize = ROUND(spsize, sizeof(uint64_t));
	if ((paddr_bits == 32) && sp->num_cpu == 1) {
		spacing = sizeof(struct cpupage_entry);
	}
	else {
		/*
		 * WARNING: we assume SMP processor has physical cache.
		 *          We allocate the cpupages in contiguous memory so that
		 *          the kernel's cpypageptr[] pointers point at virtually
		 *          contiguous mappings into the physical pages.
		 *          The user _cpupage_ptr is a different virtual address
		 *          where the same virtual address on each cpu maps the
		 *          different physical address for that cpu's cpupage.
		 */
		spacing = __PAGESIZE;
		spsize = ROUND(spsize, __PAGESIZE);
	}
	cpsize = sp->num_cpu * spacing;

	/*
	 * Allocate the system page (and cpupage entries) and save it away.
	 * The system page must be 4K aligned in a virtual system
	 */
	size = spsize + cpsize;

	syspage_paddr = alloc_ram(NULL_PADDR, size, lsp.system_private.p->pagesize);
	if (syspage_paddr == NULL_PADDR32) {
		crash("could not allocate 0x%l bytes for syspage/cpupage\n", size);
	}

	private = lsp.system_private.p;
	
	private->kern_syspageptr = TOPTR((uintptr_t)arm_map(~0L,((paddr_t)syspage_paddr),size,ARM_MAP_SYSPAGE));
	private->kern_cpupageptr = TOPTR((uintptr_t)private->kern_syspageptr + spsize);

	if (paddr_bits != 32) {
		// For LPAE, we don't have an option for kern-rw/user-ro mapping
		// So we'll create a separate user-ro/kern-ro mapping for syspage
		private->user_syspageptr = TOPTR((uintptr_t)arm_map(~0L,((paddr_t)syspage_paddr),size,ARM_MAP_SYSPAGE_RO));
	} else {
		private->user_syspageptr = private->kern_syspageptr;
	}

	// Note: for LPAE, have to map twice even on a single-cpu system as we need a user-ro mapping
	if ((paddr_bits == 32) && sp->num_cpu == 1) {
		// For the Uniprocessor, non-LPAE case only; cpupage is accessed at same virtual address as the kernel
		private->user_cpupageptr = private->kern_cpupageptr;

		sp->un.arm.L2_vaddr = 0;
	} else {
		int       i;
		paddr32_t cpupaddr = syspage_paddr + spsize;
		int       cpu_map_flag;

		if (paddr_bits != 32) {
			cpu_map_flag = ARM_MAP_SYSPAGE_RO;
		} else {
			cpu_map_flag = ARM_MAP_SYSPAGE;
		}
			
		/*
		 * We have mapped the trap vector page using a per-cpu page table
		 */
		private->user_cpupageptr = TOPTR(trap_vectors + __PAGESIZE);

		for (i = 0; i < sp->num_cpu; i++, cpupaddr += spacing) {
			arm_map_cpu(i, (uintptr_t)private->user_cpupageptr, (paddr_t)cpupaddr, cpu_map_flag);
		}

		/*
		 * Map the page directory pages so the memmgr can keep all the cpus'
		 * page table mappings consistent.
		 */
		sp->un.arm.L2_vaddr = arm_map(~0, L2_paddr, L2_size, ARM_MAP_NOEXEC | ARM_PTE_RW | armv_chip->pte_attr);

	}
	private->cpupage_spacing = spacing;

	*syspagep = syspage_paddr;
	*cpupagep = syspage_paddr + spsize;

	sp->un.arm.L1_vaddr = L1_vaddr;
	sp->un.arm.startup_base = startup_base;
	sp->un.arm.startup_size = startup_size;

	INIT_ENTRY(arm, boxinfo);
	INIT_ENTRY(arm, cpu);

	return((void *)syspage_paddr);
}

void
cpu_write_syspage_memory(paddr32_t sysp_paddr, unsigned sysp_size, unsigned callout_size)
{
	/*
	 * WARNING: currently run startup with MMU disabled.
	 *			If we ever run with MMU enabled, we need to flush the data
	 *			cache to ensure that callout code is flushed to memory.
	 */
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/cpu_syspage_memory.c $ $Rev: 759590 $")
#endif
