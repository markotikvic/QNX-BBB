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

void *
callout_memory_map(unsigned size, paddr_t phys, unsigned prot_flags)
{
	int	pf;
	uintptr_t ret;

	pf = ARM_PTE_RO | ARM_PTE_CB;
	
	if (prot_flags & PROT_WRITE)
		pf |= ARM_PTE_RW;
	if (prot_flags & PROT_USER)
		pf |= ARM_PTE_U;
	if (prot_flags & PROT_NOCACHE)
		pf &= ~ARM_PTE_CB;
	if ((prot_flags & PROT_EXEC) == 0) {
		pf |= ARM_MAP_NOEXEC;
	}

    // We use a flag for LPAE instead of twiddling the protection bits directly
    if (paddr_bits != 32 && (prot_flags & PROT_NOCACHE)) {
        pf |= ARM_MAP_NOCACHE;
    }
	
	ret = arm_map(~0, phys, size, pf);

	if (debug_flag) {
		kprintf("%s: mapping paddr:%P returns:%x\n", __func__, phys, ret);
	}
	
	return (void*)ret;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/map_callout_mem.c $ $Rev: 759590 $")
#endif
