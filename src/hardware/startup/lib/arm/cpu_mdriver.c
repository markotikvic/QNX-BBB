/*
 * $QNXLicenseC:
 * Copyright 2007, QNX Software Systems.
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

ptrdiff_t
cpu_mdriver_map(void) {
	paddr32_t	start;
	paddr32_t	end;
	uintptr_t	new;

	if(!(shdr->flags1 & STARTUP_HDR_FLAGS1_VIRTUAL)) {
		return 0;
	}
	start = TRUNCPG(shdr->ram_paddr);
	end   = ROUNDPG(shdr->ram_paddr+shdr->startup_size);
	new   = (uintptr_t)callout_memory_map(end - start, start, PROT_EXEC|PROT_READ|PROT_WRITE);
	return new - start;
}

void *
cpu_mdriver_prepare(struct mdriver_entry *md) {
	paddr32_t	start;
	paddr32_t	end;
	uintptr_t	new;

	if(!(shdr->flags1 & STARTUP_HDR_FLAGS1_VIRTUAL)) {
		return md->data;
	}
	start = TRUNCPG(md->data_paddr);
	end   = ROUNDPG(md->data_paddr+md->data_size);
	new   = (uintptr_t)callout_memory_map(end - start, start,
					PROT_NOCACHE|PROT_READ|PROT_WRITE);
	return (void *)(new + (md->data_paddr & (__PAGESIZE-1)));
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/cpu_mdriver.c $ $Rev: 763021 $")
#endif
