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


#ifndef _CS_PB_STRINGS_H
#define _CS_PB_STRINGS_H

#include <confname.h>

/************************************************************
These defines represent the different hardware features a playbook board
can support. If the board supports the feature startup sets a typed string
in the syspage for that feature with the string value "1". If the board 
doesn't support the feature the string is set to "0". The configuration 
strings can be accessed in an application using confstr with the appropriate
string number i.e confstr(CS_PB_TOUCH_CONTROL_POWER_GPIO) 
************************************************************/

#define CS_PB_TOUCH_CONTROL_POWER_GPIO   (1+_CS_VENDOR)
#define CS_PB_GPIO_CONTROL_REGULATORS    (2+_CS_VENDOR)
#define CS_PB_BRIDGE_RESET_GPIO_56       (3+_CS_VENDOR)
#define CS_PB_PLATFORM                   (4+_CS_VENDOR)
#define CS_PB_SECURE_PERSIST             (5+_CS_VENDOR)

#define TRUE_STR                             "1"
#define FALSE_STR                            "0"
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/sys/cs_pb_strings.h $ $Rev: 736706 $")
#endif
