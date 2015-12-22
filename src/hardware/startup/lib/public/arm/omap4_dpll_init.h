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

#ifndef _OMAP4_DPLL_INIT_H_
#define _OMAP4_DPLL_INIT_H_

typedef struct
{
  char * dpll_name;
  uint32_t base_addr;
  int only_check_rate;
  int no_config_if_enabled;
  int no_lock_wait;
  int bypclk_clksel;
  int dpll_ramp;
  int dpll_mult;
  int dpll_div;  // actual div, not div-1
  int m2_div;
  int m3_div;
  int m4_div;
  int m5_div;
  int m6_div;
  int m7_div;
} dpll_cfg_type;

#define DPLL_CFG_UNUSED -1

void omap4_dpll_bypass(uint32_t base_addr);

void omap4_dpll_set_output_divisor(uint32_t base_addr, int value);

int omap4_dpll_init(dpll_cfg_type * dpll_cfg);

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/arm/omap4_dpll_init.h $ $Rev: 736898 $")
#endif
