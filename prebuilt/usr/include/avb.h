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

#ifndef AVB_H
#define AVB_H

#include <stdint.h>

#define GET_TXQ_TAG(m) m_tag_find(m, PACKET_TAG_TXQ, NULL)

#define EXTRACT_TXQ_TAG(mtag) (*(uint8_t*)(mtag+1))

#define ETHERTYPE_1722	0x22F0

#define AVB_IOCTL SIOCGDRVSPEC

#define AVB_SET_BW		0x200

/* set bandwidth */
typedef struct {
    uint32_t	bandwidth[8];	/* Per-priority bw in kbits/s */
} avb_bw_t;

#define PRECISE_TIMER_DELAY	0x300

/* Precise timer delay */
typedef struct {
    uint32_t	ns;		/* Nanoseconds to delay for */
} precise_timer_delay_t;

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/io-pkt/sys/lsm/avb/public/avb.h $ $Rev: 736723 $")
#endif
