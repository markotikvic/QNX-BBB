/*
 * $QNXLicenseC: 
 * Copyright 2010, QNX Software Systems.  
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
 *    Module - USB device controller driver for the Mentor MUSBMHDRC OTG USB controller
 *
 *	  Note 1:  Driver only support DMA for bulk endpoints, and only uses type 1
 *
 *    Note 2: This driver doesn't support double buffering... The DMA performance
 *            on bulk endpoints is very good with single buffering, so the additional
 *            complexity of double buffering is not required at this point.
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <gulliver.h>
#include <sys/slog.h>
#include <hw/inout.h>
#include <drvr/common.h>
#include <descriptors.h>
#include <stdbool.h>
#include <atomic.h>
#include "musb.h"


extern char usbdc_config_descriptor[];


static iousb_pipe_methods_t musb_control_pipe_methods = {
	musb_control_endpoint_enable,
	musb_control_endpoint_disable,
	musb_control_transfer,
	musb_control_transfer_abort,
	NULL
};

static iousb_pipe_methods_t musb_interrupt_pipe_methods = {
	musb_endpoint_enable,
	musb_endpoint_disable,
	musb_transfer,
	musb_transfer_abort,
	NULL
};

static iousb_pipe_methods_t musb_bulk_pipe_methods = {
	musb_endpoint_enable,
	musb_endpoint_disable,
	musb_transfer,
	musb_transfer_abort,
	NULL
};

static iousb_pipe_methods_t musb_isoch_pipe_methods = {
	musb_isoch_endpoint_enable,
	musb_isoch_endpoint_disable,
	musb_isoch_transfer,
	musb_isoch_transfer_abort,
	NULL
};


dc_methods_t musb_controller_methods = {
		20,
		musb_init,
		musb_start,
		musb_stop,
		musb_shutdown,
		NULL,
		NULL,    
		musb_set_bus_state,
		musb_set_device_feature,
		musb_clear_device_feature,
		musb_set_device_address,
		musb_get_descriptor,
		musb_select_configuration,
#ifdef MUSB_REAL_ISR
		NULL,
#else
		musb_interrupt,
#endif
		musb_set_endpoint_state,
		musb_clear_endpoint_state,
		NULL,
		&musb_control_pipe_methods,
		&musb_interrupt_pipe_methods,
		&musb_bulk_pipe_methods,
		&musb_isoch_pipe_methods,
};

usb_controller_methods musb_usb_controller_methods = {
	NULL,
	&musb_controller_methods,
	NULL,
	NULL
};

io_usb_dll_entry_t io_usb_dll_entry = {
	USBDC_DLL_NAME,
	0xffff,  // pci device id
	0xffff,
	0xffff,
	USB_CONTROLLER_DEVICE,
	NULL,
	NULL,
	&musb_usb_controller_methods
};

// local macros
#define	DIR_MASK 			0x80
#define HOST_TO_DEVICE		0x0

static void process_epout_interrupt( dctrl_t *dc, unsigned epidx );
static void process_ep0_interrupt( dctrl_t *dc );

// 53 USB spec defined test packet.
static uint8_t musb_testmode_pkt[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
									0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
									0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
									0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
									0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd,
									0xfc, 0x7e, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e
								};

									
static unsigned musb_log2( unsigned n ) {
	unsigned cnt = 0;
	
	while( n != 1 ) {
		n = n >> 1;
		cnt++;
	}
	
	return cnt;
}


void 
musb_slogf( dctrl_t * dc, int level, const char *fmt, ...)
{
	va_list arglist;
	
	if ( dc && ( level > dc->verbosity ) ) 
		return; 
	
	va_start( arglist, fmt );
	vslogf( 12, level, fmt, arglist );
	va_end( arglist );
	return;
}

/*
 * Allocate and Initialize transfer free pool 
 */
int
musb_alloc_transfer_queue( dctrl_t * dc )
{
	musb_transfer_t	*td;
	int			i;

	SIMPLEQ_INIT( &dc->transfer_free_q );
	SIMPLEQ_INIT( &dc->transfer_complete_q );

	/* allocate memory for our chip handle  */
	if ( (td = calloc( 1, sizeof( musb_transfer_t ) * ( dc->num_td + 1 ) ) ) == NULL ) {
		return ENOMEM ;
	}

	dc->transfer_mem = td;

	for ( i = 0 ; i < dc->num_td ; i++, td++ ) {
		SIMPLEQ_INSERT_TAIL( &dc->transfer_free_q, td, link ); 
	}

	return EOK ;
}

musb_transfer_t *
musb_get_transfer( dctrl_t * dc, iousb_transfer_t *urb, ep_ctx_t *ep, _uint32 flags )
{
	musb_transfer_t	*td;

	MUSB_LOCK
	if ( (td = SIMPLEQ_FIRST( &dc->transfer_free_q ) ) == NULL ) {
		MUSB_UNLOCK
		musb_slogf( dc, _SLOG_ERROR, "%s - No transfer structures available", __func__ );
		return NULL ;
	}
	SIMPLEQ_REMOVE_HEAD( &dc->transfer_free_q, link );
	td->urb 			= urb;
	td->ep 			    = ep;
	MUSB_UNLOCK
		
	// do some init
	td->status			= 0;
	td->flags			= flags;

	return( td );
}

static void read_from_fifo(  dctrl_t * dc, unsigned fifonum, uint16_t count, uint8_t * rxbuf ) {
	
	// read as much as possible using 32-bit accesses
	while ( count >= 4 ) {
		*((uint32_t *)rxbuf) = HW_Read32( dc->IoBase, FIFO(fifonum) );
		count -= 4;
		rxbuf += 4;
	}
	
	// read the rest with 8-bit accesses
	while ( count ) {
		*rxbuf++ = HW_Read8( dc->IoBase, FIFO(fifonum) );
		count--;
	}
	
}


static void write_to_fifo(  dctrl_t * dc, unsigned fifonum, uint16_t count, uint8_t * txbuf ) {
	
	// read as much as possible using 32-bit accesses
	while ( count >= 4 ) {
		HW_Write32( dc->IoBase, FIFO(fifonum), *((uint32_t *)txbuf) );
		count -= 4;
		txbuf += 4;
	}
	
	// write the rest with 8-bit accesses
	while ( count ) {
		HW_Write8( dc->IoBase, FIFO(fifonum), *txbuf++ );
		count--;
	}
}

static void flush_fifo( dctrl_t * dc, ep_ctx_t *ep ) {
	uint16_t 	csr;
	uint32_t		csr_addr;
	uint16_t	flush_msk;
	uint16_t	rdy_msk;
	
	MUSB_SLOGF_ISR_DBG(dc, _SLOG_INFO, "%s() flushing epnum = %d epdir = 0x%x", __func__, ep->num, ep->dir  );
	
	if ( ep->num == 0 ) {
		csr = HW_Read16( dc->IoBase, MUSB_CSR0 ); 
		if ( csr & ( CSR0_RXPKTRDY | CSR0_TXPKTRDY ) ) {
			HW_Write16( dc->IoBase, MUSB_CSR0,CSR0_FLUSH_FIFO  );
		}
	} else {
		if ( ep->dir & USB_ENDPOINT_IN ) {
			csr_addr 	= MUSB_TXCSR( ep->num );
			flush_msk 	= TXCSR_FLUSHFIFO;
			rdy_msk		= TXCSR_TXPKTRDY;
		} else {
			csr_addr 	= MUSB_RXCSR( ep->num );
			flush_msk	= RXCSR_FLUSHFIFO;
			rdy_msk		= RXCSR_RXPKTRDY;
		}

		// flush fifo only if there is data in it, otherwise there may 
		// be data corruption as outlined in the programming guide
		csr = HW_Read16( dc->IoBase, csr_addr );
		if ( csr & rdy_msk ) {
			HW_Write16( dc->IoBase, csr_addr, flush_msk );
		}

#ifdef MUSB_DOUBLE_BUFFER_ENABLED
		{
			uint32_t	timeout = 1000;

			// wait until flush bit goes low before flushing again
			while( timeout-- && ( HW_Read16( dc->IoBase, csr_addr ) & flush_msk ) ) {
				nanospin_ns( 1000 );
			}
			
			if ( timeout == 0 ) {
				MUSB_SLOGF_ISR_DBG(dc, _SLOG_INFO, "%s() flush bit is still active", __func__);
			}
		
			// flush again
			csr = HW_Read16( dc->IoBase, csr_addr );
			if ( csr & rdy_msk ) {
				HW_Write16( dc->IoBase, csr_addr, flush_msk );
			}
		}

#endif
	}
}


// !!!WARNING!!! this function assumes that it is called with driver mutex
// already locked
void complete_urb( dctrl_t * dc, ep_ctx_t *ep, uint32_t urb_status ) {
	musb_transfer_t *td;
	iousb_transfer_t *urb;

	MUSB_LOCK
	if ( (td = SIMPLEQ_FIRST( &ep->transfer_q )) && (urb = td->urb)) {
		SIMPLEQ_REMOVE_HEAD(&ep->transfer_q, link);
		// we have sent/received everyting ... complete the urb
		urb->actual_len = td->bytes_xfered;

		if(ep->type != USB_ATTRIB_ISOCHRONOUS){
			ep->td = NULL;
		}

#ifdef MUSB_REAL_ISR	
		td->status = urb_status;
		SIMPLEQ_INSERT_TAIL( &dc->transfer_complete_q, td, link );
#else			
		MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "URB COMPLETE epnum = 0x%x epdir = 0x%x urb->actual_len = %d ",ep->num, ep->dir , urb->actual_len  );
		SIMPLEQ_INSERT_TAIL( &dc->transfer_free_q, td, link );

		MUSB_MUTEX_UNLOCK
		urb->urb_complete_cbf( ep->iousb_ep, urb, urb_status, 0);
		MUSB_MUTEX_LOCK

#endif
	}
	MUSB_UNLOCK
}	

static void 
ep0_transmit( dctrl_t *dc ) {
	ep_ctx_t 	*ep = &dc->ep0;
	musb_transfer_t *td = ep->td;
	uint16_t	dataend = 0;	
	uint16_t	last_packet_in_xfer;
	
	// only transfer 1 usb packet at a time
	td->req_xfer_len = td->xfer_length - td->bytes_xfered;
	if ( td->req_xfer_len > ep->mps ) {
		// too much data for 1 usb packet... so cap it to mps
		td->req_xfer_len = ep->mps;
		last_packet_in_xfer = 0;
	} else {
		last_packet_in_xfer = 1;
	}
	

	write_to_fifo( dc, 0, td->req_xfer_len,  (uint8_t *) (td->xfer_buffer + td->bytes_xfered) );
	
	// set dataend if this is the last packet in transfer
	dataend = ( last_packet_in_xfer ) ? CSR0_DATAEND : 0;
	
	if ( last_packet_in_xfer ) {
		/* last usb packet of transfer... so hit DATAEND and
		 * transition the ep_state to IDLE so that the generated interrupt
		 * doesn't trigger another dataphase transfer
		 *
		 * Also: complete the URB *before* actual transfer goes out because
		 * it is very difficult ( if not impossible ) to to determine the state
		 * of the hardware once the interrupt fires because no actual status bits
		 * are set to tell us what happend.  The generated interrupt could mean
		 * the data phase completed, or if the interrupt latency is high enough, 
		 * then a bunch of events could have happended and captured by a single interrupt
		 * firing.  Therefore, for simplicity sake, complete the urb now: if the transfer
		 * in incomplete, then the HOST will take corrective action anyway
		 */
		
		dc->ep0.ep0_state = EP0_STATE_IDLE;	
		dc->ep0.ep0_idle_substate = EP0_IDLE_SUBSTATE_STATUS_PHASE;
		
		// modify to bytes_xfered to reflect the total number of bytes about
		// to be transmitted because we are completing the urb before the actual
		// transfer goes out.
		td->bytes_xfered += td->req_xfer_len;

		complete_urb( dc, ep, 0 );
		
		dataend = CSR0_DATAEND;
	} 

	MUSB_SLOGF_ISR_DBG(dc, _SLOG_DEBUG1, "%s: ep->req_xfer_len = %d dataend = 0x%x ", __func__ , td->req_xfer_len, dataend );

	HW_Write16( dc->IoBase, MUSB_CSR0, CSR0_TXPKTRDY | dataend );
}

static void 
ep_transmit( dctrl_t *dc, ep_ctx_t 	*ep ) {
	musb_transfer_t *td = ep->td;
	uint32_t		len;
	uint16_t        txcsr;
	
	if ( USE_BUILTIN_DMA( dc, ep ) && ( td->xfer_length > 0 ) ) {
		txcsr = HW_Read16( dc->IoBase, MUSB_TXCSR(ep->num ) );
		if ( ( txcsr & TXCSR_DMA_REQ_EN ) == 0 ) {
			/* *** IMPORTANT ***
			* Only set the DMA_REQ_EN bit only if it isn't set.  The only way
			* this condition should happen is if we are coming off a ZLP transfer
			* in which case we are guaranteed that the TXPKTRDY bit will be low
			* because the ZLP will be completed by the TX EP interrupt.  
			* Otherwise ( if we unconditionally OR-IN the bit) , there is a
			* chance the TXPKTRDY bit may be set in which case a
			* read-modify-write of the bit may cause a zlp to go out over the bus
			* in the case where the hardware zeros the TXPKTRDY bit after the read
			* and we will be re-writting it to 1.  This condittion happens because
			* we complete the transfer from dma interrupt handler as we set
			* the TXPKTRDY bit to send out any residual.  We could complete the
			* residual from the endpoint interrupt, but we are taking extra interrupts
			* in this case and seems to be of little value
			*/
			
			HW_Write16( dc->IoBase, MUSB_TXCSR(ep->num ), txcsr | TXCSR_DMA_REQ_EN );
		}
		dc->dma_funcs.transfer_start( dc->dma_hdl, ep->dma_chidx, td->xfer_buffer_paddr, td->xfer_length, DMA_MODE1 );
	} else {
		// PIO TRANSFER... write packets into the fifo 1 at a time, trigger transmit,
		// take an interrupt and repeat until complete
		
		// disable the dma to make PIO work
		HW_Write16And( dc->IoBase, MUSB_TXCSR( ep->num ), ~(TXCSR_DMA_REQ_EN) & 0xffff );
		
		// only transfer 1 usb packet at a time
		len = td->xfer_length - td->bytes_xfered;
		if ( len > ep->mps ) {
			len = ep->mps;
		} 
		td->req_xfer_len = len;	

		MUSB_SLOGF_ISR_DBG(dc, _SLOG_DEBUG1, "%s: len = %d ", __func__ , len);

		if ( USE_EXTERNAL_DMA( dc, ep ) && ( len > MIN_EDMA_LEN ) ) {
			dc->dma_funcs.transfer_start( dc->dma_hdl, ep->dma_chidx, td->xfer_buffer_paddr + td->bytes_xfered, len, DMA_MODE_EXT );
		} else {
			write_to_fifo( dc, ep->num , len,  (uint8_t *) (td->xfer_buffer + td->bytes_xfered) );
			HW_Write16Or( dc->IoBase, MUSB_TXCSR( ep->num ), TXCSR_TXPKTRDY );
		}
	}
	
}

static void 
ep_receive( dctrl_t *dc, ep_ctx_t 	*ep ) {
	musb_transfer_t *td = ep->td;

	/* We cannot force an endpoint to NAK with the controller, so it is
	* possible the enpoint has already received a packet *before* the ep_receive()
	* was called. Therefore, see if we have already receive a packet for processing
	* This is most likely on the first packet, because we cannot use the poor man's NAK
	* because of boundary condition that there is no 0th packet
	*/
	if (atomic_clr_value(&ep->flags, EP_FLAG_DEFER_OUT_PKT_PROCESSING) & EP_FLAG_DEFER_OUT_PKT_PROCESSING) {
		uint16_t			rxcount;

		MUSB_SLOGF_ISR_DBG(dc, _SLOG_DEBUG1, "%s: Start defered processing for epnum = %d",__func__, ep->num );		

		// how many bytes are in the fifo
		rxcount = HW_Read16( dc->IoBase, MUSB_RXCOUNT( ep->num ) );
		td->bytes_xfered += rxcount;
		if ( ( td->bytes_xfered >= td->xfer_length) || ( rxcount != ep->mps) ) {
			iousb_transfer_t *urb = td->urb;
			// either we recevied all the data we wanted, or we received a short packet,
			// ... either way, we are done receiving data  and punt it up the stack
			atomic_set(&ep->flags, EP_FLAG_DEFER_OUT_PKT_CLEAR);
			SIMPLEQ_REMOVE_HEAD(&ep->transfer_q, link);
			// we have sent/received everyting ... complete the urb
			urb->actual_len = td->bytes_xfered;
			SIMPLEQ_INSERT_TAIL( &dc->transfer_free_q, td, link );
			ep->td = NULL;
			MUSB_UNLOCK

			read_from_fifo( dc, ep->num, rxcount, (uint8_t*) (td->xfer_buffer + td->bytes_xfered - rxcount) );
			
			MUSB_MUTEX_UNLOCK
			urb->urb_complete_cbf( ep->iousb_ep, urb, 0, 0);
			MUSB_MUTEX_LOCK
			// !!! Don't clear the RXPKTRDY bit here!!!! clear it when we
			// receive the next out transfer function as a poor man's NAK
		
		} else {
			MUSB_UNLOCK
			read_from_fifo( dc, ep->num, rxcount, (uint8_t*) (td->xfer_buffer + td->bytes_xfered - rxcount) );

			// more data to receive
			// clear the rxpktrdy bit *after* rx_cleanup to make sure a new
			// packet isn't received and processed by the dma engine accidentally.			
			HW_Write16And( dc->IoBase, MUSB_RXCSR( ep->num ), ~RXCSR_RXPKTRDY );	
		}
	} else {
		MUSB_UNLOCK
		if (atomic_clr_value(&ep->flags, EP_FLAG_DEFER_OUT_PKT_CLEAR) & EP_FLAG_DEFER_OUT_PKT_CLEAR) {
			// clear the RXPKTRDY bit for the previously received pkt as a way 
			// to clear the poor man's NAK
			HW_Write16And( dc->IoBase, MUSB_RXCSR( ep->num ), ~RXCSR_RXPKTRDY );	
		}
		if ( USE_BUILTIN_DMA( dc, ep ) ) {
			dc->dma_funcs.transfer_start( dc->dma_hdl, ep->dma_chidx, td->xfer_buffer_paddr, td->xfer_length, DMA_MODE1 );
		} 
	}
}

static void 
ep_isoch_transmit( dctrl_t *dc, ep_ctx_t *ep ) 
{
	musb_transfer_t *td = NULL;
	uint8_t 	*buf;
	int   		len;

	if ( (td = SIMPLEQ_FIRST( &ep->transfer_q)) == NULL ) {
		return ; 
	}
	ep->td = td;	

	// (assumption here is that frame_len is always < mps)
	len = td->isoch_flist[td->isoch_cur_idx].frame_len;
	buf = (uint8_t *)(td->xfer_buffer + td->bytes_xfered);
	td->req_xfer_len = len;	
	HW_Write16And( dc->IoBase, MUSB_TXCSR( ep->num ), (~(TXCSR_DMA_REQ_EN) & 0xffff)| TXCSR_ISO );
	
	write_to_fifo( dc, ep->num , len,  buf );

	HW_Write16Or( dc->IoBase, MUSB_TXCSR( ep->num ), TXCSR_TXPKTRDY );
	
	// mark frame as complete successfully because we have no
	// way of knowing whether xfer was successful or not... isoch is
	// best effort
	td->isoch_flist[td->isoch_cur_idx].frame_status = 0;
	// update the frame index and total xfered length
	td->bytes_xfered += len;
	td->isoch_cur_idx++;
}

static void 
ep_isoch_receive( dctrl_t *dc, ep_ctx_t *ep ) 
{
	musb_transfer_t *td = NULL;
	int   		len;
	uint16_t	csr;
	int			idx = ep->num;
	
	if ( (td = SIMPLEQ_FIRST( &ep->transfer_q)) == NULL ) {
		return ; 
	}
	ep->td = td;	
	csr = HW_Read16( dc->IoBase, MUSB_RXCSR( idx ) );
	if( csr & RXCSR_RXPKTRDY ){
		// how many bytes are in the fifo
		if ( csr & RXCSR_DATA_ERROR) {
			td->isoch_flist[td->isoch_cur_idx].frame_len = 0;
			td->isoch_flist[td->isoch_cur_idx].frame_status = USBD_STATUS_CRC_ERR;
			flush_fifo(dc, ep);
		} else {
			len = HW_Read16( dc->IoBase, MUSB_RXCOUNT( idx ) );
			td->isoch_flist[td->isoch_cur_idx].frame_status = 0;
			read_from_fifo( dc, idx, len, (uint8_t*) (td->xfer_buffer + td->isoch_frame_offset ) );
			/* Advance the isoch_frame_offset by the requested frame length */
			td->isoch_frame_offset += td->isoch_flist[td->isoch_cur_idx].frame_len;
			/* Update the frame_len to reflect the real length read */
			td->isoch_flist[td->isoch_cur_idx].frame_len = len;
			td->bytes_xfered += len;
		}
		td->isoch_cur_idx++;
		// clear the rxpktrdy bit to get a new pkt.			
		HW_Write16And( dc->IoBase, MUSB_RXCSR( idx ), ~RXCSR_RXPKTRDY );	
	}
}

static void process_ep0_interrupt( dctrl_t *dc ) {
	uint16_t	csr;
	uint16_t	count;
	ep_ctx_t 	*ep = &dc->ep0;
	musb_transfer_t *td = ep->td;
	static bool	fAddrPending = false;
	static uint8_t fAddr = 0x0;

	csr = HW_Read16( dc->IoBase, MUSB_CSR0 );
	
	if ( csr & CSR0_SENTSTALL ) {
		HW_Write16( dc->IoBase, MUSB_CSR0, 0 );
	}

	if ( csr & CSR0_SETUPEND ) {
		HW_Write16( dc->IoBase, MUSB_CSR0, CSR0_SERVICED_SETUP_END );

		MUSB_SLOGF_ISR_DBG(dc, _SLOG_ERROR, "%s: CSR0_SETUPEND set", __func__ );

#if 0	// what do we do on early termination??? 
		ep->flags &= ~EP_FLAG_DEFER_SETUP_PKT_CLEAR;
		dc->ep0.ep0_state = EP0_STATE_IDLE;	
		dc->ep0.ep0_idle_substate = EP0_IDLE_SUBSTATE_SETUP_PHASE;
#endif	
		// reset the flag if the status stage for the SET_ADDRESS command didn't complete
		fAddrPending = false;
	}

	if( ep->ep0_state == EP0_STATE_IDLE && fAddrPending ) {
		ep->ep0_idle_substate = EP0_IDLE_SUBSTATE_SETUP_PHASE;

		HW_Write8( dc->IoBase, MUSB_FADDR, (uint8_t) fAddr );
		fAddrPending = false;
		return;
	}
	
	switch ( ep->ep0_state ) {
		case EP0_STATE_IDLE:
			MUSB_SLOGF_ISR_DBG(dc, _SLOG_DEBUG1, "%s: state = EP0_STATE_IDLE csr = 0x%x",__func__, csr );		

			if ( csr & CSR0_RXPKTRDY ) {

				if ( ep->ep0_idle_substate == EP0_IDLE_SUBSTATE_SETUP_PHASE ) {
					uint8_t *setup_packet = &dc->setup_packet[0];

					MUSB_SLOGF_ISR_DBG(dc, _SLOG_DEBUG1, "%s: idle_substate = EP0_IDLE_SUBSTATE_SETUP_PHASE",__func__);		

					count = HW_Read16( dc->IoBase, MUSB_COUNT0 );
					
					if ( count == SIZE_SETUP_REQUEST ) {
						uint8_t  bmRequestType;
			
						// read the setup packet from the FIFO
						read_from_fifo( dc, 0, count, setup_packet );

						MUSB_SLOGF_ISR_DBG(dc, _SLOG_DEBUG1, "%s: count = %d rxpkt = 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x"
							, __func__ , count ,setup_packet[0], setup_packet[1], setup_packet[2], setup_packet[3]
							, setup_packet[4], setup_packet[5], setup_packet[6], setup_packet[7] );
						
						/* The device controller will automatically complete the status phase.
						 * This is OK when the control transfer has a dataphase because the status phase won't
						 * complete until the data phase is explicitly terminated by the Driver by hitting
						 * "dataend".  However, this is a real problem form Control transfer with no data phases.
						 * In the case of USB_SET_CONFIGURATION because we want the function drivers to have a chance
						 * to configure themselves before completing the status phase. This is also the case
						 * for CLEAR_ENDPOINT_FEATURE which clears an endpoint stall.  The Host may
						 * think the stall is cleared before the driver actually gets the chance to clear it.
						 * Therefore, we will use the clearing of CSR0_SERVICED_RXPKTRDY to back-pressure the HOST in the
						 * case where the control transfer has no data phase
						 *
						 * Also, generalize the use of CSR0_SERVICED_RXPKTRDY for all host-device control transfers.  
						 * The reason for this is we cannot currently explicitly NAK the data-out phase of a control transfer,
						 * so we can use this as a poor man's NAK.  The reason this is usefull is in the case we can't to
						 * stall the data-out dataphase for a protocol stall on EP0
						 */
						bmRequestType = setup_packet[0];
						if ( ( bmRequestType & DIR_MASK) == HOST_TO_DEVICE ) {
							uint8_t  bRequest;

							// control transfer with no data phase, or out-data phase
							// defer until status phase completed by transfer function
							MUSB_SLOGF_ISR_DBG(dc, _SLOG_DEBUG1, "%s: defer setup pkt clear",__func__);		

							bRequest = setup_packet[1];
							if( bRequest == USB_SET_ADDRESS ) {
								fAddr = setup_packet[2];
								fAddrPending = true;
								HW_Write16( dc->IoBase, MUSB_CSR0, CSR0_SERVICED_RXPKTRDY | CSR0_DATAEND );
							} else {
								atomic_set(&ep->flags, EP_FLAG_DEFER_SETUP_PKT_CLEAR);
							}
						} else {
							// clear the rxpktrdy bit by setting the serviced bit as described in the ref manual
							HW_Write16( dc->IoBase, MUSB_CSR0, CSR0_SERVICED_RXPKTRDY );
						}

						/* transition the substate to the STATUS_PHASE because we will hit the status phase
						 * before the next SETUP phase regarless of whether the control transfer
						 * contains a data phase or not
						 */
						ep->ep0_idle_substate = EP0_IDLE_SUBSTATE_STATUS_PHASE;
#ifdef MUSB_REAL_ISR
						atomic_set(&dc->usb_status, USB_EP0_SETUP_PULSE);
#else
						MUSB_MUTEX_UNLOCK
						dc->udc->usbdc_self->usbdc_setup_packet_process( dc->udc, setup_packet );
						MUSB_MUTEX_LOCK
#endif
						
					} else {
						
						if ( count > 0 ) {
							// Not sure what this data is becuase we are in the setup phase,
							// but the length is not 8 bytes so read it, but don't send the 
							// data anywhere, but log an error
							MUSB_SLOGF_ISR_DBG(dc, _SLOG_ERROR, "%s: Expected 8 bytes in the SETUP PHASE, but received %d... drop the data",__func__, count );	
							while ( count-- ) {
								HW_Read8( dc->IoBase, FIFO(0) );
							}
							HW_Write16( dc->IoBase, MUSB_CSR0, CSR0_SERVICED_RXPKTRDY );
						} 
/*						else {   
							
							It is possible and valid to get here via the following program flow:
							1. received a setup packet with an OUT data phase
							2. the setup packet rxpktrdy bit clear was delayed to give the stack the ability to stall, or nak until ready
							3. Stack stalls dataout phase
							!!!key!!!: the stall code doesn't attempt to clear the rxpktrdy bit, becase there is a race condition:
							The controller will accept the next setup packet before clearing the previous one, presumably because there is room in the fifo
							and the controller is not allowed to nak setup packets.  Clearing the rxpktrdy bit at this point could clobber the next
							setup packet
							4.  Receive a sentstall interrupt and clear it.
							5.  finally, we hit this else clause because we havven't cleared the rxpktrdy bit from the 
							last setup packet, and we havent't yet received the next setup packet.  We simply exit the handler and wait
							for the next ep0 interrupt
						}
*/


					}
				} else {
					MUSB_SLOGF_ISR_DBG(dc, _SLOG_DEBUG1, "%s: receive a packed in the IDLE state, but idle_substate != EP0_IDLE_SUBSTATE_SETUP_PHASE... defer processing",__func__);		
					atomic_set(&ep->flags, EP_FLAG_DEFER_SETUP_PKT_PROCESSING);
				}
			}

			break;	
		
		case EP0_STATE_TXDATA:
			MUSB_SLOGF_ISR_DBG(dc, _SLOG_DEBUG1, "%s: state = EP0_STATE_TXDATA csr = 0x%x",__func__, csr );		
			// still in the TXDATA state, so this means that we must have more data 
			// to send to the host
			if( td != NULL){
				td->bytes_xfered += td->req_xfer_len;
				ep0_transmit( dc );
			}else{
				MUSB_SLOGF_ISR_DBG(dc, _SLOG_ERROR, "%s(%d): NO URB", __func__, __LINE__ );
			}			
			
			break;	
	
		case EP0_STATE_RXDATA:
			MUSB_SLOGF_ISR_DBG(dc, _SLOG_DEBUG1, "%s: state = EP0_STATE_RXDATA csr = 0x%x",__func__, csr );		
			if ( csr & CSR0_RXPKTRDY ) {
				if ( td == NULL ) {
					// should not hit this case because we are delaying the the clearing of the setup packet
					// until the dataout dataphase transfer function is called
					MUSB_SLOGF_ISR_DBG(dc, _SLOG_ERROR, "%s(%d): NO URB", __func__, __LINE__ );
				} else {
					count =  HW_Read16( dc->IoBase, MUSB_COUNT0 );
					read_from_fifo( dc, 0, count, (uint8_t*) (td->xfer_buffer + td->bytes_xfered ) );
					td->bytes_xfered += count;
							
					if ( ( td->bytes_xfered >= td->xfer_length) || ( count != ep->mps) ) {
						// either we recevied all the data we wanted, or we received a short packet,
						// ... either way, we are done receiving data  and punt it up the stack
						HW_Write16( dc->IoBase, MUSB_CSR0, CSR0_SERVICED_RXPKTRDY | CSR0_DATAEND );

						complete_urb( dc, ep, 0 );	
						ep->ep0_state = EP0_STATE_IDLE;
					} else {
						// more data to receive
						MUSB_SLOGF_ISR_DBG(dc, _SLOG_DEBUG1, "%s: More Data to receive  xfer_length = %d bytes_xfered = %d ", __func__, td->xfer_length, td->bytes_xfered  );
						HW_Write16( dc->IoBase, MUSB_CSR0, CSR0_SERVICED_RXPKTRDY );

						// nothing more to do... the controller will accept the next receive packet
						// and transfer it to the urb buffer in the interrupt handler.
					}
				}
			}
			break;	
	}
}

static void process_epin_interrupt( dctrl_t *dc, unsigned epidx ) {
	ep_ctx_t			*ep = &dc->epin_arr[epidx];
	musb_transfer_t     *td; 
	uint16_t			txcsr;
	
	txcsr = HW_Read16( dc->IoBase, MUSB_TXCSR( epidx ) );
	if ( txcsr & ( TXCSR_SENT_STALL | TXCSR_UNDERRUN ) ) {
		HW_Write16And( dc->IoBase, MUSB_TXCSR( ep->num ), ~(TXCSR_SENT_STALL | TXCSR_UNDERRUN ) );		
	} 

	if(ep->type == USB_ATTRIB_ISOCHRONOUS){
		MUSB_LOCK;
		if ( (td = SIMPLEQ_FIRST( &ep->transfer_q)) != NULL ) {
			if ( td->isoch_cur_idx >= td->isoch_nframes ) {
				td->status = 0;
				td->urb->actual_len = td->bytes_xfered;
				SIMPLEQ_REMOVE(&ep->transfer_q, td, _musb_transfer, link ); 
				SIMPLEQ_INSERT_TAIL( &dc->transfer_complete_q, td, link );
			}
		}
		MUSB_UNLOCK
		//try if we have another one to schedule
		ep_isoch_transmit(dc,ep);
	}else if((td = ep->td)){
		td->bytes_xfered += td->req_xfer_len;
		if ( td->bytes_xfered < td->xfer_length) {
				ep_transmit( dc, ep );
		} else {
				complete_urb( dc, ep, 0 );	
		}
	} 
}

static void process_epout_interrupt( dctrl_t *dc, unsigned epidx ) {
	uint16_t			rxcsr;
	uint16_t			rxcount;
	ep_ctx_t			*ep = &dc->epout_arr[epidx];
	musb_transfer_t     *td;
	
	if(ep->type == USB_ATTRIB_ISOCHRONOUS){
		ep_isoch_receive(dc,ep);
		MUSB_LOCK
		if ( (td = SIMPLEQ_FIRST( &ep->transfer_q)) != NULL ) {
			if ( td->isoch_cur_idx >= td->isoch_nframes ) {
				td->status = 0;
				td->urb->actual_len = td->bytes_xfered;
				SIMPLEQ_REMOVE(&ep->transfer_q, td, _musb_transfer, link ); 
				SIMPLEQ_INSERT_TAIL( &dc->transfer_complete_q, td, link );
			}
		}
		MUSB_UNLOCK
	}else{
		MUSB_LOCK
		rxcsr = HW_Read16( dc->IoBase, MUSB_RXCSR( epidx ) );
		if ( rxcsr & RXCSR_SENT_STALL ) {
			HW_Write16And( dc->IoBase, MUSB_RXCSR( ep->num ), ~RXCSR_SENT_STALL );
		}
		if( rxcsr & RXCSR_RXPKTRDY ) {
			td = ep->td;
			if ( USE_BUILTIN_DMA( dc, ep ) && ( ( ep->flags & EP_FLAG_DEFER_OUT_PKT_PROCESSING ) == 0 ) && td != NULL ) {
				// do DMA post processing... 2 possible scenarios
				// Case1. DMA did not fully complete 
				// ( short packet received and didn't trigger an interrupt ),
				// so figure out how much data was DMA'ed  
				//
				// Case2:  DMA fully commpleted ( last packet was mps ), and urb
				// buffer is completely filled and needs to be completed, but the
				// a short packet was received ( belonging to the next transfer ) 
				// and generated this interrupt first... In this case, the rx_complete()
				// will complete the previous urb and set ep->urb to NULL.  Therefore,
				// the packet which generated this interrupt will be processed via
				// DEFERED PROCESSING.
				//
				// !!!WARNING!!!. We do no call rx_complete() when we are doing defered processing,
				// because the packet was received before the dma could have potentially started and thus 
				// was not started in the ep_recieve() transfer function
				td->bytes_xfered = dc->dma_funcs.rx_complete( dc->dma_hdl, ep->dma_chidx );
			} 
			
			if ( (td = ep->td) == NULL ) {
				atomic_set(&ep->flags, EP_FLAG_DEFER_OUT_PKT_PROCESSING);
				MUSB_UNLOCK
			} else {
				MUSB_UNLOCK

				// how many bytes are in the fifo
				rxcount = HW_Read16( dc->IoBase, MUSB_RXCOUNT( epidx ) );

				if ( USE_EXTERNAL_DMA( dc, ep ) && ( rxcount > MIN_EDMA_LEN ) ) {
					dc->dma_funcs.transfer_start( dc->dma_hdl, ep->dma_chidx, td->xfer_buffer_paddr + td->bytes_xfered, rxcount, DMA_MODE_EXT );
				} else {
					read_from_fifo( dc, epidx, rxcount, (uint8_t*) (td->xfer_buffer + td->bytes_xfered ) );
					td->bytes_xfered += rxcount;
					
					if ( ( td->bytes_xfered >= td->xfer_length) || ( rxcount != ep->mps) ) {
						// either we recevied all the data we wanted, or we received a short packet,
						// ... either way, we are done receiving data  and put it up the stack
						atomic_set(&ep->flags, EP_FLAG_DEFER_OUT_PKT_CLEAR);
						complete_urb( dc, ep, 0 );	

						// !!! Don't clear the RXPKTRDY bit here!!!! clear it when we
						// receive the next out transfer function as a poor man's NAK

					} else {
						// more data to receive

						// clear the rxpktrdy bit *after* rx_cleanup to make sure a new
						// packet isn't received and processed by the dma engine accidentally.			
						HW_Write16And( dc->IoBase, MUSB_RXCSR( epidx ), ~RXCSR_RXPKTRDY );	
					}
				}
			}
		}else{
			MUSB_UNLOCK
		}
	}
}

void process_devstate_interrupt( dctrl_t *dc )
{
	uint8_t			power;
	usbdc_device_t  *udc = dc->udc;
	int i;
	unsigned usb_intstatus = atomic_clr_value(&dc->usb_status, (USB_DEV_STATE_CHAGE|USB_EP0_SETUP_PULSE));

	musb_slogf(dc, _SLOG_INFO, "%s: usb_intstatus = %x",__func__, usb_intstatus);	

	if ( usb_intstatus & INTRUSB_RESET ) {
		MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: RESET INTERRUPT ",__func__ );

		// check to see if transfer in progress
		// this is to determine user impact of a hardware issue when transferring 0xAAAA pattern to the host
		for( i=0; i < dc->musbmhdrc_cfg.n_endpoints; i++ ) {
			if( dc->epin_arr[i].td && dc->epin_arr[i].td->urb != 0 ) {
				musb_slogf(dc, _SLOG_WARNING, "%s: Got RESET INTERRUPT while ep %d still in progress", __func__, i );
			}
		}

		// Use the USB Reset Interrupt as an indication that the device
		// was inserted. 
		if ( ( dc->flags & DC_FLAG_CONNECTED ) == 0 ) {
			dc->flags |= DC_FLAG_CONNECTED;
			
			udc->usbdc_self->usbdc_device_state_change( udc, IOUSB_DEVICE_STATE_INSERTED);
			musb_slogf(dc, _SLOG_INFO, "%s: IOUSB_DEVICE_STATE_INSERTED", __func__ );	
		}
		
		// check and report the speed
		power = HW_Read8( dc->IoBase, MUSB_POWER );
		if ( power & POWER_HS_MODE ) {
			musb_slogf(dc, _SLOG_INFO, "%s: HIGH SPEED DETECTED", __func__ );	
			udc->usbdc_self->usbdc_set_device_speed( udc, IOUSB_DEVICE_HIGH_SPEED ); 
			
		} else {
			musb_slogf(dc, _SLOG_INFO, "%s: FULL SPEED DETECTED", __func__ );
			udc->usbdc_self->usbdc_set_device_speed( udc, IOUSB_DEVICE_FULL_SPEED ); 
		}
		
		// report the reset *after* the speed has been reported
		udc->usbdc_self->usbdc_device_state_change( udc, IOUSB_DEVICE_STATE_RESET );

		// Capture the fact that EP0 is back in the IDLE state after a USB reset 
		dc->ep0.ep0_state = EP0_STATE_IDLE;
		dc->ep0.ep0_idle_substate = EP0_IDLE_SUBSTATE_SETUP_PHASE;
	}
	
	if ( usb_intstatus & INTRUSB_DISCON ) {
		MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: DISCONNECT INTERRUPT ",__func__ );
		
		udc->usbdc_self->usbdc_device_state_change( udc, IOUSB_DEVICE_STATE_REMOVED);
		dc->flags &= ~DC_FLAG_CONNECTED;
	} 
	
	if ( usb_intstatus & INTRUSB_SUSPEND ) {
		if ( dc->flags & DC_FLAG_CONNECTED ) {
			MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: SUSPEND INTERRUPT ",__func__ );
			if ( udc->usbdc_self->usbdc_device_state_change( udc, IOUSB_DEVICE_SUSPEND_REQUEST) == EOK ) { 
			}
			udc->usbdc_self->usbdc_device_state_change( udc, IOUSB_DEVICE_STATE_SUSPENDED );
		} else {
			MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: SPURIOUS SUSPEND INTERRUPT... we are not connected",__func__ );
		}
	}
	
	if ( usb_intstatus & INTRUSB_RESUME ) {
		MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: RESUME INTERRUPT ",__func__ );
		udc->usbdc_self->usbdc_device_state_change( udc, IOUSB_DEVICE_STATE_RESUMED );
	} 

	if( usb_intstatus & USB_EP0_SETUP_PULSE){
		uint8_t *setup_packet = &dc->setup_packet[0];
		MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: idle_substate = EP0_IDLE_SUBSTATE_SETUP_PHASE",__func__); 	
		MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: rxpkt = 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x"
				, __func__ , setup_packet[0], setup_packet[1], setup_packet[2], setup_packet[3]
				, setup_packet[4], setup_packet[5], setup_packet[6], setup_packet[7] );
		MUSB_MUTEX_UNLOCK
		dc->udc->usbdc_self->usbdc_setup_packet_process( dc->udc, setup_packet );
		MUSB_MUTEX_LOCK
	}
}

#ifdef MUSB_REAL_ISR
int
musb_pulse_handler( dctrl_t *dc )
{
	struct _pulse 			pulse;
	int 					error;
	ep_ctx_t 				*ep;
	musb_transfer_t 		*td;
	iousb_transfer_t        *urb;


#if (defined(_NTO_VERSION) && (_NTO_VERSION >= 632))
	/* set event handler thread name */
	if ( (error = pthread_setname_np(0, "musb_pulse_handler" ) ) != EOK ) {
		musb_slogf(dc, 1,"%s : pthread_setname_np() failed ( error = %d )", __func__, error);
	}
#endif

	if ( ThreadCtl(_NTO_TCTL_IO, NULL) == -1) {
		musb_slogf( dc, 1, "%s: Unable to obtain I/O privity.", __func__);  
    	return( -1 ); 
	}

	SIGEV_PULSE_INIT( &dc->intr_event, dc->coid, dc->priority, MUSB_PULSE_INTR, 0 );

	if ( (dc->intr_id = InterruptAttach( dc->irq , musb_interrupt, 
					dc->udc, sizeof(usbdc_device_t), _NTO_INTR_FLAGS_TRK_MSK) ) == -1 ) {
		musb_slogf( dc, 1, "%s: InterruptAttach failed.", __func__);
		return( -1 ); 
	}

	// wake up main thread we are ready to go
   	pthread_sleepon_lock( );
	dc->flags |= DC_FLAG_INITIALIZED;
	pthread_sleepon_signal( &dc->flags );
	pthread_sleepon_unlock( );

	while ( 1 ) {
		if ( MsgReceivePulse(dc->chid, &pulse, sizeof(pulse), NULL) == -1 ) {
			break;
		}
		switch ( pulse.code ) {
			case MUSB_PULSE_INTR:
				{
					MUSB_MUTEX_LOCK
					if(dc->usb_status){
						process_devstate_interrupt(dc);
					}
					MUSB_MUTEX_UNLOCK

					MUSB_LOCK
					do {
						td = SIMPLEQ_FIRST( &dc->transfer_complete_q );
						if ( td != NULL ) {
							SIMPLEQ_REMOVE_HEAD( &dc->transfer_complete_q, link );
							MUSB_UNLOCK
							ep = td->ep;
							urb = td->urb;

							MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "URB COMPLETE epnum = 0x%x epdir = 0x%x urb->actual_len = %d ",ep->num, ep->dir , urb->actual_len);
							
							urb->urb_complete_cbf( ep->iousb_ep, urb, td->status, 0);
							
							MUSB_LOCK;
							SIMPLEQ_INSERT_TAIL( &dc->transfer_free_q, td, link );
						} else {
						}
					} while( td );
					MUSB_UNLOCK
				}
				break;
				
			case MUSB_PULSE_EVENT_CANCELLATION:
				pthread_exit(NULL);

			default:
				musb_slogf( dc, 1, "%s: Unknown pulse", __func__);  
				break;
		}
	}

	return 0 ;
}

int musb_create_interrupt_handler( dctrl_t *dc )
{
	pthread_attr_t         	pattr;
	struct sched_param    	param;
	int                     status;

	if( (dc->chid = ChannelCreate( _NTO_CHF_DISCONNECT )) < 0 ) {
		musb_slogf( dc, 0, "%s: Unable to create channel",__func__);
		status = dc->chid;
		goto fail;
	}

 	if( (dc->coid = ConnectAttach( 0, 0, dc->chid, _NTO_SIDE_CHANNEL, 0 )) < 0 ) {
		musb_slogf( dc, 0, "%s: Unable to connect to channel", __func__);
		status = dc->coid;
		goto fail2;
	}

	pthread_attr_init(&pattr);
	pthread_attr_setschedpolicy(&pattr, SCHED_RR);
	param.sched_priority = dc->priority;
	pthread_attr_setschedparam(&pattr, &param);
	pthread_attr_setinheritsched(&pattr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setdetachstate( &pattr, PTHREAD_CREATE_DETACHED );

	if((status = pthread_create( &dc->tid, &pattr, (void *) musb_pulse_handler, dc )) != EOK){
		musb_slogf( dc, 0, "%s : Unable to create interrupt thread", __func__);
		goto fail3; 
    }

	//wait for the handler thread ready to go.
	pthread_sleepon_lock( );	
	while( !(dc->flags & DC_FLAG_INITIALIZED)) {
		pthread_sleepon_wait( &dc->flags );
	}
	pthread_sleepon_unlock( );	
	
	return EOK;
	
fail3:
	ConnectDetach( dc->coid );
fail2:
	ChannelDestroy( dc->chid );
fail:
	return status;
}

const   struct sigevent *
musb_interrupt( void *area, int id )
{
	usbdc_device_t *udc = (usbdc_device_t *) area;
#else
uint32_t musb_interrupt( usbdc_device_t *udc )
{
#endif
	dctrl_t  	*dc = udc->dc_data;
	uint8_t		usb_intstatus;
	uint16_t	tx_intstatus;
	uint16_t	rx_intstatus;
	int i;
	
#ifndef MUSB_REAL_ISR
	MUSB_MUTEX_LOCK
#endif

#ifdef EXTERNAL_INTERRUPT_PROCESSING
	musb_get_ext_intstatus( dc, &rx_intstatus, &tx_intstatus, &usb_intstatus);
#else
	// read interrupt status registers
	usb_intstatus = HW_Read8( dc->IoBase, MUSB_INTRUSB );
	tx_intstatus = HW_Read16( dc->IoBase, MUSB_INTRTX );
	rx_intstatus = HW_Read16( dc->IoBase, MUSB_INTRRX );
#endif

	MUSB_SLOGF_ISR_DBG(dc, _SLOG_DEBUG1, "%s:  usb_intstatus = 0x%x rx_intstatus = 0x%x tx_intstatus = 0x%x ",__func__, usb_intstatus, rx_intstatus, tx_intstatus);

	if(usb_intstatus & (USB_DEV_STATE_CHAGE)){
		atomic_set(&dc->usb_status, (usb_intstatus & USB_DEV_STATE_CHAGE));
	}

	if ( tx_intstatus & EP_INT_MSK( 0 ) ) {
		// ep0 interrupt
		tx_intstatus &= ~EP_INT_MSK( 0 );  //clr
		process_ep0_interrupt( dc );
	}

	// process epin interrupts
	if ( tx_intstatus ) {
		for(i=1; i < dc->musbmhdrc_cfg.n_endpoints; i++ ) {
			if ( tx_intstatus & EP_INT_MSK( i ) ) {
				process_epin_interrupt( dc, i );
			}
		}
	}
	
	// process epout interrupts
	if ( rx_intstatus ) {
		for(i=1; i < dc->musbmhdrc_cfg.n_endpoints; i++ ) {
			if ( rx_intstatus & EP_INT_MSK( i ) ) {
				process_epout_interrupt( dc, i );
			}
		}
	}
	
#ifdef EXTERNAL_INTERRUPT_PROCESSING
	musb_clr_ext_int( dc );
#endif
	
#ifdef MUSB_REAL_ISR
	if ( SIMPLEQ_FIRST( &dc->transfer_complete_q) || dc->usb_status) {
		return &dc->intr_event;
	}
	return NULL ;
#else
	if(dc->usb_status){
		process_devstate_interrupt(dc);
	}

	MUSB_MUTEX_UNLOCK
	return EOK;
#endif	
}


uint32_t
musb_set_device_address(  usbdc_device_t *udc, uint32_t address )
{
	dctrl_t  	*dc = udc->dc_data;

	musb_slogf(dc, _SLOG_INFO, "%s: address = 0x%x", __func__, address);
	
	/* the status phase is completed automatically by the device controller,
	 * but there is no transfer function issued by io-usb so transition
	 * the substate such that the next setup packet can be processed
	 */
	dc->ep0.ep0_idle_substate = EP0_IDLE_SUBSTATE_SETUP_PHASE;

	return EOK; 
}


uint32_t
musb_select_configuration( usbdc_device_t *udc, uint8_t config )
{
	dctrl_t  *dc = udc->dc_data;
		
	musb_slogf(dc, _SLOG_INFO, "%s: config = 0x%x", __func__, config);
	
	return EOK;
}


uint32_t
musb_set_endpoint_state( usbdc_device_t *udc, iousb_endpoint_t *iousb_ep, uint32_t ep_state )
{
	dctrl_t 		*dc 		= udc->dc_data;
	ep_ctx_t		*ep = iousb_ep->user;
	
	musb_slogf(dc, _SLOG_INFO, "%s: state = %d",__func__, ep_state );
	
	MUSB_MUTEX_LOCK
	
	switch ( ep_state ) {
		case IOUSB_ENDPOINT_STATE_READY :
			musb_slogf(dc, _SLOG_INFO, "%s: IOUSB_ENDPOINT_STATE_READY epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;
		
		case IOUSB_ENDPOINT_STATE_STALLED :
			musb_slogf(dc, _SLOG_INFO, "          STALL epnum = 0x%x",  iousb_ep->edesc.bEndpointAddress);
			if ( ep->num == 0 ) {
				HW_Write16( dc->IoBase, MUSB_CSR0, CSR0_SENDSTALL  );

				// expecting a setup packet next
				atomic_clr(&ep->flags, EP_FLAG_DEFER_SETUP_PKT_CLEAR);
				dc->ep0.ep0_state = EP0_STATE_IDLE;	
				dc->ep0.ep0_idle_substate = EP0_IDLE_SUBSTATE_SETUP_PHASE;
			} else {
				if ( ep->dir == USB_ENDPOINT_IN ) {
					// this bit must be cleared by the CPU.. not autocleared
					HW_Write16Or( dc->IoBase, MUSB_TXCSR( ep->num ), TXCSR_SEND_STALL );
				} else {
					// this bit must be cleared by the CPU.. not autocleared
					HW_Write16Or( dc->IoBase, MUSB_RXCSR( ep->num ), RXCSR_SEND_STALL );
				}
			}
			
			break;
		
		case IOUSB_ENDPOINT_STATE_RESET :
			musb_slogf(dc, _SLOG_INFO, "%s: IOUSB_ENDPOINT_STATE_RESET epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;
		case IOUSB_ENDPOINT_STATE_ENABLE :
			musb_slogf(dc, _SLOG_INFO, "%s: IOUSB_ENDPOINT_STATE_ENABLE epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;

		case IOUSB_ENDPOINT_STATE_DISABLED :
			musb_slogf(dc, _SLOG_INFO, "%s: IOUSB_ENDPOINT_STATE_DISABLED epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;
		
		case IOUSB_ENDPOINT_STATE_NAK :
			musb_slogf(dc, _SLOG_INFO, "%s: IOUSB_ENDPOINT_STATE_NAK epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;
		

		default :
			break;
	}
	
	MUSB_MUTEX_UNLOCK
	
	return EOK;
}

uint32_t
musb_clear_endpoint_state( usbdc_device_t *udc, iousb_endpoint_t *iousb_ep, uint32_t ep_state )
{
	dctrl_t 		*dc = udc->dc_data;
	ep_ctx_t		*ep = iousb_ep->user;
	uint16_t		csr;
	
	MUSB_MUTEX_LOCK
	
	switch ( ep_state ) {
		case IOUSB_ENDPOINT_STATE_READY :
			musb_slogf(dc, _SLOG_INFO, "%s: CLEAR IOUSB_ENDPOINT_STATE_READY  epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;
		
		case IOUSB_ENDPOINT_STATE_STALLED :
			musb_slogf(dc, _SLOG_INFO, "          Clear STALL  epnum = 0x%x",  iousb_ep->edesc.bEndpointAddress);

			// ep0 stall are self clearing
			if ( ep != NULL && ep->num != 0 ) {
				if ( ep->dir == USB_ENDPOINT_IN ) {
					csr = HW_Read16( dc->IoBase, MUSB_TXCSR( ep->num ) ) & ~( TXCSR_SEND_STALL | TXCSR_SENT_STALL );
					csr |= TXCSR_CLR_DATA_TOGGLE;
					HW_Write16( dc->IoBase, MUSB_TXCSR( ep->num ), csr );
					MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "  tried to clear actual stall txcsr = 0x%x" , HW_Read16( dc->IoBase, MUSB_TXCSR( ep->num ) )  );
					
				} else {
					csr = HW_Read16( dc->IoBase, MUSB_RXCSR( ep->num ) ) & ~( RXCSR_SEND_STALL | RXCSR_SENT_STALL | RXCSR_RXPKTRDY );
					csr |= RXCSR_CLR_DATA_TOGGLE;
					HW_Write16( dc->IoBase, MUSB_RXCSR( ep->num ), csr );
					MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "  tried to clear actual stall rxcsr = 0x%x" , HW_Read16( dc->IoBase, MUSB_RXCSR( ep->num ) )  );
				}
			}
			break;
		
		case IOUSB_ENDPOINT_STATE_RESET :
			musb_slogf(dc, _SLOG_INFO, "%s: CLEAR IOUSB_ENDPOINT_STATE_RESET  epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;

		case IOUSB_ENDPOINT_STATE_ENABLE :
			musb_slogf(dc, _SLOG_INFO, "%s: CLEAR IOUSB_ENDPOINT_STATE_ENABLE  epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;

		case IOUSB_ENDPOINT_STATE_DISABLED :
			musb_slogf(dc, _SLOG_INFO, "%s: CLEAR IOUSB_ENDPOINT_STATE_DISABLED  epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;

		case IOUSB_ENDPOINT_STATE_NAK :		
			musb_slogf(dc, _SLOG_INFO, "%s: CLEAR IOUSB_ENDPOINT_STATE_NAK  epnum = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress);
			break;

		default :
			break;
	}        
	
	MUSB_MUTEX_UNLOCK
	
	return EOK;
}



/* call to enable connection to host or signal resume to host */
uint32_t
musb_set_bus_state( usbdc_device_t *udc, uint32_t device_state )
{
	dctrl_t  *dc = udc->dc_data;
	
	MUSB_MUTEX_LOCK
	
	switch ( device_state ) {
	case IOUSB_BUS_STATE_DISCONNECTED :
		HW_Write8And( dc->IoBase, MUSB_POWER, ~POWER_SOFT_CONN );
		udc->usbdc_self->usbdc_device_state_change( udc, IOUSB_DEVICE_STATE_REMOVED);
		dc->flags &= ~DC_FLAG_CONNECTED;
		
		break;
		
	case IOUSB_BUS_STATE_CONNECTED :
		HW_Write8Or( dc->IoBase, MUSB_POWER, POWER_SOFT_CONN );
		break;
		
	case IOUSB_BUS_STATE_RESUME :
		break;
	}
	
	MUSB_MUTEX_UNLOCK
	
	return EOK;
}

/*
	Set port test mode. This must be done witin 3ms of status phase of request.
*/

uint32_t
musb_set_test_mode( dctrl_t *dc, uint16_t wIndex ) 
{
	uint8_t		mode;

	musb_slogf(dc, 7, "%s : 0x%x", __func__, wIndex );

	switch( wIndex ) {	
		case USB_TEST_MODE_TEST_J :
			mode = MUSB_TEST_TEST_J;
			break;

		case USB_TEST_MODE_TEST_K :
			mode = MUSB_TEST_TEST_K;
			break;

		case USB_TEST_MODE_TEST_SE0_NAK :
			mode = MUSB_TEST_SE0_NAK;
			break;

		case USB_TEST_MODE_TEST_PACKET :
			mode = MUSB_TEST_PACKET;
			write_to_fifo( dc, 0, sizeof(musb_testmode_pkt), musb_testmode_pkt );
			HW_Write16( dc->IoBase, MUSB_CSR0, CSR0_TXPKTRDY );
			break;

		case USB_TEST_MODE_TEST_FORCE_ENABLE : // only vald for downstream port(Host)
		default :	
			return( ENOTSUP );	
	}

	HW_Write8( dc->IoBase, MUSB_TESTMODE, mode );

	return( EOK );
}

/* enable a feature of a device */
uint32_t
musb_set_device_feature( usbdc_device_t *udc, uint32_t feature, uint16_t wIndex )
{
	dctrl_t  *dc = udc->dc_data;

	switch ( feature ) {
		case USB_FEATURE_DEV_WAKEUP :
			return( ENOTSUP );
			break;

		case USB_FEATURE_TEST_MODE :
			return( musb_set_test_mode( dc, wIndex ) );
			break;

		default :
			return( ENOTSUP );
			break;
	}

	return( ENOTSUP );
}

/* clear a feature of a device */
uint32_t
musb_clear_device_feature( usbdc_device_t *udc, uint32_t feature )
{
	switch ( feature ) {
		case USB_FEATURE_DEV_WAKEUP :
			return( ENOTSUP );
			break;

		case USB_FEATURE_TEST_MODE : // don't support clearing of test modes(will never get here)
		default :
			return( ENOTSUP );
	}

	return( ENOTSUP );
}

uint32_t
musb_get_descriptor( usbdc_device_t *udc, uint8_t type, uint8_t index, uint16_t lang_id, uint8_t **desc, uint32_t speed )
{	
	dctrl_t *dc = udc->dc_data;

	switch ( type ) {
	case USB_DESC_DEVICE :
		*desc = (speed == IOUSB_DEVICE_HIGH_SPEED) ? (uint8_t *) USBDC_HS_DEVICE_DESCRIPTOR : (uint8_t *) USBDC_FS_DEVICE_DESCRIPTOR;
		musb_slogf(dc, _SLOG_INFO, "%s : get USBDC_DEVICE_DESCRIPTOR", __func__);
		break;
	
	case USB_DESC_CONFIGURATION :
		if ( index < USBDC_NUM_CONFIGURATIONS ) {
			musb_slogf(dc, _SLOG_INFO, "%s : get USBDC_CONFIG_DESCRIPTOR speed = %d index = %d", __func__,speed,index);
			*desc  = (speed == IOUSB_DEVICE_HIGH_SPEED) ? (uint8_t *) USBDC_HS_CONFIG_DESCRIPTOR[index] : (uint8_t *) USBDC_FS_CONFIG_DESCRIPTOR[index];
		} else {
			return( ENOTSUP );
		}
		break;
	
	case USB_DESC_STRING :
		if ( index <= USBDC_MAX_STRING_DESCRIPTOR ) {
			musb_slogf(dc, _SLOG_INFO, "%s : get USBDC_STRING_DESCRIPTOR idx=%d", __func__,index);
			*desc = (speed == IOUSB_DEVICE_HIGH_SPEED) ? (uint8_t *) USBDC_HS_STRING_DESCRIPTOR[index] : (uint8_t *) USBDC_FS_STRING_DESCRIPTOR[index];
		} else {
			return ENOTSUP;
		}
		break;
	
	case USB_DESC_DEVICE_QUALIFIER :
		musb_slogf(dc, _SLOG_INFO, "%s : get USB_DESC_DEVICE_QUALIFIER", __func__);
		*desc = (speed == IOUSB_DEVICE_HIGH_SPEED) ? (uint8_t *) USBDC_HS_DEVICE_QUALIFIER_DESCRIPTOR : (uint8_t *) USBDC_FS_DEVICE_QUALIFIER_DESCRIPTOR;
		break;
		
	case USB_DESC_OTHER_SPEED_CONF :
		musb_slogf(dc, _SLOG_INFO, "%s : get USB_DESC_OTHER_SPEED_CONF speed = %d index = %d", __func__, speed, index);
		*desc  = (speed == IOUSB_DEVICE_HIGH_SPEED) ? (uint8_t *) USBDC_HS_CONFIG_DESCRIPTOR[0] : (uint8_t *) USBDC_FS_CONFIG_DESCRIPTOR[0];
		break;
		
		
	case USB_DESC_INTERFACE_POWER :
	case USB_DESC_INTERFACE :
	case USB_DESC_ENDPOINT :	
	default :
		return ENOTSUP;
		break;
	}
	return EOK;
}

_uint32
musb_control_endpoint_enable( void *chdl, iousb_device_t *device, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t *udc = 	( usbdc_device_t * ) chdl;
	dctrl_t * dc = 			( dctrl_t * ) udc->dc_data;
	ep_ctx_t				*ep = iousb_ep->user;

	musb_slogf(dc, _SLOG_INFO, "%s: ep = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress );

	iousb_ep->flags = IOUSB_ENDPOINT_FLAGS_BMAP;	// no dma 
	
	if ( !iousb_ep->user ) {
		//create endpoint context
		ep = iousb_ep->user = &dc->ep0;
		ep->iousb_ep = iousb_ep;	// backref
		ep->mps = iousb_ep->edesc.wMaxPacketSize;
		ep->num = iousb_ep->edesc.bEndpointAddress & 0x7f;
		ep->type = iousb_ep->edesc.bmAttributes & 3;
	
	}
	
	SIMPLEQ_INIT( &ep->transfer_q );
	
	return ( EOK );
	
}


// used by both bulk and interrupt endpoints
_uint32 
musb_endpoint_enable( void *chdl, iousb_device_t *device, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t 	*udc = ( usbdc_device_t * ) chdl;
	dctrl_t 		*dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t		*ep = iousb_ep->user; 
	uint32_t		epnum = iousb_ep->edesc.bEndpointAddress & 0x7f;
	uint32_t		epdir = iousb_ep->edesc.bEndpointAddress & USB_ENDPOINT_IN;
	uint32_t		fifosz_in_bytes;
	uint8_t			double_buffer_flag = 0;
	uint8_t			v;
	
	musb_slogf(dc, _SLOG_INFO, "%s: ep = 0x%x mps = %d", __func__, iousb_ep->edesc.bEndpointAddress, iousb_ep->edesc.wMaxPacketSize);
	
	if ( epnum >= dc->musbmhdrc_cfg.n_endpoints ) {
		musb_slogf(dc, _SLOG_ERROR, "%s: epnum %d is too large.. must be < %d", __func__, epnum, dc->musbmhdrc_cfg.n_endpoints );
		return EINVAL;
	}
	
	if ( !iousb_ep->user ) {
		ep = iousb_ep->user = ( epdir == USB_ENDPOINT_IN ) ? &dc->epin_arr[epnum] : &dc->epout_arr[epnum];
		ep->iousb_ep = iousb_ep;	// backref
		ep->mps = iousb_ep->edesc.wMaxPacketSize;
		ep->num = epnum;
		ep->dir = epdir;
		ep->type = iousb_ep->edesc.bmAttributes & 3;

		// set the mps for the endpoint		
		if ( ep->dir & USB_ENDPOINT_IN ) {
			HW_Write16( dc->IoBase, MUSB_TXMAXP( epnum ) , ep->mps );		
		} else {
			HW_Write16( dc->IoBase, MUSB_RXMAXP( epnum ) , ep->mps );		
		}

		/* Allocate fifo memory for the endpoint, and associate allocated memory with said endpoint */
		
		fifosz_in_bytes = ep->mps;
#ifdef MUSB_DOUBLE_BUFFER_ENABLED
		fifosz_in_bytes <<= 1;
		double_buffer_flag = FIFOSZ_DOUBLE_BUFFER;
#endif 

		// allocate memory... allocator will round up to the nearest power of 2 and modify the second parameter
		ep->fifoblk = fifomem_alloc( dc->fifomem_hdl, &fifosz_in_bytes );
		if ( ep->fifoblk < 0 ) {
			musb_slogf(dc, _SLOG_ERROR, "%s: fifomem_alloc() failed to allocate fifo of size = %d", __func__, 2 * ep->mps );
			return ENOMEM;
		}

		// select the endpoint index
		HW_Write8( dc->IoBase, MUSB_INDEX, epnum );
		
		/* set the fifo size and address for the endpoint... The fifo size is 
  		* specified in 8 bytes block
 		* 8 bytes: fifosize = 0
		* 16 byte: fifosize = 1
		* etc... 
		*/
		v = double_buffer_flag | musb_log2( fifosz_in_bytes >> 3 );

		
		musb_slogf(dc, _SLOG_INFO, "%s: epnum = 0x%x mps = %d  fifoblk = %d FIFOSZREG = 0x%x ", __func__, ep->num, ep->mps, ep->fifoblk, v );
		
		if ( ep->dir & USB_ENDPOINT_IN ) {
			HW_Write8( dc->IoBase, MUSB_TXFIFOSZ, v );
			HW_Write16( dc->IoBase, MUSB_TXFIFOADD, ep->fifoblk );
		} else {
			HW_Write8( dc->IoBase, MUSB_RXFIFOSZ, v );
			HW_Write16( dc->IoBase, MUSB_RXFIFOADD, ep->fifoblk );
		}
		
		// This endpoint uses both virtual and physical addresses, because
		// a mixture of both DMA and PIO are used.  use BMAP so that urb->buffer
		// is a virtual address, and access physical address via urb->bufer_paddr;
		iousb_ep->flags = IOUSB_ENDPOINT_FLAGS_BMAP;
		
		// allocate dma channel if it is a bulk endpoint, 
		if ( ( dc->flags & ( DC_FLAG_USE_BUILTIN_DMA | DC_FLAG_USE_EXTERNAL_DMA ) ) && ( ep->type == USB_ATTRIB_BULK ) ) {
			ep->dma_chidx = dc->dma_funcs.channel_claim( dc->dma_hdl, ep );
			if ( ep->dma_chidx>= 0 ) {
				musb_slogf(dc, _SLOG_INFO, "%s: epnum = 0x%x uses dma channel index = %d ", __func__, ep->num, ep->dma_chidx );
			} else {
				musb_slogf(dc, _SLOG_WARNING, "%s: epnum = 0x%x has no more dma channels to use... using PIO ", __func__, ep->num);
			}
		 } else {
			 // don't use dma
			 ep->dma_chidx = -1;
		 }
	}

	SIMPLEQ_INIT( &ep->transfer_q );
	
	// reset the toggle
	if ( ep->dir & USB_ENDPOINT_IN ) {
		HW_Write16( dc->IoBase, MUSB_TXCSR( ep->num ), TXCSR_CLR_DATA_TOGGLE );
		if ( USE_BUILTIN_DMA( dc, ep ) ) {
			// The DMA enable bit is set in the transfer function because the transfer function need
			// to use PIO for sending the ZLP
			HW_Write16Or( dc->IoBase, MUSB_TXCSR( ep->num ), TXCSR_AUTOSET | TXCSR_DMA_REQ_TYPE1  );
		}
	} else {
		HW_Write16( dc->IoBase, MUSB_RXCSR( ep->num ), RXCSR_CLR_DATA_TOGGLE );
		if ( USE_BUILTIN_DMA( dc, ep ) ) {
			// we set the DMA enable here because we try to dma every transfer... short transfers trigger an endpoint interrtupt
			// and are handled via PIO
			HW_Write16Or( dc->IoBase, MUSB_RXCSR( ep->num ), RXCSR_AUTOCLEAR | RXCSR_DMA_REQ_TYPE1 | RXCSR_DMA_REQ_EN );
		}
	}
	
	return EOK;
}

_uint32 
musb_isoch_endpoint_enable( void *chdl, iousb_device_t *device, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t	*udc = ( usbdc_device_t * ) chdl;
	dctrl_t 		*dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t		*ep = iousb_ep->user; 
	uint32_t		epnum = iousb_ep->edesc.bEndpointAddress & 0x7f;
	uint32_t		epdir = iousb_ep->edesc.bEndpointAddress & USB_ENDPOINT_IN;
	uint32_t		fifosz_in_bytes;
	uint8_t 		double_buffer_flag = 0;
	uint8_t 		v;
	
	musb_slogf(dc, _SLOG_INFO, "%s: ep = 0x%x mps = %d", __func__, iousb_ep->edesc.bEndpointAddress, iousb_ep->edesc.wMaxPacketSize);
	
	if ( epnum >= dc->musbmhdrc_cfg.n_endpoints ) {
		musb_slogf(dc, _SLOG_ERROR, "%s: epnum %d is too large.. must be < %d", __func__, epnum, dc->musbmhdrc_cfg.n_endpoints );
		return EINVAL;
	}
	
	if ( !iousb_ep->user ) {
		ep = iousb_ep->user = ( epdir == USB_ENDPOINT_IN ) ? &dc->epin_arr[epnum] : &dc->epout_arr[epnum];
		ep->iousb_ep = iousb_ep;	// backref
		ep->mps = iousb_ep->edesc.wMaxPacketSize;
		ep->num = epnum;
		ep->dir = epdir;
		ep->type = iousb_ep->edesc.bmAttributes & 3;

		// set the mps for the endpoint 	
		if ( ep->dir & USB_ENDPOINT_IN ) {
			HW_Write16( dc->IoBase, MUSB_TXMAXP( epnum ) , ep->mps );		
		} else {
			HW_Write16( dc->IoBase, MUSB_RXMAXP( epnum ) , ep->mps );		
		}

		/* Allocate fifo memory for the endpoint, and associate allocated memory with said endpoint */
		
		fifosz_in_bytes = ep->mps;

#ifdef MUSB_DOUBLE_BUFFER_ENABLED
		fifosz_in_bytes <<= 1;
		double_buffer_flag = FIFOSZ_DOUBLE_BUFFER;
#endif 

		// allocate memory... allocator will round up to the nearest power of 2 and modify the second parameter
		ep->fifoblk = fifomem_alloc( dc->fifomem_hdl, &fifosz_in_bytes );
		if ( ep->fifoblk < 0 ) {
			musb_slogf(dc, _SLOG_ERROR, "%s: fifomem_alloc() failed to allocate fifo of size = %d", __func__, 2 * ep->mps );
			return ENOMEM;
		}

		// select the endpoint index
		HW_Write8( dc->IoBase, MUSB_INDEX, epnum );
		
		/* set the fifo size and address for the endpoint... The fifo size is 
		* specified in 8 bytes block
		* 8 bytes: fifosize = 0
		* 16 byte: fifosize = 1
		* etc... 
		*/
		v = double_buffer_flag | musb_log2( fifosz_in_bytes >> 3 );

		
		musb_slogf(dc, _SLOG_INFO, "%s: epnum = 0x%x mps = %d  fifoblk = %d FIFOSZREG = 0x%x ", __func__, ep->num, ep->mps, ep->fifoblk, v );
		
		if ( ep->dir & USB_ENDPOINT_IN ) {
			HW_Write8( dc->IoBase, MUSB_TXFIFOSZ, v );
			HW_Write16( dc->IoBase, MUSB_TXFIFOADD, ep->fifoblk );
		} else {
			HW_Write8( dc->IoBase, MUSB_RXFIFOSZ, v );
			HW_Write16( dc->IoBase, MUSB_RXFIFOADD, ep->fifoblk );
		}
		
		// This endpoint uses both virtual and physical addresses, because
		// a mixture of both DMA and PIO are used.	use BMAP so that urb->buffer
		// is a virtual address, and access physical address via urb->bufer_paddr;
		iousb_ep->flags = IOUSB_ENDPOINT_FLAGS_BMAP;
		
		 // don't use dma
	}

	SIMPLEQ_INIT( &ep->transfer_q );

	// reset some state variables
	ep->td = 0;
	ep->dma_chidx = -1;
	
	if ( ep->dir & USB_ENDPOINT_IN ) {
		HW_Write16( dc->IoBase, MUSB_TXCSR( ep->num ), TXCSR_ISO );
	} else {
		HW_Write16( dc->IoBase, MUSB_RXCSR( ep->num ), RXCSR_ISO );
	}
	
	return EOK;
}

_uint32 
musb_endpoint_disable( void *chdl, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t *udc = ( usbdc_device_t * ) chdl;
	dctrl_t * dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t				*ep = iousb_ep->user;

	iousb_ep->user = 0;
	
	if ( ep ) {
		if ( ep->dma_chidx != -1 ) {
			dc->dma_funcs.channel_release( dc->dma_hdl, ep->dma_chidx );
			ep->dma_chidx = -1;
		}
		
		fifomem_free(  dc->fifomem_hdl, ep->fifoblk );
	}
	
	
	return ( EOK );
}

_uint32 
musb_control_endpoint_disable( void *chdl, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t *udc = ( usbdc_device_t * ) chdl;
	dctrl_t * dc = ( dctrl_t * ) udc->dc_data;
	
	musb_slogf(dc, _SLOG_INFO, "%s: ep = 0x%x", __func__, iousb_ep->edesc.bEndpointAddress );
	iousb_ep->user = 0;
	
	return ( EOK );
}

_uint32 
musb_isoch_endpoint_disable( void *chdl, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t *udc = ( usbdc_device_t * ) chdl;
	dctrl_t        *dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t       *ep = iousb_ep->user;

	iousb_ep->user = 0;
	
	if ( ep ) {
		fifomem_free(  dc->fifomem_hdl, ep->fifoblk );
	}
	
	return ( EOK );
}
	
_uint32 
musb_control_transfer_abort( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t *udc = ( usbdc_device_t * ) chdl;
	dctrl_t        *dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t       *ep = iousb_ep->user;
	musb_transfer_t *td;
	
	if ( ep == NULL ) {
		return EOK;
	}

	MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: ep = 0x%x  ep->num = %d ", __func__, iousb_ep->edesc.bEndpointAddress, ep->num);
	
	MUSB_MUTEX_LOCK
	
	// Null out the urb reference so that the completion function cannot complete
	// the urb just in case the abort fails to stop the hardware on time
	ep->td = NULL;
	delay(1);
	
	if ( atomic_clr_value(&ep->flags, EP_FLAG_DEFER_SETUP_PKT_CLEAR) & EP_FLAG_DEFER_SETUP_PKT_CLEAR )  {
		// complete defered processing for NO DATAPHASE control transfer
		HW_Write16( dc->IoBase, MUSB_CSR0, CSR0_SERVICED_RXPKTRDY );
	}

	MUSB_LOCK;
	if ( (td = SIMPLEQ_FIRST( &ep->transfer_q ))){
		SIMPLEQ_REMOVE_HEAD(&ep->transfer_q, link);
		SIMPLEQ_INSERT_TAIL( &dc->transfer_free_q, td, link );
	}
	MUSB_UNLOCK;
	
	flush_fifo( dc, ep );
	
	MUSB_MUTEX_UNLOCK
	
	return EOK;
}

_uint32 
musb_control_transfer( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *iousb_ep, uint8_t *buffer, _uint32 length, _uint32 flags )
{
	usbdc_device_t 	*udc = ( usbdc_device_t * ) chdl;
	dctrl_t 		*dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t 		*ep = iousb_ep->user;
	musb_transfer_t	*td = NULL;
	
	MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: ep = 0x%x  ep->num = %d buffer = 0x%x len = %d flags = 0x%x ", __func__, iousb_ep->edesc.bEndpointAddress, ep->num, buffer,length,flags );

	MUSB_MUTEX_LOCK

	if ( flags & PIPE_FLAGS_TOKEN_STATUS ) {

		/* Complete the URB now because the hardware completes the status phase automatically and 
		 * it is very difficult ( if not impossible ) to to determine the state
		 * of the hardware once the interrupt fires because no actual status bits
		 * The HOST will take corrective action anyway if something goes wrong
		 */
		MUSB_MUTEX_UNLOCK
		urb->actual_len = 0;
		urb->urb_complete_cbf( ep->iousb_ep, urb, 0, 0);
		MUSB_MUTEX_LOCK;

		dc->ep0.ep0_idle_substate = EP0_IDLE_SUBSTATE_SETUP_PHASE;

		if ( atomic_clr_value(&ep->flags, EP_FLAG_DEFER_SETUP_PKT_CLEAR) & EP_FLAG_DEFER_SETUP_PKT_CLEAR)  {
			// complete defered processing for NO DATAPHASE control transfer
			HW_Write16( dc->IoBase, MUSB_CSR0, CSR0_SERVICED_RXPKTRDY | CSR0_DATAEND );
		}

		if ( atomic_clr_value(&ep->flags, EP_FLAG_DEFER_SETUP_PKT_PROCESSING) & EP_FLAG_DEFER_SETUP_PKT_PROCESSING ) {
			/* We have already received the next setup pkt, but we elected
			 * to defer processing until the previous status phase was complete
			 * Since we have already procesded the interrupt, need to call the 
			 * ep0_interrupt() hanlder manually
			 */
			MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: Start defered processing for endpoint 0",__func__);		
			process_ep0_interrupt( dc );
			if(dc->usb_status & USB_EP0_SETUP_PULSE){
				process_devstate_interrupt(dc);
			}
		}

	} else {
		// data phase
		if( (td = musb_get_transfer(dc, urb, ep,flags)) == NULL){
			MUSB_MUTEX_UNLOCK;
			return ENOMEM ;
		}
		
		ep->td = td;
		td->xfer_buffer = (uint32_t)buffer;
		td->xfer_buffer_paddr = (uint32_t) urb->buffer_paddr;
		td->xfer_length = length;
		td->bytes_xfered = 0;

		MUSB_LOCK;
		SIMPLEQ_INSERT_TAIL( &ep->transfer_q, td, link );
		MUSB_UNLOCK;
		
		MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: ep = 0x%x	ep->num = %d buffer = 0x%x len = %d flags = 0x%x ", __func__, iousb_ep->edesc.bEndpointAddress, ep->num, buffer,length,flags );

		if ( flags & PIPE_FLAGS_TOKEN_IN ) {
			dc->ep0.ep0_state = EP0_STATE_TXDATA;
			ep0_transmit( dc );
		} else {

			if ( atomic_clr_value(&ep->flags, EP_FLAG_DEFER_SETUP_PKT_CLEAR) & EP_FLAG_DEFER_SETUP_PKT_CLEAR )  {
				// clear setup packet, and get ready to receive DATA-OUT dataphase
				HW_Write16( dc->IoBase, MUSB_CSR0, CSR0_SERVICED_RXPKTRDY );
			}

			dc->ep0.ep0_state = EP0_STATE_RXDATA;
			// nothing more to do... the controller will accept the next receive packet
			// and transfer it to the urb buffer in the interrupt handler.
		}
	}

	MUSB_MUTEX_UNLOCK
	
	return EOK;
}

	
_uint32 
musb_transfer_abort( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t 	*udc = ( usbdc_device_t * ) chdl;
	dctrl_t 		*dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t 		*ep = iousb_ep->user;
	musb_transfer_t *td;
	
	if ( ep == NULL ) {
		return EOK;
	}
	
	MUSB_MUTEX_LOCK

	MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: ep = 0x%x  ep->num = %d ", __func__, iousb_ep->edesc.bEndpointAddress, ep->num);
	
	// Null out the urb reference so that the completion function cannot complete
	// the urb just in case the abort fails to stop the hardware on time
	ep->td = NULL;
	delay(1);
	
	if ( USE_BUILTIN_DMA( dc, ep ) || USE_EXTERNAL_DMA( dc, ep ) ) {
		// stop transfer is progress
		dc->dma_funcs.transfer_abort( dc->dma_hdl, ep->dma_chidx );
	}

	MUSB_LOCK;
	if ( (td = SIMPLEQ_FIRST( &ep->transfer_q ))){
		SIMPLEQ_REMOVE_HEAD(&ep->transfer_q, link);
		SIMPLEQ_INSERT_TAIL( &dc->transfer_free_q, td, link );
	}
	MUSB_UNLOCK;
	
	flush_fifo( dc, ep );
	
	MUSB_MUTEX_UNLOCK
	
	return EOK;
}

_uint32 
musb_transfer( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *iousb_ep, uint8_t *buffer, _uint32 length, _uint32 flags )
{
	usbdc_device_t 	*udc = ( usbdc_device_t * ) chdl;
	dctrl_t 		*dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t 		*ep = iousb_ep->user;
	musb_transfer_t	*td = NULL;

	MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: ep = 0x%x  ep->num = %d buffer = 0x%x len = %d flags = 0x%x ", __func__, iousb_ep->edesc.bEndpointAddress, ep->num, buffer,length,flags );

	MUSB_MUTEX_LOCK
	if( (td = musb_get_transfer(dc, urb, ep, flags)) == NULL){
		MUSB_MUTEX_UNLOCK;
		return ENOMEM ;
	}
	
	td->xfer_buffer = (uint32_t)buffer;
	td->xfer_buffer_paddr = (uint32_t) urb->buffer_paddr;
	td->xfer_length = length;
	td->bytes_xfered = 0;

	MUSB_LOCK;
	ep->td = td;
	SIMPLEQ_INSERT_TAIL( &ep->transfer_q, td, link );
	if ( flags & PIPE_FLAGS_TOKEN_IN ) {
		MUSB_UNLOCK;
		ep_transmit( dc, ep );
	}    
	else {
		// MUSB_UNLOCK is done inside ep_receive
		// This is to make sure that the ep->td 
		// and ep->flags (defer processing or not, etc)
		ep_receive( dc, ep );
	}
	
	MUSB_MUTEX_UNLOCK
	
	return EOK;
}

_uint32 
musb_isoch_transfer_abort( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *iousb_ep )
{
	usbdc_device_t 	*udc = ( usbdc_device_t * ) chdl;
	dctrl_t 		*dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t 		*ep = iousb_ep->user;
	musb_transfer_t *td;
	
	if ( ep == NULL ) {
		return EOK;
	}

	MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: ep = 0x%x  ep->num = %d ", __func__, iousb_ep->edesc.bEndpointAddress, ep->num);
	
	MUSB_MUTEX_LOCK
	// Null out the urb reference so that the completion function cannot complete
	// the urb just in case the abort fails to stop the hardware on time
	ep->td = NULL;
	delay(2);

	MUSB_LOCK;
	while ( (td = SIMPLEQ_FIRST( &ep->transfer_q ))){
		SIMPLEQ_REMOVE_HEAD(&ep->transfer_q, link);
		SIMPLEQ_INSERT_TAIL( &dc->transfer_free_q, td, link );
	}
	MUSB_UNLOCK;

	flush_fifo( dc, ep );

	MUSB_MUTEX_UNLOCK
	
	return EOK;
}


_uint32 
musb_isoch_transfer( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *iousb_ep, uint8_t *buffer, _uint32 length, _uint32 flags )
{
	usbdc_device_t 	*udc = ( usbdc_device_t * ) chdl;
	dctrl_t 		*dc = ( dctrl_t * ) udc->dc_data;
	ep_ctx_t 		*ep = iousb_ep->user;
	musb_transfer_t	*td = NULL;

	MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: ep = 0x%x  ep->num = %d buffer = 0x%x len = %d flags = 0x%x ", __func__, iousb_ep->edesc.bEndpointAddress, ep->num, buffer,length,flags );

	MUSB_MUTEX_LOCK
	if( (td=musb_get_transfer(dc, urb, ep, flags)) == NULL){
		MUSB_MUTEX_UNLOCK;
		return (ENOMEM);
	}
	
	td->xfer_buffer = (uint32_t)buffer;
	td->xfer_buffer_paddr = (uint32_t) urb->buffer_paddr;
	td->xfer_length = length;
	td->bytes_xfered = 0;
	td->isoch_frame_offset = 0;
	td->isoch_flist		= NULL;
	td->isoch_nframes 	= 0;	
	td->isoch_cur_idx 	= 0;

	if ( flags & PIPE_FLAGS_MULTI_XFER ) {
		td->isoch_flist = ((usbd_urb_isoch_stream_xfer_t *) urb->xdata_ptr)->frame_list;
		td->isoch_nframes = ((usbd_urb_isoch_stream_xfer_t *) urb->xdata_ptr)->nframes;
		
	}	

	MUSB_LOCK;
	SIMPLEQ_INSERT_TAIL( &ep->transfer_q, td, link );
	//if this is the first to start, then start the transfer
	if((td == (musb_transfer_t *) SIMPLEQ_FIRST( &ep->transfer_q ))){
		ep->td = td;	
		MUSB_UNLOCK;
		if ( flags & PIPE_FLAGS_TOKEN_IN ) {
			ep_isoch_transmit(dc,ep);
		}else{
			uint16_t csr = HW_Read16( dc->IoBase, MUSB_RXCSR( ep->num ) );
			if( csr & RXCSR_RXPKTRDY ){
				//we don't need anything before we started.
				flush_fifo( dc, ep );
				csr &= ~RXCSR_RXPKTRDY;
			}
			HW_Write16( dc->IoBase, MUSB_RXCSR( ep->num ), csr | RXCSR_ISO );
		}
	}else{
		MUSB_UNLOCK;
	}

	MUSB_MUTEX_UNLOCK
	
	return EOK;
}


int
musb_process_args( dctrl_t *dc, char *args )
{

	int opt;
	char *value;
	char *c;
	int len;
	
	static char *driver_opts[] = {
		"verbose", 
		"ser",
		"linkup",
		"linkdown",
		"forcefs",
		"nodma",
		"no_cfg_pmic",
		"no_session_poll",
		"smartclock",
		"inherit_cfg",
		"USBPHY_ANA_CONFIG1",
		"USBPHY_ANA_CONFIG2",
		"num_td",
		"priority",
		"edma",
		"edmachannel",
		"edmairq",
		NULL
	};
	
	dc->serial_string = NULL;
	
	if( !args ) 
		return EOK;    
	
	// convert args
	len = strlen( args );
	while ( ( c = strchr( args, ':' ) ) ) {
		if ( c - args > len )
			break;
			*c = ',';
	}
		
	while( *args != '\0' ) {
		if( ( opt = getsubopt( &args, driver_opts, &value ) ) != -1 ) {
			switch ( opt ) {
				case 0 :
					if ( value )
						dc->verbosity = strtoul( value, 0, 0 );
					else 
						dc->verbosity = 5;
					continue;
				case 1 :     // this arg should move up for know we build a proper string desc
					if ( value ) {
						uint8_t  slen;
						uint32_t x;
						slen = min(strlen( value ), 127 ); // max 8bit length each char requires 2 bytes for encoding plus 2 byte header.
						dc->udc->serial_string = calloc( 1, 3 + 2 * slen );
						dc->udc->serial_string[0] = 2 + slen *2; // string header is 2 bytes
						dc->udc->serial_string[1] = USB_DESC_STRING;
						for ( x = 1 ; x < slen + 1 ; x ++ ) {
							dc->udc->serial_string[x * 2] = value[x - 1];
						}
					}
					continue;
				case 2:
					// "linkup"
					dc->flags |= DC_FLAG_SOFT_CONNECT;
				
					continue;
				
				case 3:
					// "linkdown"
					dc->flags &= ~DC_FLAG_SOFT_CONNECT;
					continue;
					
				case 4:
					// "forcefs"
					dc->flags |= DC_FLAG_FORCE_FS;
					continue;
					
				case 5:
					// "nodma"
					dc->flags &= ~DC_FLAG_USE_BUILTIN_DMA;
					break;
				case 6:
					dc->flags &= ~DC_FLAG_CFG_PMIC;
					break;
				case 7:
					dc->flags &= ~DC_FLAG_SESSION_POLL;
					break;
				case 8:
					dc->flags |= DC_FLAG_SMART_CLOCK;
					break;
				case 9:
					dc->flags |= DC_FLAG_INHERIT_CFG;
					break;
				case 10:
					// USBPHY_ANA_CONFIG1
					dc->phy_tuning[USBPHY_ANA_CONFIG1] = strtoul( value, NULL, 16 );
					break;
				case 11:
					// USBPHY_ANA_CONFIG2
					dc->phy_tuning[USBPHY_ANA_CONFIG2] = strtoul( value, NULL, 16 );
					break;
				case 12:
					if ( value ){
						dc->num_td = strtoul( value, 0, 0 );
					}
					break;
				case 13:
					if ( value ){
						dc->priority = strtoul( value, 0, 0 );
					}
					break;
				case 14:
					// "edma"
					dc->flags |= DC_FLAG_USE_EXTERNAL_DMA;
					dc->flags &= ~DC_FLAG_USE_BUILTIN_DMA;
					break;
				case 15:
					// "edmachannel"
					if ( value ) {
						dc->edma_channel = strtol( value, 0, 10 );
					}
					break;
				case 16:
					// "edmairq"
					if ( value ) {
						dc->edma_irq = strtol( value, 0, 10 );
					}
					break;
				default :
					break;
			}
		}
	}
	
	return EOK;
}

static void 
default_config_set( usbdc_device_t *udc )
{
	dctrl_t * dc = ( dctrl_t * ) udc->dc_data;
	
	dc->verbosity = _SLOG_WARNING;
	dc->flags     =  DC_FLAG_USE_BUILTIN_DMA | DC_FLAG_CFG_PMIC | DC_FLAG_SESSION_POLL;
	memset ( dc->phy_tuning, 0, sizeof( dc->phy_tuning ) );
    
	udc->hw_ctrl.cname = "musb";
	udc->hw_ctrl.max_transfer_size = 0x10000; 
	udc->hw_ctrl.max_unaligned_xfer = 0x10000; 
	udc->hw_ctrl.buff_alignment_mask = 0x1;
	udc->hw_ctrl.capabilities 	= DC_CAP_FULL_SPEED | DC_CAP_HIGH_SPEED | DC_CAP_TEST_MODES_SUPPORTED | USBD_HCD_CAP_ISOCH_STREAM;
	dc->num_td    = 100;
	dc->priority  = 22;
	dc->tid       = -1;
	dc->irq       = udc->hw_ctrl.pci_inf->Irq;
}

static int reset_controller( dctrl_t * dc ) 
{
    int         retval;
    
    retval = musb_custom_reset( dc );
    
    
    return retval;
}

static int 
chip_config ( dctrl_t * dc ) 
{
	int err;
	
	err = reset_controller( dc );
	if ( err != EOK ) {
		goto fail;
	}
	
	if ( dc->flags & DC_FLAG_FORCE_FS ) {
		HW_Write8And( dc->IoBase, MUSB_POWER, ~POWER_HS_ENABLE );
	}

#ifndef EXTERNAL_INTERRUPT_PROCESSING
	// enable main usb interupts
	HW_Write8( dc->IoBase, MUSB_INTRUSBE, 
				INTRUSB_SUSPEND | INTRUSB_RESUME  | INTRUSB_RESET | INTRUSB_DISCON );
#endif

	return EOK;	

fail:	
	return err;
}

void musb_destroy_pulse_thread( dctrl_t * dc ) {
	if(dc->tid != -1){
		MsgSendPulse(dc->coid, dc->priority, MUSB_PULSE_EVENT_CANCELLATION, 0);
		pthread_join( dc->tid, NULL );
		ConnectDetach( dc->coid );
		ChannelDestroy( dc->chid );
	}
}

uint32_t
musb_init( usbdc_device_t *udc, io_usbdc_self_t *udc_self, char *args )
{
	dctrl_t * dc;
	pthread_mutexattr_t mattr;    
	int err;
	char *args_copy;
    
    /* allocate device ctx */
	dc = udc->dc_data = calloc( 1, sizeof( dctrl_t ) );
	if ( !dc ) {
		musb_slogf( dc, _SLOG_ERROR, "%s calloc failed",__func__);
		err = ENOMEM;
		goto error;
	}
	dc->udc = udc;
    
	/* Get SoC specific parameters for the musbmhdrc controller */
	musb_cfg_get( &dc->musbmhdrc_cfg );
	
    /* set default driver configurations */
	default_config_set( udc );
	
    /* parse command line arguments to override default configs */
    
    args_copy = strdup( args );
    if ( args_copy == NULL ) {
		musb_slogf( dc, _SLOG_ERROR, "%s strdup failed",__func__);
		err = ENOMEM;
		goto error2;
    }
    
	err = musb_process_args(dc, args);
	if ( err ) {
		musb_slogf( dc, _SLOG_ERROR, "%s couldn't parse command line args",__func__);
		goto error3;        
	}

	musb_alloc_transfer_queue( dc );

	err = musb_custom_init1( dc, args_copy );
	if ( err ) {
		musb_slogf( dc, _SLOG_ERROR, "%s musb_custom_init1() failed err = %d",__func__, err);
		goto error3;        
	}
	
	// no longer need args copy, because it has alreay been processed by custom_int1
	free( args_copy );
	args_copy = NULL;
	
	// map io
	dc->IoBase =  mmap_device_memory( NULL, 
						MUSB_SIZE,  
						PROT_READ | PROT_WRITE | PROT_NOCACHE, 
						MAP_SHARED | MAP_PHYS,
						PCI_MEM_ADDR( udc->hw_ctrl.pci_inf->CpuBaseAddress[0] ) );
	if ( dc->IoBase == MAP_FAILED ) {
		musb_slogf( dc, _SLOG_ERROR, "%s mmap failed",__func__);
		err = ENOMEM;
		goto error4;
	}

	dc->pIoBase = ( uint32_t ) udc->hw_ctrl.pci_inf->CpuBaseAddress[0];

	// create the driver mutex    
	pthread_mutexattr_init( &mattr );
	pthread_mutexattr_setrecursive( &mattr, PTHREAD_RECURSIVE_ENABLE );
	if( pthread_mutex_init( &dc->mutex, &mattr ) == -1 ) {
		musb_slogf( dc, _SLOG_ERROR, "%s could not create mutex",__func__);
		err = ENOMEM;
		goto error5;
	}

	if( dc->flags & ( DC_FLAG_USE_BUILTIN_DMA | DC_FLAG_USE_EXTERNAL_DMA ) ) {

		// map the dma funcs for this SoC
		musb_dma_map( dc, &dc->dma_funcs );
	
		dc->dma_hdl = dc->dma_funcs.init( dc ); 
		if ( dc->dma_hdl == NULL ) {
			musb_slogf( dc, _SLOG_ERROR, "%s could not initialize the DMA",__func__);
			err = -1;
			goto error6;
		}
	}

	// Initialize FIFO memory allocator
	dc->fifomem_hdl = fifomem_init( dc->musbmhdrc_cfg.fiforam_size );
	if ( dc->fifomem_hdl == NULL ) {
		musb_slogf( dc, _SLOG_ERROR, "%s failed to initialize fifo memory allocator",__func__);
		goto error7;
	}

	// setup usb controller
	err = chip_config( dc );
	if ( err ) {
		musb_slogf( dc, _SLOG_ERROR, "%s could not init the controller",__func__);
		goto error8;
	}

	err = musb_custom_init2( dc );
	if ( err ) {
		musb_slogf( dc, _SLOG_ERROR, "%s musb_custom_init2() failed err = %d",__func__, err);
		goto error8;        
	}
	
	udc->usbdc_self->usbdc_set_device_speed( udc, IOUSB_DEVICE_FULL_SPEED );  
	
	// set Soft-Connect state	
	if ( dc->flags & DC_FLAG_SOFT_CONNECT ) {
		HW_Write8Or( dc->IoBase, MUSB_POWER, POWER_SOFT_CONN );
	} else {
		HW_Write8And( dc->IoBase, MUSB_POWER, ~POWER_SOFT_CONN );
	}

#ifdef MUSB_REAL_ISR
	musb_create_interrupt_handler( dc );
#endif

	return EOK;

error8:
	fifomem_fini( dc->fifomem_hdl ) ;
error7:
	if ( dc->flags & ( DC_FLAG_USE_BUILTIN_DMA | DC_FLAG_USE_EXTERNAL_DMA ) ) {
		dc->dma_funcs.fini( dc->dma_hdl );
	}
error6:
	pthread_mutex_destroy( &dc->mutex );
error5:
	munmap( dc->IoBase, MUSB_SIZE );
error4:
	musb_custom_fini1( dc );
error3:
	if ( args_copy ) 
		free( args_copy );
error2:
	musb_destroy_pulse_thread( dc );
	if(dc->transfer_mem)
		free( dc->transfer_mem );
	free(dc);  
error:    
	return err;
}

uint32_t
musb_start( usbdc_device_t *udc )
{
	return EOK;
}

uint32_t
musb_stop( usbdc_device_t *udc )
{
	return EOK;
}

uint32_t
musb_shutdown( usbdc_device_t *udc )
{
	dctrl_t * dc = ( dctrl_t * ) udc->dc_data;
	
	// force disconnect
	HW_Write8And( dc->IoBase, MUSB_POWER, ~POWER_SOFT_CONN );

	// free resources
	fifomem_fini( dc->fifomem_hdl ) ;
	if ( dc->flags & ( DC_FLAG_USE_BUILTIN_DMA | DC_FLAG_USE_EXTERNAL_DMA ) ) {
		dc->dma_funcs.fini( dc->dma_hdl );
	}	
	musb_destroy_pulse_thread(dc);
	pthread_mutex_destroy( &dc->mutex );
	munmap( dc->IoBase, MUSB_SIZE);
	musb_custom_fini2( dc );
	musb_custom_fini1( dc );
	if(dc->transfer_mem)
		free( dc->transfer_mem );
	free(dc);  
	
	return EOK;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/musbmhdrc/musb.c $ $Rev: 746301 $")
#endif
