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

/* 
 * Specify number of 'v' characters included in startup parameters to allow
 * extra verbosity during startup.
 */
#define STARTUP_DEBUG_LEVEL            (3)

#define CM_CLOCK_DISABLE                0x0
#define CM_CLOCK_ENABLE                 0x2

/* CPU Clock depends on power for Beaglebone Board */
#define AM335X_MPUFREQ_500M            (500 * 1000 * 1000)
#define AM335X_MPUFREQ_720M            (720 * 1000 * 1000)

#define AM335X_I2C0_PMIC               0x24
#define AM335X_I2C0_CPLD               0x35
#define AM335X_I2C0_BBID               0x50
#define AM335X_I2C0_DRID               0x51
#define AM335X_I2C0_DYID               0x52
#define AM335X_I2C0_CAPE0              0x54
#define AM335X_I2C0_MAXCAPES	       4	

/* It could operates in different speed for different devices */
#define AM335X_I2C0_EEPROM_SPEED       100000
#define AM335X_I2C0_PMIC_SPEED         400000

#define AM335X_BDID_HEADER_LEN         4
#define AM335X_BDID_BDNAME_LEN         8
#define AM335X_BDID_VERSION_LEN        4
#define AM335X_BDID_SERIAL_LEN         12
#define AM335X_BDID_CONFIG_LEN         32
#define AM335X_BDID_MAC_LEN            6
#define AM335X_BDID_SKU_LEN            6

#define AM335X_BDID_BDNAME_OFFSET      (AM335X_BDID_HEADER_LEN)
#define AM335X_BDID_VERSION_OFFSET     (AM335X_BDID_BDNAME_OFFSET  + AM335X_BDID_BDNAME_LEN)
#define AM335X_BDID_SERIAL_OFFSET      (AM335X_BDID_VERSION_OFFSET + AM335X_BDID_VERSION_LEN)
#define AM335X_BDID_CONFIG_OFFSET      (AM335X_BDID_SERIAL_OFFSET  + AM335X_BDID_SERIAL_LEN)
#define AM335X_BDID_MAC1_OFFSET        (AM335X_BDID_CONFIG_OFFSET  + AM335X_BDID_CONFIG_LEN)
#define AM335X_BDID_MAC2_OFFSET        (AM335X_BDID_MAC1_OFFSET    + AM335X_BDID_MAC_LEN)

#define AM335X_MACS                    3

#define AM335X_CPLDREG_DEVICE_HDR      0x00
#define AM335X_CPLDREG_DEVICE_ID       0x04
#define AM335X_CPLDREG_DEVICE_REV      0x0C
#define AM335X_CPLDREG_CFG_REG         0x10

#define AM335X_CPLD_HEADER_LEN         4
#define AM335X_CPLD_IDENT_LEN          8
#define AM335X_CPLD_REV_LEN            4
#define AM335X_CPLD_CONFIG_LEN         2

typedef struct board_identity
{
    unsigned int    header;
    char            bdname [AM335X_BDID_BDNAME_LEN +1];
    char            version[AM335X_BDID_VERSION_LEN+1];
    char            serial [AM335X_BDID_SERIAL_LEN +1];
    char            config [AM335X_BDID_CONFIG_LEN +1];
    uint8_t         macaddr[AM335X_MACS][AM335X_BDID_MAC_LEN];
} BDIDENT;

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/ti-am335x/board.h $ $Rev: 761375 $")
#endif
