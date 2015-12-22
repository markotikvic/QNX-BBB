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
	Descriptor informtion for USB RNDIS device
*/

#ifndef _USB_RNDIS_DESCRIPTORS_H_INCLUDED
#define _USB_RNDIS_DESCRIPTORS_H_INCLUDED

#include <sys/usb100_dcd.h>
#include <hw/usbdc_vid_did.h>
#include <hw/usbdc_desc_common.h>

#define USBDC_PROTOCOL_NAME "RNDIS"

#ifndef USBDC_VENDOR_ID
	#define USBDC_VENDOR_ID USBDC_QNX_SOFTWARE_VENDOR_ID
#endif

#ifndef USBDC_PRODUCT_ID
	#define USBDC_PRODUCT_ID USBDC_QNX_SOFTWARE_PRODUCT_ID_RNDIS
#endif

#ifndef USBDC_REVISION
	#define USBDC_REVISION 		0x0100
#endif


#define DEVICE_DESCRIPTOR_LEN 18
#define USBDC_NUM_CONFIGURATIONS		1

char usbdc_device_descriptor[] = {
	0x12,   							/* bLength              */
	0x01,   							/* bDescriptorType      */
	0x00,   							/* bcdUSB (LSB)         */
	0x02,   							/* bcdUSB (MSB)         */
	0x02,   							/* bDeviceClass         */
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

#define SIZE_CONFIG_DESC		62
#define BULK_ENDPOINT_SIZE		64

char usbdc_fs_config_descriptor_0[] = {
	0x9,					/* bLength 				*/
	0x2,					/* bDescriptorType 		*/
	SIZE_CONFIG_DESC,		/* wLength 				*/
	0x0,
	0x2,					/* bNumInterfaces 		*/
	0x1,					/* bConfigurationValue 	*/
	0x0,					/* iConfiguration 		*/
	0xC0,					/* bmAttributes			*/
	0x00,					/* max power 			*/

// interface descriptor
	0x09,					/* bLength 						*/
	0x04,					/* bDescriptorType 				*/
							/* Interface descriptor type 	*/
	0x00,					/* bInterfaceNumber 			*/
	0x00,					/* bAlternateSetting 			*/
	0x01,					/* bNumEndpoints 				*/
	0x02,					/* bInterfaceClass 				*/
	0x02,					/* bInterfaceSubClass 			*/
	0xff,					/* nInterfaceProtocol 			*/
	0x00,					/* iInterface 					*/

// USB Communication Class Header Functional Descriptor
	0x05,
	0x24,
	0x00,
	0x01,
	0x10,

// USB Communication Class Call Management Functional Descriptor
	0x05,
	0x24,
	0x01,
	0x00,
	0x00,

// USB Communication Class Abstract Control Management Functional Descriptor
	0x04,
	0x24,
	0x02,
	0x00,

/* Endpoint 1 Interrupt In descriptor */
	0x07,					/* bLength 						*/
	0x05,					/* bDescriptorType 				*/
	0x81,					/* bEndpointAddress 			*/
	0x03,					/* bmAttributes 				*/
	0x08,					/* wMaxPacketSize 				*/
	0x00,
	0x01,					/* bInterval - changes with device speed */ 

// interface descriptor
	0x09,					/* bLength 						*/
	0x04,					/* bDescriptorType 				*/
							/* Interface descriptor type 	*/
	0x01,					/* bInterfaceNumber 			*/
	0x00,					/* bAlternateSetting 			*/
	0x02,					/* bNumEndpoints 				*/
	0x0a,					/* bInterfaceClass 				*/
	0x00,					/* bInterfaceSubClass 			*/
	0x00,					/* nInterfaceProtocol 			*/
	0x00,					/* iInterface 					*/

/*Endpoint 2 Bulk In Descriptor */
	0x07,					/* bLength 						*/
	0x05,					/* bDescriptorType 				*/
	0x82,					/* bEndpointAddress: 			*/
	0x02,					/* bmAttributes 				*/
	0x40,					/* wMaxPacketSize 				*/
	0x00,
	0x00,					/* bInterval 					*/

/*Endpoint 3 Bulk Out Descriptor */
	0x07,					/* bLength 						*/
	0x05,					/* bDescriptorType 				*/
	0x03,					/* bEndpointAddress: 			*/
	0x02,					/* bmAttributes 				*/
	0x40,					/* wMaxPacketSize 				*/
	0x00,
	0x00,					/* bInterval 					*/


};

char usbdc_hs_config_descriptor_0[] = {
	0x9,								/* bLength 				*/
	0x2,								/* bDescriptorType 		*/
	SIZE_CONFIG_DESC,					/* wLength 				*/
	0x0,
	0x2,								/* bNumInterfaces 		*/
	0x1,								/* bConfigurationValue 	*/
	0x0,								/* iConfiguration 		*/
	0xC0,								/* bmAttributes			*/
	0x00,								/* max power 			*/

// interface descriptor
	0x09,								/* bLength 						*/
	0x04,								/* bDescriptorType 				*/
										/* Interface descriptor type 	*/
	0x00,								/* bInterfaceNumber 			*/
	0x00,								/* bAlternateSetting 			*/
	0x01,								/* bNumEndpoints 				*/
	0x02,								/* bInterfaceClass 				*/
	0x02,								/* bInterfaceSubClass 			*/
	0xff,								/* nInterfaceProtocol 			*/
	0x00,								/* iInterface 					*/

// USB Communication Class Header Functional Descriptor
	0x05,
	0x24,
	0x00,
	0x01,
	0x10,

// USB Communication Class Call Management Functional Descriptor
	0x05,
	0x24,
	0x01,
	0x00,
	0x00,

// USB Communication Class Abstract Control Management Functional Descriptor
	0x04,
	0x24,
	0x02,
	0x00,

/* Endpoint 1 Interrupt In descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
	0x81,								/* bEndpointAddress 			*/
	0x03,								/* bmAttributes 				*/
	0x08,								/* wMaxPacketSize 				*/
	0x00,
	0x04,								/* bInterval - changes with device speed */ 

// interface descriptor
	0x09,								/* bLength 						*/
	0x04,								/* bDescriptorType 				*/
										/* Interface descriptor type 	*/
	0x01,								/* bInterfaceNumber 			*/
	0x00,								/* bAlternateSetting 			*/
	0x02,								/* bNumEndpoints 				*/
	0x0a,								/* bInterfaceClass 				*/
	0x00,								/* bInterfaceSubClass 			*/
	0x00,								/* nInterfaceProtocol 			*/
	0x00,								/* iInterface 					*/

/*Endpoint 2 Bulk In Descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
	0x82,								/* bEndpointAddress: 			*/
	0x02,								/* bmAttributes 				*/
	(USBDC_HIGH_SPEED_MPS & 0xff),		/* wMaxPacketSize (2 bytes)		*/
	(USBDC_HIGH_SPEED_MPS >> 8 & 0xff),
	0x00,								/* bInterval 					*/

/*Endpoint 3 Bulk Out Descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
	0x03,								/* bEndpointAddress: 			*/
	0x02,								/* bmAttributes 				*/
	(USBDC_HIGH_SPEED_MPS & 0xff),		/* wMaxPacketSize (2 bytes)		*/
	(USBDC_HIGH_SPEED_MPS >> 8 & 0xff),
	0x00,								/* bInterval 					*/


};

/* SuperSpeed */

char usbdc_ss_device_descriptor[] = {
	0x12,   							/* bLength              */
	0x01,   							/* bDescriptorType      */
	0x00,   							/* bcdUSB (LSB)         */
	0x03,   							/* bcdUSB (MSB)         */
	0x02,   							/* bDeviceClass         */
	0x00,   							/* bDeviceSubClass      */
	0x00,   							/* bDeviceProtocol      */
	0x09,   							/* bMaxPacketSize       */
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

#define SIZE_SS_CONFIG_DESC		80

char usbdc_ss_config_descriptor_0[] = {
	0x9,								/* bLength 				*/
	0x2,								/* bDescriptorType 		*/
	SIZE_SS_CONFIG_DESC,				/* wLength 				*/
	0x0,
	0x2,								/* bNumInterfaces 		*/
	0x1,								/* bConfigurationValue 	*/
	0x0,								/* iConfiguration 		*/
	0xC0,								/* bmAttributes			*/
	0x00,								/* max power 			*/

// interface descriptor
	0x09,								/* bLength 						*/
	0x04,								/* bDescriptorType 				*/
										/* Interface descriptor type 	*/
	0x00,								/* bInterfaceNumber 			*/
	0x00,								/* bAlternateSetting 			*/
	0x01,								/* bNumEndpoints 				*/
	0x02,								/* bInterfaceClass 				*/
	0x02,								/* bInterfaceSubClass 			*/
	0xff,								/* nInterfaceProtocol 			*/
	0x00,								/* iInterface 					*/

// USB Communication Class Header Functional Descriptor
	0x05,
	0x24,
	0x00,
	0x01,
	0x10,

// USB Communication Class Call Management Functional Descriptor
	0x05,
	0x24,
	0x01,
	0x00,
	0x00,

// USB Communication Class Abstract Control Management Functional Descriptor
	0x04,
	0x24,
	0x02,
	0x00,

/* Endpoint 1 Interrupt In descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
	0x81,								/* bEndpointAddress 			*/
	0x03,								/* bmAttributes 				*/
	0x08,								/* wMaxPacketSize 				*/
	0x00,
	0x04,								/* bInterval - changes with device speed */ 

/* SuperSpeed Companion Descriptor */
	0x06,                   			/* bLength                      */
	0x30,                   			/* bDescriptorType              */
	0x00,                               /* bMaxBurst                    */
	0x00,                               /* bmAttributes                 */
	0x08,                               /* wBytesPerInterval            */
	0x00,                               /* wBytesPerInterval            */
	
	
// interface descriptor
	0x09,								/* bLength 						*/
	0x04,								/* bDescriptorType 				*/
										/* Interface descriptor type 	*/
	0x01,								/* bInterfaceNumber 			*/
	0x00,								/* bAlternateSetting 			*/
	0x02,								/* bNumEndpoints 				*/
	0x0a,								/* bInterfaceClass 				*/
	0x00,								/* bInterfaceSubClass 			*/
	0x00,								/* nInterfaceProtocol 			*/
	0x00,								/* iInterface 					*/

/*Endpoint 2 Bulk In Descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
	0x82,								/* bEndpointAddress: 			*/
	0x02,								/* bmAttributes 				*/
	(USBDC_SUPER_SPEED_MPS & 0xff),		/* wMaxPacketSize (2 bytes)		*/
	(USBDC_SUPER_SPEED_MPS >> 8 & 0xff),
	0x00,								/* bInterval 					*/

/* SuperSpeed Companion Descriptor */
	0x06,                   			/* bLength                      */
	0x30,                   			/* bDescriptorType              */
	0x0f,                               /* bMaxBurst                    */
	0x00,                               /* bmAttributes                 */
	0x00,                               /* wBytesPerInterval            */
	0x00,                               /* wBytesPerInterval            */
	
	
/*Endpoint 3 Bulk Out Descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
	0x03,								/* bEndpointAddress: 			*/
	0x02,								/* bmAttributes 				*/
	(USBDC_SUPER_SPEED_MPS & 0xff),		/* wMaxPacketSize (2 bytes)		*/
	(USBDC_SUPER_SPEED_MPS >> 8 & 0xff),
	0x00,								/* bInterval 					*/

/* SuperSpeed Companion Descriptor */
	0x06,                   			/* bLength                      */
	0x30,                   			/* bDescriptorType              */
	0x0f,                               /* bMaxBurst                    */
	0x00,                               /* bmAttributes                 */
	0x00,                               /* wBytesPerInterval            */
	0x00 	                            /* wBytesPerInterval            */

};

const char string_descriptors[] = {
		0x04,
		0x03,
		0x09,
		0x04,
		34,
		0x03,
		'U',0, 'S',0, 'B',0, ' ',0, 'R',0, 'N',0, 'D',0, 'I',0,  // 32 bytes
		'S',0, ' ',0, 'D',0, 'e',0, 'v',0, 'i',0, 'c',0, 'e',0,
		56,
		0x03,
		'R', 0, 'N', 0, 'D', 0, 'I', 0, 'S', 0, ' ', 0, 'C', 0, 'o', 0,	// 54 bytes
		'm', 0, 'm', 0, 'u', 0, 'n', 0, 'i', 0, 'c', 0, 'a', 0, 't', 0,
		'i', 0, 'o', 0, 'n', 0, 's', 0, ' ', 0, 'D', 0, 'e', 0, 'v', 0,
		'i', 0, 'c', 0, 'e',0,
		22,
		0x03,
		'1',0, '9',0, '/',0, '0',0, '7',0, '/',0, '2',0, '0',0, 	// 20 bytes
		'0',0, '0',0
};

const char *usbdc_fs_config_descriptor[] = { usbdc_fs_config_descriptor_0 };
const char *usbdc_hs_config_descriptor[] = { usbdc_hs_config_descriptor_0 };
const char *usbdc_ss_config_descriptor[] = { usbdc_ss_config_descriptor_0 };

#define USBDC_MAX_STRING_DESCRIPTOR	 3
const char *usbdc_string_descriptor[] = { &string_descriptors[0],
										  &string_descriptors[4],
										  &string_descriptors[38],
										  &string_descriptors[94]
										};

#define USBDC_FS_DEVICE_DESCRIPTOR 				usbdc_device_descriptor
#define USBDC_FS_CONFIG_DESCRIPTOR 				usbdc_fs_config_descriptor
#define USBDC_FS_DEVICE_QUALIFIER_DESCRIPTOR 	usbdc_device_qualifier_descriptor
#define USBDC_FS_STRING_DESCRIPTOR				usbdc_string_descriptor

#define USBDC_HS_DEVICE_DESCRIPTOR 				usbdc_device_descriptor
#define USBDC_HS_CONFIG_DESCRIPTOR  			usbdc_hs_config_descriptor
#define USBDC_HS_DEVICE_QUALIFIER_DESCRIPTOR 	usbdc_device_qualifier_descriptor
#define USBDC_HS_STRING_DESCRIPTOR				usbdc_string_descriptor

#define USBDC_SS_DEVICE_DESCRIPTOR     			usbdc_ss_device_descriptor
#define USBDC_SS_CONFIG_DESCRIPTOR  			usbdc_ss_config_descriptor	
#define USBDC_SS_STRING_DESCRIPTOR     			usbdc_string_descriptor
#define USBDC_SS_DEVICE_QUALIFIER_DESCRIPTOR 	usbdc_device_qualifier_descriptor

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/private/hw/usbdc_desc_rndis.h $ $Rev: 736722 $")
#endif
