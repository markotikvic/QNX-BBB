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

#include "proto.h"
#include "context_restore.h"

void
omap_fini(void *hdl)
{
    omap_dev_t  *dev = hdl;

    omap_clock_enable(dev);
    out16(dev->regbase + OMAP_I2C_CON, 0);
    out16(dev->regbase + OMAP_I2C_IE, 0);
    omap_clock_disable(dev);
	InterruptDetach(dev->iid);
	ConnectDetach(dev->coid);
	ChannelDestroy(dev->chid);

	if (dev->clkctrl_base) {
		munmap_device_io (dev->clkctrl_base, 4);
	}

	if (dev->clkstctrl_base) {
		munmap_device_io (dev->clkstctrl_base, 4);
	}

	context_restore_fini(dev);

	munmap_device_io (dev->regbase, dev->reglen);
	free (hdl);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/i2c/omap35xx/fini.c $ $Rev: 680332 $")
#endif
