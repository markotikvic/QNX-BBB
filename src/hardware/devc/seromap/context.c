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

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <arm/omap.h>

#include "externs.h"

#ifdef PWR_MAN

/*
 * When the CPU goes off mode low power state the UART context is lost.
 * After the CPU wakes up from off mode low power state for the UART 
 * to continue to run its context must be restored. The UART context
 * consists of registers described in context.h
 */

#define OMAP_LSR_TX_E           (OMAP_LSR_TXRDY | OMAP_LSR_TSRE)

#define GET_UART_LOSTCTX(ctp)	(in32(ctp->ctx_reg) & OMAP_UART_LOSTCONTEXT_RFF)
#define CLR_UART_LOSTCTX(ctp)	out32(ctp->ctx_reg, OMAP_UART_LOSTCONTEXT_RFF)

#define CONFIG_MODE_A(port)		write_omap(port[OMAP_UART_LCR], 0x80)
#define CONFIG_MODE_B(port)		write_omap(port[OMAP_UART_LCR], 0xBF)
#define OPERATIONAL_MODE(port)	write_omap(port[OMAP_UART_LCR], 0x00)

uint32_t rm_l4per_uart1_context = RM_L4PER_UART1_CONTEXT;
uint32_t rm_l4per_uart2_context = RM_L4PER_UART2_CONTEXT;
uint32_t rm_l4per_uart3_context = RM_L4PER_UART3_CONTEXT;
uint32_t rm_l4per_uart4_context = RM_L4PER_UART4_CONTEXT;

// restore UART context
int omap_uart_ctx_restore(DEV_OMAP *dev)
{
	uintptr_t *port = dev->port;
	uart_ctx_t *ctp = (uart_ctx_t *) dev->uart_ctx;

    // restore context if it is lost
	if (ctp == NULL || !GET_UART_LOSTCTX(ctp))
		return 0;

	// clear lost context
	CLR_UART_LOSTCTX(ctp);

	//  disable mode select
	write_omap(port[OMAP_UART_MDR1], OMAP_MDR1_MODE_DISABLE);

	/* disable all interrupts */
	write_omap(port[OMAP_UART_IER], 0);

   	write_omap(port[OMAP_UART_SYSC], ctp->SYSC); // restore SYSC

   	// switch to Configuration Mode B
	CONFIG_MODE_B(port);

	// enable access to TLR register
	set_port(port[OMAP_UART_EFR], OMAP_EFR_ENHANCED, OMAP_EFR_ENHANCED);

	// switch to Configuration Mode A
	CONFIG_MODE_A(port);

	// set MCR bit 6 to enable access to TCR and TLR registers
    set_port(port[OMAP_UART_MCR], OMAP_MCR_TCRTLR, OMAP_MCR_TCRTLR);

	// clear FIFO
	write_omap(port[OMAP_UART_FCR], OMAP_FCR_ENABLE|OMAP_FCR_RXCLR|OMAP_FCR_TXCLR);

   	// switch to Configuration Mode B
	CONFIG_MODE_B(port);

	write_omap(port[OMAP_UART_TCR], ctp->TCR); // restore TCR
	write_omap(port[OMAP_UART_TLR], ctp->TLR); // restore TLR

	write_omap(port[OMAP_UART_DLL], ctp->DLL); // restore DLL
	write_omap(port[OMAP_UART_DLH], ctp->DLH); // restore DLH
#ifdef OMAP5910
	write_omap(port[OMAP_UAR_OSC_12M_SEL], ctp->osc_12m_sel); // restore OSC_12M_SEL
#endif
	write_omap(port[OMAP_UART_MDR1], ctp->MDR1); // restore MDR1
	write_omap(port[OMAP_UART_EFR], ctp->EFR); // restore EFR

	// switch to Operational Mode
	OPERATIONAL_MODE(port);

	write_omap(port[OMAP_UART_SCR], ctp->SCR); // restore SCR
	write_omap(port[OMAP_UART_WER], ctp->WER); // restore WER
   	write_omap(port[OMAP_UART_IER], ctp->IER); // restore IER

	write_omap(port[OMAP_UART_MCR], ctp->MCR); // restore MCR
	write_omap(port[OMAP_UART_LCR], ctp->LCR); // restore LCR

	return 1;
}

int omap_uart_ctx_save(DEV_OMAP *dev)
{
	uintptr_t *port = dev->port;
	uart_ctx_t *ctp = (uart_ctx_t *) dev->uart_ctx;

	if (ctp == NULL || GET_UART_LOSTCTX(ctp))
		return 0;

	// wait until TX is empty
	while ((read_omap(port[OMAP_UART_LSR]) & OMAP_LSR_TX_E) != OMAP_LSR_TX_E);

	// clear FIFOs
	set_port(port[OMAP_UART_FCR], OMAP_FCR_RXCLR | OMAP_FCR_TXCLR, OMAP_FCR_RXCLR | OMAP_FCR_TXCLR);
	read_omap(port[OMAP_UART_LSR]); // clear Line Status Interrupt
	read_omap(port[OMAP_UART_MSR]);	// clear Modem Interrupt

	ctp->LCR = read_omap(port[OMAP_UART_LCR]); // save LCR

	// switch to Configuration Mode B
	CONFIG_MODE_B(port);

	ctp->EFR = read_omap(port[OMAP_UART_EFR]); // save EFR

	// enable access to TCR and TLR registers
	set_port(port[OMAP_UART_EFR], OMAP_EFR_ENHANCED, OMAP_EFR_ENHANCED);

	// switch to Configuration Mode A
	CONFIG_MODE_A(port);

   	ctp->MCR = read_omap(port[OMAP_UART_MCR]); // save MCR

	// enable access to TCR and TLR registers
    set_port(port[OMAP_UART_MCR], OMAP_MCR_TCRTLR, OMAP_MCR_TCRTLR);

	// switch to Configuration Mode B
	CONFIG_MODE_B(port);

	ctp->TCR = read_omap(port[OMAP_UART_TCR]); // save TCR
	ctp->TLR = read_omap(port[OMAP_UART_TLR]); // save TLR
   	ctp->DLL = read_omap(port[OMAP_UART_DLL]); // save DLL
   	ctp->DLH = read_omap(port[OMAP_UART_DLH]); // save DLH
#ifdef OMAP5910
	ctp->osc_12m_sel = read_omap(port[OMAP_UAR_OSC_12M_SEL]); // save OSC_12M_SEL
#endif
	ctp->MDR1 = read_omap(port[OMAP_UART_MDR1]); // save MDR1

	// restore EFR
	write_omap(port[OMAP_UART_EFR], ctp->EFR);

	// switch to Operational Mode
	OPERATIONAL_MODE(port);

	ctp->IER  = read_omap(port[OMAP_UART_IER]);  // save IER
	ctp->SYSC = read_omap(port[OMAP_UART_SYSC]); // save SYSC
	ctp->SCR  = read_omap(port[OMAP_UART_SCR]);  // save SCR
	ctp->WER  = read_omap(port[OMAP_UART_WER]);  // save WER

	// restore MCR
	write_omap(port[OMAP_UART_MCR], ctp->MCR);

	/* restore LCR */    
	write_omap(port[OMAP_UART_LCR], ctp->LCR);

	return 1;
}

int omap_uart_ctx_init(DEV_OMAP *dev)
{
    uart_ctx_t *ctp;
	uintptr_t uartctx_phys;

	if (OMAP_UART1_PHYSBASE == dev->physbase) {
		uartctx_phys = rm_l4per_uart1_context;
	} else if (OMAP_UART2_PHYSBASE == dev->physbase) {
		uartctx_phys = rm_l4per_uart2_context;
	} else if (OMAP_UART3_PHYSBASE == dev->physbase) {
		uartctx_phys = rm_l4per_uart3_context;
	} else if (OMAP_UART4_PHYSBASE == dev->physbase) {
		uartctx_phys = rm_l4per_uart4_context;
        } else if (OMAP5_UART5_PHYSBASE == dev->physbase) {
                uartctx_phys = RM_L4PER_UART5_CONTEXT_OMAP5;
        } else if (OMAP5_UART6_PHYSBASE == dev->physbase) {
                uartctx_phys = RM_L4PER_UART6_CONTEXT_OMAP5;
	} else {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR,
		      "%s: Unrecognized physical address, UART context not enabled", __FUNCTION__);
		return EINVAL;
	}

    // malloc UART_CTX
	ctp = malloc(sizeof(uart_ctx_t));
	if (ctp == NULL) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR,
		      "%s: malloc failed due to %s", __FUNCTION__, strerror(errno));
        return errno;
    }

	// map L4PER_PRM registers
	ctp->ctx_reg = mmap_device_io(4, uartctx_phys);
	if (ctp->ctx_reg == (uintptr_t)MAP_FAILED) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR,
		      "%s: mmap_device_io failed due to %s", __FUNCTION__, strerror(errno));
		free(ctp);
		return errno;
    }

	// clear context status
	CLR_UART_LOSTCTX(ctp);

	// done
	dev->uart_ctx = ctp;

	return EOK;
}

#endif /* PWR_MAN */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap/context.c $ $Rev: 736883 $")
#endif
