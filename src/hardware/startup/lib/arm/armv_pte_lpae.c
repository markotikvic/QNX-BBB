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


#if (_PADDR_BITS-0) == 64

/*
 * ARMv7 page table entries use fixed values
 * LPAE uses different values and even fewer of the fields
 * 
 * At this point we don't need these values
 * 
 */

const struct armv_pte	armv_pte_lpae = {
	0, // upte_ro
	0, // upte_rw
	0, // kpte_ro
	0, // kpte_rw
	0, // mask_nc
	0, // l1_pgtable
	0, // kscn_ro
	0, // kscn_rw
	0  // kscn_cb
};


#endif



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/armv_pte_lpae.c $ $Rev: 759590 $")
#endif
