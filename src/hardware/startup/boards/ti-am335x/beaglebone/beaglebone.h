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

#ifndef __BEAGLEBONE_H_INCLUDED
#define __BEAGLEBONE_H_INCLUDED

enum enum_basebd_type
{
    bb_not_detected     = 0,
    bb_BeagleBone       = 1,        /* BeagleBone Base Board */
    bb_BeagleBoneBlack  = 2,
    bb_unknown          = 99,
};

enum enum_cape_type
{
    ct_not_detected = 0,
    ct_unknown      = 99,
};

typedef struct beaglebone_id
{
    /* Base board */
    enum enum_basebd_type    basebd_type;

    /* Daughter board, they're called cape. */
    enum enum_cape_type    cape_type[4];

} BEAGLEBONE_ID;

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/ti-am335x/beaglebone/beaglebone.h $ $Rev: 722974 $")
#endif
