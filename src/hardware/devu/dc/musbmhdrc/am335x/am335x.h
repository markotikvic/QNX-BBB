/*
 * $QNXLicenseC: 
 * Copyright 2012, QNX Software Systems.  
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

 
#ifndef AM335X_H
#define AM335X_H

#ifndef TRUE
	#define TRUE	(1)
#endif
#ifndef FALSE
	#define FALSE	(0)
#endif



/* Note that the memory region sizes that are mmaped do not indicate the true size of 
 * the given memory region since we only mmap up to the registers that we need.
 */

/* 
 * The USB Controller registers are located at USBx_CORE - 0x400.
 * These registers contain glue logic that TI adds to control the USB
 * Controllers.
 */
#define AM335x_USB0_ADDR		0x47401000
#define AM335x_USB1_ADDR		0x47401800
#define AM335x_USBx_OFFSET		0x400
#define AM335X_USBx_SIZE		0xF0

/* USB Controller Registers */
	
#define USBx_REVID			(*(volatile unsigned  *)(xdc->IoBase + 0x0))
#define USBx_CTRL			(*(volatile unsigned  *)(xdc->IoBase + 0x14))
	#define SOFT_RESET_ISOLATION	(1 << 5) /* Should be set at the same time as SOFT_RESET */
	#define UINT			(1 << 3) /* Control whether USB interrupts need to be configured via
						  * core registers (UINT=1) or core registers _and_ USB controller registers (UINT=0) */
	#define SOFT_RESET		(1 << 0) /* Soft Reset */

#define USBx_STAT			(*(volatile unsigned  *)(xdc->IoBase + 0x18))
#define USBx_IRQMSTAT			(*(volatile unsigned  *)(xdc->IoBase + 0x20))
#define USBx_IRQSTATRAW0		(*(volatile unsigned  *)(xdc->IoBase + 0x28))
#define USBx_IRQSTATRAW1		(*(volatile unsigned  *)(xdc->IoBase + 0x2c))
#define USBx_IRQSTAT0			(*(volatile unsigned  *)(xdc->IoBase + 0x30))
#define USBx_IRQSTAT1			(*(volatile unsigned  *)(xdc->IoBase + 0x34))
#define USBx_IRQENABLESET0		(*(volatile unsigned  *)(xdc->IoBase + 0x38))
#define USBx_IRQENABLESET1		(*(volatile unsigned  *)(xdc->IoBase + 0x3c))
	#define IRQBANK1_USB9		(1 << 9)
#define USBx_IRQENABLECLR0		(*(volatile unsigned  *)(xdc->IoBase + 0x40))
#define USBx_IRQENABLECLR1		(*(volatile unsigned  *)(xdc->IoBase + 0x44))
	#define IRQCLEARALL		(0x0) 
#define USBx_TXMODE			(*(volatile unsigned  *)(xdc->IoBase + 0x70))
#define USBx_RXMODE			(*(volatile unsigned  *)(xdc->IoBase + 0x74))
#define USBx_GENRNDISEP1		(*(volatile unsigned  *)(xdc->IoBase + 0x80))
#define USBx_GENRNDISEP2		(*(volatile unsigned  *)(xdc->IoBase + 0x84))
#define USBx_GENRNDISEP3		(*(volatile unsigned  *)(xdc->IoBase + 0x88))
#define USBx_GENRNDISEP4		(*(volatile unsigned  *)(xdc->IoBase + 0x8c))
#define USBx_GENRNDISEP5		(*(volatile unsigned  *)(xdc->IoBase + 0x90))
#define USBx_GENRNDISEP6		(*(volatile unsigned  *)(xdc->IoBase + 0x94))
#define USBx_GENRNDISEP7		(*(volatile unsigned  *)(xdc->IoBase + 0x98))
#define USBx_GENRNDISEP8		(*(volatile unsigned  *)(xdc->IoBase + 0x9c))
#define USBx_GENRNDISEP9		(*(volatile unsigned  *)(xdc->IoBase + 0xa0))
#define USBx_GENRNDISEP10		(*(volatile unsigned  *)(xdc->IoBase + 0xa4))
#define USBx_GENRNDISEP11		(*(volatile unsigned  *)(xdc->IoBase + 0xa8))
#define USBx_GENRNDISEP12		(*(volatile unsigned  *)(xdc->IoBase + 0xac))
#define USBx_GENRNDISEP13		(*(volatile unsigned  *)(xdc->IoBase + 0xb0))
#define USBx_GENRNDISEP14		(*(volatile unsigned  *)(xdc->IoBase + 0xb4))
#define USBx_GENRNDISEP15		(*(volatile unsigned  *)(xdc->IoBase + 0xb8))
#define USBx_AUTOREQ			(*(volatile unsigned  *)(xdc->IoBase + 0xd0))
#define USBx_SRPFIXTIME			(*(volatile unsigned  *)(xdc->IoBase + 0xd4))
#define USBx_TDOWN			(*(volatile unsigned  *)(xdc->IoBase + 0xd8))
#define USBx_UTMI			(*(volatile unsigned  *)(xdc->IoBase + 0xe0))
	#define UTMI_OTGDISABLE		(1 << 21)
#define USBx_MGCUTMILB			(*(volatile unsigned  *)(xdc->IoBase + 0xe4))
#define USBx_MODE			(*(volatile unsigned  *)(xdc->IoBase + 0xe8))
	#define MODE_IDDIG		(1 << 8)
	/* Bit 7 is an enable bit which forces the USB controller to act as a 
	 * Host (IDDIG=0) or a Device (IDDIG=1) based on the IDDIG bit regardless
	 * of the value of the USB ID pin
	 */
	#define MODE_DEVICE		(1 << 7)
	#define MODE_HOST		(0 << 7)

/* 
 * Control Module Registers 
 * These registers allow the USB PHYs to be enabled/disabled
 */
#define AM335x_CM_BASE			0x44E10000
#define AM335X_CM_SIZE			0x700

#define CM_USB_CTRL0			(*(volatile unsigned  *)(xdc->CM_IoBase + 0x620))
#define CM_USB_CTRL1			(*(volatile unsigned  *)(xdc->CM_IoBase + 0x628))
	#define USB_CTRL_DATAPOLARITY_INV (1 << 23)	/* Data Polarity Invert */
	#define USB_CTRL_OTGSESSENDEN	(1 << 20)	/* Session End Detect Enable */
	#define USB_CTRL_OTGVDET_EN	(1 << 19)	/* VBUS detect enable */
	#define USB_CTRL_GPIOMODE	(1 << 12)	/* GPIO Mode -> 0 = USB Mode, 1 = GPIO mode */
	#define USB_CTRL_CDET_EXTCTL	(1 << 10)	/* Bypass the charger detection state machine */
	#define USB_CTRL_DPPULLUP	(1 << 9)	/* Pullup on DP line */
	#define USB_CTRL_DMPULLUP	(1 << 8)	/* Pullup on DM line */
	#define USB_CTRL_CHGVSRC_EN	(1 << 7)	/* Enable VSRC on DP line */
	#define USB_CTRL_CHGISINK_EN	(1 << 6)	/* Enable ISINK on DM line */
	#define USB_CTRL_SINKONDP	(1 << 5)	/* Sink on DP */
	#define USB_CTRL_SRCONDM	(1 << 4)	/* Source on DM */
	#define USB_CTRL_CHGDET_RSTRT	(1 << 3)	/* Restart Charger Detect */
	#define USB_CTRL_CHGDET_DIS	(1 << 2)	/* Charger Detect disable */
	#define USB_CTRL_OTG_PWRDN	(1 << 1)	/* Power down USB OTG PHY */
	#define USB_CTRL_CM_PWRDN	(1 << 0)	/* Power down USB CM PHY */		

/* 
 * Clock Management Peripheral (CM_PER) Registers.
 * These registers are used to enable/disable clocks to peripherals
 */
#define AM335x_CM_PER_BASE		0x44E00000
#define AM335X_CM_PER_SIZE		0x20

#define CM_PER_USB0_CLKCTRL		(*(volatile unsigned  *)(xdc->CM_PER_IoBase + 0x1c))
	#define CLOCK_MASK		(0x3)
	#define CLOCK_ENABLE		(0x2)
	#define CLOCK_DISABLE		(0x0)

/*
 * Can be used to power USB PHYs on or off
 */
enum phy_configure {
	USB_PHY_ON = 0,
	USB_PHY_OFF = 1
};
	
typedef struct _xdctrl_t {
	uint8_t		*IoBase;    
	uint8_t		*CM_IoBase;
	uint8_t		*CM_PER_IoBase;
	uint8_t		controller_id;
	unsigned	preserve_usb_settings;
	unsigned	force_device_mode;
} xdctrl_t;


#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/musbmhdrc/am335x/am335x.h $ $Rev: 724084 $")
#endif
