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
	Descriptor information for USB Printer device.
*/

#ifndef _USBDC_PRINTER_DESCRIPTORS_H_INCLUDED
#define _USBDC_PRINTER_DESCRIPTORS_H_INCLUDED

#include <sys/usb100_dcd.h>
#include <hw/usbdc_vid_did.h>
#include <hw/usbdc_desc_common.h>

#define USBDC_PROTOCOL_NAME "PRINTER"

#ifndef USBDC_VENDOR_ID
	#define USBDC_VENDOR_ID USBDC_QNX_SOFTWARE_VENDOR_ID
#endif

#ifndef USBDC_PRODUCT_ID
	#define USBDC_PRODUCT_ID USBDC_QNX_SOFTWARE_PRODUCT_ID_PRINTER
#endif

#ifndef USBDC_REVISION
	#define USBDC_REVISION 		0x0100
#endif

#define USBDC_NUM_CONFIGURATIONS		1

/* From Printer class spec */
#define USB_PRN_SUBCLASS							1
#define USB_PRN_INTERFACE_PROTOCOL_UNI		0x01
#define USB_PRN_INTERFACE_PROTOCOL_BI		0x02
#define USB_PRN_INTERFACE_PROTOCOL_1284_4	0x03
#define USB_PRN_INTERFACE_PROTOCOL_VENDOR	0xff

#ifndef USBDC_PRINTER_PROTOCOL
	#define USBDC_PRINTER_PROTOCOL	USB_PRN_INTERFACE_PROTOCOL_UNI
#endif

char usbdc_device_descriptor[] = {
	DEVICE_DESCRIPTOR_LEN,   			/* bLength              */
	0x01,   							/* bDescriptorType      */
	0x00,   							/* bcdUSB (LSB)         */
	0x02,   							/* bcdUSB (MSB)         */
	0x00,			  					/* bDeviceClass         */
	0x00, 			 					/* bDeviceSubClass      */
	0x00, 								/* bDeviceProtocol      */
	USBDC_EP0_MPS,   					/* bMaxPacketSize       */
	(USBDC_VENDOR_ID & 0xff),  			/* idVendor (LSB)       */
	(( USBDC_VENDOR_ID >> 8) & 0xff),   /* idVendor (MSB)       */
	USBDC_PRODUCT_ID,					/* idProduct (LSB)      */
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

#if (USBDC_PRINTER_PROTOCOL != USB_PRN_INTERFACE_PROTOCOL_UNI)
	#define SIZE_CONFIG_DESC		32
	#define USBDC_NUM_ENDPOINTS		2
#else
	#define SIZE_CONFIG_DESC		25
	#define USBDC_NUM_ENDPOINTS		1
#endif

char usbdc_fs_config_descriptor_0[] = {
	0x9,							/* bLength 				*/
	0x2,							/* bDescriptorType 		*/
	SIZE_CONFIG_DESC,				/* wLength 				*/
	0x0,
	0x1,							/* bNumInterfaces 		*/
	0x1,							/* bConfigurationValue 	*/
	0x0,							/* iConfiguration 		*/
	0xC0,							/* bmAttributes			*/
	0x00,							/* max power 			*/

// interface descriptor
	0x09,							/* bLength 						*/
	0x04,							/* bDescriptorType 				*/
									/* Interface descriptor type 	*/
	0x00,							/* bInterfaceNumber 			*/
	0x00,							/* bAlternateSetting 			*/
	USBDC_NUM_ENDPOINTS,			/* bNumEndpoints 				*/
	USB_CLASS_PRN,					/* bInterfaceClass 				*/
	USB_PRN_SUBCLASS,				/* bInterfaceSubClass 			*/
	USBDC_PRINTER_PROTOCOL,			/* nInterfaceProtocol 			*/
	0x00,							/* iInterface 					*/

/* Endpoint 1 descriptor */
	0x07,							/* bLength 						*/
	0x05,							/* bDescriptorType 				*/
    USBDC_EPOUT1_NUM,				/* bEndpointAddress 			*/
	0x02,							/* bmAttributes 				*/
	USBDC_EP1_MPS,	        		/* wMaxPacketSize 				*/
	0x00,
	0x00,							/* bInterval 					*/


#if (USBDC_PRINTER_PROTOCOL != USB_PRN_INTERFACE_PROTOCOL_UNI)
/*Endpoint 2 Descriptor */
	0x07,							/* bLength 						*/
	0x05,							/* bDescriptorType 				*/
	USBDC_EPIN1_NUM,				/* bEndpointAddress: 			*/
	0x02,							/* bmAttributes 				*/
	USBDC_EP2_MPS,	        		/* wMaxPacketSize 				*/
	0x00,
	0x00,							/* bInterval 					*/
#endif
};

char usbdc_hs_config_descriptor_0[] = {
	0x9,								/* bLength 				*/
	0x2,								/* bDescriptorType 		*/
	SIZE_CONFIG_DESC,					/* wLength 				*/
	0x0,
	0x1,								/* bNumInterfaces 		*/
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
	USBDC_NUM_ENDPOINTS,				/* bNumEndpoints 				*/
	USB_CLASS_PRN,						/* bInterfaceClass 				*/
	USB_PRN_SUBCLASS,					/* bInterfaceSubClass 			*/
	USBDC_PRINTER_PROTOCOL,				/* nInterfaceProtocol 			*/
	0x00,								/* iInterface 					*/

/* Endpoint 1 descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
    USBDC_EPOUT1_NUM,					/* bEndpointAddress 			*/
	0x02,								/* bmAttributes 				*/
	(USBDC_HIGH_SPEED_MPS & 0xff),		/* wMaxPacketSize (2 bytes)		*/
	(USBDC_HIGH_SPEED_MPS >> 8 & 0xff),
	0x00,								/* bInterval 					*/

#if (USBDC_PRINTER_PROTOCOL != USB_PRN_INTERFACE_PROTOCOL_UNI)
/*Endpoint 2 Descriptor */
	0x07,								/* bLength 						*/
	0x05,								/* bDescriptorType 				*/
	USBDC_EPIN2_NUM,					/* bEndpointAddress: 			*/
	0x02,								/* bmAttributes 				*/
	(USBDC_HIGH_SPEED_MPS & 0xff),		/* wMaxPacketSize (2 bytes)		*/
	(USBDC_HIGH_SPEED_MPS >> 8 & 0xff),
	0x00,								/* bInterval 					*/
#endif




};

const char string_descriptors[] = {
		0x04,
		0x03,
		0x09,
		0x04,
		26,
		0x03,
		'Q',0, 'N',0, 'X',0, ' ',0, 'S',0, 'o',0, 'f',0, 't',0,  // 24 bytes
		'w',0, 'a',0, 'r',0, 'e',0,
		16,
		0x03,
		'P',0, 'r',0, 'i',0, 'n',0, 't',0, 'e',0, 'r',0, 	// 14 bytes
		22,
		0x03,
		'1',0, '9',0, '/',0, '0',0, '7',0, '/',0, '2',0, '0',0, // 20 bytes
		'0',0, '0',0
};



const char *usbdc_fs_config_descriptor[] = { usbdc_fs_config_descriptor_0 };
const char *usbdc_hs_config_descriptor[] = { usbdc_hs_config_descriptor_0 };


#define USBDC_MAX_STRING_DESCRIPTOR	 3
const char *usbdc_string_descriptor[] = { &string_descriptors[0],		// offset into string table
										  &string_descriptors[4],
										  &string_descriptors[30],
										  &string_descriptors[46]
										};

#define USBDC_FS_DEVICE_DESCRIPTOR 				usbdc_device_descriptor
#define USBDC_FS_CONFIG_DESCRIPTOR 				usbdc_fs_config_descriptor
#define USBDC_FS_STRING_DESCRIPTOR				usbdc_string_descriptor
#define USBDC_FS_DEVICE_QUALIFIER_DESCRIPTOR 	usbdc_device_qualifier_descriptor

#define USBDC_HS_DEVICE_DESCRIPTOR 				usbdc_device_descriptor
#define USBDC_HS_CONFIG_DESCRIPTOR 				usbdc_hs_config_descriptor
#define USBDC_HS_STRING_DESCRIPTOR				usbdc_string_descriptor
#define USBDC_HS_DEVICE_QUALIFIER_DESCRIPTOR 	usbdc_device_qualifier_descriptor

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/private/hw/usbdc_desc_printer.h $ $Rev: 736722 $")
#endif
