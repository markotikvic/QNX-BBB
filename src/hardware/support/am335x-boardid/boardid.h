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

#ifndef __AM335X_BDID_H_INCLUDED
#define __AM335X_BDID_H_INCLUDED

#define AM335X_I2C0_CPLD					0x35
#define AM335X_I2C0_BBID					0x50
#define AM335X_I2C0_CAPE0					0x54
#define AM335X_I2C0_MAXCAPES				4

#define AM335X_BDID_HEADER_LEN				 4
#define AM335X_BDID_BDNAME_LEN				 8
#define AM335X_BDID_VERSION_LEN				 4
#define AM335X_BDID_SERIAL_LEN				12
#define AM335X_BDID_CONFIG_LEN				32
#define AM335X_BDID_MAC_LEN					 6

#define AM335X_BDID_SKU_LEN					 6

#define AM335X_BDID_BDNAME_OFFSET			(AM335X_BDID_HEADER_LEN)
#define AM335X_BDID_VERSION_OFFSET			(AM335X_BDID_BDNAME_OFFSET  +AM335X_BDID_BDNAME_LEN)
#define AM335X_BDID_SERIAL_OFFSET			(AM335X_BDID_VERSION_OFFSET +AM335X_BDID_VERSION_LEN)
#define AM335X_BDID_CONFIG_OFFSET			(AM335X_BDID_SERIAL_OFFSET  +AM335X_BDID_SERIAL_LEN)
#define AM335X_BDID_MAC1_OFFSET				(AM335X_BDID_CONFIG_OFFSET  +AM335X_BDID_CONFIG_LEN)
#define AM335X_BDID_MAC2_OFFSET				(AM335X_BDID_MAC1_OFFSET    +AM335X_BDID_MAC_LEN)

#define AM335X_MACS							3

#define BOARDID_I2C_DEVICE "/dev/i2c0"

typedef struct board_identity
{
	unsigned int  header;
	char          bdname [AM335X_BDID_BDNAME_LEN+1];
	char          version[AM335X_BDID_VERSION_LEN+1];
	char          serial [AM335X_BDID_SERIAL_LEN+1];
	char          config [AM335X_BDID_CONFIG_LEN+1];
	uint8_t       macaddr[AM335X_MACS][AM335X_BDID_MAC_LEN];
} BDIDENT;

enum enum_basebd_type
{
	bb_not_detected	= 0,
	bb_BeagleBone	= 1,		/* BeagleBone Base Board */
	bb_unknown		= 99,
};

enum enum_cape_type
{
	ct_not_detected	= 0,
	ct_unknown		= 99,
};

typedef struct beaglebone_id
{
	/* Base board */
	enum enum_basebd_type	basebd_type;

	/* Daughter board, they're called cape. */
	enum enum_cape_type		cape_type[4];

} BEAGLEBONE_ID;

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/support/am335x-boardid/boardid.h $ $Rev: 725446 $")
#endif
