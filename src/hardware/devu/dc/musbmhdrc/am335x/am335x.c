/*
 * $QNXLicenseC: 
 * Copyright 2012 QNX Software Systems.  
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
 *	Custom AM 335X Callouts
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/slog.h>

#include "musb.h"
#include "am335x.h" 

////////////////////////////////////////////////////////////////////////////////
//                         PRIVATE FUNCTIONS                                  //
////////////////////////////////////////////////////////////////////////////////	

static int parse_command_line( xdctrl_t  *xdc, char * args ) {
	char *value;
	char *am335x_opts[] = {
	#define AM335x_FORCE_DEVICE_MODE  0
		"force_device_mode",
		NULL};

	if ( !args )
		return TRUE;

	// Default behaviour is to use the ID pin to determine Host or Device mode
	xdc->force_device_mode = FALSE;

	while (*args != '\0') {
		switch ( getsubopt( &args, am335x_opts, &value) ) {
		case AM335x_FORCE_DEVICE_MODE:
			xdc->force_device_mode = TRUE;
			break;
		default:
			break;
		}
	}
	return TRUE;

}

/*
 * PHY control settings can be accessed via a control module register called USB_CTRL0
 */
static int set_phy_power(dctrl_t * dc, uint32_t power)
{
	xdctrl_t *xdc = dc->xdc;

	if ((power != USB_PHY_ON) && (power != USB_PHY_OFF)) {
		musb_slogf( dc, _SLOG_ERROR, "%s Unknown Controller, only USB Controller 0 or 1 can be configured",__func__);
		return FALSE;
	}

	if (xdc->controller_id == 0) {
		if (power == USB_PHY_ON)
			CM_USB_CTRL0 &= ~(USB_CTRL_OTG_PWRDN | USB_CTRL_CM_PWRDN);
		else
			CM_USB_CTRL0 |= (USB_CTRL_OTG_PWRDN | USB_CTRL_CM_PWRDN);
	}
	else if (xdc->controller_id == 1) {
		if (power == USB_PHY_ON)
			CM_USB_CTRL0 &= ~(USB_CTRL_OTG_PWRDN | USB_CTRL_CM_PWRDN);
		else
			CM_USB_CTRL0 |= (USB_CTRL_OTG_PWRDN | USB_CTRL_CM_PWRDN);
	}
	else {
		musb_slogf( dc, _SLOG_ERROR, "%s Unknown Controller, only USB Controller 0 or 1 can be configured",__func__);
		return FALSE;
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//                         PUBLIC FUNCTIONS                                   //
////////////////////////////////////////////////////////////////////////////////

/* 
 * Mmap USB, Control registers that are used by this driver.  Also ensure that the USB
 * clock is enabled.
 */
int musb_custom_init1( dctrl_t * dc, char * args ) {
	int 		err;
	xdctrl_t 	*xdc;
	uint32_t	addr;
	uint32_t	timeout = 5000;

	// allocate device specific controller context
	dc->xdc = xdc = calloc( 1, sizeof( xdctrl_t ) );
	if ( dc->xdc == NULL ) {
		musb_slogf( dc, _SLOG_ERROR, "%s calloc() failed ",__func__);
		err = ENOMEM;
		goto error;
	}
		
	/* mmap USB Controller Registers (USBx) */
	addr = PCI_MEM_ADDR( dc->udc->hw_ctrl.pci_inf->CpuBaseAddress[0] ) - AM335x_USBx_OFFSET;
	xdc->IoBase =  mmap_device_memory( NULL, 
						AM335X_USBx_SIZE,  
						PROT_READ | PROT_WRITE | PROT_NOCACHE, 
						MAP_SHARED | MAP_PHYS,
						addr );
	if ( xdc->IoBase == MAP_FAILED ) {
		musb_slogf( dc, _SLOG_ERROR, "%s mmap failed",__func__);
		err = ENOMEM;
		goto error2;
	}

	/* determine which USB controller we are using */
	xdc->controller_id = (addr == AM335x_USB0_ADDR) ? 0 : 1;

	/* mmap Control Module (CM) registers */
	xdc->CM_IoBase =  mmap_device_memory( NULL, 
						AM335X_CM_SIZE,  
						PROT_READ | PROT_WRITE | PROT_NOCACHE, 
						MAP_SHARED | MAP_PHYS,
						AM335x_CM_BASE );
	if ( xdc->CM_IoBase == MAP_FAILED ) {
		musb_slogf( dc, _SLOG_ERROR, "%s mmap failed",__func__);
		err = ENOMEM;
		goto error3;
	}

	/* mmap Clock Management Peripheral (CM_PER) registers */
	xdc->CM_PER_IoBase =  mmap_device_memory( NULL, 
						AM335X_CM_PER_SIZE,  
						PROT_READ | PROT_WRITE | PROT_NOCACHE, 
						MAP_SHARED | MAP_PHYS,
						AM335x_CM_PER_BASE );
	if ( xdc->CM_PER_IoBase == MAP_FAILED ) {
		musb_slogf( dc, _SLOG_ERROR, "%s mmap failed",__func__);
		err = ENOMEM;
		goto error4;
	}

	// specify whether we should power down PHY, USB modules when the driver is no longer running
	xdc->preserve_usb_settings = dc->flags & DC_FLAG_INHERIT_CFG;
	
	if (parse_command_line( xdc, args ) == FALSE) {
		musb_slogf( dc, _SLOG_ERROR, "%s parse_command_line() failed ",__func__);
		goto error5;
	}
	
	/* enable the USB OTG clock */
	if ((CM_PER_USB0_CLKCTRL & CLOCK_MASK) != CLOCK_ENABLE) {
		CM_PER_USB0_CLKCTRL = CLOCK_ENABLE;
		do {
			usleep(1);
		} while (--timeout && (CM_PER_USB0_CLKCTRL != CLOCK_ENABLE) );
	}

	return EOK;

error5:
	munmap(xdc->CM_PER_IoBase, AM335X_CM_PER_SIZE );
error4:
	munmap(xdc->CM_IoBase, AM335X_CM_SIZE );
error3:
	munmap( xdc->IoBase, AM335X_USBx_SIZE);
error2:
	free( xdc );	
error:
	return err;
	
}

int musb_custom_init2( dctrl_t * dc ) {
	xdctrl_t 	*xdc = dc->xdc;

	// confgure the otg module register... this must happen in init2 because the module is reset in musb_custom_reset 
	// between init1 and init2

	if (xdc->force_device_mode) {
		/* Override ID pin, and force USB OTG module to operate in Device Mode */
		USBx_MODE = MODE_IDDIG | MODE_DEVICE;
	}
	
	/* Reset USB PHYs */
	set_phy_power(dc, USB_PHY_OFF);
	delay(5);
	set_phy_power(dc, USB_PHY_ON);

	/* Use Legacy Interrupt Mode */
	USBx_CTRL |= UINT;	

	/* Enable Generic Mentor Interrupt */
	USBx_IRQENABLESET1 |= IRQBANK1_USB9;

	/* Enable OTG mode */
	USBx_UTMI &= ~UTMI_OTGDISABLE;	
		
	return EOK;	
}

int musb_custom_fini1( dctrl_t * dc ) {
	xdctrl_t 	*xdc = dc->xdc;
	
	munmap(xdc->CM_PER_IoBase, AM335X_CM_PER_SIZE );
	munmap(xdc->CM_IoBase, AM335X_CM_SIZE );
	munmap( xdc->IoBase, AM335X_USBx_SIZE);
	
	free( xdc );	
	
	return EOK;
}

int musb_custom_fini2( dctrl_t * dc ) {
	xdctrl_t 	*xdc = dc->xdc;

	/* Disable All Interrupts */
	USBx_IRQENABLECLR0 = IRQCLEARALL;
	USBx_IRQENABLECLR1 = IRQCLEARALL;

	/* Power Down PHY */
	if (!(xdc->preserve_usb_settings)) {
		set_phy_power(dc, USB_PHY_OFF);
		CM_PER_USB0_CLKCTRL = CLOCK_DISABLE;
	}

	return EOK;
}

/* Reset USBx Controller */
int musb_custom_reset( dctrl_t * dc ) {
	xdctrl_t *xdc = dc->xdc;
	unsigned timeout = 5000;
   
	USBx_CTRL = SOFT_RESET; 
	do {
		usleep(1);
	} while (--timeout && (USBx_CTRL & SOFT_RESET) );

	return EOK;
}

/*
 * AM335x uses external DMA (CPPI 4.1) which is currently not supported.
 * The EDMA is another type of external DMA which works in PIO mode from
 * the USBSS' perspective.
 */
void musb_dma_map( dctrl_t * dc, musb_dma_funcs_t * dma_funcs ) {
	if ( dc->flags & DC_FLAG_USE_EXTERNAL_DMA ) {
		dma_funcs->init             = edma_init;
		dma_funcs->fini             = edma_fini;
		dma_funcs->channel_claim    = edma_channel_claim;
		dma_funcs->channel_release  = edma_channel_release;
		dma_funcs->transfer_start   = edma_transfer_start;
		dma_funcs->transfer_abort   = edma_transfer_abort;
		dma_funcs->rx_complete      = edma_rx_complete;
	} else {
		dma_funcs->init             = NULL;
		dma_funcs->fini             = NULL;
		dma_funcs->channel_claim    = NULL;
		dma_funcs->channel_release  = NULL;
		dma_funcs->transfer_start   = NULL;
		dma_funcs->transfer_abort   = NULL;
		dma_funcs->rx_complete      = NULL;
	}
}

void	musb_cfg_get( musbmhdrc_cfg_t * cfg ) {
	cfg->fiforam_size 	= 1024 * 16;
	cfg->n_endpoints	= 16;
	cfg->dma_irq 		= 125;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/musbmhdrc/am335x/am335x.c $ $Rev: 750964 $")
#endif
