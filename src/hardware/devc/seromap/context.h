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


#ifndef CONTEXT_H_
#define CONTEXT_H_

#define RM_L4PER_UART1_CONTEXT  0x4A307544
#define RM_L4PER_UART2_CONTEXT  0x4A30754C
#define RM_L4PER_UART3_CONTEXT  0x4A307554
#define RM_L4PER_UART4_CONTEXT  0x4A30755C

/******** OMAP5 UART Context registers have different addresses ********************/
#define RM_L4PER_UART1_CONTEXT_OMAP5 0x4AE07544
#define RM_L4PER_UART2_CONTEXT_OMAP5 0x4AE0754C
#define RM_L4PER_UART3_CONTEXT_OMAP5 0x4AE07554
#define RM_L4PER_UART4_CONTEXT_OMAP5 0x4AE0755C
#define RM_L4PER_UART5_CONTEXT_OMAP5 0x4AE07574 
#define RM_L4PER_UART6_CONTEXT_OMAP5 0x4AE0757C

#define OMAP_UART_LOSTCONTEXT_RFF       (0x1 << 1)
#define OMAP_UART_LOSTMEM_RETAINED_BANK (0x1 << 8)

uint32_t rm_l4per_uart1_context;
uint32_t rm_l4per_uart2_context;
uint32_t rm_l4per_uart3_context;
uint32_t rm_l4per_uart4_context;

typedef struct
{
    uint32_t DLL;
    uint32_t DLH;
    uint32_t IER;
    uint32_t LCR;
    uint32_t MCR;
    uint32_t SCR;
    uint32_t SYSC;
    uint32_t WER;
	uint32_t TCR;
	uint32_t TLR;
	uint32_t MDR1;
	uint32_t EFR;
    uint32_t ctx_reg;

} uart_ctx_t;


#endif /* CONTEXT_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap/context.h $ $Rev: 736735 $")
#endif
