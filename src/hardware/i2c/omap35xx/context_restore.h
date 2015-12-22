/*
 * $QNXLicenseC:
 * Copyright 2011, QNX Software Systems.
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

#ifndef CONTEXT_RESTORE_H_
#define CONTEXT_RESTORE_H_

/*
 * Context registers for I2C, used to determine if we've lost
 * context on clock enable.
 */
 
/* OMAP4 register definition */ 
#define RM_L4PER_I2C1_CONTEXT   0x4A3074A4
#define RM_L4PER_I2C2_CONTEXT   0x4A3074AC
#define RM_L4PER_I2C3_CONTEXT   0x4A3074B4
#define RM_L4PER_I2C4_CONTEXT   0x4A3074BC
#define RM_L4PER_L4_PER_CONTEXT 0x4A3074C0

/* OMAP5 register definition */
#define OMAP5_ES1_RM_L4PER_I2C1_CONTEXT   0x4AE074A4
#define OMAP5_ES1_RM_L4PER_I2C2_CONTEXT   0x4AE074AC
#define OMAP5_ES1_RM_L4PER_I2C3_CONTEXT   0x4AE074B4
#define OMAP5_ES1_RM_L4PER_I2C4_CONTEXT   0x4AE074BC
#define OMAP5_ES1_RM_L4PER_I2C5_CONTEXT   0x4AE0756C

#define OMAP5_ES2_RM_L4PER_I2C1_CONTEXT   0x4AE070A4
#define OMAP5_ES2_RM_L4PER_I2C2_CONTEXT   0x4AE070AC
#define OMAP5_ES2_RM_L4PER_I2C3_CONTEXT   0x4AE070B4
#define OMAP5_ES2_RM_L4PER_I2C4_CONTEXT   0x4AE070BC
#define OMAP5_ES2_RM_L4PER_I2C5_CONTEXT   0x4AE0716C

#define OMAP5_RM_L4PER_L4_PER_CONTEXT 0x4AE074C0

/* J6 register definition */
#define J6_RM_L4PER_I2C1_CONTEXT    0x4AE074A4
#define J6_RM_L4PER_I2C2_CONTEXT    0x4AE074AC
#define J6_RM_L4PER_I2C3_CONTEXT    0x4AE074B4
#define J6_RM_L4PER_I2C4_CONTEXT    0x4AE074BC
#define J6_RM_IPU_I2C5_CONTEXT      0x4AE0657C

#define LOSTCONTEXT_DFF_MASK  0x1
#define LOSTCONTEXT_RFF_MASK  (0x1 << 1)

int
context_restore_init(omap_dev_t *dev);

void
context_restore_fini(omap_dev_t *dev);

void
context_restore_save(omap_dev_t *dev);

void
context_restore(omap_dev_t *dev);

#endif /* CONTEXT_RESTORE_H_ */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/i2c/omap35xx/context_restore.h $ $Rev: 739568 $")
#endif
