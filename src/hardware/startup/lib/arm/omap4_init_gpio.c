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


#include "startup.h"
#include <stdint.h>
#include <errno.h>
#include "arm/omap4_init_gpio.h"

// 0-31
#define OMAP4_GPIO1_BASE 0x4a310000
// 32-63
#define OMAP4_GPIO2_BASE 0x48055000
// 64-95
#define OMAP4_GPIO3_BASE 0x48057000
// 96-127
#define OMAP4_GPIO4_BASE 0x48059000
// 128-159
#define OMAP4_GPIO5_BASE 0x4805b000
// 160-191
#define OMAP4_GPIO6_BASE 0x4805d000

#define OMAP4_GPIO_OFF_IRQWAKEN_0      0x044
#define OMAP4_GPIO_OFF_OE              0x134
#define OMAP4_GPIO_OFF_LEVELDETECT0    0x140
#define OMAP4_GPIO_OFF_LEVELDETECT1    0x144
#define OMAP4_GPIO_OFF_RISINGDETECT    0x148
#define OMAP4_GPIO_OFF_FALLINGDETECT   0x14c
#define OMAP4_GPIO_OFF_DEBOUNCEENABLE  0x150
#define OMAP4_GPIO_OFF_DEBOUNCINGTIME  0x154
#define OMAP4_GPIO_OFF_CLEARDATAOUT    0x190
#define OMAP4_GPIO_OFF_SETDATAOUT      0x194

#define GPIO_GROUP(x)           ((x)/32)
#define GPIO_BIT_MASK(x)        ((1U) << ((x)%32))

unsigned GPIO_BANK_OFF[6] =
{
    OMAP4_GPIO1_BASE,
    OMAP4_GPIO2_BASE,
    OMAP4_GPIO3_BASE,
    OMAP4_GPIO4_BASE,
    OMAP4_GPIO5_BASE,
    OMAP4_GPIO6_BASE,
};

int
omap4_gpio_init (
  uint8_t      my_version,
  gpio_cfg_t * gpio_tbl,
  unsigned     table_size
)
{
  int i;
  uint32_t flags;
  unsigned gpio;

  for (i=0; i<table_size; ++i)
  {
    gpio = gpio_tbl[i].gpio_num;
    flags = gpio_tbl[i].flags;

    // If version specific flag is set, check if the version entry matches
    // our version.
    if (flags & GPIO_VERSION_SPECIFIC)
    {
      if ((my_version < gpio_tbl[i].version_start) ||
          (my_version > gpio_tbl[i].version_end))
      {
        continue;
      }
    }

    // if output, set output value, then OE
    if ((flags & (1<<GPIO_FLAGS_DIR_BIT)) == 0)
    { // output
      if (flags & (1<<GPIO_FLAGS_OUTPUT_BIT))
      { // hi
        out32(OMAP4_GPIO_OFF_SETDATAOUT + GPIO_BANK_OFF[GPIO_GROUP(gpio)],
              GPIO_BIT_MASK(gpio));
      }
      else
      { // low
        out32(OMAP4_GPIO_OFF_CLEARDATAOUT + GPIO_BANK_OFF[GPIO_GROUP(gpio)],
              GPIO_BIT_MASK(gpio));
      }

      out32(OMAP4_GPIO_OFF_OE + GPIO_BANK_OFF[GPIO_GROUP(gpio)],
            in32(OMAP4_GPIO_OFF_OE + GPIO_BANK_OFF[GPIO_GROUP(gpio)]) &
            ~GPIO_BIT_MASK(gpio));
    }
    else
    { // input
      out32(OMAP4_GPIO_OFF_OE + GPIO_BANK_OFF[GPIO_GROUP(gpio)],
            in32(OMAP4_GPIO_OFF_OE + GPIO_BANK_OFF[GPIO_GROUP(gpio)]) |
            GPIO_BIT_MASK(gpio));
    }

    // set any edge detect flags
    if (flags & (1<<GPIO_FLAGS_EDGE_HI_BIT))
    {
      out32(OMAP4_GPIO_OFF_RISINGDETECT + GPIO_BANK_OFF[GPIO_GROUP(gpio)], 
        in32(OMAP4_GPIO_OFF_RISINGDETECT + GPIO_BANK_OFF[GPIO_GROUP(gpio)]) |
        GPIO_BIT_MASK(gpio));  
    }
    if (flags & (1<<GPIO_FLAGS_EDGE_LO_BIT))
    {
      out32(OMAP4_GPIO_OFF_FALLINGDETECT + GPIO_BANK_OFF[GPIO_GROUP(gpio)], 
        in32(OMAP4_GPIO_OFF_FALLINGDETECT + GPIO_BANK_OFF[GPIO_GROUP(gpio)]) |
        GPIO_BIT_MASK(gpio));  
    }
    // set debounce
    if (flags & (1<<GPIO_FLAGS_DO_DEB_BIT))
    {
      out32(OMAP4_GPIO_OFF_DEBOUNCEENABLE + GPIO_BANK_OFF[GPIO_GROUP(gpio)], 
        in32(OMAP4_GPIO_OFF_DEBOUNCEENABLE + GPIO_BANK_OFF[GPIO_GROUP(gpio)]) |
        GPIO_BIT_MASK(gpio));  
      // Even though this is written per-gpio, the debounce time is common
      // to all GPIOs in a bank an so the input table needs to ensure all
      // debounced GPIOs in a bank have the same debounce time.
      // The caller is also responsible for enabling the debounce FCLK.
      out32(OMAP4_GPIO_OFF_DEBOUNCINGTIME + GPIO_BANK_OFF[GPIO_GROUP(gpio)], 
            gpio_tbl[i].debounce );

    }
    // set wakeup
    if (flags & (1<<GPIO_FLAGS_DO_WAKE_BIT))
    {
      out32(OMAP4_GPIO_OFF_IRQWAKEN_0 + GPIO_BANK_OFF[GPIO_GROUP(gpio)], 
        in32(OMAP4_GPIO_OFF_IRQWAKEN_0 + GPIO_BANK_OFF[GPIO_GROUP(gpio)]) |
        GPIO_BIT_MASK(gpio));  
    }
    
  }
  return EOK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/omap4_init_gpio.c $ $Rev: 680332 $")
#endif
