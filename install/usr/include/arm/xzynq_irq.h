/*
 * $QNXLicenseC:
 * Copyright 2007, QNX Software Systems.
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

#ifndef	__ARM_XZYNQ_IRQ_H_INCLUDED
#define	__ARM_XZYNQ_IRQ_H_INCLUDED

/* Interrupts */
#define XZYNQ_IRQ_GLOBAL_TIMER		27
#define XZYNQ_IRQ_XADC				39
#define XZYNQ_IRQ_TTC0_TIMER0		42
#define XZYNQ_IRQ_TTC0_TIMER1		43
#define XZYNQ_IRQ_TTC0_TIMER2		44
#define XZYNQ_IRQ_DMA_ABORT			45
#define XZYNQ_IRQ_DMA_CHAN0			46
#define XZYNQ_IRQ_DMA_CHAN1			47
#define XZYNQ_IRQ_DMA_CHAN2			48
#define XZYNQ_IRQ_DMA_CHAN3			49
#define XZYNQ_IRQ_GEM0				54
#define XZYNQ_IRQ_UART0				59
#define XZYNQ_IRQ_CAN0				60
#define XZYNQ_IRQ_TTC1_TIMER0		69
#define XZYNQ_IRQ_TTC1_TIMER1		70
#define XZYNQ_IRQ_TTC1_TIMER2		71
#define XZYNQ_IRQ_DMA_CHAN4			72
#define XZYNQ_IRQ_DMA_CHAN5			73
#define XZYNQ_IRQ_DMA_CHAN6			74
#define XZYNQ_IRQ_DMA_CHAN7			75
#define XZYNQ_IRQ_UART1				82
#define XZYNQ_IRQ_CAN1				83
#define XZYNQ_IRQ_I2C1				57
#define XZYNQ_IRQ_I2C2				80

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/arm/xzynq_irq.h $ $Rev: 752035 $")
#endif
