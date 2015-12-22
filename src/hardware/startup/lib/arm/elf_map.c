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
 * Convert a set of ELF permission bits to ARM specific ones and
 * map in the specified memory region with the given permissions.
 */
void
elf_map(uintptr_t vaddr, paddr32_t paddr, size_t size, int flags)
{
#define	MAX_PHDR	2	/* We assume we only have 2 load segments */
	static uintptr_t	va[MAX_PHDR];
	static paddr32_t	pa[MAX_PHDR];
	static size_t		sz[MAX_PHDR];
	static int			fl[MAX_PHDR];
	static int			phdr;
	int chunk_size;
	int chunk_mask;

	if (debug_flag > 1) kprintf("elf_map: enter vaddr:%x paddr:%x\n", vaddr, paddr);
	
#if (_PADDR_BITS-0) == 64
	// Use the LPAE sizes if enabled in the 64-bit build
	if (paddr_bits != 32) {
		chunk_size = ARM_LPAE_BSIZE;
		chunk_mask = ARM_LPAE_BMASK;
	} else
#endif
	{
		chunk_size = ARM_SCSIZE;
		chunk_mask = ARM_SCMASK;
	}
	
	if (flags & PF_W) {
		flags = ARM_PTE_RW | ARM_PTE_CB;
	}
	else {
		flags = ARM_PTE_RO | ARM_PTE_CB;
	}

	/*
	 * We assume all the elf sections are physically contiguous
	 * If they are not, we must remap using 4K pages
	 */
	if ((vaddr & chunk_mask) != (paddr & chunk_mask) ||
	    (phdr && (phdr == -1 || pa[phdr-1] + sz[phdr-1] != paddr))) {
		int	i;

		for (i = 0; i < phdr; i++) {
			/*
			 * assumes segment was <= 1MB...
			 */
#if (_PADDR_BITS-0) == 64
			// Use the LPAE version if enabled in the 64-bit build
			if (paddr_bits != 32) {
				arm_lpae_bunmap(va[i]);
			} else 	
#endif
			{
				arm_scunmap(va[i]);
			}
			
		}
		for (i = 0; i < phdr; i++) {
			if (debug_flag > 1) {
				kprintf("elf_map:phdr 4K va=%x pa=%x sz=%x fl=%x\n", va[i], pa[i], sz[i], fl[i]);
			}
			arm_map(va[i], (paddr_t)pa[i], sz[i], fl[i]);
		}
		if (debug_flag > 1) {
			kprintf("elf_map: 4K va=%x pa=%x sz=%x fl=%x\n", vaddr, paddr, size, flags);
		}
		arm_map(vaddr, (paddr_t)paddr, size, flags);
		phdr = -1;
		if (debug_flag > 1) kprintf("elf_map: middle exit\n");
		return;
	}

	if (phdr == MAX_PHDR) {
		crash("bootstrap has too many load segments");
	}
	va[phdr] = vaddr;
	pa[phdr] = paddr;
	sz[phdr] = size;
	fl[phdr] = flags;
	phdr++;

	size = (size + (vaddr & chunk_mask) + chunk_mask) & ~chunk_mask;
	vaddr &= ~chunk_mask;
	paddr &= ~chunk_mask;
	if (debug_flag > 1) {
		kprintf("elf_map: chunk va=%x pa=%x sz=%x\n", vaddr, paddr, size);
	}
	while (size) {
#if (_PADDR_BITS-0) == 64
		// Use the LPAE version if enabled in the 64-bit build
		if (paddr_bits != 32) {		
			arm_lpae_bmap(vaddr, (paddr_t)paddr, ARM_PTE_RW | ARM_PTE_CB);
 		} else  
#endif
		{
			arm_scmap(vaddr, paddr, ARM_PTE_RW | ARM_PTE_CB);
		}
		
		vaddr += chunk_size;
		paddr += chunk_size;
		size  -= chunk_size;
	}
	
	if (debug_flag > 1) kprintf("elf_map: exit\n");
}





#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/elf_map.c $ $Rev: 759590 $")
#endif
