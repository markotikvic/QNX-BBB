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


/*
 * TI AM335xx interrupt controller support.
 */

#include "startup.h"
#include <arm/omap.h>
#include <arm/omap2420.h>
#include <arm/am335x.h>

#define AM335XX_INTC_MIR_SET3			0x0EC

static paddr_t	am335x_intc_base      = AM335X_INTC_BASE;
static paddr_t	am335x_edma_intr_base = 0x49001000;
static paddr_t	am335x_gpio0_base     = 0x44E07000;
static paddr_t	am335x_gpio1_base     = 0x4804C000;
static paddr_t	am335x_gpio2_base     = 0x481AC000;
static paddr_t	am335x_gpio3_base     = 0x481AE000;

extern struct callout_rtn interrupt_id_am335x;
extern struct callout_rtn interrupt_eoi_am335x;
extern struct callout_rtn interrupt_mask_am335x;
extern struct callout_rtn interrupt_unmask_am335x;

extern struct callout_rtn interrupt_id_edma;
extern struct callout_rtn interrupt_eoi_edma;
extern struct callout_rtn interrupt_mask_edma;
extern struct callout_rtn interrupt_unmask_edma;

extern struct callout_rtn interrupt_id_am335x_gpio;
extern struct callout_rtn interrupt_eoi_am335x_gpio;
extern struct callout_rtn interrupt_mask_am335x_gpio;
extern struct callout_rtn interrupt_unmask_am335x_gpio;

const static struct startup_intrinfo    intrs[] = {
    /* primary interrupt controller */
    {   _NTO_INTR_CLASS_EXTERNAL,       // vector base
        128,                            // number of vectors
        _NTO_INTR_SPARE,                // cascade vector
        0,                              // CPU vector base
        0,                              // CPU vector stride
        0,                              // flags
        { INTR_GENFLAG_LOAD_SYSPAGE,    0, &interrupt_id_am335x},
        { INTR_GENFLAG_LOAD_SYSPAGE | INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_am335x},
        &interrupt_mask_am335x,         // mask   callout
        &interrupt_unmask_am335x,       // unmask callout
        0,                              // config callout
        &am335x_intc_base
    },
	// EDMA interrupt
	{	0x200,							// vector base
		64,								// number of vectors
		12,								// cascade vector
		0,								// CPU vector base
		0,								// CPU vector stride
		0,								// flags
		{INTR_GENFLAG_LOAD_SYSPAGE, 0, &interrupt_id_edma},
		{INTR_GENFLAG_LOAD_SYSPAGE | INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_edma},
		&interrupt_mask_edma,			// mask   callout
		&interrupt_unmask_edma,			// unmask callout
		0,								// config callout
		&am335x_edma_intr_base
	},
	// GPIO0 interrupt
	{   0x300,                          // vector base
	    32,                             // number of vectors
	    96,                             // cascade vector
	    0,                              // CPU vector base
	    0,                              // CPU vector stride
	    0,                              // flags
	    { 0, 0, &interrupt_id_am335x_gpio},
	    { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_am335x_gpio},
	    &interrupt_mask_am335x_gpio,    // mask   callout
	    &interrupt_unmask_am335x_gpio,  // unmask callout
	    0,                              // config callout
	    &am335x_gpio0_base
	},
	// GPIO1 interrupt
	{   0x320,                          // vector base
	    32,                             // number of vectors
	    98,                             // cascade vector
	    0,                              // CPU vector base
	    0,                              // CPU vector stride
	    0,                              // flags
	    { 0, 0, &interrupt_id_am335x_gpio},
	    { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_am335x_gpio},
	    &interrupt_mask_am335x_gpio,    // mask   callout
	    &interrupt_unmask_am335x_gpio,  // unmask callout
	    0,                              // config callout
	    &am335x_gpio1_base
	},
	// GPIO2 interrupt
	{   0x340,                          // vector base
	    32,                             // number of vectors
	    32,                             // cascade vector
	    0,                              // CPU vector base
	    0,                              // CPU vector stride
	    0,                              // flags
	    { 0, 0, &interrupt_id_am335x_gpio},
	    { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_am335x_gpio},
	    &interrupt_mask_am335x_gpio,    // mask   callout
	    &interrupt_unmask_am335x_gpio,  // unmask callout
	    0,                              // config callout
	    &am335x_gpio2_base
	},
	// GPIO3 interrupt
	{   0x360,                          // vector base
	    32,                             // number of vectors
	    62,                             // cascade vector
	    0,                              // CPU vector base
	    0,                              // CPU vector stride
	    0,                              // flags
	    { 0, 0, &interrupt_id_am335x_gpio},
	    { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_am335x_gpio},
	    &interrupt_mask_am335x_gpio,    // mask   callout
	    &interrupt_unmask_am335x_gpio,  // unmask callout
	    0,                              // config callout
	    &am335x_gpio3_base
	},
};

void
init_intrinfo()
{
    int i;

    /*
     * Reset interrupt control module
     */
    out32(am335x_intc_base + OMAP2420_INTC_SYSCONFIG, 1 << 1);
    while (!(in32(am335x_intc_base + OMAP2420_INTC_SYSSTATUS) & 1))
        ;

    /*
     * Mask off all the interrupts
     */
    out32(am335x_intc_base + OMAP2420_INTC_MIR_SET0, 0xFFFFFFFF);
    out32(am335x_intc_base + OMAP2420_INTC_MIR_SET1, 0xFFFFFFFF);
    out32(am335x_intc_base + OMAP2420_INTC_MIR_SET2, 0xFFFFFFFF);
    out32(am335x_intc_base + AM335XX_INTC_MIR_SET3 , 0xFFFFFFFF);

    /*
     * Mask all GPIO interrupts
     */
    out32(am335x_gpio0_base + GPIO_IRQSTATUS_CLR_0, 0xffffffff);
    out32(am335x_gpio1_base + GPIO_IRQSTATUS_CLR_0, 0xffffffff);
    out32(am335x_gpio2_base + GPIO_IRQSTATUS_CLR_0, 0xffffffff);
    out32(am335x_gpio3_base + GPIO_IRQSTATUS_CLR_0, 0xffffffff);

    /*
     * Set the priority to the highest and route to IRQ
     */
    for (i = OMAP2420_INTC_ILR(0); i <= OMAP2420_INTC_ILR(128); i += 4)
        out32(am335x_intc_base + i, 0);

    add_interrupt_array(intrs, sizeof(intrs));
}




#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/ti-am335x/init_intrinfo.c $ $Rev: 710799 $")
#endif
