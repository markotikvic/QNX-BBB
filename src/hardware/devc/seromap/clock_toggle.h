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


#ifndef CLOCK_TOGGLE_H_
#define CLOCK_TOGGLE_H_

// We use these to determine which CLKCTRL to use
#define OMAP_UART1_PHYSBASE 0x4806A000
#define OMAP_UART2_PHYSBASE 0x4806C000
#define OMAP_UART3_PHYSBASE 0x48020000
#define OMAP_UART4_PHYSBASE 0x4806E000

/********* New for OMAP5 ************************/
#define OMAP5_UART5_PHYSBASE 0x48066000
#define OMAP5_UART6_PHYSBASE 0x48068000

#define CM_L4PER_UART1_CLKCTRL 0x4A009540
#define CM_L4PER_UART2_CLKCTRL 0x4A009548
#define CM_L4PER_UART3_CLKCTRL 0x4A009550
#define CM_L4PER_UART4_CLKCTRL 0x4A009558

/********* New for OMAP5 ************************/
#define CM_L4PER_UART5_CLKCTRL 0x4A009570
#define CM_L4PER_UART6_CLKCTRL 0x4A009578

#define OMAP_CLKCTRL_MODMODE_MASK 0x3
#define OMAP_CLKCTRL_MODMODE_ENABLE 0x2
#define OMAP_CLKCTRL_MODMODE_DISABLE 0x0
#define OMAP_CLKCTRL_IDLEST_MASK 0x30000
#define OMAP_CLKCTRL_IDLEST_FUNCTIONAL (0x0 << 16)

#define OMAP_UART_SYSC_IDLEMODE_MASK 		0x18
#define OMAP_UART_SYSC_IDLEMODE_SMART 		(0x2 << 3)
#define OMAP_UART_SYSC_IDLEMODE_NONE 		(0x1 << 3)
#define OMAP_UART_SYSC_IDLEMODE_FORCE 		(0x0 << 3)
#define OMAP_UART_SYSC_IDLEMODE_SMARTWAKEUP (0x3 << 3)

#define OMAP_UART_SYSC_ENAWAKEUP_DISABLE	(0x0 << 2)
#define OMAP_UART_SYSC_ENAWAKEUP_ENABLE		(0x1 << 2)

#define OMAP_UART_SYSC_AUTOIDLE_MASK 0x1
#define OMAP_UART_SYSC_AUTOIDLE_ENABLE 0x1
#define OMAP_UART_SYSC_AUTOIDLE_DISABLE 0x0

#define OMAP_UART_WER_CTS_ENABLE			(0x1 << 0)
#define OMAP_UART_WER_CTS_DISABLE			(0x0 << 0)

typedef enum clk_enable_e {
    clk_enable_force_idle = OMAP_UART_SYSC_IDLEMODE_FORCE,
    clk_enable_none       = OMAP_UART_SYSC_IDLEMODE_NONE,
    clk_enable_smart      = OMAP_UART_SYSC_IDLEMODE_SMART,
    clk_enable_smart_wkup = OMAP_UART_SYSC_IDLEMODE_SMARTWAKEUP,
    clk_enable_skip       = -1
}clk_enable_t;

#endif /* CLOCK_TOGGLE_H_ */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap/clock_toggle.h $ $Rev: 736735 $")
#endif
