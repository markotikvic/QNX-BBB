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
	Blank descriptor information
*/

#ifndef _USB_MSC_DESCRIPTORS_H_INCLUDED
#define _USB_MSC_DESCRIPTORS_H_INCLUDED

#include <hw/usbdc_desc_common.h>

#define USBDC_PROTOCOL_NAME "BLANK"
#define USBDC_NUM_CONFIGURATIONS 1

static const char usbdc_device_descriptor[] = {
	0x12,           /* bLength              */
	0x01,           /* bDescriptorType      */
	0x00,           /* bcdUSB (LSB)         */
	0x02,           /* bcdUSB (MSB)         */
	0x00,           /* bDeviceClass         */
	0x00,           /* bDeviceSubClass      */
	0x00,           /* bDeviceProtocol      */
	0x40,           /* bMaxPacketSize       */
	0x00,           /* idVendor (LSB)       */
	0x00,           /* idVendor (MSB)       */
	0x00,           /* idProduct (LSB)      */
	0x00,           /* idProduct (MSB)      */
	0x00,           /* bcdDevice (LSB)      */
	0x00,           /* bcdDevice (MSB)      */
	0x00,           /* iManufacturer        */
	0x00,           /* iProduct             */
	0x00,           /* iSerialNumber        */
	0x01,           /* bNumConfigurations   */
};

static const char usbdc_device_qualifier_descriptor[] = {
	0x0a,           /* bLength              */
	0x06,           /* bDescriptorType      */
	0x00,           /* bcdUSB (LSB)         */
	0x02,           /* bcdUSB (MSB)         */
	0x00,           /* bDeviceClass         */
	0x00,           /* bDeviceSubClass      */
	0x00,           /* bDeviceProtocol      */
	0x40,           /* bMaxPacketSize       */
	0x01,           /* bNumConfigurations   */
	0x00            /* bReserved            */
};

char usbdc_fs_config_descriptor_0[] = {
	0x9,            /* bLength              */
	0x2,            /* bDescriptorType      */
	0x9,            /* wLength              */
	0x0,
	0x0,            /* bNumInterfaces       */
	0x1,            /* bConfigurationValue  */
	0x0,            /* iConfiguration       */
	0xC0,           /* bmAttributes         */
	0x00,           /* max power            */
};

char usbdc_hs_config_descriptor_0[] = {
	0x9,            /* bLength              */
	0x2,            /* bDescriptorType      */
	0x9,            /* wLength              */
	0x0,
	0x0,            /* bNumInterfaces       */
	0x1,            /* bConfigurationValue  */
	0x0,            /* iConfiguration       */
	0xC0,           /* bmAttributes         */
	0x00,           /* max power            */
};

/* SuperSpeed */

static const char usbdc_ss_device_descriptor[] = {
	0x12,           /* bLength              */
	0x01,           /* bDescriptorType      */
	0x00,           /* bcdUSB (LSB)         */
	0x03,           /* bcdUSB (MSB)         */
	0x00,           /* bDeviceClass         */
	0x00,           /* bDeviceSubClass      */
	0x00,           /* bDeviceProtocol      */
	0x09,           /* bMaxPacketSize  ss: 9  is 512 bytes    */
	0x00,           /* idVendor (LSB)       */
	0x00,           /* idVendor (MSB)       */
	0x00,           /* idProduct (LSB)      */
	0x00,           /* idProduct (MSB)      */
	0x00,           /* bcdDevice (LSB)      */
	0x00,           /* bcdDevice (MSB)      */
	0x00,           /* iManufacturer        */
	0x00,           /* iProduct             */
	0x00,           /* iSerialNumber        */
	0x01,           /* bNumConfigurations   */
};

char usbdc_ss_config_descriptor_0[] = {
	0x9,            /* bLength              */
	0x2,            /* bDescriptorType      */
	0x9,            /* wLength              */
	0x0,
	0x0,            /* bNumInterfaces       */
	0x1,            /* bConfigurationValue  */
	0x0,            /* iConfiguration       */
	0xC0,           /* bmAttributes         */
	0x00,           /* max power            */
};

static const char string_descriptors[] = {
	0x04,
	0x03,
	0x09,
	0x04,
};

const char *usbdc_fs_config_descriptor[] = { usbdc_fs_config_descriptor_0 };
const char *usbdc_hs_config_descriptor[] = { usbdc_hs_config_descriptor_0 };
const char *usbdc_ss_config_descriptor[] = { usbdc_ss_config_descriptor_0 };

#define USBDC_MAX_STRING_DESCRIPTOR     0

static const char *usbdc_string_descriptor[] = {
		&string_descriptors[0],
	};

#define USBDC_FS_DEVICE_DESCRIPTOR             usbdc_device_descriptor
#define USBDC_FS_CONFIG_DESCRIPTOR             usbdc_fs_config_descriptor
#define USBDC_FS_STRING_DESCRIPTOR             usbdc_string_descriptor
#define USBDC_FS_DEVICE_QUALIFIER_DESCRIPTOR   usbdc_device_qualifier_descriptor

#define USBDC_HS_DEVICE_DESCRIPTOR             usbdc_device_descriptor
#define USBDC_HS_CONFIG_DESCRIPTOR             usbdc_hs_config_descriptor
#define USBDC_HS_STRING_DESCRIPTOR             usbdc_string_descriptor
#define USBDC_HS_DEVICE_QUALIFIER_DESCRIPTOR   usbdc_device_qualifier_descriptor

#define USBDC_SS_DEVICE_DESCRIPTOR             usbdc_ss_device_descriptor
#define USBDC_SS_CONFIG_DESCRIPTOR             usbdc_ss_config_descriptor
#define USBDC_SS_STRING_DESCRIPTOR             usbdc_string_descriptor
#define USBDC_SS_DEVICE_QUALIFIER_DESCRIPTOR   usbdc_device_qualifier_descriptor

#endif /* #ifndef _USB_MSC_DESCRIPTORS_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
