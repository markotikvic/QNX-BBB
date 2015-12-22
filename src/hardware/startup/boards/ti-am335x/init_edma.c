/*
 * $QNXLicenseC:
 * Copyright 2013, QNX Software Systems.
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
#include <arm/am335x.h>

void
init_edma(void)
{
	unsigned	chnl;
	uintptr_t	base = AM335X_EDMA0_CC_BASE;	// EDMA base
	
	// DMA Channel mapping
	for (chnl = 0; chnl < 64; chnl++)
		out32(base + 0x100 + chnl * 4, chnl << 5);

	// Enable region 0 access for all channels
	out32(base + AM335X_EDMA_DRAE(0), 0xFFFFFFFF);
	out32(base + AM335X_EDMA_DRAEH(0), 0xFFFFFFFF);

	// Disable all events
	base += AM335X_EDMA_GLOBAL;
	out32(base + AM335X_EDMA_EECR, 0xFFFFFFFF);
	out32(base + AM335X_EDMA_EECRH, 0xFFFFFFFF);
	out32(base + AM335X_EDMA_SECR, 0xFFFFFFFF);
	out32(base + AM335X_EDMA_SECRH, 0xFFFFFFFF);

	// Clear events just in case
	out32(base + AM335X_EDMA_ECR, 0xFFFFFFFF);
	out32(base + AM335X_EDMA_ECRH, 0xFFFFFFFF);

	// Disable all interrupts
	out32(base + AM335X_EDMA_IECR, 0xFFFFFFFF);
	out32(base + AM335X_EDMA_IECRH, 0xFFFFFFFF);
	// Clear all interrupts just in case
	out32(base + AM335X_EDMA_ICR, 0xFFFFFFFF);
	out32(base + AM335X_EDMA_ICRH, 0xFFFFFFFF);
}



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/ti-am335x/init_edma.c $ $Rev: 710799 $")
#endif
