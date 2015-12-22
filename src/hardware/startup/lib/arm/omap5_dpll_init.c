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
#include <stdint.h>
#include <errno.h>
#include "arm/omap5_dpll_init.h"

#define LDELAY 12000000

#define DPLL_CLKMODE_OFFSET                   0x00
#define DPLL_IDLEST_OFFSET                    0x04
#define DPLL_AUTOIDLE_OFFSET                  0x08
#define DPLL_CLKSEL_OFFSET                    0x0c
#define DPLL_DIV_M2_OFFSET                    0x10
#define DPLL_DIV_M3_OFFSET                    0x14
#define DPLL_DIV_H11_OFFSET                   0x18
#define DPLL_DIV_H12_OFFSET                   0x1c
#define DPLL_DIV_H13_OFFSET                   0x20
#define DPLL_DIV_H14_OFFSET                   0x24
#define DPLL_DIV_H22_OFFSET                   0x34
#define DPLL_DIV_H23_OFFSET                   0x38
#define DPLL_BYPCLK_OFFSET                    0x3c

#define DPLL_CLKMODE_EN_MASK                  0x7
#define DPLL_CLKMODE_EN_MN_POWER_BYPASS       0x4
#define DPLL_CLKMODE_EN_IDLE_BYPASS_LOW_POWER 0x5
#define DPLL_CLKMODE_EN_FAST_RELOCK_BYPASS    0x6
#define DPLL_CLKMODE_EN_ON                    0x7

#define DPLL_CLKMODE_RAMP_MASK                0x18
#define DPLL_CLKMODE_RAMP_SHIFT               3

#define DPLL_CLKMODE_DRIFTGUARD         8
#define DPLL_CLKMODE_RELOCK_RAMP        9
#define DPLL_CLKMODE_LPMODE             10
#define DPLL_CLKMODE_REGMAXEN           11
#define DPLL_CLKMODE_SSC_EN             12
#define DPLL_CLKMODE_SSC_ACK            13
#define DPLL_CLKMODE_SSC_DOWNSPREAD     14

#define DPLL_IDLEST_LOCKED                    0x1

#define DPLL_AUTOIDLE_MODE_MASK               0x7
#define DPLL_AUTOIDLE_MODE_DISABLE            0x0

#define DPLL_CLKSEL_MULT_MASK                 0x7ff00
#define DPLL_CLKSEL_MULT_SHIFT                8
#define DPLL_CLKSEL_DIV_MASK                  0x7f
#define DPLL_CLKSEL_DIV_SHIFT                 0

#define DPLL_CLKOUT_DIV_MASK                  0x1f
#define DPLL_CLKOUT_DIV_SHIFT                 0
#define DPLL_CLKOUT_GATE_MASK                 0x100

#define DPLL_BYPCLK_CLKSEL_MASK               0x3

void omap5_dpll_bypass(uint32_t base_addr)
{
  int delay;
  // place DPLL in bypass mode and wait for that to take effect
  out32(base_addr + DPLL_CLKMODE_OFFSET,
    (in32(base_addr + DPLL_CLKMODE_OFFSET) & ~DPLL_CLKMODE_EN_MASK) | DPLL_CLKMODE_EN_MN_POWER_BYPASS);

  delay = LDELAY;
  while ((in32(base_addr + DPLL_IDLEST_OFFSET) & DPLL_IDLEST_LOCKED) && delay--);
}

void omap5_dpll_set_output_divisor(uint32_t base_addr, int value)
{
  if (value != DPLL_CFG_UNUSED)
  {
    out32(base_addr, (in32(base_addr) & ~DPLL_CLKOUT_DIV_MASK) |
                     value<<DPLL_CLKOUT_DIV_SHIFT);
    out32(base_addr, in32(base_addr) | DPLL_CLKOUT_GATE_MASK);
  }
}

int omap5_dpll_init(dpll_cfg_type * dpll_cfg)
{
  int delay;
  unsigned clk_mode;

  clk_mode = in32(dpll_cfg->base_addr + DPLL_CLKMODE_OFFSET) & DPLL_CLKMODE_EN_MASK;

  // if requested, dpll is not configured if it is already on
  if (dpll_cfg->no_config_if_enabled)
  {
    if (clk_mode == DPLL_CLKMODE_EN_ON)
      return EOK;
  }

  // if requested, assert that DPLL is on already at specified rate
  if (dpll_cfg->only_check_rate)
  {
    unsigned mult;
    unsigned div;
    if (clk_mode != DPLL_CLKMODE_EN_ON)
    {
      kprintf("Fatal Error: %s DPLL is not enabled.\n", dpll_cfg->dpll_name);
      return -1;
    }

    mult = in32(dpll_cfg->base_addr + DPLL_CLKSEL_OFFSET);
    div = ((mult & DPLL_CLKSEL_DIV_MASK) >> DPLL_CLKSEL_DIV_SHIFT)+1;
    mult = (mult & DPLL_CLKSEL_MULT_MASK) >> DPLL_CLKSEL_MULT_SHIFT;
    if ((mult != dpll_cfg->dpll_mult) || (div != dpll_cfg->dpll_div))
    {
      kprintf("Error: %s DPLL is not configured as expected -- skipping divisor configuration.\n");
      return -1;
    }
  }
  else
  {
    // place DPLL in bypass mode and wait for that to take effect
    omap5_dpll_bypass(dpll_cfg->base_addr);

    // configure bypclk
    if (dpll_cfg->bypclk_clksel != DPLL_CFG_UNUSED)
    {
      out32(dpll_cfg->base_addr + DPLL_BYPCLK_OFFSET,
        (in32(dpll_cfg->base_addr + DPLL_BYPCLK_OFFSET) & ~DPLL_BYPCLK_CLKSEL_MASK) | dpll_cfg->bypclk_clksel);
    }

    // auto control disabled
    out32(dpll_cfg->base_addr + DPLL_AUTOIDLE_OFFSET,
      (in32(dpll_cfg->base_addr + DPLL_AUTOIDLE_OFFSET) & ~DPLL_AUTOIDLE_MODE_MASK) | DPLL_AUTOIDLE_MODE_DISABLE);

    // configure DPLL rate
    out32(dpll_cfg->base_addr + DPLL_CLKSEL_OFFSET,
      (in32(dpll_cfg->base_addr + DPLL_CLKSEL_OFFSET) & ~(DPLL_CLKSEL_MULT_MASK | DPLL_CLKSEL_DIV_MASK)) | 
      dpll_cfg->dpll_mult<<DPLL_CLKSEL_MULT_SHIFT |
      (dpll_cfg->dpll_div-1)<<DPLL_CLKSEL_DIV_SHIFT);
  }

  // configure output divisors
  omap5_dpll_set_output_divisor(dpll_cfg->base_addr + DPLL_DIV_M2_OFFSET,
                                dpll_cfg->m2_div);
  omap5_dpll_set_output_divisor(dpll_cfg->base_addr + DPLL_DIV_M3_OFFSET,
                                dpll_cfg->m3_div);
  omap5_dpll_set_output_divisor(dpll_cfg->base_addr + DPLL_DIV_H11_OFFSET,
                                dpll_cfg->h11_div);
  omap5_dpll_set_output_divisor(dpll_cfg->base_addr + DPLL_DIV_H12_OFFSET,
                                dpll_cfg->h12_div);
  omap5_dpll_set_output_divisor(dpll_cfg->base_addr + DPLL_DIV_H13_OFFSET,
                                dpll_cfg->h13_div);
  omap5_dpll_set_output_divisor(dpll_cfg->base_addr + DPLL_DIV_H14_OFFSET,
                                dpll_cfg->h14_div);
  omap5_dpll_set_output_divisor(dpll_cfg->base_addr + DPLL_DIV_H22_OFFSET,
                                dpll_cfg->h22_div);
  omap5_dpll_set_output_divisor(dpll_cfg->base_addr + DPLL_DIV_H23_OFFSET,
                                dpll_cfg->h23_div);

  if (!dpll_cfg->only_check_rate)
  {
    // place DPLL in on mode and wait for that to take effect
    out32(dpll_cfg->base_addr + DPLL_CLKMODE_OFFSET,
      (in32(dpll_cfg->base_addr + DPLL_CLKMODE_OFFSET) & ~(DPLL_CLKMODE_EN_MASK | DPLL_CLKMODE_RAMP_MASK)) |
      DPLL_CLKMODE_EN_ON | (dpll_cfg->dpll_ramp<<DPLL_CLKMODE_RAMP_SHIFT));

    if (!dpll_cfg->no_lock_wait)
    {
      delay = LDELAY;
      while (((in32(dpll_cfg->base_addr + DPLL_IDLEST_OFFSET) & DPLL_IDLEST_LOCKED) == 0) && delay--);
    }
  }

  return EOK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/omap5_dpll_init.c $ $Rev: 736706 $")
#endif
