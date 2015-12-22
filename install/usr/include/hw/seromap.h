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


#ifndef SEROMAP_H_
#define SEROMAP_H_

#include <devctl.h>

#define _DCMD_SEROMAP  _DCMD_MISC

/**
 * General mechanism to signal the Bluetooth Enable pin to be pulled up or down
 * byte - 0 - pull the BT_EN pin low
 * byte - 1 - pull the BT_EN pin high
 */
#define DCMD_SEROMAP_BT_EN				__DIOT(_DCMD_SEROMAP, 1, int)

#endif /* SEROMAP_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/public/hw/seromap.h $ $Rev: 736896 $")
#endif
