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
 * Additional XScale specific CPU initialisation
 */

void
armv_setup_xscale(struct cpuinfo_entry *cpu, unsigned cpuid)
{
	unsigned	aux;

	/*
	 * Make sure write-buffer coalescing is enabled.
	 */
	__asm__("mrc	p15, 0, %0, c1, c0, 1" : "=r" (aux) : );
	aux &= ~1;		// enable write buffer coalescing
	__asm__("mcr	p15, 0, %0, c1, c0, 1" : : "r" (aux) );

	/*
	 * Enable access to CP0 (and disable all other coprocessors)
	 */
	aux = 1;
	__asm__("mcr	p15, 0, %0, c15, c1, 0" : : "r" (aux) );

	cpu->flags |= ARM_CPU_FLAG_XSCALE_CP0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/armv_setup_xscale.c $ $Rev: 733817 $")
#endif
