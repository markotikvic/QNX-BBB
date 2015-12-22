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


#ifndef __MINI_DRIVER_H_INCLUDED
#define __MINI_DRIVER_H_INCLUDED

#define MINI_DATA_BUF_SIZE				(64*1024)
#define MINI_DATA_STARTUP_DEFAULT_RATE	(16)

#define MINI_TIMER_COUNTER_NASK			(0x0FFFFFFF)
#define MINI_TIMER_STATE_SHIFT			(28)

#define MINI_DATA_NAME					("mini-data")
#define MINI_TIMER_NAME					("mini-timer")
#define MINI_CAN_NAME					("mini-can")

typedef struct mini_data
{
    uint16_t    nstartup;
    uint16_t    nstartupp;
    uint16_t    nstartupf;
    uint16_t    nkernel;
    uint16_t    nprocess;
    uint16_t    data_len;
} mini_data_t;



typedef struct minitimer_data
{
    uintptr_t	canport;        /* Base register mapping */
    uintptr_t	canport_k;      /* Base register mapping */
    uint16_t	nstartup;       /* Stats for number of calls to MDRIVER_STARTUP */
    uint16_t	nstartupp;      /* Stats for number of calls to MDRIVER_STARTUP_PREPARE */
    uint16_t	nstartupf;      /* Stats for number of calls to MDRIVER_STARTUP_FINI */
    uint16_t	nkernel;        /* Stats for number of calls to MDRIVER_KERNEL */
    uint16_t	nprocess;       /* Stats for number of calls to MDRIVER_PROCESS */
	uint16_t	data_len;
} minitimer_data_t;


/* CAN Message Object Data Structure */
typedef struct can_msg_obj
{
    uint32_t        canmcf;                 /* Message Control Field */
    uint32_t        canmid;                 /* Message Identifier */
    uint32_t        canmdl;                 /* Message Data Low Word */
    uint32_t        canmdh;                 /* Message Data High Word */
} can_msg_obj_t;

/* Mini-driver Data in memory - this must exactly match the definition used by the mdriver in Startup */
typedef struct minican_data
{
    uintptr_t        canport;        /* CAN base register mapping */
    uintptr_t        canport_k;      /* CAN base register mapping */
    can_msg_obj_t    *canmem;        /* CAN base memory mapping */
    can_msg_obj_t    *canmem_k;      /* CAN base memory mapping */
    uint16_t         nstartup;       /* Stats for number of calls to MDRIVER_STARTUP */
    uint16_t         nstartupp;      /* Stats for number of calls to MDRIVER_STARTUP_PREPARE */
    uint16_t         nstartupf;      /* Stats for number of calls to MDRIVER_STARTUP_FINI */
    uint16_t         nkernel;        /* Stats for number of calls to MDRIVER_KERNEL */
    uint16_t         nprocess;       /* Stats for number of calls to MDRIVER_PROCESS */
    uint16_t         nrx;            /* Number of received messages */
    uint32_t         tx_enabled;     /* Flags to keep track of which mailboxes have a tx in progress */
    /* Buffered messages of type CAN_MSG_OBJ follow this data structure in memory */
}  minican_data_t;

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/hw/mini_driver.h $ $Rev: 744795 $")
#endif
