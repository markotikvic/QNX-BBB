/*
 * $QNXLicenseC:
 * Copyright 2010, QNX Software Systems.
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

#ifndef __OFFSETS_H_INCLUDED
#define __OFFSETS_H_INCLUDED

// override includes picked up from <arm/omap.h>
#ifdef  OMAP_I2C_REV
#undef  OMAP_I2C_REV
#endif
#define OMAP_I2C_REV      0x00

#ifdef  OMAP_I2C_IE
#undef  OMAP_I2C_IE
#endif
#define OMAP_I2C_IE       0x2c

#ifdef  OMAP_I2C_STAT
#undef  OMAP_I2C_STAT
#endif
#define OMAP_I2C_STAT     0x28

#ifdef  OMAP_I2C_SYSS
#undef  OMAP_I2C_SYSS
#endif
#define OMAP_I2C_SYSS     0x90

#ifdef  OMAP_I2C_BUF
#undef  OMAP_I2C_BUF
#endif
#define OMAP_I2C_BUF      0x94

#ifdef  OMAP_I2C_CNT
#undef  OMAP_I2C_CNT
#endif
#define OMAP_I2C_CNT      0x98

#ifdef  OMAP_I2C_DATA
#undef  OMAP_I2C_DATA
#endif
#define OMAP_I2C_DATA     0x9c

#ifdef  OMAP_I2C_SYSC
#undef  OMAP_I2C_SYSC
#endif
#define OMAP_I2C_SYSC     0x10

#ifdef  OMAP_I2C_CON
#undef  OMAP_I2C_CON
#endif
#define OMAP_I2C_CON      0xa4

#ifdef  OMAP_I2C_OA
#undef  OMAP_I2C_OA
#endif
#define OMAP_I2C_OA       0xa8

#ifdef  OMAP_I2C_SA
#undef  OMAP_I2C_SA
#endif
#define OMAP_I2C_SA       0xac

#ifdef  OMAP_I2C_PSC
#undef  OMAP_I2C_PSC
#endif
#define OMAP_I2C_PSC      0xb0

#ifdef  OMAP_I2C_SCLL
#undef  OMAP_I2C_SCLL
#endif
#define OMAP_I2C_SCLL     0xb4

#ifdef  OMAP_I2C_SCLH
#undef  OMAP_I2C_SCLH
#endif
#define OMAP_I2C_SCLH     0xb8

#ifdef  OMAP_I2C_SYSTEST
#undef  OMAP_I2C_SYSTEST
#endif
#define OMAP_I2C_SYSTEST  0xbc

// these are not overridden
#define OMAP_I2C_WE       0x34
#define OMAP_I2C_BUFSTAT  0xc0

#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/i2c/omap35xx/arm/j5/offsets.h $ $Rev: 699928 $")
#endif
