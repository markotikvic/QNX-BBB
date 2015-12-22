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
  * This module implements controls the MUSBMHDRC *INTERNAL* DMA controller 
  * which is not supported by all SoC Implementations. In the case of 
  * a customer DMA engine, a new DMA implementation will be required
  *
  *
  * This module support DMA MODE 1 only!
  */
 
#include <malloc.h>  
#include <sys/slog.h>
#include <unistd.h>
#include <errno.h>
  
#include "musb.h"

  
#define MAX_DMA_CHANNELS		8
#define DMA_PULSE_INTR			(_PULSE_CODE_MINAVAIL+1)
  
typedef struct _channel_ctx {
	uint8_t			channel_in_use;
	uint8_t			channel_active;
	ep_ctx_t 		*ep;
	uint32_t		start_addr_paddr;
	unsigned		length;
	uint8_t			process_residual_required;
} channel_ctx_t;
  
  
typedef struct _dma_ctx {
	dctrl_t 				*dc;
	channel_ctx_t			dma_channel_ctx_arr[MAX_DMA_CHANNELS];

	/* dma interrupt members */
	struct sigevent     	dma_event;
	int                    	chid;
	int                    	coid;
	int                    	tid;
	int		               	irq;      /* PIO IRQ Number   */
	int		               	iid;      /* interrrupt handle*/
} dma_ctx_t;


////////////////////////////////////////////////////////////////////////////////
//                        PRIVATE FUNCTIONS                                   //
////////////////////////////////////////////////////////////////////////////////
 
static void dma_channel_interrupt_handler( dma_ctx_t * dma_ctx, int chidx ) {
	dctrl_t 		*dc = dma_ctx->dc;
	channel_ctx_t	*ch_ctx = &dma_ctx->dma_channel_ctx_arr[chidx];
	ep_ctx_t 		*ep = ch_ctx->ep; 
	musb_transfer_t *td = ep->td;
	uint32_t		dma_cntl;

	if ( ch_ctx->channel_active ) {
		ch_ctx->channel_active = 0;

		dma_cntl = HW_Read32( dc->IoBase, MUSB_DMA_CNTL(chidx) );
		MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: epnum = 0x%x epdir = 0x%x chidx = % ddma_cntl = 0x%x csr = 0x%x"
			,__func__, ep->num, ep->dir, chidx, dma_cntl, 
			( ep->dir ) ? HW_Read16( dc->IoBase, MUSB_TXCSR( ch_ctx->ep->num ) ) : HW_Read16( dc->IoBase, MUSB_RXCSR( ch_ctx->ep->num ) ) );	
		
		if ( ch_ctx->ep->dir == USB_ENDPOINT_IN ) {
			if ( ch_ctx->process_residual_required ) {
				// the musbmhdrc ref manual says that the last usb packet will not
				// be sent out if if is < mps
				MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: residual processing... hit TXPKTRDY bit",__func__, dma_cntl );	
				
				// the data is already in the fifo so  trigger the transfer
				td->req_xfer_len = HW_Read32( dc->IoBase, MUSB_DMA_COUNT(chidx) );
				HW_Write16Or( dc->IoBase, MUSB_TXCSR( ch_ctx->ep->num ), TXCSR_TXPKTRDY );
			}
		} 
	
		// calc how much data we sent...
		td->bytes_xfered = ch_ctx->length - HW_Read32( dc->IoBase, MUSB_DMA_COUNT(chidx) );
		MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: epnum = 0x%x epdir = 0x%x bytes_xfered %d bytes, dmacount = 0x%x",__func__,ep->num, ep->dir, td->bytes_xfered, HW_Read32( dc->IoBase, MUSB_DMA_COUNT(chidx) ) );	
		
		// complete the urb if we can
		if ( ( dma_cntl & DMA_CNTL_ERROR ) == 0 ) {
			// no dma error, so dma transfer must have completed successfully
	
			if ( ch_ctx->ep->dir == USB_ENDPOINT_IN ) {
				// we have sent all data, including a possible residual... so complete
				complete_urb( dc, ep, 0 );
			} else {
				if ( td->bytes_xfered >= td->xfer_length) {
					// we have received all  the expected data... complete the urb
					complete_urb( dc, ep, 0 );
				} 
				// else {// may be expecting a short packet... will be handled via endpoint interrupts } 
			}
		} else { 
			// dma error... cleanup and complete urb...
			complete_urb( dc, ep, USBD_STATUS_DMA_ERR );
		}
	} 
}


static void dma_interrupt_handler( dma_ctx_t * dma_ctx ) {
	dctrl_t  	*dc = dma_ctx->dc;
	uint8_t		dma_intr;
	int			chidx;
	
	if( pthread_mutex_lock( &dc->mutex ) )
		musb_slogf(dc, _SLOG_ERROR, "%s: mutex lock failed ", __func__ );
	
	dma_intr = HW_Read8( dc->IoBase, MUSB_DMA_INTR );
	MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: interrupt fired dma_intr = 0x%x ",__func__, dma_intr );	
	
	for( chidx=0; chidx < MAX_DMA_CHANNELS; chidx++ ) {
		if ( dma_intr & ( 1 << chidx ) ) {
			dma_channel_interrupt_handler( dma_ctx, chidx );
		}
	}
	
	if( pthread_mutex_unlock( &dc->mutex ) )
		musb_slogf(dc, _SLOG_ERROR, "%s: mutex unlock failed ", __func__ );
}

static void * dma_event_handler( dma_ctx_t * dma_ctx ) {
	struct _pulse 		pulse;
   	iov_t				iov;
	int 				rcvid;

	SETIOV( &iov, &pulse, sizeof( pulse ) );

	while( 1 ) {
		if( ( rcvid = MsgReceivev( dma_ctx->chid, &iov, 1, NULL ) ) == -1 ) {
			continue;
		}
		switch( pulse.code ) {
			case DMA_PULSE_INTR:
				dma_interrupt_handler( dma_ctx );
				InterruptUnmask( dma_ctx->irq, dma_ctx->iid );
				break;
			default:
				if( rcvid ) {
					MsgReplyv( rcvid, ENOTSUP, &iov, 1 );
				}
				break;
		}
	}

	return NULL;
}


static int create_dma_interrupt_handler( dma_ctx_t * dma_ctx ) {
	dctrl_t 				*dc = dma_ctx->dc;
	pthread_attr_t     		attr;
	struct sched_param  	param;
	int						error;
	
	dma_ctx->chid = ChannelCreate( _NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK );
	if ( dma_ctx->chid < 0 ) {
		error = errno;
		musb_slogf(dc, _SLOG_ERROR, "%s: failed to create channel (error=%d)",__func__,error);	
		goto fail;
	}
	
	dma_ctx->coid = ConnectAttach( 0, 0, dma_ctx->chid, _NTO_SIDE_CHANNEL, 0 );
	if ( dma_ctx->coid < 0 ) {
		error = errno;
		musb_slogf(dc, _SLOG_ERROR, "%s: failed to attach to channel (error=%d)",__func__);	
		goto fail2;
	}
	
	dma_ctx->dma_event.sigev_notify          = SIGEV_PULSE;
	dma_ctx->dma_event.sigev_coid            = dma_ctx->coid;
	dma_ctx->dma_event.sigev_code            = DMA_PULSE_INTR;
	dma_ctx->dma_event.sigev_priority        = 21;
	dma_ctx->dma_event.sigev_value.sival_ptr = NULL;
	
	pthread_attr_init( &attr );
	pthread_attr_setschedpolicy( &attr, SCHED_RR );
	param.sched_priority = 21;
	pthread_attr_setschedparam( &attr, &param );
	pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
	
	error = pthread_create( &dma_ctx->tid, &attr, (void*)dma_event_handler, dma_ctx );
	if( error != EOK ) {
		musb_slogf(dc, _SLOG_ERROR, "%s: failed to create interrupt handler thread (error = %d) ",__func__,error);
		goto fail3;
	}
	
	// Attach Interrupt Handler for PIO interrupt
	dma_ctx->iid = InterruptAttachEvent( dma_ctx->irq, &dma_ctx->dma_event, _NTO_INTR_FLAGS_TRK_MSK );
	if (  dma_ctx->iid < 0 ) {
		error = errno;
		musb_slogf(dc, _SLOG_ERROR, "%s: InterruptAttachEvent() failed (error=%d)",__func__,error);
		goto fail4;
	}

	return EOK;

fail4:
	pthread_cancel( dma_ctx->tid );
fail3:
	ConnectDetach( dma_ctx->coid );	
fail2:
	ChannelDestroy( dma_ctx->chid );
fail:
	return error;	
}
 
static void destroy_dma_interrupt_handler( dma_ctx_t * dma_ctx ) {
	InterruptDetach( dma_ctx->iid );
	pthread_cancel( dma_ctx->tid );
	ConnectDetach( dma_ctx->coid );	
	ChannelDestroy( dma_ctx->chid );
}


////////////////////////////////////////////////////////////////////////////////
//                         PUBLIC FUNCTIONS                                   //
////////////////////////////////////////////////////////////////////////////////


// dma_interrupt specifies the interrupt vector for dma interrupt.. must be positive number
void * 	dma_init( dctrl_t * dc  ) {
	dma_ctx_t	*dma_ctx;
	
	dma_ctx = calloc( 1, sizeof( dma_ctx_t ) );
	if ( dma_ctx == NULL ) {
		musb_slogf(dc, _SLOG_ERROR, "%s: calloc failed",__func__);		
		goto fail;
	}
	dma_ctx->dc = dc;
	
	// only attach to interrupt if we have a valid vector
	if ( dc->musbmhdrc_cfg.dma_irq > 0 ) {
		dma_ctx->irq = dc->musbmhdrc_cfg.dma_irq;
		if ( create_dma_interrupt_handler( dma_ctx ) != EOK ) {
			musb_slogf(dc, _SLOG_ERROR, "%s: failed to create interrupt handler ",__func__);		
			goto fail2;	
		}
	}
	
	return dma_ctx;
fail2:
	free( dma_ctx );	
fail:
	return NULL;	
	
}

void    dma_fini( void * hdl ) {
	dma_ctx_t	*dma_ctx = hdl;
	destroy_dma_interrupt_handler( dma_ctx );
	free ( dma_ctx );
}

// success : return channel number 0 -> 7
// fail: return -1
// USE the returned channel number in subsequent dma operations
// this function forces the use of MODE1 DMA
int	dma_channel_claim( void * hdl,  ep_ctx_t *ep  ) {
	int 			chidx;
	dma_ctx_t		*dma_ctx = hdl;
	channel_ctx_t	*ch_ctx;
	dctrl_t 		*dc = dma_ctx->dc;
	uint32_t		dma_cntl;
	
	
	for( chidx= 0 ; chidx < MAX_DMA_CHANNELS; chidx++ ) {
		ch_ctx = &dma_ctx->dma_channel_ctx_arr[chidx];
		
		if ( ch_ctx->channel_in_use == 0) {
			ch_ctx->channel_in_use = 1;
			ch_ctx->ep = ep;
			
			dma_cntl = 	( ( ep->num << DMA_CNTL_EP_POS ) & DMA_CNTL_EP_MSK ) | 
			            ( ( ep->dir ) ?  DMA_CNTL_DIR_TX : DMA_CNTL_DIR_RX ) |
			            DMA_CNTL_BURSTMODE_UNSPEC_4_8_16                   |
			            DMA_CNTL_INTR_EN                                   |
			            DMA_CNTL_MODE1;
			HW_Write32( dc->IoBase, MUSB_DMA_CNTL(chidx), dma_cntl );

			return chidx;	
		}
	}
	
	// if we fall out of the loop, that means we didn't find a channel, so return error
	return -1;
}

void    dma_channel_release( void * hdl, int chidx ) {
	dma_ctx_t		*dma_ctx = hdl;
	dctrl_t 		*dc = dma_ctx->dc;

	HW_Write32( dc->IoBase, MUSB_DMA_CNTL(chidx), 0 );
	dma_ctx->dma_channel_ctx_arr[chidx].channel_in_use = 0;
}


// this dma implementation only supports type 1 dma, so the dmamode parameter is currently ignored
void dma_transfer_start( void * hdl, int chidx, uint32_t start_addr_paddr, unsigned length, dmamode_t dmamode ) {
	dma_ctx_t		*dma_ctx = hdl;
	channel_ctx_t	*ch_ctx = &dma_ctx->dma_channel_ctx_arr[chidx];
	uint32_t		dma_cntl;
	dctrl_t 		*dc = dma_ctx->dc;
	
	MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: epnum = 0x%x epdir = 0x%x chidx = 0x%x start_addr_paddr = 0x%x length = 0x%x ",__func__,ch_ctx->ep->num, ch_ctx->ep->dir, chidx , start_addr_paddr, length );
	
	MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: pre csr = 0x%x dmacount = %d dmactl = 0x%x ",__func__, 
		( ch_ctx->ep->dir )? HW_Read16( dc->IoBase, MUSB_TXCSR(ch_ctx->ep->num) ) : HW_Read16( dc->IoBase, MUSB_RXCSR(ch_ctx->ep->num) ), 
		HW_Read32( dc->IoBase, MUSB_DMA_COUNT(chidx) ),
		HW_Read32( dc->IoBase, MUSB_DMA_CNTL(chidx) ) 	);
	
	
	
	
	ch_ctx->start_addr_paddr = start_addr_paddr;
	ch_ctx->length = length;
	ch_ctx->channel_active = 1;
	ch_ctx->process_residual_required = 0;

	if ( ch_ctx->ep->dir == USB_ENDPOINT_IN  ) {
		if ( length % ch_ctx->ep->mps ) {
			// ref manual says that residual processing is required when last
			// usb packet is not mps
			ch_ctx->process_residual_required = 1;
		}
	} 
	
	// program the dma control regs
	HW_Write32( dc->IoBase, MUSB_DMA_ADDR(chidx), start_addr_paddr );
	HW_Write32( dc->IoBase, MUSB_DMA_COUNT(chidx), length );	
	dma_cntl = HW_Read32( dc->IoBase, MUSB_DMA_CNTL(chidx) );
	dma_cntl &= ~DMA_CNTL_ERROR ;
	HW_Write32( dc->IoBase, MUSB_DMA_CNTL(chidx), dma_cntl | DMA_CNTL_ENABLE );



/*... for debug only	
	usleep(1000);

	MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: post csr = 0x%x dmacount = %d dmactl = 0x%x ",__func__, 
		( ch_ctx->ep->dir )? HW_Read16( dc->IoBase, MUSB_TXCSR(ch_ctx->ep->num) ) : HW_Read16( dc->IoBase, MUSB_RXCSR(ch_ctx->ep->num) ), 
		HW_Read32( dc->IoBase, MUSB_DMA_COUNT(chidx) ),
		HW_Read32( dc->IoBase, MUSB_DMA_CNTL(chidx) ) 	);
*/	
	
}


uint32_t dma_rx_complete( void * hdl, int chidx ) {
	dma_ctx_t		*dma_ctx = hdl;
	channel_ctx_t	*ch_ctx = &dma_ctx->dma_channel_ctx_arr[chidx];
	dctrl_t 		*dc = dma_ctx->dc;
	uint32_t		dma_count = 0;


	
	if ( HW_Read32( dc->IoBase, MUSB_DMA_CNTL(chidx) ) & DMA_CNTL_ENABLE ) {
		/* Terminate the DMA, and report how much Data was transfered to URB Buffer 
		 * because no Interrupt was generated ( short transfer received )
		 */
		
		// disable the dma in case it is still active
		HW_Write32And( dc->IoBase, MUSB_DMA_CNTL(chidx), ~DMA_CNTL_ENABLE );
		ch_ctx->channel_active = 0;
		
		MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: REPORT DMA COUNT epnum = 0x%x epdir = 0x%x chidx = %d dma_cntl = 0x%x dmacount = %d dmalength = %d"
			,__func__, ch_ctx->ep->num, ch_ctx->ep->dir, chidx, HW_Read32( dc->IoBase, MUSB_DMA_CNTL(chidx) ), 
			HW_Read32( dc->IoBase, MUSB_DMA_COUNT(chidx) ),  ch_ctx->length );
		
		dma_count = ch_ctx->length - HW_Read32( dc->IoBase, MUSB_DMA_COUNT(chidx) );
		
	} else  {
		if ( ch_ctx->channel_active ) {
			/* DMA complete, but handler wasn't executed ( out of order interrupts? ) before the
			 * next short packet ( belonging to the next transfer ) was processed by the 
			 * endpoint interrupt.  So, complete it now.
			 */
			MUSB_SLOGF_DBG(dc, _SLOG_DEBUG1, "%s: epnum = 0x%x epdir = 0x%x chidx = %d Interrupt hasn't processed yet...force handler execution"
				,__func__ , ch_ctx->ep->num, ch_ctx->ep->dir, chidx );

			if ( HW_Read8( dc->IoBase, MUSB_DMA_INTR ) & ( 1 << chidx ) ) {
				dma_channel_interrupt_handler( dma_ctx, chidx );
			} else {
				musb_slogf(dc, _SLOG_ERROR, "%s: Interrupt status bit isn't set!!!!",__func__ );
			}
		}
	}

	return dma_count;
}


void	dma_transfer_abort( void * hdl, int chidx ) {
	dma_ctx_t		*dma_ctx = hdl;
	channel_ctx_t	*ch_ctx = &dma_ctx->dma_channel_ctx_arr[chidx];
	
	dctrl_t 		*dc = dma_ctx->dc;
	
	HW_Write32And( dc->IoBase, MUSB_DMA_CNTL(chidx), ~DMA_CNTL_ENABLE );
	ch_ctx->channel_active = 0;
}



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/musbmhdrc/dma.c $ $Rev: 746301 $")
#endif
