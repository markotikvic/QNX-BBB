/*
 * $QNXLicenseC: 
 * Copyright 2008, QNX Software Systems.  
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

#ifndef PROTO_H
#define PROTO_H

DEV_OMAP *create_device(TTYINIT_OMAP *dip, unsigned unit, unsigned maxim_xcvr_kick);
void ser_stty(DEV_OMAP *dev);
void ser_ctrl(DEV_OMAP *dev, unsigned flags);
void set_port(unsigned port, unsigned mask, unsigned data);
void *query_default_device(TTYINIT_OMAP *dip, void *link);
const struct sigevent *ser_intr(void *area, int id);
unsigned options(int argc, char *argv[]);

#ifdef PWR_MAN
#include <clock_toggle.h>
#include <context.h>

int extra_devctl(resmgr_context_t * ctp, io_devctl_t * msg, iofunc_ocb_t * ocb);
int get_chip_type();

#ifdef WINBT

// control of the Bluetooth pins
void bt_ctrl_init();
void bt_enable(int enable);
#endif // End of #ifdef WINBT

void omap_clock_enable(DEV_OMAP* dev, clk_enable_t  clk_cfg);
void omap_clock_disable(DEV_OMAP* dev);
int omap_clock_toggle_init(DEV_OMAP* dev);
void omap_clock_enable_isr(DEV_OMAP* dev);
void omap_clock_disable_isr(DEV_OMAP* dev);
int omap_force_rts_init(struct dev_omap* dev);
void omap_force_rts(struct dev_omap* dev, int level);

int omap_uart_ctx_init(DEV_OMAP *dev);
int omap_uart_ctx_save(DEV_OMAP *dev);
int omap_uart_ctx_restore(DEV_OMAP *dev);

#endif // End of #ifdef PWR_MAN

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap/proto.h $ $Rev: 714766 $")
#endif
