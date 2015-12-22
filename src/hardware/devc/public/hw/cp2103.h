/*
 * $QNXLicenseC:
 * Copyright 2007, 2008, QNX Software Systems.
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

#ifndef __CP2103_H
#define __CP2103_H

/*
Desc:	This call is made to get the current pin states.
Args:	uint32_t to store bitmask of pin states.
*/
#define DCMD_CP210X_READ_LATCH      __DIOF(_DCMD_MISC, 0x01, uint32_t)

/* Conveinence macro to exract a pin state out of the bitmask */
#define CP210X_GET_PIN(pin,value) ((value>>pin) & 1)

typedef struct _cp210x_wlatch {
	uint32_t pin_mask;    /* Bitmask of pins to modify */
	uint32_t pin_values;  /* Bitmask of pin states     */
} cp210x_wlatch_t;

/*
Desc: This call is made to update the selected pin states.
Args: cp210x_wlatch_t wlatch.
*/
#define DCMD_CP210X_WRITE_LATCH     __DIOT  (_DCMD_MISC, 0x02, cp210x_wlatch_t)

/* Conveinence functions for configuring the wlatch pin_mask and pin_values members */
static inline cp210x_wlatch_t cp210x_set_pin ( uint32_t pin, cp210x_wlatch_t wlatch )
{
	wlatch.pin_values |= (1<<pin);
	wlatch.pin_mask |= (1<<pin);

	return (wlatch);
}

static inline cp210x_wlatch_t cp210x_clr_pin ( uint32_t pin, cp210x_wlatch_t wlatch )
{
	wlatch.pin_values &= ~(1<<pin);
	wlatch.pin_mask |= (1<<pin);

	return (wlatch);
}

typedef struct _cp210x_pinconf {
	uint8_t pin;        /* Pin to a configuratio to */
	uint8_t value;      /* Pin configuration value */
} cp210x_pinconf_t;

/*
Desc: This call is made to set a pins configuration.
Args: pinconf_t structure.
*/
#define DCMD_CP210X_CHANGE_MODE     __DIOT  (_DCMD_MISC, 0x03, cp210x_pinconf_t)

/* Values for CP210X_CHANGE_MODE */ 
#define CP210X_PUSH_PULL              0x00000001
#define CP210X_OPEN_DRAIN             0x00000002
#define CP210X_LOGIC_HIGH             0x00000004
#define CP210X_LOGIC_LOW              0x00000008
#define CP210X_AUTO_LATCH_CONTROL     0x00000010
#define CP210X_DISABLE_LATCH_CONTROL  0x00000020
#define CP210X_WEAKPULLUP             0x00000040
#define CP210X_DISABLE_WEAKPULLUP     0x00000080

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/public/hw/cp2103.h $ $Rev: 694962 $")
#endif
