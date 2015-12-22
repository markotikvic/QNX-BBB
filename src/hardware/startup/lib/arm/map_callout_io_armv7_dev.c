/*
 * $QNXLicenseC:
 * Copyright 2012, QNX Software Systems. 
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
 * On ARMv7 callout_io_map() will set memory attributes to strongly ordered
 * (assuming armv_pte_v7mp.mask_nc = ARM_PTE_CB | ARM_PTE_V6_SP_TEX_MASK).
 * For ARMv7 "shareable device" memory type add ARM_PTE_B bit:
 */
uintptr_t
callout_io_map_armv7_dev(unsigned size, paddr_t phys)
{
	uint32_t prot;
	uintptr_t ret;

	prot = ARM_PTE_RW | ARM_MAP_NOEXEC | ARM_PTE_B;
	if (paddr_bits != 32) {
		// Normal IO mappings are strongly-ordered.
		// This callout was specifically created to provide device memory. Make it so.
		prot |= ARM_MAP_DEVICE;
	}

	ret = arm_map(~0, (paddr_t)phys, size, prot);

	if (debug_flag) {
		kprintf("%s: mapping paddr:%P returns:%x\n", __func__, phys, ret);
	}

	return ret;
}


uintptr_t
callout_io_map32_armv7_dev(unsigned size, paddr32_t phys)
{
	return callout_io_map_armv7_dev(size, (paddr_t)phys);
}



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/map_callout_io_armv7_dev.c $ $Rev: 759590 $")
#endif
