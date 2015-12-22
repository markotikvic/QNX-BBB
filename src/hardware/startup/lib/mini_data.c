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
#include <hw/mini_driver.h>

/*
 * Sample minidriver handler function for debug purposes
 *
 * Counts the number of calls for each state and
 * fills the data area with the current handler state
 */
int
mini_data(int state, void *data)
{
	uint8_t				*dptr;
	mini_data_t	*mdata;

	mdata = (mini_data_t *) data;
	dptr = (uint8_t *) (mdata + 1);

	/* on MDRIVER_INIT setup the data area */
	if (state == MDRIVER_INIT)
	{
		// Set the Startup poll rate
        mdriver_max = KILO(MINI_DATA_STARTUP_DEFAULT_RATE);

		mdata->nstartup = 0;
		mdata->nstartupf = 0;
		mdata->nstartupp = 0;
		mdata->nkernel = 0;
		mdata->nprocess = 0;
		mdata->data_len = 0;
	}

	/* count the number of calls we get for each type */
	if (state == MDRIVER_STARTUP)				mdata->nstartup++;
	else if (state == MDRIVER_STARTUP_PREPARE)	mdata->nstartupp++;
	else if (state == MDRIVER_STARTUP_FINI)		mdata->nstartupf++;
	else if (state == MDRIVER_KERNEL)			mdata->nkernel++;
	else if (state == MDRIVER_PROCESS)			mdata->nprocess++;
	else if (state == MDRIVER_INTR_ATTACH)
	{
		/* normally disable my interrupt */
		return (1);
	}

	/* put the state diff in the data area */
	if (mdata->data_len < MINI_DATA_BUF_SIZE - sizeof(mini_data_t)) {
		dptr[mdata->data_len] = (uint8_t) state;
		mdata->data_len++;
	}
	return (0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/mini_data.c $ $Rev: 744481 $")
#endif
