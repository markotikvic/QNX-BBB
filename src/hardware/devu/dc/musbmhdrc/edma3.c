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

#include "musb.h"
#include "stdio.h"
#include <malloc.h>
#include <errno.h>
#include <atomic.h>
#include <sys/mman.h>
#include <sys/slog.h>
#include <unistd.h>
#include <sys/rsrcdbmgr.h>
#include <arm/dm6446.h>

#define DEFAULT_EDMA_IRQ_BASE			0x200
#define	DEFAULT_EDMA_BASE				0x49000000
#define INVALID_EDMA_CHNL				-1

#define EDMA_TRANSFER_RECEIVE			0
#define EDMA_TRANSFER_TRANSMIT			1

#define EDMA_PULSE_INTR					(_PULSE_CODE_MINAVAIL + 1)
#define EDMA_PULSE_INTR_EPNUM_1			( EDMA_PULSE_INTR + 1 )
#define EDMA_PULSE_INTR_EPNUM_2			( EDMA_PULSE_INTR + 2 )
#define EDMA_PULSE_INTR_EPNUM_3			( EDMA_PULSE_INTR + 3 )
#define EDMA_PULSE_INTR_EPNUM_4			( EDMA_PULSE_INTR + 4 )
#define EDMA_PULSE_INTR_EPNUM_5			( EDMA_PULSE_INTR + 5 )
#define EDMA_PULSE_INTR_EPNUM_6			( EDMA_PULSE_INTR + 6 )
#define EDMA_PULSE_INTR_EPNUM_7			( EDMA_PULSE_INTR + 7 )
#define EDMA_PULSE_INTR_EPNUM_8			( EDMA_PULSE_INTR + 8 )
#define EDMA_PULSE_INTR_EPNUM_9			( EDMA_PULSE_INTR + 9 )
#define EDMA_PULSE_INTR_EPNUM_10		( EDMA_PULSE_INTR + 10 )
#define EDMA_PULSE_INTR_EPNUM_11		( EDMA_PULSE_INTR + 11 )
#define EDMA_PULSE_INTR_EPNUM_12		( EDMA_PULSE_INTR + 12 )
#define EDMA_PULSE_INTR_EPNUM_13		( EDMA_PULSE_INTR + 13 )
#define EDMA_PULSE_INTR_EPNUM_14		( EDMA_PULSE_INTR + 14 )
#define EDMA_PULSE_INTR_EPNUM_15		( EDMA_PULSE_INTR + 15 )

struct _edma_ctx;
typedef struct {
	struct _edma_ctx *edma_ctx;
	int				chidx;				/* EDMA channel number */
	int				irq;
	int				iid;
	uint32_t		buff_addr;			/* vaddr of the buffer is being transfered */
	int				transfer_len;		/* length of the active transfer */
	struct sigevent	edma_event;
	ep_ctx_t		*ep;
} channel_ctx_t;

typedef struct _edma_ctx {
	channel_ctx_t	edma_channel_ctx_arr[MAX_N_ENDPOINTS];
	dctrl_t			*dc;
	uintptr_t		base;				/* EDMA Channel Controller Base Address */
	int				num_ep;
	int				chid;
	int				coid;
	int				tid;
} edma_ctx_t;

typedef struct {
	volatile uint32_t	opt;
	volatile uint32_t	src;
	volatile uint16_t	acnt;
	volatile uint16_t	bcnt;
	volatile uint32_t	dst;
	volatile uint16_t	srcbidx;
	volatile uint16_t	dstbidx;
	volatile uint16_t	link;
	volatile uint16_t	bcntrld;
	volatile uint16_t	srccidx;
	volatile uint16_t	dstcidx;
	volatile uint16_t	ccnt;
	volatile uint16_t	reserved;
} edma3_param_t;

static void
edma3_setbit( edma_ctx_t *edma_ctx, int reg, int chidx )
{
	uint32_t base = edma_ctx->base + DM6446_EDMA_REGION0;

	if( chidx > 31 ) {
		reg += 4;
		chidx -= 32;
	}

	HW_Write32( ( uint8_t * )base, reg, ( 1 << chidx ) );
}

static void
edma3_enable_transfer( edma_ctx_t *edma_ctx, int chidx)
{
	edma3_setbit( edma_ctx, DM6446_EDMA_IESR, chidx );
	edma3_setbit( edma_ctx, DM6446_EDMA_EESR, chidx );
	edma3_setbit( edma_ctx, DM6446_EDMA_ESR, chidx );
}

static int
edma3_transfer_done( edma_ctx_t *edma_ctx, int chidx )
{
	/* Clear IPR bit */
	edma3_setbit( edma_ctx, DM6446_EDMA_ICR, chidx );

	/* Disable this EDMA event */
	edma3_setbit( edma_ctx, DM6446_EDMA_EECR, chidx );

	/* Disable interrupt */
	edma3_setbit( edma_ctx, DM6446_EDMA_IECR, chidx );
	return EOK;
}

static int
edma_setup_transfer( edma_ctx_t *edma_ctx, int chidx, uint32_t dst, uint32_t src, int len, int dir )
{
	edma3_param_t *param;
	param = ( edma3_param_t * )( edma_ctx->base + DM6446_EDMA_PARAM_BASE + ( DM6446_EDMA_PARAM_SIZE * chidx ) );

	/* In case of there is a pending events */
	edma3_setbit( edma_ctx, DM6446_EDMA_ECR, chidx );

	param->opt = (0 << 23)		/* ITCCHEN = 0 */
				| (0 << 22)		/* TCCHEN = 0 */
				| (0 << 21)		/* ITCINTEN = 0*/
				| (1 << 20)		/* TCINTEN = 0*/
				| (chidx << 12)	/* TCC */
				| (0 << 11)		/* Normal completion */
				| (0 << 8)		/* FWID, don't care */
				| (1 << 3)		/* PaRAM set is static */
				| (1 << 2)		/* AB-synchronizad */
				| (0 << 1)		/* Destination address increment mode */
				| (0 << 0);		/* Source address non-increment mode */

	param->src = src;
	param->dst = dst;
	param->acnt = 4;

	/*
	 * For AB-synchronized transfers, the start address	of Array N
	 * is equal to the start address of Array N – 1
	 * plus source (SRC) or destination (DST) BIDX
	 */
	if( dir == EDMA_TRANSFER_TRANSMIT ) {
		param->srcbidx = param->acnt;
		param->dstbidx = 0;
	} else {
		param->srcbidx = 0;
		param->dstbidx = param->acnt;
	}

	/* Assume no less than 4 bytes */
	param->bcnt = len / param->acnt;
	param->link = 0xFFFF;
	param->bcntrld = 0;
	param->srccidx = 0;
	param->dstcidx = 0;
	param->ccnt = 1;
	param->reserved = 0;

	/* Enable transfer */
	edma3_enable_transfer( edma_ctx, chidx);
	return 0;
}

void
edma_transfer_start( void * hdl, int epnum, uint32_t start_addr_paddr, unsigned length, dmamode_t dmamode )
{
	edma_ctx_t		*edma_ctx = ( edma_ctx_t * )hdl;
	channel_ctx_t	*ch_ctx;
	musb_transfer_t *td;

	ch_ctx = &edma_ctx->edma_channel_ctx_arr[epnum];
	td = ch_ctx->ep->td;

	/* Save the virtual address for later use */
	ch_ctx->buff_addr = td->xfer_buffer + start_addr_paddr - td->xfer_buffer_paddr;
	ch_ctx->transfer_len = length;

	if( ch_ctx->ep->dir == USB_ENDPOINT_IN ) {
		/* Transmit */
		edma_setup_transfer( edma_ctx, ch_ctx->chidx, edma_ctx->dc->pIoBase + FIFO( epnum ), start_addr_paddr, length, EDMA_TRANSFER_TRANSMIT );
	} else {
		/* Receive */
		edma_setup_transfer( edma_ctx, ch_ctx->chidx, start_addr_paddr, edma_ctx->dc->pIoBase + FIFO( epnum ), length, EDMA_TRANSFER_RECEIVE );
	}
	return;
}

const struct sigevent *
edma_intr_handler( void *area, int id )
{
	channel_ctx_t	*ch_ctx = ( channel_ctx_t * )area;
	edma_ctx_t		*edma_ctx = ch_ctx->edma_ctx;
	dctrl_t			*dc = edma_ctx->dc;
	musb_transfer_t	*td = ch_ctx->ep->td;
	int				epnum = ch_ctx->ep->num;
	int				remainder;
	uint8_t			*pbuf;

	/* Clean up EDMA channel */
	edma3_transfer_done( edma_ctx, ch_ctx->chidx );

	if( ( remainder = ch_ctx->transfer_len & 0x3 ) ) {
		pbuf = ( uint8_t *)( ch_ctx->buff_addr + ch_ctx->transfer_len - remainder );
		if( ch_ctx->ep->dir == USB_ENDPOINT_OUT ) { /* receive */
			while( remainder ) {
				*pbuf++ = HW_Read8( ( uint8_t * )dc->IoBase, FIFO( epnum ) );
				remainder--;
			}
		}
		else {	/* transmit */
			while( remainder ) {
				HW_Write8( ( uint8_t * )dc->IoBase, FIFO( epnum ), *pbuf++ );
				remainder--;
			}
		}
	}

	/* For Tx transfers, set the data ready flag */
	if( ch_ctx->ep->dir == USB_ENDPOINT_IN ) {
		HW_Write16Or( dc->IoBase, MUSB_TXCSR( epnum ), TXCSR_TXPKTRDY );
	} else {
		td->bytes_xfered += ch_ctx->transfer_len;
		if( ( td->bytes_xfered >= td->xfer_length) || ( ch_ctx->transfer_len != ch_ctx->ep->mps) ) {

			/* The complete_urb() should be called in thread context */
			return &( ch_ctx->edma_event );
		} else {
			// more data to receive
			HW_Write16And( dc->IoBase, MUSB_RXCSR( epnum ), ~RXCSR_RXPKTRDY );
		}
	}

	return NULL;
}

void
edma_transfer_abort( void * hdl, int epnum )
{
	edma_ctx_t *edma_ctx = ( edma_ctx_t * )hdl;
	edma3_transfer_done( edma_ctx, edma_ctx->edma_channel_ctx_arr[epnum].chidx );
}

static void
edma_detach_channel( edma_ctx_t *edma_ctx, int epnum )
{
	if( edma_ctx->edma_channel_ctx_arr[epnum].chidx != INVALID_EDMA_CHNL ) {
		rsrc_request_t req = { 0 };

		req.length = 1;
		req.flags = RSRCDBMGR_DMA_CHANNEL;
		req.start = req.end = edma_ctx->edma_channel_ctx_arr[epnum].chidx;
		rsrcdbmgr_detach( &req, 1 );
		edma_ctx->edma_channel_ctx_arr[epnum].chidx = INVALID_EDMA_CHNL;
	}

	if( edma_ctx->edma_channel_ctx_arr[epnum].iid != -1 ) {
		InterruptDetach( edma_ctx->edma_channel_ctx_arr[epnum].iid );
		edma_ctx->edma_channel_ctx_arr[epnum].iid = -1;
	}
}

/*
 * Literally, edma_channel_claim() is supposed to return the allocated EDMA channel
 * But we return end point number here in order to make future look up easier.
 * Since musb.c doesn't really use the DMA channel, so this trick works fine.
 */
int
edma_channel_claim( void * hdl, ep_ctx_t *ep )
{
	edma_ctx_t		*edma_ctx = ( edma_ctx_t *)hdl;
	dctrl_t			*dc = edma_ctx->dc;
	channel_ctx_t	*ch_ctx;
	rsrc_request_t	req = { 0 };

	if( ( ep->num <= 0 ) || ( ep->num >= edma_ctx->num_ep) )
		return INVALID_EDMA_CHNL;

	ch_ctx = &edma_ctx->edma_channel_ctx_arr[ep->num];
	ch_ctx->ep = ep;

	if( ch_ctx->chidx != INVALID_EDMA_CHNL )
		return ep->num;

	req.start = dc->edma_channel;
	req.end = ~0;
	req.length = 1;
	req.flags = RSRCDBMGR_FLAG_RANGE | RSRCDBMGR_DMA_CHANNEL;

	if( rsrcdbmgr_attach( &req, 1 ) == EOK ) {
		/* dc->edma_channel is the first available EDMA channel */
		ch_ctx->chidx = req.start;

		/* dc->edma_irq is the IRQ corresponds to the first available EDMA channel */
		if( dc->edma_irq )
			ch_ctx->irq = req.start + dc->edma_irq - dc->edma_channel;
		else
			ch_ctx->irq = req.start + DEFAULT_EDMA_IRQ_BASE;
	} else {
		musb_slogf( dc, _SLOG_ERROR, "%s - Failed to reserve EDMA channel", __func__);
		return INVALID_EDMA_CHNL;
	}

	ch_ctx->iid = InterruptAttach( ch_ctx->irq, edma_intr_handler,
			ch_ctx, sizeof(*ch_ctx), _NTO_INTR_FLAGS_TRK_MSK);

	if( ch_ctx->iid == -1 ) {
		musb_slogf(dc, _SLOG_ERROR, "%s - failed to attached EDMA intrrupt", __func__);
		rsrcdbmgr_detach( &req, 1 );
		ch_ctx->chidx = INVALID_EDMA_CHNL;
	}

	musb_slogf( dc, _SLOG_INFO, "%s - EP %d reserved EDMA channel %d IRQ %d",
				__func__, ep->num, ch_ctx->chidx, ch_ctx->irq );

	return ep->num;
}

void
edma_channel_release( void * hdl, int epnum )
{
	edma_ctx_t *edma_ctx = ( edma_ctx_t *)hdl;

	edma3_transfer_done( edma_ctx, edma_ctx->edma_channel_ctx_arr[epnum].chidx );
	edma_detach_channel( edma_ctx, epnum );
	edma_ctx->edma_channel_ctx_arr[epnum].chidx = INVALID_EDMA_CHNL;
}

uint32_t
edma_rx_complete( void * hdl, int epnum )
{
	return EOK;
}

static void *
edma_event_handler( edma_ctx_t * edma_ctx )
{
	dctrl_t			*dc = edma_ctx->dc;
	channel_ctx_t	*ch_ctx;
	struct _pulse	pulse;
	iov_t			iov;
	int				rcvid;

	SETIOV( &iov, &pulse, sizeof( pulse ) );

	while( 1 ) {
		if( ( rcvid = MsgReceivev( edma_ctx->chid, &iov, 1, NULL ) ) == -1 ) {
			continue;
		}

		switch( pulse.code ) {
			case EDMA_PULSE_INTR_EPNUM_1:
			case EDMA_PULSE_INTR_EPNUM_2:
			case EDMA_PULSE_INTR_EPNUM_3:
			case EDMA_PULSE_INTR_EPNUM_4:
			case EDMA_PULSE_INTR_EPNUM_5:
			case EDMA_PULSE_INTR_EPNUM_6:
			case EDMA_PULSE_INTR_EPNUM_7:
			case EDMA_PULSE_INTR_EPNUM_8:
			case EDMA_PULSE_INTR_EPNUM_9:
			case EDMA_PULSE_INTR_EPNUM_10:
			case EDMA_PULSE_INTR_EPNUM_11:
			case EDMA_PULSE_INTR_EPNUM_12:
			case EDMA_PULSE_INTR_EPNUM_13:
			case EDMA_PULSE_INTR_EPNUM_14:
			case EDMA_PULSE_INTR_EPNUM_15:
				ch_ctx = &edma_ctx->edma_channel_ctx_arr[pulse.code - EDMA_PULSE_INTR];
				atomic_set(&ch_ctx->ep->flags, EP_FLAG_DEFER_OUT_PKT_CLEAR);

				/* complete_urb() assumes mutex is locked */
				MUSB_MUTEX_LOCK
				complete_urb( edma_ctx->dc, ch_ctx->ep, 0 );
				MUSB_MUTEX_UNLOCK
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

void *
edma_init( dctrl_t * dc )
{
	edma_ctx_t			*edma_ctx;
	pthread_attr_t		attr;
	struct sched_param	param;
	int epnum;

	if( ( edma_ctx = (edma_ctx_t *) calloc( 1, sizeof( edma_ctx_t ) ) ) == NULL ) {
		musb_slogf( dc, _SLOG_ERROR, "%s - calloc failed", __func__);
		goto out;
	}

	edma_ctx->num_ep = MAX_N_ENDPOINTS;
	edma_ctx->dc = dc;

	for( epnum = 0; epnum < edma_ctx->num_ep; epnum++ ) {
		edma_ctx->edma_channel_ctx_arr[epnum].edma_ctx = edma_ctx;
		edma_ctx->edma_channel_ctx_arr[epnum].chidx = INVALID_EDMA_CHNL;
		edma_ctx->edma_channel_ctx_arr[epnum].irq = -1;
		edma_ctx->edma_channel_ctx_arr[epnum].iid = -1;
	}

	/* Map in DMA registers */
	if( ( edma_ctx->base = (uintptr_t)mmap_device_memory ( 0, DM6446_EDMA_SIZE,
			PROT_READ | PROT_WRITE | PROT_NOCACHE, 0, DEFAULT_EDMA_BASE ) ) == (uintptr_t)MAP_FAILED ) {
		musb_slogf( dc, _SLOG_ERROR, "%s - calloc failed", __func__);
		goto fail1;
	}

	edma_ctx->chid = ChannelCreate( _NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK );
	if( edma_ctx->chid < 0 ) {
		musb_slogf( dc, _SLOG_ERROR, "%s: failed to create channel (error=%d)", __func__, errno );
		goto fail2;
	}

	edma_ctx->coid = ConnectAttach( 0, 0, edma_ctx->chid, _NTO_SIDE_CHANNEL, 0 );
	if( edma_ctx->coid < 0 ) {
		musb_slogf(dc, _SLOG_ERROR, "%s: failed to attach to channel (error=%d)", __func__, errno );
		goto fail3;
	}

	for( epnum = 1; epnum < edma_ctx->num_ep; epnum++ ) {
		channel_ctx_t * ch_ctx = &edma_ctx->edma_channel_ctx_arr[epnum];
		ch_ctx->edma_event.sigev_notify			= SIGEV_PULSE;
		ch_ctx->edma_event.sigev_coid			= edma_ctx->coid;
		ch_ctx->edma_event.sigev_code			= EDMA_PULSE_INTR + epnum;
		ch_ctx->edma_event.sigev_priority		= 21;
		ch_ctx->edma_event.sigev_value.sival_ptr = NULL;
	}

	pthread_attr_init( &attr );
	pthread_attr_setschedpolicy( &attr, SCHED_RR );
	param.sched_priority = 21;
	pthread_attr_setschedparam( &attr, &param );
	pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

	if( pthread_create( &edma_ctx->tid, &attr, (void*)edma_event_handler, edma_ctx ) != EOK) {
		musb_slogf(dc, _SLOG_ERROR, "%s: failed to create interrupt handler thread (error = %d) ",__func__, errno);
		goto fail4;
	}

	goto out;

fail4:
	ConnectDetach( edma_ctx->coid );
fail3:
	ChannelDestroy( edma_ctx->chid );
fail2:
	munmap_device_memory( ( void * )edma_ctx->base, DM6446_EDMA_SIZE );
fail1:
	free( edma_ctx );
	edma_ctx = NULL;
out:
	return edma_ctx;
}

void
edma_fini( void * hdl )
{
	int epnum;
	edma_ctx_t *edma_ctx = ( edma_ctx_t *)hdl;

	/* Skip control EP */
	for( epnum = 1; epnum < edma_ctx->num_ep; epnum++ ) {
		if( edma_ctx->edma_channel_ctx_arr[epnum].chidx != INVALID_EDMA_CHNL ) {
			edma3_transfer_done( edma_ctx, edma_ctx->edma_channel_ctx_arr[epnum].chidx );
			edma_detach_channel( edma_ctx, epnum );
		}
	}

	munmap_device_memory( ( void * )edma_ctx->base, DM6446_EDMA_SIZE );

	pthread_cancel( edma_ctx->tid );
	ConnectDetach( edma_ctx->coid );
	ChannelDestroy( edma_ctx->chid );
	free( edma_ctx );
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/musbmhdrc/edma3.c $ $Rev: 762989 $")
#endif
