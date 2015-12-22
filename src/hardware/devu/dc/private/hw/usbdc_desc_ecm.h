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
    Descriptor information for USB ECM device
*/

#ifndef _USB_ECM_DESCRIPTORS_H_INCLUDED
#define _USB_ECM_DESCRIPTORS_H_INCLUDED


#include <sys/usb100_dcd.h>
#include <hw/usbdc_vid_did.h>
#include <hw/usbdc_desc_common.h>

#define USBDC_PROTOCOL_NAME         "CDCECM"

#ifndef USBDC_VENDOR_ID
    #define USBDC_VENDOR_ID         USBDC_QNX_SOFTWARE_VENDOR_ID
#endif

#ifndef USBDC_PRODUCT_ID
    #define USBDC_PRODUCT_ID        USBDC_QNX_SOFTWARE_PRODUCT_ID_TEST
#endif

#ifndef USBDC_REVISION
    #define USBDC_REVISION          0x0001
#endif

#define  USBDC_MAX_CURRENT          0

// Segment Length
//#define  ECM_SEG_LEN              8100
#define  ECM_SEG_LEN                1514

#define DEVICE_DESCRIPTOR_LEN       18
#define USBDC_NUM_CONFIGURATIONS    1

char usbdc_device_descriptor[] = {
    0x12,                               /* bLength              */
    0x01,                               /* bDescriptorType      */
    0x00,                               /* bcdUSB (LSB)         */
    0x02,                               /* bcdUSB (MSB)         */
    0x02,                               /* bDeviceClass         */
    0x06,                               /* bDeviceSubClass      */
    0x00,                               /* bDeviceProtocol      */
    0x40,                               /* bMaxPacketSize       */
    (USBDC_VENDOR_ID & 0xff),           /* idVendor (LSB)       */
    (( USBDC_VENDOR_ID >> 8) & 0xff),   /* idVendor (MSB)       */
    (USBDC_PRODUCT_ID & 0xff),          /* idProduct (LSB)      */
    (( USBDC_PRODUCT_ID >> 8) & 0xff),  /* idProduct (MSB)      */
    (USBDC_REVISION & 0xff),            /* bcdDevice (LSB)      */
    (( USBDC_REVISION >> 8) & 0xff),    /* bcdDevice (MSB)      */
    0x01,                               /* iManufacture         */
    0x02,                               /* iProduct             */
    0x03,                               /* iSerialNumber        */
    USBDC_NUM_CONFIGURATIONS            /* bNumConfigurations   */
};

char usbdc_device_qualifier_descriptor[] = {
    0x0a,   /* bLength              */
    0x06,   /* bDescriptorType      */
    0x00,   /* bcdUSB (LSB)         */
    0x02,   /* bcdUSB (MSB)         */
    0x02,   /* bDeviceClass         */
    0x06,   /* bDeviceSubClass      */
    0x00,   /* bDeviceProtocol      */
    0x40,   /* bMaxPacketSize       */
    0x01,   /* bNumConfigurations   */
    0x00    /* bReserved            */
};

#define INCLUDE_IAD         0

#if INCLUDE_IAD
    #define SIZE_IAD        8
#else
    #define SIZE_IAD        0
#endif

#define SIZE_CONFIG_DESC    ( 80 + SIZE_IAD )

char usbdc_fs_config_descriptor_0[] = {
    0x9,                    /* bLength              */
    0x2,                    /* bDescriptorType      */
    SIZE_CONFIG_DESC,       /* wLength              */
    0x0,
    0x2,                    /* bNumInterfaces       */
    0x1,                    /* bConfigurationValue  */
    0x04,                   /* iConfiguration       */
    0x40,                   /* bmAttributes         */
    USBDC_MAX_CURRENT,      /* max power            */

#if INCLUDE_IAD
/* Interface Association Descriptor */
    0x08,                   /* bLength                      */
    0x0B,                   /* bDescriptorType              */
    0x00,                   /* bFirstInterface              */
    0x02,                   /* bInterfaceCount              */
    0x02,                   /* bInterfaceClass              */
    0x06,                   /* bInterfaceSubClass           */
    0x00,                   /* nInterfaceProtocol           */
    0x05,                   /* iInterface                   */
#endif

/* ECM Communication Interface descriptor */
    0x09,                   /* bLength                      */
    0x04,                   /* bDescriptorType              */
                            /* Interface descriptor type    */
    0x00,                   /* bInterfaceNumber             */
    0x00,                   /* bAlternateSetting            */
    0x01,                   /* bNumEndpoints                */
    0x02,                   /* bInterfaceClass              */
    0x06,                   /* bInterfaceSubClass           */
    0x00,                   /* nInterfaceProtocol           */
    0x06,                   /* iInterface                   */

/* USB Communication Class Header Functional Descriptor */
    0x05,                               /* bLength                      */
    0x24,                               /* bDescriptorType              */
    0x00,                               /* bDescriptorSubType           */
    0x10,                               /* bcdCDC                       */
    0x01,

/* USB Communication Class Union Functional Descriptor */
    0x05,                               /* bLength                      */
    0x24,                               /* bDescriptorType              */
    0x06,                               /* bDescriptorSubType           */
    0x00,                               /* bControlInterface            */
    0x01,                               /* bSubordinateInterface0       */

/* USB Communication Class Ethernet Networking Functional Descriptor */
    0x0d,                               /* bLength                      */
    0x24,                               /* bDescriptorType              */
    0x0f,                               /* bDescriptorSubType           */
    0x08,                               /* iMACAddress                  */
    0x00,                               /* bmEthernetStatistics         */
    0x00,
    0x00,
    0x00,
    (ECM_SEG_LEN & 0xff),               /* wMaxSegmentSize Low          */
    ((ECM_SEG_LEN >> 8) & 0xff),        /* wMaxSegmentSize High         */
    0x00,                               /* wNumberMCFilters             */
    0x00,
    0x00,                               /* bNumberPowerFilters          */

/* Endpoint 1 Interrupt In descriptor */
    0x07,                   /* bLength                      */
    0x05,                   /* bDescriptorType              */
    0x81,                   /* bEndpointAddress             */
    0x03,                   /* bmAttributes                 */
    0x10,                   /* wMaxPacketSize               */
    0x00,
    0x01,                   /* bInterval - changes with device speed */

/* DATA interface descriptor ... alternate 0 */
    0x09,                   /* bLength                      */
    0x04,                   /* bDescriptorType              */
                            /* Interface descriptor type    */
    0x01,                   /* bInterfaceNumber             */
    0x00,                   /* bAlternateSetting            */
    0x00,                   /* bNumEndpoints                */
    0x0a,                   /* bInterfaceClass              */
    0x00,                   /* bInterfaceSubClass           */
    0x00,                   /* nInterfaceProtocol           */
    0x07,                   /* iInterface                   */

/* DATA interface descriptor ... alternate 1 */
    0x09,                   /* bLength                      */
    0x04,                   /* bDescriptorType              */
                            /* Interface descriptor type    */
    0x01,                   /* bInterfaceNumber             */
    0x01,                   /* bAlternateSetting            */
    0x02,                   /* bNumEndpoints                */
    0x0a,                   /* bInterfaceClass              */
    0x00,                   /* bInterfaceSubClass           */
    0x00,                   /* nInterfaceProtocol           */
    0x07,                   /* iInterface                   */

/* Endpoint 2 Bulk In Descriptor */
    0x07,                   /* bLength                      */
    0x05,                   /* bDescriptorType              */
    0x82,                   /* bEndpointAddress:            */
    0x02,                   /* bmAttributes                 */
    0x40,                   /* wMaxPacketSize               */
    0x00,
    0x00,                   /* bInterval                    */

/* Endpoint 3 Bulk Out Descriptor */
    0x07,                   /* bLength                      */
    0x05,                   /* bDescriptorType              */
    0x03,                   /* bEndpointAddress:            */
    0x02,                   /* bmAttributes                 */
    0x40,                   /* wMaxPacketSize               */
    0x00,
    0x00,                   /* bInterval                    */
};

char usbdc_hs_config_descriptor_0[] = {
    0x9,                    /* bLength              */
    0x2,                    /* bDescriptorType      */
    SIZE_CONFIG_DESC,       /* wLength              */
    0x0,
    0x2,                    /* bNumInterfaces       */
    0x1,                    /* bConfigurationValue  */
    0x04,                   /* iConfiguration       */
    0x40,                   /* bmAttributes         */
    USBDC_MAX_CURRENT,      /* max power            */

#if INCLUDE_IAD
/* Interface Association Descriptor */
    0x08,                   /* bLength                      */
    0x0B,                   /* bDescriptorType              */
    0x00,                   /* bFirstInterface              */
    0x02,                   /* bInterfaceCount              */
    0x02,                   /* bInterfaceClass              */
    0x06,                   /* bInterfaceSubClass           */
    0x00,                   /* nInterfaceProtocol           */
    0x05,                   /* iInterface                   */
#endif

/* ECM Communication Interface descriptor */
    0x09,                   /* bLength                      */
    0x04,                   /* bDescriptorType              */
                            /* Interface descriptor type    */
    0x00,                   /* bInterfaceNumber             */
    0x00,                   /* bAlternateSetting            */
    0x01,                   /* bNumEndpoints                */
    0x02,                   /* bInterfaceClass              */
    0x06,                   /* bInterfaceSubClass           */
    0x00,                   /* nInterfaceProtocol           */
    0x06,                   /* iInterface                   */

/* USB Communication Class Header Functional Descriptor */
    0x05,                               /* bLength                      */
    0x24,                               /* bDescriptorType              */
    0x00,                               /* bDescriptorSubType           */
    0x10,                               /* bcdCDC                       */
    0x01,

/* USB Communication Class Union Functional Descriptor */
    0x05,                               /* bLength                      */
    0x24,                               /* bDescriptorType              */
    0x06,                               /* bDescriptorSubType           */
    0x00,                               /* bControlInterface            */
    0x01,                               /* bSubordinateInterface0       */

/* USB Communication Class Ethernet Networking Functional Descriptor */
    0x0d,                               /* bLength                      */
    0x24,                               /* bDescriptorType              */
    0x0f,                               /* bDescriptorSubType           */
    0x08,                               /* iMACAddress                  */
    0x00,                               /* bmEthernetStatistics         */
    0x00,
    0x00,
    0x00,
    (ECM_SEG_LEN & 0xff),               /* wMaxSegmentSize Low          */
    ((ECM_SEG_LEN >> 8) & 0xff),        /* wMaxSegmentSize High         */
    0x00,                               /* wNumberMCFilters             */
    0x00,
    0x00,                               /*  bNumberPowerFilters         */

/* Endpoint 1 Interrupt In descriptor */
    0x07,                   /* bLength                      */
    0x05,                   /* bDescriptorType              */
    0x81,                   /* bEndpointAddress             */
    0x03,                   /* bmAttributes                 */
    0x10,                   /* wMaxPacketSize               */
    0x00,
    0x04,                   /* bInterval - changes with device speed */

/* DATA interface descriptor ... alternate 0 */
    0x09,                   /* bLength                      */
    0x04,                   /* bDescriptorType              */
                            /* Interface descriptor type    */
    0x01,                   /* bInterfaceNumber             */
    0x00,                   /* bAlternateSetting            */
    0x00,                   /* bNumEndpoints                */
    0x0a,                   /* bInterfaceClass              */
    0x00,                   /* bInterfaceSubClass           */
    0x00,                   /* nInterfaceProtocol           */
    0x07,                   /* iInterface                   */

/* DATA interface descriptor ... alternate 1 */
    0x09,                   /* bLength                      */
    0x04,                   /* bDescriptorType              */
                            /* Interface descriptor type    */
    0x01,                   /* bInterfaceNumber             */
    0x01,                   /* bAlternateSetting            */
    0x02,                   /* bNumEndpoints                */
    0x0a,                   /* bInterfaceClass              */
    0x00,                   /* bInterfaceSubClass           */
    0x00,                   /* nInterfaceProtocol           */
    0x07,                   /* iInterface                   */

/* Endpoint 2 Bulk In Descriptor */
    0x07,                   /* bLength                      */
    0x05,                   /* bDescriptorType              */
    0x82,                   /* bEndpointAddress:            */
    0x02,                   /* bmAttributes                 */
    0x00,                   /* wMaxPacketSize               */
    0x02,
    0x00,                   /* bInterval                    */

/* Endpoint 3 Bulk Out Descriptor */
    0x07,                   /* bLength                      */
    0x05,                   /* bDescriptorType              */
    0x03,                   /* bEndpointAddress:            */
    0x02,                   /* bmAttributes                 */
    0x00,                   /* wMaxPacketSize               */
    0x02,
    0x00,                   /* bInterval                    */
};

const char string_descriptors[] = {
/* Language Code Descriptor  */
    0x04,                   /* bLength                      */
    0x03,                   /* bDescriptorType              */
    0x09,                   /* wLANGID (LSB)                */
    0x04,                   /* wLANGIF (MSB)                */

/* Manufacturer String Descriptor */
    42,                             /* bLength              */
    0x3,                            /* bDescriptorType      */
    'Q', '\0', 'N', '\0', 'X', '\0', ' ', '\0',
    'S', '\0', 'o', '\0', 'f', '\0', 't', '\0', 'w', '\0', 'a', '\0', 'r', '\0', 'e', '\0', ' ', '\0',
    'S', '\0', 'y', '\0', 's', '\0', 't', '\0', 'e', '\0', 'm', '\0', 's', '\0',

/* Product String Descriptor */
    46,                             /* bLength              */
    0x3,                            /* bDescriptorType      */
    'Q', '\0', 'N', '\0', 'X', '\0', ' ', '\0',
    'E', '\0', 'C', '\0', 'M', '\0', ' ', '\0',
    'N', '\0', 'e', '\0', 't', '\0', 'w', '\0', 'o', '\0', 'r', '\0', 'k', '\0', ' ', '\0',
    'D', '\0', 'e', '\0', 'v', '\0', 'i', '\0', 'c', '\0', 'e', '\0',

/* Serial String Descriptor */
    30,                             /* bLength              */
    0x3,                            /* bDescriptorType      */
    'A', '\0', 'B', '\0', 'C', '\0', '-', '\0',
    '0', '\0', '1', '\0', '2', '\0', '3', '\0', '4', '\0', '5', '\0', '6', '\0', '7','\0',
    '8', '\0', '9', '\0',

/* Configuration String Descriptor */
    44,                             /* bLength              */
    0x03,                           /* bDescriptorType      */
    'D', '\0', 'e', '\0', 'f', '\0', 'a', '\0', 'u', '\0', 'l', '\0', 't', '\0', ' ', '\0',
    'C', '\0', 'o', '\0', 'n', '\0', 'f', '\0', 'i', '\0', 'g', '\0', 'u', '\0', 'r', '\0',
    'a', '\0', 't', '\0', 'i', '\0', 'o', '\0', 'n', '\0',

/* IAD Interface Descriptor */
    28,                             /* bLength              */
    0x03,                           /* bDescriptorType      */
    'I', '\0', 'A', '\0', 'D', '\0', ' ', '\0',
    'I', '\0', 'n', '\0', 't', '\0', 'e', '\0', 'r', '\0', 'f', '\0', 'a', '\0', 'c', '\0', 'e','\0',

/* CDC Ethernet Control Model (ECM) */
    68,                             /* bLength              */
    0x03,                           /* bDescriptorType      */
    'C', '\0', 'D', '\0', 'C', '\0', ' ', '\0',
    'E', '\0', 't', '\0', 'h', '\0', 'e', '\0', 'r', '\0', 'n', '\0', 'e', '\0', 't', '\0', ' ', '\0',
    'C', '\0', 'o', '\0', 'n', '\0', 't', '\0', 'r', '\0', 'o', '\0', 'l', '\0', ' ', '\0',
    'M', '\0', 'o', '\0', 'd', '\0', 'u', '\0', 'l', '\0', 'e', '\0', ' ', '\0',
    '(', '\0', 'E', '\0', 'C', '\0', 'M', '\0', ')', '\0',

/* CDC Network Data */
    34,                             /* bLength              */
    0x03,                           /* bDescriptorType      */
    'C', '\0', 'D', '\0', 'C', '\0', ' ', '\0',
    'N', '\0', 'e', '\0', 't', '\0', 'w', '\0', 'o', '\0', 'r', '\0', 'k', '\0', ' ', '\0',
    'D', '\0', 'a', '\0', 't', '\0', 'a', '\0',

/* MAC Address */
    26,                             /* bLength              */
    0x03,                           /* bDescriptorType      */
    /*
     * Note that the second-least-significant bit of the most significant byte should be '1'
     * to indicate locally administrated MAC address
     * The least-significant bit of the most significant byte should be '0'
     * to indicate unicast MAC address
     */
    '0', '\0', '2', '\0', '0', '\0', '0', '\0', '0', '\0', '0', '\0',
    '0', '\0', '4', '\0', '0', '\0', '5', '\0', '0', '\0', '6', '\0',
};

const char *usbdc_fs_config_descriptor[] = { usbdc_fs_config_descriptor_0 };
const char *usbdc_hs_config_descriptor[] = { usbdc_hs_config_descriptor_0 };

#define USBDC_MAX_STRING_DESCRIPTOR  8
static const char *usbdc_string_descriptor[] = {
        &string_descriptors[0],
        &string_descriptors[4],
        &string_descriptors[4 + 42],
        &string_descriptors[4 + 42 + 46],
        &string_descriptors[4 + 42 + 46 + 30],
        &string_descriptors[4 + 42 + 46 + 30 + 44],
        &string_descriptors[4 + 42 + 46 + 30 + 44 + 28],
        &string_descriptors[4 + 42 + 46 + 30 + 44 + 28 + 68],
        &string_descriptors[4 + 42 + 46 + 30 + 44 + 28 + 68 + 34],
};

#define USBDC_FS_DEVICE_DESCRIPTOR              usbdc_device_descriptor
#define USBDC_FS_CONFIG_DESCRIPTOR              usbdc_fs_config_descriptor
#define USBDC_FS_STRING_DESCRIPTOR              usbdc_string_descriptor
#define USBDC_FS_DEVICE_QUALIFIER_DESCRIPTOR    usbdc_device_qualifier_descriptor

#define USBDC_HS_DEVICE_DESCRIPTOR              usbdc_device_descriptor
#define USBDC_HS_CONFIG_DESCRIPTOR              usbdc_hs_config_descriptor
#define USBDC_HS_STRING_DESCRIPTOR              usbdc_string_descriptor
#define USBDC_HS_DEVICE_QUALIFIER_DESCRIPTOR    usbdc_device_qualifier_descriptor

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/private/hw/usbdc_desc_ecm.h $ $Rev: 762989 $")
#endif
