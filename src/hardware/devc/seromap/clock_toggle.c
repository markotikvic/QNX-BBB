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




#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include "externs.h"

#ifdef PWR_MAN

static const unsigned sleep_slice_us = 1000; /* 1ms */
static const unsigned max_sleep_us = 100000; /* 100ms */
static const unsigned max_isr_poll_count = 1000;

static int
poll_for_condition(uintptr_t addr, uint_t mask, uint_t desired_val)
{
	uint32_t val;
	unsigned slept_us = 0;

	val = (in32(addr) & mask);
	while (val != desired_val) {
		if (0 == usleep(sleep_slice_us)) {
			slept_us += sleep_slice_us;

			if (slept_us >= max_sleep_us) {
				return -1;
			}
		}
		val = (in32(addr) & mask);
	}

	return 0;
}

void
omap_clock_enable(DEV_OMAP* dev, clk_enable_t clk_cfg)
{
	int enable_rc = 0;
	int functional_rc = 0;

	/* Our idle state can be changed by the ISR so we must use a spinlock */
	InterruptLock(&dev->idle_spinlock);

	/* Only enable clocks if they aren't enabled already */
	if (dev->idle == 0) {
		goto done;
	}

	if (dev->clkctrl_base) {
		/* Enable the clock */
		out32(dev->clkctrl_base, OMAP_CLKCTRL_MODMODE_ENABLE);

		/* Wait for the module mode to have been written */
		enable_rc = poll_for_condition(dev->clkctrl_base, OMAP_CLKCTRL_MODMODE_MASK, OMAP_CLKCTRL_MODMODE_ENABLE);

		/* Wait for the module idle status to report "fully functional" */
		functional_rc = poll_for_condition(dev->clkctrl_base, OMAP_CLKCTRL_IDLEST_MASK, OMAP_CLKCTRL_IDLEST_FUNCTIONAL);
        
		/* Re-configure clock if specified otherwise simply skip it */
		if (clk_cfg != clk_enable_skip) {

			/* Set the idle mode to smart idle with wake up */
			set_port(dev->port[OMAP_UART_SYSC], OMAP_UART_SYSC_IDLEMODE_MASK, clk_cfg);	
        }

		/* Enable the CTS wakeup */
		write_omap(dev->port[OMAP_UART_WER], OMAP_UART_WER_CTS_ENABLE);

		/* Indicate clocks are enabled */
		dev->idle = 0;
	}

done:
#ifdef WINBT
	/* clear CTS debounce timer and OHW_PAGED flag */
	if (dev->tty.un.s.spare_tmr > 0) {
		dev->tty.un.s.spare_tmr = 0;
		if (dev->tty.flags & OHW_PAGED)
			atomic_clr (&dev->tty.flags, OHW_PAGED);
	}
#endif
	omap_uart_ctx_restore(dev);
	InterruptUnlock(&dev->idle_spinlock);

	/* Don't slog while interrupts are disabled - otherwise slogf() will re-enable interrupts */
	if (enable_rc) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Failed to set module mode to 'enabled'", __FUNCTION__);
	}

	if (functional_rc) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Module failed to report 'fully functional'", __FUNCTION__);
	}
}

static int
poll_for_condition_isr(uintptr_t addr, uint_t mask, uint_t desired_val)
{
	uint32_t val;
	unsigned poll_count = 0;

	val = (in32(addr) & mask);
	while (val != desired_val) {
		poll_count += 1;
		if (poll_count >= max_isr_poll_count) {
			return -1;
		}
		val = (in32(addr) & mask);
	}

	return 0;
}

void
omap_clock_enable_isr(DEV_OMAP* dev)
{
	/*
	 * Only enable clocks if they aren't enabled already. Note that the ISR must
	 * acquire the idle_spinlock to ensure we don't race with the devctl
	 */
	if (dev->idle == 0) {
		return;
	}
	
	if (dev->clkctrl_base) {

		/* Enable the clock */
		out32(dev->clkctrl_base, OMAP_CLKCTRL_MODMODE_ENABLE);

		/* Wait for the module mode to have been written */
		poll_for_condition_isr(dev->clkctrl_base, OMAP_CLKCTRL_MODMODE_MASK, OMAP_CLKCTRL_MODMODE_ENABLE);

		/* Wait for the module idle status to report "fully functional" */
		poll_for_condition_isr(dev->clkctrl_base, OMAP_CLKCTRL_IDLEST_MASK, OMAP_CLKCTRL_IDLEST_FUNCTIONAL);

		/* Indicate clocks are enabled */
		dev->idle = 0;

		/* restore UART context */
		omap_uart_ctx_restore(dev);
	}
}

void
omap_clock_disable(DEV_OMAP* dev)
{
	int rc = 0;

	/* Our idle state can be changed by the ISR so we must use a spinlock */
	InterruptLock(&dev->idle_spinlock);

	/* Only disable clocks if needed */
	if (dev->idle == 1) {
		goto done;
	}
	
	if (dev->clkctrl_base) {

		/* Indicate clocks are disabled */
		dev->idle = 1;

		/* Save UART context */
		omap_uart_ctx_save(dev);

		/* Set the idle mode to smart idle with wake up */
		write_omap(dev->port[OMAP_UART_SYSC], 	OMAP_UART_SYSC_IDLEMODE_SMARTWAKEUP |
												OMAP_UART_SYSC_ENAWAKEUP_ENABLE |
												OMAP_UART_SYSC_AUTOIDLE_ENABLE);

		/* Enable the wakeup event */
		set_port(dev->port[OMAP_UART_SCR], OMAP_SCR_WAKEUPEN, OMAP_SCR_WAKEUPEN);

		/* Disable the clock */
		out32(dev->clkctrl_base, OMAP_CLKCTRL_MODMODE_DISABLE);

		/* Wait for the module mode to have been written */
		rc = poll_for_condition(dev->clkctrl_base, OMAP_CLKCTRL_MODMODE_MASK, OMAP_CLKCTRL_MODMODE_DISABLE);

	}

done:
	InterruptUnlock(&dev->idle_spinlock);

	/* Don't slog while interrupts are disabled - otherwise slogf() will re-enable interrupts */
	if (rc) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Failed to set module mode to 'disabled'", __FUNCTION__);
	}
}

int
omap_clock_toggle_init(DEV_OMAP* dev)
{
	if (OMAP_UART1_PHYSBASE == dev->physbase) {
		dev->clkctrl_phys = CM_L4PER_UART1_CLKCTRL;
	} else if (OMAP_UART2_PHYSBASE == dev->physbase) {
		dev->clkctrl_phys = CM_L4PER_UART2_CLKCTRL;
	} else if (OMAP_UART3_PHYSBASE == dev->physbase) {
		dev->clkctrl_phys = CM_L4PER_UART3_CLKCTRL;
	} else if (OMAP_UART4_PHYSBASE == dev->physbase) {
		dev->clkctrl_phys = CM_L4PER_UART4_CLKCTRL;
	} else if (OMAP5_UART5_PHYSBASE == dev->physbase) {
                dev->clkctrl_phys = CM_L4PER_UART5_CLKCTRL;
        } else if (OMAP5_UART6_PHYSBASE == dev->physbase) {
                dev->clkctrl_phys = CM_L4PER_UART6_CLKCTRL;
        } else {
		dev->clkctrl_phys = 0;
		dev->clkctrl_base = 0;
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Unrecognized physical address, clock toggling not enabled", __FUNCTION__);
		return -1;
	}

	if (dev->clkctrl_phys) {
		dev->clkctrl_base = mmap_device_io(4, dev->clkctrl_phys);
		if (dev->clkctrl_base == (uintptr_t)MAP_FAILED) {
			slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: mmap_device_io clkctrl_base: %s", __FUNCTION__, strerror(errno));
			return -1;
		}
	}

	/* Indicate clocks are disabled by default */
	dev->idle = 1;
	
	/* oband notification set to off */
	dev->signal_oband_notification = 0;

	/* initialize UART context */
	omap_uart_ctx_init(dev);

	return 0;
}


#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap/clock_toggle.c $ $Rev: 736735 $")
#endif
