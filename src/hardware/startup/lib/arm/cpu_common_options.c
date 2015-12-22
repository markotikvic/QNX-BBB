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

int	arm_altpte;

int
cpu_handle_common_option(int opt)
{
	switch (opt) {
	case 'w':
		arm_altpte = optarg[0];
		return 1;
		
#if (_PADDR_BITS-0) == 64
	// The lower-case 'x' flag is used by X86 for PAE, so we use it for ARM too.
	// Some existing ARM startups use -x for something else; this shouldn't be a problem
	// as they won't be using LPAE anyway.
	case 'x':
		// LPAE support is present and requested
		paddr_bits = 40;
		if (debug_flag) {
			kprintf("%s: LPAE enabled; paddr bits now %d\n",
					__func__, paddr_bits);
		}

		// Chip detection is done very early, we need to redetect to set the CPU up properly
		if (armv_chip) {
			if (debug_flag) {
				kprintf("LPAE %s: before redetect armv_chip:(%s) \n",
						__func__, armv_chip->name?armv_chip->name:"noname");
			}
			armv_chip = NULL;
		}
		armv_chip_detect();

		if (armv_chip) {
			if (debug_flag) {
				kprintf("LPAE %s: after redetect armv_chip:(%s) \n",
						__func__, armv_chip->name?armv_chip->name:"noname");
			}
		}
		return 1;
#endif
	}
	return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/cpu_common_options.c $ $Rev: 759590 $")
#endif
