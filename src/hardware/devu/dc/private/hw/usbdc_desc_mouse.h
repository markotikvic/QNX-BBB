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

/* 
	Descriptor informtion for USB HID class mouse device
*/

#ifndef _USBDC_HIDMOUSE_DESCRIPTORS_H_INCLUDED
#define _USBDC_HIDMOUSE_DESCRIPTORS_H_INCLUDED

#include <sys/usb100_dcd.h>
#include <hw/usbdc_vid_did.h>

#define USBDC_PROTOCOL_NAME "HIDMOUSE"

#ifndef USBDC_VENDOR_ID
	#define USBDC_VENDOR_ID USBDC_QNX_SOFTWARE_VENDOR_ID
#endif

#ifndef USBDC_PRODUCT_ID
	#define USBDC_PRODUCT_ID USBDC_QNX_SOFTWARE_PRODUCT_ID_HID
#endif

#ifndef USBDC_REVISION
	#define USBDC_REVISION 		0x0100
#endif

#define USBDC_FULL_SPEED_MPS		64
#define USBDC_HIGH_SPEED_MPS		512

#define DEVICE_DESCRIPTOR_LEN 18

#define USBDC_NUM_CONFIGURATIONS 		1

char usbdc_device_descriptor[] = {
	0x12,   							/* bLength              */
	0x01,   							/* bDescriptorType      */
	0x00,   							/* bcdUSB (LSB)         */
	0x01,   							/* bcdUSB (MSB)         */
	0x00,   							/* bDeviceClass         */
	0x00,   							/* bDeviceSubClass      */
	0x00,   							/* bDeviceProtocol      */
	0x40,   							/* bMaxPacketSize       */
	(USBDC_VENDOR_ID & 0xff),  			/* idVendor (LSB)       */
	(( USBDC_VENDOR_ID >> 8) & 0xff),   /* idVendor (MSB)       */
	USBDC_PRODUCT_ID,					/* idProduct (LSB)      */
	(( USBDC_PRODUCT_ID >> 8) & 0xff), 	/* idProduct (MSB)      */
	(USBDC_REVISION & 0xff),			/* bcdDevice (LSB)      */
	(( USBDC_REVISION >> 8) & 0xff),	/* bcdDevice (MSB)      */
	0x01,   							/* iManufacture         */
	0x02,   							/* iProduct             */
	0x03,   							/* iSerialNumber        */
	USBDC_NUM_CONFIGURATIONS  			/* bNumConfigurations   */
};

char usbdc_device_qualifier_descriptor[] = {
	0x0a,   /* bLength              */
	0x06,   /* bDescriptorType      */
	0x00,   /* bcdUSB (LSB)         */
	0x02,   /* bcdUSB (MSB)         */
	0x00,   /* bDeviceClass         */
	0x00,   /* bDeviceSubClass      */
	0x00,   /* bDeviceProtocol      */
	0x40,   /* bMaxPacketSize       */
	0x01,   /* bNumConfigurations   */
	0x00    /* bReserved            */
};

#define BULK_ENDPOINT_SIZE		64

char usbdc_fs_config_descriptor_0[] = {
/* Configuration Descriptor */
	0x9,					/* bLength 				*/
	0x2,					/* bDescriptorType 		*/
	0x22,					/* wTotalLength (LSB)	*/	/* Total = Config_len + (Total_interfaces * Interface_len) + (Total_endpoints * Endpoint_len) */
	0x00,					/* wTotalLength (MSB)   */
	0x01,					/* bNumInterfaces 		*/
	0x01,					/* bConfigurationValue 	*/
	0x00,					/* iConfiguration 		*/
	0xC0,					/* bmAttributes			*/
	0x01,					/* max power 			*/

// interface descriptor
	0x09,					/* bLength 						*/
	0x04,					/* bDescriptorType 				*/
							/* Interface descriptor type 	*/
	0x00,					/* bInterfaceNumber 			*/
	0x00,					/* bAlternateSetting 			*/
	0x01,					/* bNumEndpoints 				*/
	0x03,					/* bInterfaceClass 				*/
	0x01,					/* bInterfaceSubClass 			*/
	0x02,					/* nInterfaceProtocol 			*/
	0x00,					/* iInterface 					*/
							/*  HID descriptor 				*/

/* HID descriptor */
	0x09,
	0x21,
	0x00,
	0x01,
	0x00,
	0x01,
	0x22,
	0x42,
	0x00,

	/* Endpoint 1 Interrupt In descriptor */
		0x07,					/* bLength 						*/
		0x05,					/* bDescriptorType 				*/
		0x81,					/* bEndpointAddress 			*/
		0x03,					/* bmAttributes 				*/
		0x08,					/* wMaxPacketSize 				*/
		0x00,
		0x0a,					/* bInterval - changes with device speed */ 
};

const char string_descriptors[] = {
/* Language Code Descriptor	*/
	4,		/* bLength			*/
	0x03,	/* bDescriptorType	*/
	0x09,	/* wLANGID (LSB)	*/
	0x04,	/* wLANGIF (MSB)	*/

/*  Manufacturer String Descriptor */
	38,		/*  bLength			*/
	0x03,	/* bDescriptorType  */
	'Q', 0, 'N', 0, 'X', 0, ' ', 0, 'S', 0, 'o', 0, 'f', 0, 														// 14 bytes
	't', 0, 'w', 0, 'a', 0, 'r', 0, 'e', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0,							// 22 bytes

/* Product String Descriptor */
	58,		/* bLength			*/
	0x03,	/* bDescriptorType  */
	'E', 0, 'x', 0, 'a', 0, 'm', 0, 'p', 0, 'l', 0, 'e', 0,															// 14 bytes
	'O', 0, 'n', 0,	'l', 0, 'y', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0,											// 18 bytes
	' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0,																	// 12 bytes
	' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0,																	// 12 bytes

/* Serial Number String Descriptor */
	30,		/* bLength          */
	0x03,	/* bDescriptorType  */
	'x', 0, 'x', 0, 'x', 0, 'x', 0, '-', 0, 'x', 0, 'x', 0, 'x', 0, 'x', 0, '-', 0, 'x', 0, 'x', 0, 'x', 0, 'x', 0,	// 28 bytes
};

const char *usbdc_fs_config_descriptor[] = { usbdc_fs_config_descriptor_0 };

const char *usbdc_string_descriptor[] = { &string_descriptors[0],
										  &string_descriptors[4],
										  &string_descriptors[42],
										  &string_descriptors[100],
										};

#define USBDC_MAX_STRING_DESCRIPTOR 3	

#define USBDC_FS_DEVICE_DESCRIPTOR 				usbdc_device_descriptor
#define USBDC_FS_CONFIG_DESCRIPTOR 				usbdc_fs_config_descriptor
#define USBDC_FS_STRING_DESCRIPTOR				usbdc_string_descriptor
#define USBDC_FS_DEVICE_QUALIFIER_DESCRIPTOR 	usbdc_device_qualifier_descriptor

#define USBDC_HS_DEVICE_DESCRIPTOR 				usbdc_device_descriptor
#define USBDC_HS_CONFIG_DESCRIPTOR 				usbdc_fs_config_descriptor
#define USBDC_HS_STRING_DESCRIPTOR				usbdc_string_descriptor
#define USBDC_HS_DEVICE_QUALIFIER_DESCRIPTOR 	usbdc_device_qualifier_descriptor

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/private/hw/usbdc_desc_mouse.h $ $Rev: 736722 $")
#endif
