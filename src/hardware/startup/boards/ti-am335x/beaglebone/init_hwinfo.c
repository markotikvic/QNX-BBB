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

#include "startup.h"
#include <arm/am335x.h>

// Audio: EDMA Event Mux
#define TPCC_EVTMUX_0_3     0xF90
#define TPCC_EVTMUX_4_7     0xF94
#define TPCC_EVTMUX_8_11    0xF98
#define TPCC_EVTMUX_12_15   0xF9C
#define TPCC_EVTMUX_16_19   0xFA0
#define TPCC_EVTMUX_20_23   0xFA4
#define TPCC_EVTMUX_24_27   0xFA8
#define TPCC_EVTMUX_28_31   0xFAC
#define TPCC_EVTMUX_32_35   0xFB0
#define TPCC_EVTMUX_36_39   0xFB4
#define TPCC_EVTMUX_40_43   0xFB8
#define TPCC_EVTMUX_44_47   0xFBC
#define TPCC_EVTMUX_48_51   0xFC0
#define TPCC_EVTMUX_52_55   0xFC4
#define TPCC_EVTMUX_56_59   0xFC8
#define TPCC_EVTMUX_60_63   0xFCC

#define MCASP5_TX_EVTNUM    0x1A
#define MCASP5_RX_EVTNUM    0x1B

#define delay(x) {volatile unsigned _delay = x; while (_delay--); } /* 5+ cycles per loop */

void usb_delay(int t)
{
    volatile uint32_t i = t*1000;
    while(i--);
}


void phy_on(uint32_t USBx_CTRL)
{
    uint32_t regVal;
    
    regVal = in32(USBx_CTRL);
    
    regVal &= ~(USBPHY_CM_PWRDN   | USBPHY_OTG_PWRDN
                | USBPHY_DMPULLUP | USBPHY_DPPULLUP
                | USBPHY_DATA_POLARITY);
    regVal |= (USBPHY_SRCONDM       | USBPHY_SINKONDP
               | USBPHY_CHGISINK_EN | USBPHY_CHGVSRC_EN
               | USBPHY_CDET_EXTCTL | USBPHY_DPOPBUFCTL
               | USBPHY_DMOPBUFCTL  | USBPHY_DPGPIO_PD
               | USBPHY_DMGPIO_PD   | USBPHY_OTGVDET_EN
               | USBPHY_OTGSESSEND_EN);
    
    out32(USBx_CTRL, regVal);
    
}

void
init_hwinfo()
{
    /* start the on-chip PHY and its PLL */
    phy_on(AM335X_USBCTRL0);
    usb_delay(5);
    phy_on(AM335X_USBCTRL1);
    usb_delay(5);

    /* Force enable of CLKDCOLDO clock */
    out32(AM335X_CM_CLKDCOLDO_DPLL_PER, DPLL_CLKDCOLDO_GATE_CTRL);

}



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/ti-am335x/beaglebone/init_hwinfo.c $ $Rev: 716637 $")
#endif
