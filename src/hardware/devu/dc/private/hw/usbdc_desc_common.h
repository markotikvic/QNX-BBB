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

#ifndef _USB_DESCCRIPTORS_COMMON_H_INCLUDED
#define _USB_DESCCRIPTORS_COMMON_H_INCLUDED

#define DEVICE_DESCRIPTOR_LEN       18
#define USBDC_FULL_SPEED_MPS		64
#define USBDC_HIGH_SPEED_MPS		512
#define USBDC_SUPER_SPEED_MPS		1024

#ifndef USBDC_EP0_MPS
    #define USBDC_EP0_MPS           USBDC_FULL_SPEED_MPS
#endif

#ifndef USBDC_EP1_MPS
    #define USBDC_EP1_MPS           USBDC_FULL_SPEED_MPS
#endif
#ifndef USBDC_EPOUT1_NUM
    #define USBDC_EPOUT1_NUM        0x1
#endif
#ifndef USBDC_EPIN1_NUM
    #define USBDC_EPIN1_NUM         0x81
#endif

#ifndef USBDC_EP2_MPS
    #define USBDC_EP2_MPS           USBDC_FULL_SPEED_MPS
#endif
#ifndef USBDC_EPOUT2_NUM
    #define USBDC_EPOUT2_NUM        0x2
#endif
#ifndef USBDC_EPIN2_NUM
    #define USBDC_EPIN2_NUM         0x82
#endif

#ifndef USBDC_EP3_MPS
    #define USBDC_EP3_MPS           USBDC_FULL_SPEED_MPS
#endif
#ifndef USBDC_EPOUT3_NUM
    #define USBDC_EPOUT3_NUM        0x3
#endif
#ifndef USBDC_EPIN3_NUM
    #define USBDC_EPIN3_NUM         0x83
#endif

#ifndef USBDC_EP4_MPS
    #define USBDC_EP4_MPS           USBDC_FULL_SPEED_MPS
#endif
#ifndef USBDC_EPOUT4_NUM
    #define USBDC_EPOUT4_NUM        0x4
#endif
#ifndef USBDC_EPIN4_NUM
    #define USBDC_EPIN4_NUM         0x84
#endif

#ifndef USBDC_EP5_MPS
    #define USBDC_EP5_MPS           USBDC_FULL_SPEED_MPS
#endif
#ifndef USBDC_EPOUT5_NUM
    #define USBDC_EPOUT5_NUM        0x5
#endif
#ifndef USBDC_EPIN5_NUM
    #define USBDC_EPIN5_NUM         0x85
#endif

#ifndef USBDC_EP6_MPS
    #define USBDC_EP6_MPS           USBDC_FULL_SPEED_MPS
#endif
#ifndef USBDC_EPOUT6_NUM
    #define USBDC_EPOUT6_NUM        0x6
#endif
#ifndef USBDC_EPIN6_NUM
    #define USBDC_EPIN6_NUM         0x86
#endif

#ifndef USBDC_EP7_MPS
    #define USBDC_EP7_MPS           USBDC_FULL_SPEED_MPS
#endif
#ifndef USBDC_EPOUT7_NUM
    #define USBDC_EPOUT7_NUM        0x7
#endif
#ifndef USBDC_EPIN7_NUM
    #define USBDC_EPIN7_NUM         0x87
#endif


/* BOS descriptor is needed for for usb 2.1 and above... Add common definition here for all drivers here... override in descriptor.h as necessary */

const char usbdc_bos_descriptor[] = {
	0x5,								/* bLength 				*/
	0xf,								/* bDescriptorType 		*/
	0x16,								/* wLength 				*/
	0x0,
	0x2,								/* bNumDeviceCaps		*/
	
	/* USB 2.0 Extension */
	0x7,								/* bLength 				*/
	0x10,								/* bDescriptorType 		*/
	0x2,								/* bDeviceCapabilityType */
	0x2,								/* bmAttributes */
	0x0,
	0x0,
	0x0,
	
	/* SuperSpeed USB Device Capability */
	0xa,								/* bLength 				*/
	0x10,								/* bDescriptorType 		*/
	0x3,								/* bDeviceCapabilityType */
	0x0,								/* bmAttributes */
	0xf,								/* wSpeedsSupported */
	0x0,
	0x1,								/* bFunctionalitySupport */

	/* These defaults were measured on the dwc usb3 controller running on msm8974-tigris...
	 * Measurements were taken with a usb3 analyzer running the chapter9 u1&u2 tests
	 * need to make this configurable on a per platform basis via a commandline option to the driver
         */
	0x4,								/* bU1DevExitLat			*/
	0x57,								/* wU2DevExitLat			*/
	0x00,
};

#ifndef USBDC_BOS_DESCRIPTOR
#define USBDC_BOS_DESCRIPTOR usbdc_bos_descriptor
#endif

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/private/hw/usbdc_desc_common.h $ $Rev: 736722 $")
#endif

