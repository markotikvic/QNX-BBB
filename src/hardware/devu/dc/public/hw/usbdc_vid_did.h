/*
 * $QNXLicenseC:  
 * Copyright 2009, QNX Software Systems.
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
 *  hw/usbdc_vid_did.h
 *
 */

#ifndef __USBDC_VID_DID_H_INCLUDED
#define __USBDC_VID_DID_H_INCLUDED

#define USBDC_QNX_SOFTWARE_VENDOR_ID		0x1234

	#define USBDC_QNX_SOFTWARE_PRODUCT_ID_MSC		0x0001
	#define USBDC_QNX_SOFTWARE_PRODUCT_ID_CDC_ECM	0x0002
	#define USBDC_QNX_SOFTWARE_PRODUCT_ID_CDC_EEM	0x0003
	#define USBDC_QNX_SOFTWARE_PRODUCT_ID_CDC_ACM	0x0004
	#define USBDC_QNX_SOFTWARE_PRODUCT_ID_RNDIS		0x0005
	#define USBDC_QNX_SOFTWARE_PRODUCT_ID_HID		0x0006
	#define USBDC_QNX_SOFTWARE_PRODUCT_ID_AUDIO		0x0007
	#define USBDC_QNX_SOFTWARE_PRODUCT_ID_PRINTER	0x0008
	#define USBDC_QNX_SOFTWARE_PRODUCT_ID_CDC_OBEX	0x0009
	#define USBDC_QNX_SOFTWARE_PRODUCT_ID_TEST		0xFFF1

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/public/hw/usbdc_vid_did.h $ $Rev: 762989 $")
#endif
