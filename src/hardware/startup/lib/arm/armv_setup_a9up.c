/*
 * $QNXLicenseC:
 * Copyright 2009, QNX Software Systems. 
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
#define	CPUID_REVISION(x)	((x) & 0x00f0000f)
#define	CPUID_R2P0			0x00200000
#define CPUID_R3P0			0x00300000

/*
 * Additional Cortex A9 specific CPU initialisation
 */

void
armv_setup_a9up(struct cpuinfo_entry *cpu, unsigned cpuid)
{
	unsigned diag;
	/*
	 * Apply fix for ARM_ERRATA_743622 for all r2 revisions:
	 * "Faulty logic in the Store Buffer may lead to data corruption"
	 */
	if ((CPUID_REVISION(cpuid) >= CPUID_R2P0) &&
		(CPUID_REVISION(cpuid) < CPUID_R3P0))
	{
		/*
	 	 * Apply fix for ARM_ERRATA_743622 for all r2 revisions:
		 * "Faulty logic in the Store Buffer may lead to data corruption"
		 */
		__asm__ __volatile__("mrc p15, 0, %0, c15, c0, 1" : "=r"(diag));	// read diagnostic register
		diag |= (1<<6);								// set bit #6
		__asm__ __volatile__("mcr p15, 0, %0, c15, c0, 1" : : "r"(diag));	// write diagnostic register
	}

	/*
	 * Perform generic ARMv7 CPU initialisation
	 */
	armv_setup_v7(cpu, cpuid, 0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/armv_setup_a9up.c $ $Rev: 685886 $")
#endif
