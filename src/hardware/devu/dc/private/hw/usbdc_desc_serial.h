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
	Descriptor information for USB CDC serial device
 */

#ifndef _USB_CDC_SER_DESCRIPTORS_H_INCLUDED
#define _USB_CDC_SER_DESCRIPTORS_H_INCLUDED

#include <sys/usb100_dcd.h>
#include <hw/usbdc_vid_did.h>
#include <hw/usbdc_desc_common.h>

#define USBDC_PROTOCOL_NAME "SERIAL"

#ifndef USBDC_VENDOR_ID
	#define USBDC_VENDOR_ID USBDC_QNX_SOFTWARE_VENDOR_ID
#endif

#ifndef USBDC_PRODUCT_ID
	#define USBDC_PRODUCT_ID USBDC_QNX_SOFTWARE_PRODUCT_ID_CDC_ACM
#endif

#ifndef USBDC_REVISION
	#define USBDC_REVISION 		0x0100
#endif

#define USBDC_NUM_CONFIGURATIONS		1

#define USBDC_FULL_SPEED_MPS		64
#define USBDC_HIGH_SPEED_MPS		512

/* XP SP2 doesn't support composite( need hotfix or SP3 )*/
char usbdc_device_descriptor[] = {
	0x12,   							/* bLength              */
	0x01,   							/* bDescriptorType      */
	0x00,   							/* bcdUSB (LSB)         */
	0x02,   							/* bcdUSB (MSB)         */

#ifndef USBDC_SERIAL_COMPOSITE_DEVICE
	0x02,   							/* bDeviceClass         */
	0x02,   							/* bDeviceSubClass      */
	0x00,   							/* bDeviceProtocol      */
#else
	0x00,   							/* bDeviceClass         */
	0x00,   							/* bDeviceSubClass      */
	0x00,   							/* bDeviceProtocol      */
#endif

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
	USBDC_NUM_CONFIGURATIONS 			/* bNumConfigurations   */
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

char usbdc_fs_config_descriptor_0[] = {
/* Configuration Descriptor */
	0x9,					/* bLength 				*/
	0x2,					/* bDescriptorType 		*/
	0x4b,					/* wTotalLength (LSB)	*/	/* Total = Config_len + (Total_interfaces * Interface_len) + (Total_endpoints * Endpoint_len) */
	0x00,					/* wTotalLength (MSB)   */
	0x02,					/* bNumInterfaces 		*/
	0x01,					/* bConfigurationValue 	*/
	0x04,					/* iConfiguration 		*/
	0xC0,					/* bmAttributes			*/
	0x00,					/* max power 			*/

/* Application Port Interface Association Descriptor */
	0x08,					/* bLength 						*/
	0x0B,					/* bDescriptorType 				*/
	0x00,					/* bFirstInterface				*/
	0x02,					/* bInterfaceCount				*/
	0x02,					/* bInterfaceClass 				*/
	0x02,					/* bInterfaceSubClass 			*/

    0x00, 					/* bInterfaceProtocol 			*/
	0x05,					/* iFunction 				    */

/* Application Port Communication Class Interface Descriptor */
	0x09,					/* bLength 						*/
	0x04,					/* bDescriptorType 				*/
	0x00,					/* bInterfaceNumber 			*/
	0x00,					/* bAlternateSetting 			*/
	0x01,					/* bNumEndpoints 				*/
	0x02,					/* bInterfaceClass 				*/
	0x02,					/* bInterfaceSubClass 			*/
    
	0x00,  					/* nInterfaceProtocol 			*/
	0x06,					/* iInterface 					*/

/* Header Functional Descriptor */
	0x05,					/* bFunctinLength				*/
	0x24,					/* bDescriptorType				*/
	0x00,					/* bDescriptorSubType			*/
	0x10,					/* bcdCDC	(LSB)				*/
	0x01,					/* bcdCDC	(MSB)				*/

/* Call Managment Functional Descriptor */
	0x05,					/* bFunctinLength               */
	0x24,					/* bDescriptorType              */
	0x01,					/* bDescriptorSubType	        */
	0x00,					/* bmCapabilities - ENOSUP		*/
	0x01,					/* bDataInterface				*/

/* Abstract Control Model Function Descriptor */
	0x04,					/* bFunctinLength               */
	0x24,					/* bDescriptorType              */
	0x02,					/* bDescriptorSubType           */
	0x02,					/* bmCapabilities 				*/

/* Abstract Control Model  Union Descriptor */
	0x05,					/* bFunctinLength               */
	0x24,					/* bDescriptorType              */
	0x06,					/* bDescriptorSubType           */
	0x00,					/* bControlInterface            */
	0x01,					/* bSubordinate interface		*/

/* Application Port Notification Endpoint Descriptor */
	0x07,					/* bLength 						*/
	0x05,					/* bDescriptorType 				*/
	0x81,					/* bEndpointAddress 			*/
	0x03,					/* bmAttributes 				*/
	0x08,					/* wMaxPacketSize (LSB)			*/
	0x00,					/* wMaxPacketSize (MSB)			*/
	0x08,					/* bInterval 					*/
    
/* Application Port Data Class Interface Descriptor */
	0x09,					/* bLength 						*/
	0x04,					/* bDescriptorType 				*/
	0x01,					/* bInterfaceNumber 			*/
	0x00,					/* bAlternateSetting 			*/
	0x02,					/* bNumEndpoints 				*/
	0x0A,					/* bInterfaceClass 				*/
	0x00,					/* bInterfaceSubClass 			*/
	0x00,					/* nInterfaceProtocol 			*/
	0x07,					/* iInterface 					*/

/* Application Port Data Out Endpoint Descriptor */
	0x07,					/* bLength 						*/
	0x05,					/* bDescriptorType 				*/
	0x02,					/* bEndpointAddress: 			*/
	0x02,					/* bmAttributes 				*/
	USBDC_FULL_SPEED_MPS,	/* wMaxPacketSize (LSB)			*/
	0x00,					/* wMaxPacketSize (MSB)			*/
	0x00,					/* bInterval 					*/

/* Applicatin Port Data In Endpoint Descriptor */
	0x07,					/* bLength 						*/
	0x05,					/* bDescriptorType 				*/
	0x83,					/* bEndpointAddress: 			*/
	0x02,					/* bmAttributes 				*/
	USBDC_FULL_SPEED_MPS,	/* wMaxPacketSize (LSB)			*/
	0x00,					/* wMaxPacketSize (MSB)			*/
	0x00,					/* bInterval 					*/

};

char usbdc_hs_config_descriptor_0[] = {
/* Configuration Descriptor */
	0x9,								/* bLength 				*/
	0x2,								/* bDescriptorType 		*/
	0x4b,								/* wTotalLength (LSB)	*/	/* Total = Config_len + (Total_interfaces * Interface_len) + (Total_endpoints * Endpoint_len) */
	0x00,								/* wTotalLength (MSB)   */
	0x02,								/* bNumInterfaces 		*/
	0x01,								/* bConfigurationValue 	*/
	0x04,								/* iConfiguration 		*/
	0xC0,								/* bmAttributes			*/
	0x00,								/* max power 			*/

/* Application Port Interface Association Descriptor */
	0x08,								/* bLength 						*/
	0x0B,								/* bDescriptorType 				*/
	0x00,								/* bFirstInterface				*/
	0x02,								/* bInterfaceCount				*/
	0x02,								/* bInterfaceClass 				*/
	0x02,								/* bInterfaceSubClass 			*/

    0x00, 								/* bInterfaceProtocol 			*/
	0x05,								/* iFunction 				    */

/* Application Port Communication Class Interface Descriptor */
	0x09,								/* bLength 						*/
	0x04,								/* bDescriptorType 				*/
	0x00,								/* bInterfaceNumber 			*/
	0x00,								/* bAlternateSetting 			*/
	0x01,								/* bNumEndpoints 				*/
	0x02,								/* bInterfaceClass 				*/
	0x02,								/* bInterfaceSubClass 			*/
    
	0x00,  								/* nInterfaceProtocol 			*/
	0x06,								/* iInterface 					*/

/* Header Functional Descriptor */
	0x05,								/* bFunctinLength				*/
	0x24,								/* bDescriptorType				*/
	0x00,								/* bDescriptorSubType			*/
	0x10,								/* bcdCDC	(LSB)				*/
	0x01,								/* bcdCDC	(MSB)				*/

/* Call Managment Functional Descriptor */
	0x05,								/* bFunctinLength               */
	0x24,								/* bDescriptorType              */
	0x01,								/* bDescriptorSubType	        */
	0x00,								/* bmCapabilities - ENOSUP		*/
	0x01,								/* bDataInterface				*/

/* Abstract Control Model Function Descriptor */
	0x04,								/* bFunctinLength               */
	0x24,								/* bDescriptorType              */
	0x02,								/* bDescriptorSubType           */
	0x02,								/* bmCapabilities 				*/

/* Abstract Control Model Union Descriptor */
	0x05,								/* bFunctinLength               */
	0x24,								/* bDescriptorType              */
	0x06,								/* bDescriptorSubType           */
	0x00,								/* bControlInterface            */
	0x01,								/* bSubordinate interface		*/

/* Application Port Notification Endpoint Descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
	0x81,								/* bEndpointAddress 			*/
	0x03,								/* bmAttributes 				*/
	0x08,								/* wMaxPacketSize (LSB)			*/
	0x00,								/* wMaxPacketSize (MSB)			*/
	0x08,								/* bInterval 					*/
    
/* Application Port Data Class Interface Descriptor */
	0x09,								/* bLength 						*/
	0x04,								/* bDescriptorType 				*/
	0x01,								/* bInterfaceNumber 			*/
	0x00,								/* bAlternateSetting 			*/
	0x02,								/* bNumEndpoints 				*/
	0x0A,								/* bInterfaceClass 				*/
	0x00,								/* bInterfaceSubClass 			*/
	0x00,								/* nInterfaceProtocol 			*/
	0x07,								/* iInterface 					*/

/* Application Port Data Out Endpoint Descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
	0x02,								/* bEndpointAddress: 			*/
	0x02,								/* bmAttributes 				*/
	(USBDC_HIGH_SPEED_MPS & 0xff),		/* wMaxPacketSize (2 bytes)		*/
	(USBDC_HIGH_SPEED_MPS >> 8 & 0xff),
	0x00,								/* bInterval 					*/

/* Applicatin Port Data In Endpoint Descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
	0x83,								/* bEndpointAddress: 			*/
	0x02,								/* bmAttributes 				*/
	(USBDC_HIGH_SPEED_MPS & 0xff),		/* wMaxPacketSize (2 bytes)		*/
	(USBDC_HIGH_SPEED_MPS >> 8 & 0xff),
	0x00,								/* bInterval 					*/

};

// SuperSpeed


/* XP SP2 doesn't support composite( need hotfix or SP3 )*/
char usbdc_ss_device_descriptor[] = {
	0x12,   							/* bLength              */
	0x01,   							/* bDescriptorType      */
	0x00,   							/* bcdUSB (LSB)         */
	0x03,   							/* bcdUSB (MSB)         */

#ifndef USBDC_SERIAL_COMPOSITE_DEVICE
	0x02,   							/* bDeviceClass         */
	0x02,   							/* bDeviceSubClass      */
	0xff,   							/* bDeviceProtocol      */
#else
	0x00,   							/* bDeviceClass         */
	0x00,   							/* bDeviceSubClass      */
	0x00,   							/* bDeviceProtocol      */
#endif

	0x09,   							/* bMaxPacketSize       */
	(USBDC_VENDOR_ID & 0xff),  			/* idVendor (LSB)       */
	(( USBDC_VENDOR_ID >> 8) & 0xff),   /* idVendor (MSB)       */
	(USBDC_PRODUCT_ID & 0xff),			/* idProduct (LSB)      */
	(( USBDC_PRODUCT_ID >> 8) & 0xff), 	/* idProduct (MSB)      */
	(USBDC_REVISION & 0xff),			/* bcdDevice (LSB)      */
	(( USBDC_REVISION >> 8) & 0xff),	/* bcdDevice (MSB)      */
	0x01,   							/* iManufacture         */
	0x02,   							/* iProduct             */
	0x03,   							/* iSerialNumber        */
	USBDC_NUM_CONFIGURATIONS 			/* bNumConfigurations   */
};


char usbdc_ss_config_descriptor_0[] = {
/* Configuration Descriptor */
	0x9,								/* bLength 				*/
	0x2,								/* bDescriptorType 		*/
	0x5d,								/* wTotalLength (LSB)	*/	/* Total = Config_len + (Total_interfaces * Interface_len) + (Total_endpoints * Endpoint_len) */
	0x00,								/* wTotalLength (MSB)   */
	0x02,								/* bNumInterfaces 		*/
	0x01,								/* bConfigurationValue 	*/
	0x04,								/* iConfiguration 		*/
	0xC0,								/* bmAttributes			*/
	0x00,								/* max power 			*/

/* Application Port Interface Association Descriptor */
	0x08,								/* bLength 						*/
	0x0B,								/* bDescriptorType 				*/
	0x00,								/* bFirstInterface				*/
	0x02,								/* bInterfaceCount				*/
	0x02,								/* bInterfaceClass 				*/
	0x02,								/* bInterfaceSubClass 			*/

    0x00, 								/* bInterfaceProtocol 			*/
	0x05,								/* iFunction 				    */

/* Application Port Communication Class Interface Descriptor */
	0x09,								/* bLength 						*/
	0x04,								/* bDescriptorType 				*/
	0x00,								/* bInterfaceNumber 			*/
	0x00,								/* bAlternateSetting 			*/
	0x01,								/* bNumEndpoints 				*/
	0x02,								/* bInterfaceClass 				*/
	0x02,								/* bInterfaceSubClass 			*/
    
	0x00,  								/* nInterfaceProtocol 			*/
	0x06,								/* iInterface 					*/

/* Header Functional Descriptor */
	0x05,								/* bFunctinLength				*/
	0x24,								/* bDescriptorType				*/
	0x00,								/* bDescriptorSubType			*/
	0x10,								/* bcdCDC	(LSB)				*/
	0x01,								/* bcdCDC	(MSB)				*/

/* Call Managment Functional Descriptor */
	0x05,								/* bFunctinLength               */
	0x24,								/* bDescriptorType              */
	0x01,								/* bDescriptorSubType	        */
	0x00,								/* bmCapabilities - ENOSUP		*/
	0x01,								/* bDataInterface				*/

/* Abstract Control Model Function Descriptor */
	0x04,								/* bFunctinLength               */
	0x24,								/* bDescriptorType              */
	0x02,								/* bDescriptorSubType           */
	0x02,								/* bmCapabilities 				*/

/* Abstract Control Model Union Descriptor */
	0x05,								/* bFunctinLength               */
	0x24,								/* bDescriptorType              */
	0x06,								/* bDescriptorSubType           */
	0x00,								/* bControlInterface            */
	0x01,								/* bSubordinate interface		*/

/* Application Port Notification Endpoint Descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
	0x81,								/* bEndpointAddress 			*/
	0x03,								/* bmAttributes 				*/
	0x08,								/* wMaxPacketSize (LSB)			*/
	0x00,								/* wMaxPacketSize (MSB)			*/
	0x08,								/* bInterval 					*/
   
/* SuperSpeed Companion Descriptor */
	0x06,                   			/* bLength                      */
	0x30,                   			/* bDescriptorType              */
	0x00,                               /* bMaxBurst                    */
	0x00,                               /* bmAttributes                 */
	0x08,                               /* wBytesPerInterval            */
	0x00,                               /* wBytesPerInterval            */
	
	
/* Application Port Data Class Interface Descriptor */
	0x09,								/* bLength 						*/
	0x04,								/* bDescriptorType 				*/
	0x01,								/* bInterfaceNumber 			*/
	0x00,								/* bAlternateSetting 			*/
	0x02,								/* bNumEndpoints 				*/
	0x0A,								/* bInterfaceClass 				*/
	0x00,								/* bInterfaceSubClass 			*/
	0x00,								/* nInterfaceProtocol 			*/
	0x07,								/* iInterface 					*/

/* Application Port Data Out Endpoint Descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
	0x02,								/* bEndpointAddress: 			*/
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
	0x00, 	                            /* wBytesPerInterval            */
	
	
/* Applicatin Port Data In Endpoint Descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
	0x83,								/* bEndpointAddress: 			*/
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
	0x00 ,	                            /* wBytesPerInterval            */
	
	
};


const char string_descriptors[] = {
/* Language Code Descriptor	*/
	4,		/* bLength			*/
	0x03,	/* bDescriptorType	*/
	0x09,	/* wLANGID (LSB)	*/
	0x04,	/* wLANGIF (MSB)	*/

/*  Manufacturer String Descriptor */
	34,		/*  bLength			*/
	0x03,	/* bDescriptorType  */
	'A', '\0', 'c', '\0', 'm', '\0', 'e', '\0', ' ', '\0',
	'C', '\0', 'o', '\0', 'r', '\0', 'p', '\0', 'o', '\0', 'r', '\0', 'a', '\0', 't', '\0', 
	'i', '\0', 'o', '\0', 'n', '\0',

/* Product String Descriptor */
	44,		/* bLength			*/
	0x03,	/* bDescriptorType  */
	'C', '\0', 'D', '\0', 'C', '\0', ' ', '\0', 
	'S', '\0', 'e', '\0',	'r', '\0', 'i', '\0', 'a', '\0', 'l', '\0', ' ', '\0', 
	'P', '\0', 'e', '\0', 'r', '\0', 'i', '\0', 'p', '\0', 'h', '\0', 'e', '\0', 'r', '\0', 'a', '\0', 'l', '\0', 

/* Serial Number String Descriptor */
	30,		/* bLength          */
	0x03,	/* bDescriptorType  */
	'x', '\0', 'x', '\0', 'x', '\0', 'x', '\0', '-', '\0', 'x', '\0', 'x', '\0', 'x', '\0', 'x', '\0', 
	'-', '\0', 'x', '\0', 'x', '\0', 'x', '\0', 'x', '\0',

/* Configuration String Descriptor */
	44,		/* bLength          */
	0x03,	/* bDescriptorType  */
	'D', '\0', 'e', '\0', 'f', '\0', 'a', '\0', 'u', '\0', 'l', '\0', 't', '\0', ' ', '\0', 
	'C', '\0', 'o', '\0', 'n', '\0', 'f', '\0', 'i', '\0', 'g', '\0', 'u', '\0', 'r', '\0', 
	'a', '\0', 't', '\0', 'i', '\0', 'o', '\0', 'n', '\0',

/* Serial Port IAD String Descriptor */
	44,		/* bLength          */
	0x03,	/* bDescriptorType  */
	'S', '\0', 'e', '\0', 'r', '\0', 'i', '\0', 'a', '\0', 'l', '\0', ' ', '\0',
	'P', '\0', 'o', '\0', 'r', '\0', 't', '\0', ' ', '\0',
	'I', '\0', 'n', '\0', 't', '\0', 'e', '\0', 'r', '\0', 'f', '\0', 'a', '\0', 'c', '\0', 'e', '\0',

/* Serial Port Communication Class Interface String Descriptor */
	84,		/* bLength          */
	0x03,	/* bDescriptorType  */
	'S', '\0', 'e', '\0', 'r', '\0', 'i', '\0', 'a', '\0', 'l', '\0', ' ', '\0',
	'P', '\0', 'o', '\0', 'r', '\0', 't', '\0', ' ', '\0',
	'C', '\0', 'o', '\0', 'm', '\0', 'm', '\0', 'u', '\0', 'n', '\0', 'i', '\0', 
	'c', '\0', 'a', '\0', 't', '\0', 'i', '\0', 'o', '\0', 'n', '\0', ' ', '\0',
	'C', '\0', 'l', '\0', 'a', '\0', 's', '\0', 's', '\0', ' ', '\0',
	'I', '\0', 'n', '\0', 't', '\0', 'e', '\0', 'r', '\0', 'f', '\0', 'a', '\0', 'c', '\0', 'e', '\0',

/* Serial Port Data Class Interface String Descriptor */
	68,		/* bLength          */
	0x03,	/* bDescriptorType  */
	'S', '\0', 'e', '\0', 'r', '\0', 'i', '\0', 'a', '\0', 'l', '\0', ' ', '\0',
	'P', '\0', 'o', '\0', 'r', '\0', 't', '\0', ' ', '\0',
	'D', '\0', 'a', '\0', 't', '\0', 'a', '\0', ' ', '\0',
	'C', '\0', 'l', '\0', 'a', '\0', 's', '\0', 's', '\0', ' ', '\0',
	'I', '\0', 'n', '\0', 't', '\0', 'e', '\0', 'r', '\0', 'f', '\0', 'a', '\0', 'c', '\0', 'e', '\0', ' ', '\0',

};

const char *usbdc_fs_config_descriptor[] = { usbdc_fs_config_descriptor_0 };
const char *usbdc_hs_config_descriptor[] = { usbdc_hs_config_descriptor_0 };
const char *usbdc_ss_config_descriptor[] = { usbdc_ss_config_descriptor_0 };

#define USBDC_MAX_STRING_DESCRIPTOR 	7

const char *usbdc_string_descriptor[] = { 
		&string_descriptors[0],
		&string_descriptors[4],
		&string_descriptors[4 + 34],
		&string_descriptors[4 + 34 + 44],
		&string_descriptors[4 + 34 + 44 + 30],
		&string_descriptors[4 + 34 + 44 + 30 + 44],
		&string_descriptors[4 + 34 + 44 + 30 + 44 + 44],
		&string_descriptors[4 + 34 + 44 + 30 + 44 + 44 + 84],
	};

#define USBDC_FS_DEVICE_DESCRIPTOR 				usbdc_device_descriptor
#define USBDC_FS_CONFIG_DESCRIPTOR 				usbdc_fs_config_descriptor
#define USBDC_FS_STRING_DESCRIPTOR				usbdc_string_descriptor
#define USBDC_FS_DEVICE_QUALIFIER_DESCRIPTOR 	usbdc_device_qualifier_descriptor

#define USBDC_HS_DEVICE_DESCRIPTOR 				usbdc_device_descriptor
#define USBDC_HS_CONFIG_DESCRIPTOR 				usbdc_hs_config_descriptor
#define USBDC_HS_STRING_DESCRIPTOR				usbdc_string_descriptor
#define USBDC_HS_DEVICE_QUALIFIER_DESCRIPTOR 	usbdc_device_qualifier_descriptor

#define USBDC_SS_DEVICE_DESCRIPTOR     			usbdc_ss_device_descriptor
#define USBDC_SS_CONFIG_DESCRIPTOR  			usbdc_ss_config_descriptor	
#define USBDC_SS_STRING_DESCRIPTOR     			usbdc_string_descriptor
#define USBDC_SS_DEVICE_QUALIFIER_DESCRIPTOR 	usbdc_device_qualifier_descriptor


#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/private/hw/usbdc_desc_serial.h $ $Rev: 736722 $")
#endif
