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

const struct armv_chip	*armv_chip;

const struct armv_chip *
armv_chip_detect()
{
	const struct armv_chip	**cp;
	const struct armv_chip	*chip;
	unsigned				cpuid;

	if (armv_chip != 0) {
		return armv_chip;
	}

	/*
	 * Read CP15 ID register
	 */
	cpuid = arm_mmu_cpuid();

	for (cp = armv_list; (chip = *cp) != 0; cp++) {
		if (chip->cpuid == (cpuid & 0xfff0)) {
			break;
		}
	}
	if (chip && chip->name == 0 && chip->detect) {
		chip = chip->detect();
	}
	armv_chip = chip;
	return chip;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/armv_chip_detect.c $ $Rev: 680332 $")
#endif
