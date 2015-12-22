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

#include "startup.h"

/*
 * Determine which variant of A7 we should use
 */

const struct armv_chip *
armv_detect_a7(void)
{
	struct armv_chip const *chip;
	
	if (paddr_bits == 32) {
		chip = &armv_chip_a7_32;
	} else {
#if (_PADDR_BITS-0) == 64
		chip = &armv_chip_a7_lpae;
#else
	    crash("Unsupported paddr_bits:%x", paddr_bits);
#endif
	}

	return chip;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/startup/lib/arm/armv_detect_a7.c $ $Rev: 759590 $")
#endif
