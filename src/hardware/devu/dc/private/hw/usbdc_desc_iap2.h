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
	Descriptor information for iAP2 Accessory Device
*/

#ifndef _USB_MSC_DESCRIPTORS_H_INCLUDED
#define _USB_MSC_DESCRIPTORS_H_INCLUDED

#include <sys/usb100_dcd.h>
#include <hw/usbdc_vid_did.h>
#include <hw/usbdc_desc_common.h>

#define USBDC_PROTOCOL_NAME "IAP2"

#ifndef USBDC_VENDOR_ID
	#define USBDC_VENDOR_ID USBDC_QNX_SOFTWARE_VENDOR_ID
#endif

#ifndef USBDC_PRODUCT_ID
	#define USBDC_PRODUCT_ID USBDC_QNX_SOFTWARE_PRODUCT_ID_TEST
#endif

#ifndef USBDC_REVISION
	#define USBDC_REVISION 		0x0200
#endif

#define USBDC_MAX_CURRENT 			(0)
#define USBDC_NUM_CONFIGURATIONS 		1

static const char usbdc_device_descriptor[] = {
	0x12,   							/* bLength              */
	0x01,   							/* bDescriptorType      */
	0x00,   							/* bcdUSB (LSB)         */
	0x02,   							/* bcdUSB (MSB)         */
	0x00,   							/* bDeviceClass         */
	0x00,   							/* bDeviceSubClass      */
	0x00,   							/* bDeviceProtocol      */
	0x40,   							/* bMaxPacketSize       */
	(USBDC_VENDOR_ID & 0xff),  			/* idVendor (LSB)       */
	(( USBDC_VENDOR_ID >> 8) & 0xff),   /* idVendor (MSB)       */
	(USBDC_PRODUCT_ID & 0xff),			/* idProduct (LSB)      */
	(( USBDC_PRODUCT_ID >> 8) & 0xff), 	/* idProduct (MSB)      */
	(USBDC_REVISION & 0xff),			/* bcdDevice (LSB)      */
	(( USBDC_REVISION >> 8) & 0xff),	/* bcdDevice (MSB)      */
	0x01,   							/* iManufacture         */
	0x02,   							/* iProduct             */
	0x03,   							/* iSerialNumber        */
	USBDC_NUM_CONFIGURATIONS  			/* bNumConfigurations   */
};

static const char usbdc_device_qualifier_descriptor[] = {
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

#define SIZE_CONFIG_DESC        (0x9 + 1 * 0x9 + 2 * 0x7)	/* config_len + total_interfaces * interface_len + total_endpoints * endpoint_len */

char usbdc_fs_config_descriptor_0[] = {
	0x09,                               /* bLength              */
	0x02,                               /* bDescriptorType      */
	(SIZE_CONFIG_DESC & 0xFF),          /* wTotalLength (LSB)   */
	(SIZE_CONFIG_DESC >> 8 & 0xFF),     /* wTotalLength (MSB)   */
	0x01,                               /* bNumInterfaces       */
	0x01,                               /* bConfigurationValue  */
	0x04,                               /* iConfiguration       */
	0x40,                               /* bmAttributes         */
	USBDC_MAX_CURRENT,                  /* max power            */

/* iAP2 Interface interface descriptor */
	0x09,                               /* bLength                      */
	0x04,                               /* bDescriptorType              */
	0x00,                               /* bInterfaceNumber             */
	0x00,                               /* bAlternateSetting            */
	0x02,                               /* bNumEndpoints                */
	0xFF,                               /* bInterfaceClass              */
	0xF0,                               /* bInterfaceSubClass           */
	0x00,                               /* nInterfaceProtocol           */
	0x05,                               /* iInterface                   */

/* Endpoint 1 Bulk In Descriptor */
	0x07,                               /* bLength                      */
	0x05,                               /* bDescriptorType              */
	0x81,                               /* bEndpointAddress             */
	0x02,                               /* bmAttributes                 */
	(USBDC_FULL_SPEED_MPS & 0xFF),      /* wMaxPacketSize (LSB)         */
	(USBDC_FULL_SPEED_MPS >> 8 & 0xFF), /* wMaxPacketSize (MSB)         */
	0x00,                               /* bInterval                    */

/* Endpoint 2 Bulk Out Descriptor */
	0x07,                               /* bLength                      */
	0x05,                               /* bDescriptorType              */
	0x02,                               /* bEndpointAddress             */
	0x02,                               /* bmAttributes                 */
	(USBDC_FULL_SPEED_MPS & 0xFF),      /* wMaxPacketSize (LSB)         */
	(USBDC_FULL_SPEED_MPS >> 8 & 0xFF), /* wMaxPacketSize (MSB)         */
	0x00                                /* bInterval                    */
};

char usbdc_hs_config_descriptor_0[] = {
	0x09,                               /* bLength              */
	0x02,                               /* bDescriptorType      */
	(SIZE_CONFIG_DESC & 0xFF),          /* wTotalLength (LSB)   */
	(SIZE_CONFIG_DESC >> 8 & 0xFF),     /* wTotalLength (MSB)   */
	0x01,                               /* bNumInterfaces       */
	0x01,                               /* bConfigurationValue  */
	0x04,                               /* iConfiguration       */
	0x40,                               /* bmAttributes         */
	USBDC_MAX_CURRENT,                  /* max power            */

/* iAP2 Interface interface descriptor */
	0x09,                               /* bLength                      */
	0x04,                               /* bDescriptorType              */
	0x00,                               /* bInterfaceNumber             */
	0x00,                               /* bAlternateSetting            */
	0x02,                               /* bNumEndpoints                */
	0xFF,                               /* bInterfaceClass              */
	0xF0,                               /* bInterfaceSubClass           */
	0x00,                               /* nInterfaceProtocol           */
	0x05,                               /* iInterface                   */

/* Endpoint 1 Bulk In Descriptor */
	0x07,                               /* bLength                      */
	0x05,                               /* bDescriptorType              */
	0x81,                               /* bEndpointAddress             */
	0x02,                               /* bmAttributes                 */
	(USBDC_HIGH_SPEED_MPS & 0xFF),      /* wMaxPacketSize (LSB)         */
	(USBDC_HIGH_SPEED_MPS >> 8 & 0xFF), /* wMaxPacketSize (MSB)         */
	0x00,                               /* bInterval                    */

/* Endpoint 2 Bulk Out Descriptor */
	0x07,                               /* bLength                      */
	0x05,                               /* bDescriptorType              */
	0x02,                               /* bEndpointAddress             */
	0x02,                               /* bmAttributes                 */
	(USBDC_HIGH_SPEED_MPS & 0xFF),      /* wMaxPacketSize (LSB)         */
	(USBDC_HIGH_SPEED_MPS >> 8 & 0xFF), /* wMaxPacketSize (MSB)         */
	0x00                                /* bInterval                    */
};

const char string_descriptors[] = {
/* Language Code Descriptor		*/
	0x04,                               /* bLength                      */
	0x03,                               /* bDescriptorType              */
	0x09,                               /* wLANGID (LSB)                */
	0x04,                               /* wLANGIF (MSB)                */

/* Manufacturer String Descriptor */
	42,                                 /* bLength                      */
	0x3,                                /* bDescriptorType              */
	'Q', '\0', 'N', '\0', 'X', '\0', ' ', '\0',
	'S', '\0', 'o', '\0', 'f', '\0', 't', '\0', 'w', '\0', 'a', '\0', 'r', '\0', 'e', '\0', ' ', '\0',
	'S', '\0', 'y', '\0', 's', '\0', 't', '\0', 'e', '\0', 'm', '\0', 's', '\0',

/* Product String Descriptor */
	30,                                 /* bLength                      */
	0x3,                                /* bDescriptorType              */
	'i', '\0', 'A', '\0', 'P', '\0', '2', '\0', ' ', '\0',
	'A', '\0', 'c', '\0', 'c', '\0', 'e', '\0', 's', '\0', 's', '\0', 'o', '\0', 'r', '\0', 'y', '\0',

/* Serial String Descriptor */
	30,                                 /* bLength                      */
	0x3,                                /* bDescriptorType              */
	'A', '\0', 'B', '\0', 'C', '\0', '-', '\0',
	'0', '\0', '1', '\0', '2', '\0', '3', '\0', '4', '\0', '5', '\0', '6', '\0', '7','\0',
	'8', '\0', '9', '\0',

/* Configuration String Descriptor */
	44,                                 /* bLength                      */
	0x03,                               /* bDescriptorType              */
	'D', '\0', 'e', '\0', 'f', '\0', 'a', '\0', 'u', '\0', 'l', '\0', 't', '\0', ' ', '\0',
	'C', '\0', 'o', '\0', 'n', '\0', 'f', '\0', 'i', '\0', 'g', '\0', 'u', '\0', 'r', '\0',
	'a', '\0', 't', '\0', 'i', '\0', 'o', '\0', 'n', '\0',

/* iAP Interface Descriptor */	
	28,                                 /* bLength                      */
	0x03,                               /* bDescriptorType              */
	'i', '\0', 'A', '\0', 'P', '\0', ' ', '\0', 'I', '\0', 'n', '\0', 't', '\0', 'e', '\0',
	'r', '\0', 'f', '\0', 'a', '\0', 'c', '\0', 'e','\0'
};

const char *usbdc_fs_config_descriptor[] = { usbdc_fs_config_descriptor_0 };
const char *usbdc_hs_config_descriptor[] = { usbdc_hs_config_descriptor_0 };
const char *usbdc_ss_config_descriptor[] = { usbdc_hs_config_descriptor_0 };

#define USBDC_MAX_STRING_DESCRIPTOR	 5

static const char *usbdc_string_descriptor[] = {
		&string_descriptors[0],
		&string_descriptors[4],
		&string_descriptors[4 + 42],
		&string_descriptors[4 + 42 + 30],
		&string_descriptors[4 + 42 + 30 + 30],
		&string_descriptors[4 + 42 + 30 + 30 + 44],
};

#define USBDC_FS_DEVICE_DESCRIPTOR     		usbdc_device_descriptor
#define USBDC_FS_CONFIG_DESCRIPTOR  		usbdc_fs_config_descriptor
#define USBDC_FS_STRING_DESCRIPTOR     		usbdc_string_descriptor
#define USBDC_FS_DEVICE_QUALIFIER_DESCRIPTOR 	usbdc_device_qualifier_descriptor

#define USBDC_HS_DEVICE_DESCRIPTOR     		usbdc_device_descriptor
#define USBDC_HS_CONFIG_DESCRIPTOR  		usbdc_hs_config_descriptor
#define USBDC_HS_STRING_DESCRIPTOR     		usbdc_string_descriptor
#define USBDC_HS_DEVICE_QUALIFIER_DESCRIPTOR 	usbdc_device_qualifier_descriptor

#define USBDC_SS_DEVICE_DESCRIPTOR     		usbdc_device_descriptor
#define USBDC_SS_CONFIG_DESCRIPTOR  		usbdc_ss_config_descriptor
#define USBDC_SS_STRING_DESCRIPTOR     		usbdc_string_descriptor
#define USBDC_SS_DEVICE_QUALIFIER_DESCRIPTOR 	usbdc_device_qualifier_descriptor

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/private/hw/usbdc_desc_iap2.h $ $Rev: 736722 $")
#endif
