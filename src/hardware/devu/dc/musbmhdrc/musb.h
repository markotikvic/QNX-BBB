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

#ifndef _MUSB_H_INCLUDED
#define _MUSB_H_INCLUDED

#include "sys/io-usb_dcd.h"
#include <queue.h>

////////////////////////////////////////////////////////////////////////////////
//                              DEBUG OPTIONS                                 //
////////////////////////////////////////////////////////////////////////////////

//#define MUSB_DEBUG
#ifdef MUSB_REAL_ISR
#define MUSB_LOCK InterruptLock(&(dc->lock));
#define MUSB_UNLOCK InterruptUnlock(&(dc->lock));
#else
#define MUSB_LOCK
#define MUSB_UNLOCK
#endif

#ifdef MUSB_DEBUG
	#define MUSB_SLOGF_DBG( dc, level, fmt, ... ) musb_slogf( dc, level, fmt, __VA_ARGS__ )
#ifndef MUSB_REAL_ISR
	#define MUSB_SLOGF_ISR_DBG( dc, level, fmt, ... ) musb_slogf( dc, level, fmt, __VA_ARGS__ )
#else
	#define MUSB_SLOGF_ISR_DBG( dc, level, fmt, ... ) 
#endif
#else
	#define MUSB_SLOGF_DBG( dc, level, fmt, ... )
	#define MUSB_SLOGF_ISR_DBG( dc, level, fmt, ... ) 
#endif


#define MUSB_MUTEX_LOCK 	if( pthread_mutex_lock( &dc->mutex ) ) { \
		musb_slogf( dc, _SLOG_ERROR, "%s(%d): mutex lock failed", __FILE__, __LINE__ ); 	}
#define MUSB_MUTEX_UNLOCK 	if( pthread_mutex_unlock( &dc->mutex ) ) { \
		musb_slogf( dc, _SLOG_ERROR, "%s(%d): mutex unlock failed", __FILE__, __LINE__ ); }

#undef SIMPLEQ_REMOVE_HEAD

#define	SIMPLEQ_REMOVE_HEAD(head, field) do {					\
	if (((head)->sqh_first = (head)->sqh_first->field.sqe_next) == _NULL)	\
 		(head)->sqh_last = &(head)->sqh_first;				\
 } while (/*CONSTCOND*/0)

#define	SIMPLEQ_REMOVE(head, elm, type, field) do {			  \
 	if ((head)->sqh_first == (elm)) {				  \
		SIMPLEQ_REMOVE_HEAD((head), field);			  \
 	} else {							  \
 		struct type *curelm = (head)->sqh_first;		  \
 		while (curelm->field.sqe_next != (elm))		 	  \
 			curelm = curelm->field.sqe_next;		  \
 		if ((curelm->field.sqe_next =				  \
 			curelm->field.sqe_next->field.sqe_next) == _NULL) \
 			    (head)->sqh_last = &(curelm)->field.sqe_next; \
 	}								  \
 } while (/*CONSTCOND*/0)

#define MUSB_PULSE_INTR  				(_PULSE_CODE_MINAVAIL+1)
#define MUSB_PULSE_EVENT_CANCELLATION 	(_PULSE_CODE_MINAVAIL+2)

////////////////////////////////////////////////////////////////////////////////


#define MUSB_SIZE                 	0x400        

// controller suports a maximum of 16 bidirectional endpoints.
#define MAX_N_ENDPOINTS			 	16

#define MIN_EDMA_LEN					64 /* For external DMA, if the transfer less this will use PIO transfer */

/* Registers Offsets, and bit definitions */

// COMMON USB REGISTERS
#define MUSB_FADDR					0x0		// 8-bit

#define MUSB_POWER					0x1		// 8-bit
	#define POWER_ENABLE_SUSPEND_M			( 1 << 0 )
	#define POWER_SUSPEND_MODE				( 1 << 1 )
	#define POWER_RESUME					( 1 << 2 )
	#define POWER_RESET						( 1 << 3 )
	#define POWER_HS_MODE					( 1 << 4 )
	#define POWER_HS_ENABLE					( 1 << 5 )
	#define POWER_SOFT_CONN					( 1 << 6 )
	#define POWER_ISO_UPDATE				( 1 << 7 )
	
#define MUSB_INTRTX					0x2		// 16-bit
#define MUSB_INTRRX					0x4		// 16-bit
#define MUSB_INTRTXE				0x6		// 16-bit
#define MUSB_INTRRXE				0x8		// 16-bit
	#define EP_INT_MSK(epnum)			( 1 << epnum )

#define MUSB_INTRUSB				0xa		// 8-bit
#define MUSB_INTRUSBE				0xb		// 8-bit
	#define INTRUSB_SUSPEND					( 1 << 0 )
	#define INTRUSB_RESUME					( 1 << 1 )
	#define INTRUSB_BABBLE					( 1 << 2 )
	#define INTRUSB_RESET					( 1 << 2 )
	#define INTRUSB_SOF						( 1 << 3 )
	#define INTRUSB_CONN					( 1 << 4 )
	#define INTRUSB_DISCON					( 1 << 5 )
	#define INTRUSB_SESS_REQ				( 1 << 6 )
	#define INTRUSB_VBUS_ERR				( 1 << 7 )

#define USB_DEV_STATE_CHAGE					(INTRUSB_RESET|INTRUSB_DISCON|INTRUSB_SUSPEND|INTRUSB_RESUME)
#define USB_EP0_SETUP_PULSE					(1<<31)


#define MUSB_FRAME					0xc		// 16-bit
#define MUSB_INDEX					0xe		// 8-bit

#define MUSB_TESTMODE				0xf		// 8-bit
	#define MUSB_TEST_SE0_NAK					(1<<0)
	#define MUSB_TEST_TEST_J					(1<<1)
	#define MUSB_TEST_TEST_K					(1<<2)
	#define MUSB_TEST_PACKET					(1<<3)
	#define MUSB_TEST_FORCE_HS					(1<<4)
	#define MUSB_TEST_FORCE_FS					(1<<5)


// EP FIFOs
#define FIFO(n) 					( 0x20 + ( (n) * 4) )	// 8,16,32 bit access

// Additional Control and Configuration Registers
#define MUSB_DEVCTL					0x60	// 8-bit
	#define DEVCTL_SESSION					( 1 << 0 )
	#define DEVCTL_HOST_REQ					( 1 << 1 )
	#define DEVCTL_HOST_MODE				( 1 << 2 )
	#define DEVCTL_VBUS0					( 1 << 3 )
	#define DEVCTL_VBUS1					( 1 << 4 )
	#define DEVCTL_LSDEV					( 1 << 5 )
	#define DEVCTL_FSDEV					( 1 << 6 )
	#define DEVCTL_B_DEVICE					( 1 << 7 )
	
#define MUSB_MISC					0x61	// 8-bit
	#define MISC_RX_EDMA					( 1 << 0 )
	#define MISC_TX_EDMA					( 1 << 1 )
	
#define MUSB_TXFIFOSZ				0x62	// 8-bit
#define MUSB_RXFIFOSZ				0x63	// 8-bit
	#define FIFOSZ_DOUBLE_BUFFER		( 1 << 4 )
#define MUSB_TXFIFOADD				0x64	// 16-bit
#define MUSB_RXFIFOADD				0x66	// 16-bit
#define MUSB_VCONTROL_VSTATUS		0x68	// 32-bit
#define MUSB_HWVERS					0x6c	// 16-bit

// ULPI Registers... 0x70 - 0x77   see mushhdrc_ulpi_an.pdf for details

#define MUSB_EPINFO					0x78	// 8-bit
#define MUSB_RAMINFO				0x79	// 8-bit
#define MUSB_LINKINFO				0x7a	// 8-bit
#define MUSB_VPLEN					0x7b	// 8-bit
#define MUSB_HS_EOF1				0x7c	// 8-bit
#define MUSB_FS_EOF1				0x7d	// 8-bit
#define MUSB_LS_EOF1				0x7e	// 8-bit
#define MUSB_SOFT_RST				0x7f	// 8-bit


// Target Address Registers
#define MUSB_TXFUNCADDR(n)			( 0x80 + ( (n) * 8 ) )	// 8-bit
#define MUSB_TXHUBADDR(n)			( 0x82 + ( (n) * 8 ) )	// 8-bit
#define MUSB_TXHUBPORT(n)			( 0x83 + ( (n) * 8 ) )	// 8-bit
#define MUSB_RXFUNCADDR(n)			( 0x84 + ( (n) * 8 ) )	// 8-bit
#define MUSB_RXHUBADDR(n)			( 0x86 + ( (n) * 8 ) )	// 8-bit
#define MUSB_RXHUBPORT(n)			( 0x87 + ( (n) * 8 ) )	// 8-bit


// EP Control  Host and Peripheral Mode ( indexed and non indexed ) 
#define MUSB_TXMAXP(n)				( 0x100 + ( 0x10 * (n) ) ) // 16-bit
#define MUSB_IDX_TXMAXP				0x10	// 16-bit
	#define TXMAXP_MPS_POS					0
	#define TXMAXP_MPS_MSK					( 0x3ff << TXMAXP_MPS_POS )
	#define TXMAXP_MULT_POS					11
	#define TXMAXP_MULT_MSK					( 0x1f << TXMAXP_MULT_POS )

#define MUSB_CSR0					0x102  // 16-bit
#define MUSB_IDX_CSR0				0x12	// 16-bit
	/* peripheral mode */
	#define CSR0_RXPKTRDY					( 1 << 0 ) 
	#define CSR0_TXPKTRDY					( 1 << 1 ) 
	#define CSR0_SENTSTALL					( 1 << 2 ) 
	#define CSR0_DATAEND					( 1 << 3 ) 
	#define CSR0_SETUPEND					( 1 << 4 ) 
	#define CSR0_SENDSTALL					( 1 << 5 ) 
	#define CSR0_SERVICED_RXPKTRDY			( 1 << 6 ) 
	#define CSR0_SERVICED_SETUP_END			( 1 << 7 ) 
	#define CSR0_FLUSH_FIFO					( 1 << 8 )

	/* host mode */
	//#define CSR0_RXPKTRDY					( 1 << 0 ) 
	//#define CSR0_TXPKTRDY					( 1 << 1 ) 
	#define CSR0_RXSTALL					( 1 << 2 ) 
	#define CSR0_SETUPPKT					( 1 << 3 ) 
	#define CSR0_ERROR						( 1 << 4 ) 
	#define CSR0_REQ_PKT					( 1 << 5 ) 
	#define CSR0_STATUS_PKT					( 1 << 6 ) 
	#define CSR0_NAK_TIMEOUT				( 1 << 7 ) 
	//#define CSR0_FLUSH_FIFO				( 1 << 8 )
	#define CSR0_DATA_TOGGLE				( 1 << 9 )
	#define CSR0_DATA_TOGGLE_WR_EN			( 1 << 10 )
	#define CSR0_DISPING					( 1 << 11 )
	
#define MUSB_TXCSR(n)				( 0x102 + ( 0x10 * (n) ) ) // 16-bit
#define MUSB_IDX_TXCSR				0x12	// 16-bit
	/* peripheral mode */
	#define TXCSR_TXPKTRDY					( 1 << 0 )
	#define TXCSR_FIFO_NOT_EMPTY			( 1 << 1 )
	#define TXCSR_UNDERRUN					( 1 << 2 )
	#define TXCSR_FLUSHFIFO					( 1 << 3 )
	#define TXCSR_SEND_STALL				( 1 << 4 )
	#define TXCSR_SENT_STALL				( 1 << 5 )
	#define TXCSR_CLR_DATA_TOGGLE			( 1 << 6 )
	#define TXCSR_INCOMP_TX					( 1 << 7 )
	#define TXCSR_DMA_REQ_MODE				( 1 << 10 )
	#define TXCSR_DMA_REQ_TYPE0				( 0 << 10 )
	#define TXCSR_DMA_REQ_TYPE1				( 1 << 10 )
	
	#define TXCSR_FORCE_DATA_TOGGLE			( 1 << 11 )
	#define TXCSR_DMA_REQ_EN				( 1 << 12 )
	#define TXCSR_MODE						( 1 << 13 )
	#define TXCSR_ISO						( 1 << 14 )
	#define TXCSR_AUTOSET					( 1 << 15 )
	
	/* host mode */
	//#define TXCSR_TXPKTRDY				( 1 << 0 )
	//#define TXCSR_FIFO_NOT_EMPTY			( 1 << 1 )
	#define TXCSR_ERROR						( 1 << 2 )
	//#define TXCSR_FLUSHFIFO				( 1 << 3 )
	#define TXCSR_SETUP_PKT					( 1 << 4 )
	#define TXCSR_RX_STALL					( 1 << 5 )
	//#define TXCSR_CLR_DATA_TOGGLE			( 1 << 6 )
	//#define TXCSR_INCOMP_TX				( 1 << 7 )
	#define TXCSR_NAK_TIMEOUT				( 1 << 7 )
	#define TXCSR_DATA_TOGGLE				( 1 << 8 )
	#define TXCSR_DATA_TOGGLE_WR_EN			( 1 << 9 )
	//#define TXCSR_DMA_REQ_MODE			( 1 << 10 )
	//#define TXCSR_FORCE_DATA_TOGGLE		( 1 << 11 )
	//#define TXCSR_DMA_REQ_EN				( 1 << 12 )
	//#define TXCSR_MODE					( 1 << 13 )
	//#define TXCSR_AUTOSET					( 1 << 15 )

#define MUSB_RXMAXP(n)				( 0x104 + ( 0x10 * (n) ) ) // 16-bit
#define MUSB_IDX_RXMAXP				0x14	// 16-bit
	#define RXMAXP_MPS_POS					0
	#define RXMAXP_MPS_MSK					( 0x3ff << RXMAXP_MPS_POS )
	#define RXMAXP_MULT_POS					11
	#define RXMAXP_MULT_MSK					( 0x1f << RXMAXP_MULT_POS )

#define MUSB_RXCSR(n)				( 0x106 + ( 0x10 * (n) ) ) // 16-bit
#define MUSB_IDX_RXCSR				0x16	// 16-bit

	/* peripheral */
	#define RXCSR_RXPKTRDY					( 1 << 0 )
	#define RXCSR_FIFO_FULL					( 1 << 1 )
	#define RXCSR_OVERRUN					( 1 << 2 )
	#define RXCSR_DATA_ERROR				( 1 << 3 )
	#define RXCSR_FLUSHFIFO					( 1 << 4 )
	#define RXCSR_SEND_STALL				( 1 << 5 )
	#define RXCSR_SENT_STALL				( 1 << 6 )
	#define RXCSR_CLR_DATA_TOGGLE			( 1 << 7 )
	#define RXCSR_INCOMP_RX					( 1 << 8 )
	#define RXCSR_DMA_REQ_MODE				( 1 << 11 )
	#define RXCSR_DMA_REQ_TYPE0				( 0 << 11 )
	#define RXCSR_DMA_REQ_TYPE1				( 1 << 11 )
	#define RXCSR_DISNYET					( 1 << 12 )
	#define RXCSR_PID_ERR					( 1 << 12 )
	#define RXCSR_DMA_REQ_EN				( 1 << 13 )
	#define RXCSR_ISO						( 1 << 14 )
	#define RXCSR_AUTOCLEAR					( 1 << 15 )

	/* host mode */ 	
	//#define RXCSR_RXPKTRDY				( 1 << 0 )
	//#define RXCSR_FIFO_FULL				( 1 << 1 )
	#define RXCSR_ERROR						( 1 << 2 )
	//#define RXCSR_DATA_ERROR				( 1 << 3 )
	#define RXCSR_NAK_TIMEOUT				( 1 << 3 )
	//#define RXCSR_FLUSH_FIFO				( 1 << 4 )
	#define RXCSR_REQ_PKT					( 1 << 5 )
	#define RXCSR_RX_STALL					( 1 << 6 )
	//#define RXCSR_CLR_DATA_TOGGLE			( 1 << 7 )
	//#define RXCSR_INCOMP_RX					( 1 << 8 )
	#define RXCSR_DATA_TOGGLE				( 1 << 9 )
	#define RXCSR_DATA_TOGGLE_WR_EN			( 1 << 10 )
	//#define RXCSR_DMA_REQ_MODE			( 1 << 11 )
	//#define RXCSR_PID_ERR					( 1 << 12 )
	//#define RXCSR_DMA_REQ_EN				( 1 << 13 )
	#define RXCSR_AUTOREQ					( 1 << 14 )
	//#define RXCSR_AUTOCLEAR				( 1 << 15 )
	
	
#define MUSB_COUNT0					0x108	// 16-bit
#define MUSB_IDX_COUNT0				0x18	// 16-bit

#define MUSB_RXCOUNT(n)				( 0x108 + ( 0x10 * (n) ) ) // 16-bit
#define MUSB_IDX_RXCOUNT			0x18	// 16-bit

#define MUSB_TYPE0(n)				( 0x10a + ( 0x10 * (n) ) ) // 8-bit
#define MUSB_IDX_TYPE0				0x1a	// 8-bit
	/* host only */
	#define TYPE0_SPEED_HIGH				1
	#define TYPE0_SPEED_FULL				2
	#define TYPE0_SPEED_LOW					3

#define MUSB_TXTYPE(n)				( 0x10a + ( 0x10 * (n) ) ) // 8-bit
#define MUSB_IDX_TXTYPE				0x1a	// 8-bit
	/* host only */
	#define TXTYPE_EPNUM_POS				0
	#define TXTYPE_EPNUM_MSK				( 0xf << TXTYPE_EPNUM_POS )
	#define TXTYPE_PROTOCOL_POS				4
	#define TXTYPE_PROTOCOL_MSK				( 3 << TXTYPE_PROTOCOL_POS )
	#define TXTYPE_PROTOCOL_CONTROL			( 0 << TXTYPE_PROTOCOL_POS )
	#define TXTYPE_PROTOCOL_ISOCH			( 1 << TXTYPE_PROTOCOL_POS )
	#define TXTYPE_PROTOCOL_BULK			( 2 << TXTYPE_PROTOCOL_POS )
	#define TXTYPE_PROTOCOL_INTERRUPT		( 3 << TXTYPE_PROTOCOL_POS )
	#define TXTYPE_SPEED_POS				6
	#define TXTYPE_SPEED_MSK				( 3 << TXTYPE_SPEED_POS )
	#define TXTYPE_SPEED_HIGH				( 1 << TXTYPE_SPEED_POS )
	#define TXTYPE_SPEED_FULL				( 2 << TXTYPE_SPEED_POS )
	#define TXTYPE_SPEED_LOW				( 3 << TXTYPE_SPEED_POS )
	

#define MUSB_NAKLIMIT0(n)			( 0x10b + ( 0x10 * (n) ) ) // 8-bit
#define MUSB_IDX_NAKLIMIT0			0x1b	// 8-bit

#define MUSB_TXINTERVAL(n)			( 0x10b + ( 0x10 * (n) ) ) // 8-bit
#define MUSB_IDX_TXINTERVAL			0x1b	// 8-bit

#define MUSB_RXTYPE(n)				( 0x10c + ( 0x10 * (n) ) ) // 8-bit
#define MUSB_IDX_RXTYPE				0x1c	// 8-bit
	/* host only */
	#define RXTYPE_EPNUM_POS				0
	#define RXTYPE_EPNUM_MSK				( 0xf << RXTYPE_EPNUM_POS )
	#define RXTYPE_PROTOCOL_POS				4
	#define RXTYPE_PROTOCOL_MSK				( 3 << RXTYPE_PROTOCOL_POS )
	#define RXTYPE_PROTOCOL_CONTROL			( 0 << RXTYPE_PROTOCOL_POS )
	#define RXTYPE_PROTOCOL_ISOCH			( 1 << RXTYPE_PROTOCOL_POS )
	#define RXTYPE_PROTOCOL_BULK			( 2 << RXTYPE_PROTOCOL_POS )
	#define RXTYPE_PROTOCOL_INTERRUPT		( 3 << RXTYPE_PROTOCOL_POS )
	#define RXTYPE_SPEED_POS				6
	#define RXTYPE_SPEED_MSK				( 3 << RXTYPE_SPEED_POS )
	#define RXTYPE_SPEED_HIGH				( 1 << RXTYPE_SPEED_POS )
	#define RXTYPE_SPEED_FULL				( 2 << RXTYPE_SPEED_POS )
	#define RXTYPE_SPEED_LOW				( 3 << RXTYPE_SPEED_POS )

#define MUSB_RXINTERVAL(n)			( 0x10d + ( 0x10 * (n) ) ) // 8-bit
#define MUSB_IDX_RXINTERVAL			0x1d	// 8-bit

#define MUSB_CONFIGDATA(n)			( 0x10f + ( 0x10 * (n) ) ) // 8-bit
#define MUSB_IDX_CONFIGDATA			0x1f	// 8-bit
	#define CONFIG_UTMI_DATA_WIDTH		( 1 << 0 ) 
	#define CONFIG_SOFT_CONN_EN			( 1 << 1 ) 
	#define CONFIG_DYNFIFO_SIZING		( 1 << 2 ) 
	#define CONFIG_HBTXE				( 1 << 3 ) 
	#define CONFIG_HBRXE				( 1 << 4 ) 
	#define CONFIG_BIG_ENDIAN			( 1 << 5 ) 
	#define CONFIG_MPTXE				( 1 << 6 ) 
	#define CONFIG_MPRXE				( 1 << 7 ) 
	
#define MUSB_FIFOSIZE(n)			( 0x10f + ( 0x10 * (n) ) ) // 8-bit
#define MUSB_IDX_FIFOSIZE			0x1f	// 8-bit

/* Optional DMA registers */
#define MUSB_DMA_INTR				0x200
	
#define MUSB_DMA_CNTL(n)			( 0x204 + ( 0x10 * (n) ) )	// 32-bit
	#define DMA_CNTL_ENABLE				( 1 << 0 )
	#define DMA_CNTL_DIR_MSK			( 1 << 1 )
	#define DMA_CNTL_DIR_TX				( 1 << 1 )
	#define DMA_CNTL_DIR_RX				( 0 << 1 )
	#define DMA_CNTL_MODE_MSK			( 1 << 2 )
	#define DMA_CNTL_MODE0				( 0 << 2 )
	#define DMA_CNTL_MODE1				( 1 << 2 )
	#define DMA_CNTL_INTR_EN			( 1 << 3 )
	#define DMA_CNTL_EP_POS				4
	#define DMA_CNTL_EP_MSK				( 0xf << DMA_CNTL_EP_POS )
	#define DMA_CNTL_ERROR				( 1 << 8 )
	#define DMA_CNTL_BURSTMODE_POS		9
	#define DMA_CNTL_BURSTMODE_MSK		( 0x3 << DMA_CNTL_BURSTMODE_POS )
	#define DMA_CNTL_BURSTMODE_UNSPEC			( 0x0 << DMA_CNTL_BURSTMODE_POS )
	#define DMA_CNTL_BURSTMODE_UNSPEC_4			( 0x1 << DMA_CNTL_BURSTMODE_POS )
	#define DMA_CNTL_BURSTMODE_UNSPEC_4_8		( 0x2 << DMA_CNTL_BURSTMODE_POS )
	#define DMA_CNTL_BURSTMODE_UNSPEC_4_8_16	( 0x3 << DMA_CNTL_BURSTMODE_POS )

#define MUSB_DMA_ADDR(n)			( 0x208 + ( 0x10 * (n) ) )	// 32-bit
#define MUSB_DMA_COUNT(n)			( 0x20c + ( 0x10 * (n) ) )	// 32-bit

// if the dma index is greater or equal to zero, it means that a dma channel
// was allocated to the endpoint
#define USE_BUILTIN_DMA(dc, ep)		( ( dc->flags & DC_FLAG_USE_BUILTIN_DMA ) && ( ( ep->dma_chidx >= 0 ) ? 1 : 0 ) )
#define USE_EXTERNAL_DMA(dc, ep)	( ( dc->flags & DC_FLAG_USE_EXTERNAL_DMA ) && ( ( ep->dma_chidx >= 0 ) ? 1 : 0 ) )

/* Extended MAP registers */
#define MUSB_RQPTKCOUNT(n)				( 0x300 + ( 4 * (n) ) )	// 16-bit
#define MUSB_RXDPKTBUFDIS				0x340	// 16-bit
#define MUSB_TXDPKTBUFDIS				0x342	// 16-bit
#define	MUSB_CTUCH						0x344	// 16-bit
#define MUSB_CTHSRTN					0x346	// 16-bit



/* LPM Registers */
#define MUSB_LPM_ATTR					0x360	// 16-bit
#define MUSB_LPM_CNTRL					0x362	// 8-bit
#define MUSB_LPM_INTREN					0x363	// 8-bit
#define MUSB_LPM_INTR					0x364	// 8-bit
#define MUSB_LPM_FADDR					0x365	// 8-bit



/* driver type definitions */

typedef struct _dctrl_t dctrl_t; 
typedef struct _musb_dma_funcs_t musb_dma_funcs_t;
typedef enum { DMA_MODE0, DMA_MODE1, DMA_MODE_EXT } dmamode_t;
typedef struct _ep_ctx ep_ctx_t;
typedef struct _musbmhdrc_cfg musbmhdrc_cfg_t;
typedef struct _musb_transfer 	musb_transfer_t;


struct _musb_dma_funcs_t {
	void * 	(*init)( dctrl_t * dc );
	void    (*fini)( void * hdl );
	int	    (*channel_claim )( void * hdl, ep_ctx_t *ep );
	void    (*channel_release )( void * hdl, int chidx );
	void	(*transfer_start)( void * hdl, int chidx, uint32_t start_addr_paddr, unsigned length , dmamode_t dmamode);	
	void	(*transfer_abort)( void * hdl, int chidx );
	uint32_t	(*rx_complete)( void * hdl, int chidx );
};

/* driver endpoint context */

// based on EP0 state machine described in the MUSBMHDRC reference manual
typedef enum {  EP0_STATE_IDLE, EP0_STATE_TXDATA, EP0_STATE_RXDATA  } ep0_state_t;

// The IDLE state is very broad ( p122 of musbmhdrc ref manual), so break it down into substates 
typedef enum {  EP0_IDLE_SUBSTATE_SETUP_PHASE, EP0_IDLE_SUBSTATE_STATUS_PHASE  } ep0_idle_substate_t;

typedef enum {
    USBPHY_ANA_CONFIG1,
    USBPHY_ANA_CONFIG2,
    USBPHY_MAX_REG
} phy_tuning_t;

#define EP_FLAG_DEFER_SETUP_PKT_CLEAR				( 1 << 0 )
#define EP_FLAG_DEFER_SETUP_PKT_PROCESSING			( 1 << 1 )
#define EP_FLAG_DEFER_OUT_PKT_PROCESSING			( 1 << 2 )
#define EP_FLAG_DEFER_OUT_PKT_CLEAR					( 1 << 3 )

struct _musb_transfer {
	SIMPLEQ_ENTRY(_musb_transfer) 	link;
	ep_ctx_t						*ep;
	iousb_transfer_t 				*urb;
	uint32_t						flags;

	uint32_t						status; 		/* urb return status	*/
	uint32_t						xfer_buffer;
	uint32_t						xfer_buffer_paddr;	
	uint32_t						xfer_length;	
	uint32_t						req_xfer_len;
	uint32_t						bytes_xfered;	
	uint32_t						initial_pktcnt;	

	// isoch streaming vars
	usbd_isoch_frame_request_t		*isoch_flist;
	int								isoch_nframes;
	int								isoch_cur_idx;
	uint32_t						isoch_frame_offset;
};


struct _ep_ctx {
	SIMPLEQ_HEAD(, _musb_transfer)	transfer_q;					/* transfer queue 							*/
	volatile unsigned		flags;
	uint32_t				num;
	uint32_t				dir;
	uint32_t				mps;
	uint32_t				type;	// control, bulk, isoch, or int
	iousb_endpoint_t		*iousb_ep;
	ep0_state_t				ep0_state;			// only valid for ep0;
	ep0_idle_substate_t		ep0_idle_substate;	// only valid for ep0...  
	
	int						fifoblk;
	int						dma_chidx;
	
	// current working td, for non isoch trander, it will always be the first and only one
	musb_transfer_t			*td;			
};



/* SoC-specific MUSBMHDRC configurations */

struct _musbmhdrc_cfg {
	unsigned	fiforam_size;
	unsigned 	n_endpoints;
	int			dma_irq;
};

#define DC_FLAG_CONNECTED         ( 1 << 0 )
#define DC_FLAG_SOFT_CONNECT      ( 1 << 1 )
#define DC_FLAG_FORCE_FS          ( 1 << 2 )
#define DC_FLAG_USE_BUILTIN_DMA   ( 1 << 3 )
#define DC_FLAG_CFG_PMIC          ( 1 << 4 )
#define DC_FLAG_SESSION_POLL      ( 1 << 5 )
#define DC_FLAG_SMART_CLOCK       ( 1 << 6 )
#define DC_FLAG_INHERIT_CFG       ( 1 << 7 )
#define DC_FLAG_INITIALIZED		  ( 1 << 8 )
#define DC_FLAG_USE_EXTERNAL_DMA  ( 1 << 9 )

/* device controller context */
struct _dctrl_t {
	uint32_t				flags;
	usbdc_device_t			*udc;   
	char					*serial_string;    
	uint8_t					*IoBase;    
	uint32_t 				pIoBase;
	int						verbosity;
	pthread_mutex_t			mutex;

	void *					fifomem_hdl;
	
	ep_ctx_t				ep0;
	ep_ctx_t				epout_arr[MAX_N_ENDPOINTS]; // idx 0 is unused ep0 is used instead
	ep_ctx_t				epin_arr[MAX_N_ENDPOINTS];  // idx 0 is unused ep0 is used instead
	uint8_t					setup_packet[SIZE_SETUP_REQUEST];	

	musbmhdrc_cfg_t			musbmhdrc_cfg;

	intrspin_t				lock;  //interrupt lock
	int						coid;
	int						chid;
	int						priority;
	pthread_t				tid;
	int                 	irq;
	int						intr_id; 
	int						num_td;
	volatile unsigned		usb_status; //core usb int status
	struct sigevent     	intr_event;
	void					*transfer_mem;
	SIMPLEQ_HEAD(, _musb_transfer)  transfer_free_q;
	SIMPLEQ_HEAD(, _musb_transfer)  transfer_complete_q;

	void					*xdc;	// soc specific device controller context
	musb_dma_funcs_t		dma_funcs;
	void					*dma_hdl;
	uint32_t				phy_tuning[USBPHY_MAX_REG];

	int						edma_channel;
	int						edma_irq;
};



/* Regsiters Access Macros */


static inline uint32_t HW_Read32( uint8_t * iobase, uint32_t offset ) {
	uint32_t data;
	
	data = *((uint32_t*)(iobase + offset));
	
	return data;
}

static inline void HW_Write32( uint8_t * iobase, uint32_t offset, uint32_t data ) {
	*((uint32_t*)(iobase + offset)) = data;
}

static inline void HW_Write32Or( uint8_t * iobase, uint32_t offset, uint32_t data ) {
	data = HW_Read32( iobase, offset ) | data;
	HW_Write32( iobase, offset, data);
}

static inline void HW_Write32And( uint8_t * iobase, uint32_t offset, uint32_t data ) {
	data = HW_Read32( iobase, offset ) & data;
	HW_Write32( iobase, offset, data);
}

static inline uint16_t HW_Read16( uint8_t * iobase, uint32_t offset ) {
	uint16_t data;
	data = *((uint16_t*)(iobase + offset));
	return data;
}

static inline void HW_Write16( uint8_t * iobase, uint32_t offset, uint16_t data ) {
	*((uint16_t*)(iobase + offset)) = data;
}

static inline void HW_Write16Or( uint8_t * iobase, uint32_t offset, uint16_t data ) {
	data = HW_Read16( iobase, offset ) | data;
	HW_Write16( iobase, offset, data);
}

static inline void HW_Write16And( uint8_t * iobase, uint32_t offset, uint16_t data ) {
	data = HW_Read16( iobase, offset ) & data;
	HW_Write16( iobase, offset, data);
}

static inline uint8_t HW_Read8( uint8_t * iobase, uint32_t offset ) {
	uint8_t data;
	data = *((uint8_t*)(iobase + offset));
	return data;
}

static inline void HW_Write8( uint8_t * iobase, uint32_t offset, uint8_t data ) {
	*((uint8_t*)(iobase + offset)) = data;
}

static inline void HW_Write8Or( uint8_t * iobase, uint32_t offset, uint8_t data ) {
	data = HW_Read8( iobase, offset ) | data;
	HW_Write8( iobase, offset, data);
}

static inline void HW_Write8And( uint8_t * iobase, uint32_t offset, uint8_t data ) {
	data = HW_Read8( iobase, offset ) & data;
	HW_Write8( iobase, offset, data);
}




/* function prototypes */

uint32_t musb_init( usbdc_device_t *udc, io_usbdc_self_t *udc_self, char *args);
uint32_t musb_start( usbdc_device_t *udc );
uint32_t musb_stop( usbdc_device_t *udc );
uint32_t musb_shutdown( usbdc_device_t *udc );
uint32_t musb_set_bus_state( usbdc_device_t *udc, uint32_t device_state );
uint32_t musb_set_device_feature( usbdc_device_t *udc, uint32_t feature, uint16_t wIndex );
uint32_t musb_clear_device_feature( usbdc_device_t *udc, uint32_t feature );

uint32_t musb_set_device_address(  usbdc_device_t *udc, uint32_t device_address );
uint32_t musb_select_configuration( usbdc_device_t *udc, uint8_t config );
uint32_t musb_get_descriptor( usbdc_device_t *udc, uint8_t type, uint8_t index, uint16_t lang_id, uint8_t **ddesc, uint32_t speed );
uint32_t musb_get_device_descriptor( usbdc_device_t *udc, uint8_t **ddesc, uint32_t speed );
uint32_t musb_get_config_descriptor( usbdc_device_t *udc, uint8_t **cdesc, uint8_t config_num, uint32_t speed );
uint32_t musb_get_string_descriptor( usbdc_device_t *udc, uint8_t **sdesc, uint8_t index , uint32_t speed);

uint32_t musb_set_endpoint_state( usbdc_device_t *udc, iousb_endpoint_t *ep, uint32_t ep_state );
uint32_t musb_clear_endpoint_state( usbdc_device_t *udc, iousb_endpoint_t *ep, uint32_t ep_state );
#ifdef MUSB_REAL_ISR
const   struct sigevent *musb_interrupt( void *area, int id );
#else
uint32_t musb_interrupt( usbdc_device_t *udc );
#endif

uint32_t musb_control_endpoint_enable( void *chdl, iousb_device_t *device, iousb_endpoint_t *ep );
uint32_t musb_control_endpoint_disable( void *chdl, iousb_endpoint_t *ep );
uint32_t musb_control_transfer_abort( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *ED );
uint32_t musb_control_transfer( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *endp, uint8_t *buffer, _uint32 length, _uint32 flags );

uint32_t musb_endpoint_enable( void *chdl, iousb_device_t *device, iousb_endpoint_t *ep );
uint32_t musb_endpoint_disable( void *chdl, iousb_endpoint_t *ep );
uint32_t musb_transfer_abort( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *ED );
uint32_t musb_transfer( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *endp, uint8_t *buffer, _uint32 length, _uint32 flags );

uint32_t musb_isoch_endpoint_enable( void *chdl, iousb_device_t *device, iousb_endpoint_t *ep );
uint32_t musb_isoch_endpoint_disable( void *chdl, iousb_endpoint_t *ep );
uint32_t musb_isoch_transfer_abort( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *ED );
uint32_t musb_isoch_transfer( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *endp, uint8_t *buffer, _uint32 length, _uint32 flags );

void  musb_slogf( dctrl_t * dc, int level, const char *fmt, ...);
void complete_urb( dctrl_t * dc, ep_ctx_t *ep, uint32_t urb_status );


/* fifo allocation functions */
void * 	fifomem_init( unsigned fifosz_in_bytes );
void 	fifomem_fini( void * hdl );
int 	fifomem_alloc( void * hdl, unsigned int * size );
void 	fifomem_free( void * hdl, int blknum );
int 	fifomem_alloc_ep0( void * hdl, unsigned int size );
void 	fifomem_free_ep0( void * hdl, int blknum );


/* platform specific callouts */

int 	musb_custom_init1( dctrl_t * dc, char * args );
int 	musb_custom_init2( dctrl_t * dc );
int 	musb_custom_fini1( dctrl_t * dc );
int 	musb_custom_fini2( dctrl_t * dc );
int 	musb_custom_reset( dctrl_t * dc );
void	musb_cfg_get( musbmhdrc_cfg_t * cfg );
void 	musb_dma_map( dctrl_t * dc, musb_dma_funcs_t * dma_funcs );
#ifdef EXTERNAL_INTERRUPT_PROCESSING
void  musb_get_ext_intstatus( dctrl_t * dc, uint16_t *wIntrRxValue, uint16_t *wIntrTxValue, uint8_t *wIntrUsbValue );
void  musb_clr_ext_int( dctrl_t *dc );
#endif


/* Internal DMA Engine ( not supported by all Soc implementations ) */
/* Note dma_support_get() *MUST* be implemented by all plartforms using this
 * driver.  
 */

void * 	 dma_init( dctrl_t * dc  );
void     dma_fini( void * hdl );
int	     dma_channel_claim( void * hdl,  ep_ctx_t *ep );
void     dma_channel_release( void * hdl, int chidx );
void	 dma_transfer_start( void * hdl, int chidx, uint32_t start_addr_paddr, unsigned length, dmamode_t dmamode );	
void	 dma_transfer_abort( void * hdl, int chidx );
uint32_t dma_rx_complete( void * hdl, int chidx );

void	*edma_init( dctrl_t * dc  );
void	edma_fini( void * hdl );
int		edma_channel_claim( void * hdl,  ep_ctx_t *ep );
void	edma_channel_release( void * hdl, int epnum );
void	edma_transfer_start( void * hdl, int epnum, uint32_t start_addr_paddr, unsigned length, dmamode_t dmamode );	
void	edma_transfer_abort( void * hdl, int epnum );
uint32_t edma_rx_complete( void * hdl, int epnum );

#endif



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/musbmhdrc/musb.h $ $Rev: 746301 $")
#endif
