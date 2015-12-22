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

#ifndef _OMAP4_INIT_GPIO_H_
#define _OMAP4_INIT_GPIO_H_

#define GPIO_FLAGS_DIR_BIT     0
#define GPIO_FLAGS_OUTPUT_BIT  1
#define GPIO_FLAGS_EDGE_HI_BIT 2
#define GPIO_FLAGS_EDGE_LO_BIT 3
#define GPIO_FLAGS_DO_DEB_BIT  4
#define GPIO_FLAGS_DO_WAKE_BIT 5
#define GPIO_FLAGS_VER_BIT     6

#define DIR_INPUT   1
#define DIR_OUTPUT  0

#define GPIO_INPUT    (DIR_INPUT<<GPIO_FLAGS_DIR_BIT)
#define GPIO_OUT_HI   ((DIR_OUTPUT<<GPIO_FLAGS_DIR_BIT)|(1<<GPIO_FLAGS_OUTPUT_BIT))
#define GPIO_OUT_LO   ((DIR_OUTPUT<<GPIO_FLAGS_DIR_BIT)|(0<<GPIO_FLAGS_OUTPUT_BIT))
#define GPIO_EDGE_HI  (1<<GPIO_FLAGS_EDGE_HI_BIT)
#define GPIO_EDGE_LO  (1<<GPIO_FLAGS_EDGE_LO_BIT)
#define GPIO_DEB_ON   (1<<GPIO_FLAGS_DO_DEB_BIT)
#define GPIO_WAKE_ON  (1<<GPIO_FLAGS_DO_WAKE_BIT)
#define GPIO_VERSION_SPECIFIC (1<<GPIO_FLAGS_VER_BIT)

typedef struct
{
  uint8_t version_start;
  uint8_t version_end;
  uint8_t gpio_num;
  uint8_t debounce;
  uint32_t flags;
} gpio_cfg_t;

int
omap4_gpio_init ( 
  uint8_t      my_version,
  gpio_cfg_t * gpio_tbl,
  unsigned     table_size
);

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/arm/omap4_init_gpio.h $ $Rev: 680332 $")
#endif
