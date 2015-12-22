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
#include <sys/kdebug.h>

void init_ringbuffers(const char *name, paddr_t ringbuffer_addr, size_t ringbuffer_size)
{
	size_t buf_size = ringbuffer_size/2;
	paddr_t	active, inactive;
	char bufname[1024];

	alloc_ram(ringbuffer_addr, ringbuffer_size, 4);
	struct kprintf_ringbuffer *krb1, *krb2;
   
	krb1 = startup_memory_map(ringbuffer_size, ringbuffer_addr, PROT_READ|PROT_WRITE);
	krb2 = (struct kprintf_ringbuffer *)((((uintptr_t)krb1) + buf_size));

	if (krb1->buf_magic == KPRINTF_ACTIVE_RINGBUFFER) {
		krb1->buf_magic = KPRINTF_INACTIVE_RINGBUFFER;

		active = ringbuffer_addr + buf_size;
		inactive = ringbuffer_addr;
	} else {
		if (krb2->buf_magic == KPRINTF_ACTIVE_RINGBUFFER) {
			krb2->buf_magic = KPRINTF_INACTIVE_RINGBUFFER;
		}

		active = ringbuffer_addr;
		inactive = ringbuffer_addr + buf_size;
	}


	startup_memory_unmap(krb1);

	as_add_containing( active, active + buf_size - 1, AS_ATTR_RAM, name, "ram" );
	ksprintf(bufname, "inactive_%s", name);
	as_add_containing( inactive, inactive + buf_size - 1, AS_ATTR_RAM, bufname, "ram" );
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/ringbuffer.c $ $Rev: 736706 $")
#endif
