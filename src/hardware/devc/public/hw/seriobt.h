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


#ifndef __SERIOBT_H_INCLUDED
#define __SERIOBT_H_INCLUDED

#include <devctl.h>

#define _DCMD_SERIOBT  _DCMD_MISC

#define DCMD_SERIOBT_SETADDR             __DIOT(_DCMD_SERIOBT, 1, char[17])

/**
 * Mechanism to signal to the devc-seriobt that the open/close
 * override should signal to the bluetooth stack to create/destroy a service
 * based upon the open/close resource manager override methods
 * byte - 0 - do not link open/close with a service connection
 * byte - 1 - link open/close with a service connection (this is default behavior)
 */
#define DCMD_SERIOBT_CONNECTION_SCHEME   __DIOT(_DCMD_SERIOBT, 2, 1) // expecting a byte identifier

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/public/hw/seriobt.h $ $Rev: 736897 $")
#endif
