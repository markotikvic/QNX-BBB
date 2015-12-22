/*
 * $QNXLicenseC:
 * Copyright 2008, QNX Software Systems. 
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
 *  hw/scc.h
 *
 *  Register definitions for a Zilog Serial Communication Controller
 *
 */
#ifndef _HW_SCC_INCLUDED
#define _HW_SCC_INCLUDED

#ifndef __PLATFORM_H_INCLUDED
#include <sys/platform.h>
#endif

#define SCC_ROFF_A						0x04
#define	SCC_ROFF_B						0x00
#define SCC_ROFF_CMD					0x00
#define SCC_ROFF_DATA					0x08

/*
** WR0 - command register (Z85x30)
*/
#define	SCC_WR0_CMD_MASK				_BITFIELD8L(3,7)
#define SCC_WR0_CMD_NULL				_BITFIELD8L(3,0)
#define SCC_WR0_CMD_POINTHIGH			_BITFIELD8L(3,1)
#define SCC_WR0_CMD_RESETSTAT			_BITFIELD8L(3,2)
#define SCC_WR0_CMD_SENDABORT			_BITFIELD8L(3,3)
#define SCC_WR0_CMD_INTNEXTRX			_BITFIELD8L(3,4)
#define SCC_WR0_CMD_RESETTXINT			_BITFIELD8L(3,5)
#define SCC_WR0_CMD_RESETERR			_BITFIELD8L(3,6)
#define SCC_WR0_CMD_RESETIUS			_BITFIELD8L(3,7)
#define SCC_WR0_RESET_MASK				_BITFIELD8L(6,3)
#define SCC_WR0_RESET_NULL				_BITFIELD8L(6,0)
#define SCC_WR0_RESET_RXCRC				_BITFIELD8L(6,1)
#define SCC_WR0_RESET_TXCRC				_BITFIELD8L(6,2)
#define SCC_WR0_RESET_TXUNDERRUN		_BITFIELD8L(6,3)

/*
** WR1 - Transmit/Receive Interrupt and Data Transfer Mode
*/
#define SCC_WR1_EIE						_ONEBIT8L(0)
#define SCC_WR1_TXIE					_ONEBIT8L(1)
#define SCC_WR1_PARITY					_ONEBIT8L(2)
#define SCC_WR1_RXI_MASK				_BITFIELD8L(3,3)
#define SCC_WR1_RXI_DISABLE				_BITFIELD8L(3,0)
#define SCC_WR1_RXI_FIRST				_BITFIELD8L(3,1)
#define SCC_WR1_RXI_ALL					_BITFIELD8L(3,2)
#define SCC_WR1_RXI_SPECIAL				_BITFIELD8L(3,3)
#define SCC_WR1_WR_ON_TR				_ONEBIT8L(5)
#define SCC_WR1_WAIT_DMA_REQUEST		_ONEBIT8L(6)
#define SCC_WR1_WAIT_DMA_ENABLE			_ONEBIT8L(7)

/*
** WR2 - Interrupt Vector
*/

/*
** WR3 - Receive Paramters and Control
*/
#define SCC_WR3_RX_ENABLE				_ONEBIT8L(0)
#define SCC_WR3_SYNC_LOAD_INHIBIT		_ONEBIT8L(1)
#define SCC_WR3_ADDR_SEARCH_MODE		_ONEBIT8L(2)
#define SCC_WR3_RX_CRC_ENABLE			_ONEBIT8L(3)
#define SCC_WR3_ENTER_HUNT_MODE			_ONEBIT8L(4)
#define SCC_WR3_AUTO_ENABLES			_ONEBIT8L(5)
#define SCC_WR3_BITS_MASK				_BITFIELD8L(6,3)
#define SCC_WR3_BITS_5					_BITFIELD8L(6,0)
#define SCC_WR3_BITS_6					_BITFIELD8L(6,1)
#define SCC_WR3_BITS_7					_BITFIELD8L(6,2)
#define SCC_WR3_BITS_8					_BITFIELD8L(6,3)

/*
** WR4 - Transmit/Receive Misc. Parameters and Modes
*/
#define SCC_WR4_PARITY_ENABLE			_ONEBIT8L(0)
#define SCC_WR4_PARITY_EVEN				_ONEBIT8L(1)
#define SCC_WR4_STOP_MASK				_BITFIELD8L(2,3)
#define SCC_WR4_STOP_SYNC				_BITFIELD8L(2,0)
#define SCC_WR4_STOP_1					_BITFIELD8L(2,1)
#define SCC_WR4_STOP_1_5				_BITFIELD8L(2,2)
#define SCC_WR4_STOP_2					_BITFIELD8L(2,3)
#define SCC_WR4_SYNC_MASK				_BITFIELD8L(4,3)
#define SCC_WR4_SYNC_8					_BITFIELD8L(4,0)
#define SCC_WR4_SYNC_16					_BITFIELD8L(4,1)
#define SCC_WR4_SYNC_SDLC				_BITFIELD8L(4,2)
#define SCC_WR4_SYNC_EXTERNAL			_BITFIELD8L(4,3)
#define SCC_WR4_CLOCK_MASK				_BITFIELD8L(6,3)
#define SCC_WR4_CLOCK_X1				_BITFIELD8L(6,0)
#define SCC_WR4_CLOCK_X16				_BITFIELD8L(6,1)
#define SCC_WR4_CLOCK_X32				_BITFIELD8L(6,2)
#define SCC_WR4_CLOCK_X64				_BITFIELD8L(6,3)

/*
** WR5 - Transmit Parameters and Control
*/
#define SCC_WR5_TX_CRC_ENABLE			_ONEBIT8L(0)
#define SCC_WR5_RTS						_ONEBIT8L(1)
#define SCC_WR5_CRC16					_ONEBIT8L(2)
#define SCC_WR5_TX_ENABLE				_ONEBIT8L(3)
#define SCC_WR5_SENDBREAK				_ONEBIT8L(4)
#define SCC_WR5_BITS_MASK				_BITFIELD8L(5,3)
#define SCC_WR5_BITS_5					_BITFIELD8L(5,0)	
#define SCC_WR5_BITS_6					_BITFIELD8L(5,1)
#define SCC_WR5_BITS_7					_BITFIELD8L(5,2)
#define SCC_WR5_BITS_8					_BITFIELD8L(5,3)
#define SCC_WR5_DTR						_ONEBIT8L(7)
/*
** When SCC_WR5_BITS_5 is selected, or'ing one of the following bitsets
** to the data character will cause fewer bits to be transmitted.
*/
#define SCC_DATA_BITS_1					0xf0
#define SCC_DATA_BITS_2					0xe0
#define SCC_DATA_BITS_3					0xc0
#define SCC_DATA_BITS_4					0x80

/*
** WR6 - Sync Characters or SDLC Address Field
*/

/*
** WR7 - Sync Character or SDLC Flag
*/

/*
** WR7P - Write Reg 7 Prime 
*/
#define SCC_WR7P_AUTO_TX				_ONEBIT8L(0)
#define SCC_WR7P_AUTO_EOM_RESET			_ONEBIT8L(1)
#define SCC_WR7P_AUTO_RTS_DEACTIVE		_ONEBIT8L(2)
#define SCC_WR7P_RX_FIFO_HALF			_ONEBIT8L(3)	/* ESCC */
#define SCC_WR7P_FORCE_TX_HIGH			_ONEBIT8L(3)	/* 85C30 */
#define SCC_WR7P_DTR_TIMING				_ONEBIT8L(4)
#define SCC_WR7P_TX_FIFO_EMPTY			_ONEBIT8L(5)	/* ESCC */
#define SCC_WR7P_COMPLETE_CRC_RECPT		_ONEBIT8L(5)	/* 85C30 */
#define SCC_WR7P_EXTENDED_READ			_ONEBIT8L(6)

/*
** WR8 - Transmit Buffer
*/

/*
** WR9 - Master Interrupt Control
*/
#define SCC_WR9_VIS						_ONEBIT8L(0)
#define SCC_WR9_NV						_ONEBIT8L(1)
#define SCC_WR9_DLC						_ONEBIT8L(2)
#define SCC_WR9_MIE						_ONEBIT8L(3)
#define SCC_WR9_STATUS_HIGH				_ONEBIT8L(4)
#define SCC_WR9_SW_INTACK				_ONEBIT8L(5)
#define SCC_WR9_RESET_MASK				_BITFIELD8L(6,3)
#define SCC_WR9_RESET_NONE				_BITFIELD8L(6,0)
#define SCC_WR9_RESET_A					_BITFIELD8L(6,1)
#define SCC_WR9_RESET_B					_BITFIELD8L(6,2)
#define SCC_WR9_RESET_HW				_BITFIELD8L(6,3)

/*
** WR10 - Misc. Transmitter/Receiver Control Bits
*/
#define SCC_WR10_6BIT_SYNC				_ONEBIT8L(0)
#define SCC_WR10_LOOP					_ONEBIT8L(1)
#define SCC_WR10_ABORT_UNDERRUN			_ONEBIT8L(2)
#define SCC_WR10_MARK_IDLE				_ONEBIT8L(3)
#define SCC_WR10_ACTIVE_POLL			_ONEBIT8L(4)
#define SCC_WR10_DATA_MASK				_BITFIELD8L(5,3)
#define SCC_WR10_DATA_NRZ				_BITFIELD8L(5,0)
#define SCC_WR10_DATA_NRZI				_BITFIELD8L(5,1)
#define SCC_WR10_DATA_FM1				_BITFIELD8L(5,2)
#define SCC_WR10_DATA_FM0				_BITFIELD8L(5,3)
#define SCC_WR10_CRC_PRESET				_ONEBIT8L(7)

/*
** WR11 - Clock Mode Control
*/
#define SCC_WR11_TRxC_MASK				_BITFIELD8L(0,3)
#define SCC_WR11_TRxC_XTAL				_BITFIELD8L(0,0)
#define SCC_WR11_TRxC_TXCLK				_BITFIELD8L(0,1)
#define SCC_WR11_TRxC_BRGEN				_BITFIELD8L(0,2)
#define SCC_WR11_TRxC_DPLL				_BITFIELD8L(0,3)
#define SCC_WR11_TRxC_OUTPUT			_ONEBIT8L(2)
#define SCC_WR11_TXCLK_MASK				_BITFIELD8L(3,3)
#define SCC_WR11_TXCLK_RTxC				_BITFIELD8L(3,0)
#define SCC_WR11_TXCLK_TRxC				_BITFIELD8L(3,1)
#define SCC_WR11_TXCLK_BRGEN			_BITFIELD8L(3,2)
#define SCC_WR11_TXCLK_DPLL				_BITFIELD8L(3,3)
#define SCC_WR11_RXCLK_MASK				_BITFIELD8L(5,3)
#define SCC_WR11_RXCLK_RTxC				_BITFIELD8L(5,0)
#define SCC_WR11_RXCLK_TRxC				_BITFIELD8L(5,1)
#define SCC_WR11_RXCLK_BRGEN			_BITFIELD8L(5,2)
#define SCC_WR11_RXCLK_DPLL				_BITFIELD8L(5,3)
#define SCC_WR11_RTxC_XTAL				_ONEBIT8L(7)

/*
** WR12 - Lower Byte of Baud Rate Generator Time Constant
*/

/*
** WR13 - Upper Byte of Baud Rate Generator Time Constant
*/

/*
** WR14 - Misc. Control Bits
*/
#define SCC_WR14_BRGEN_ENABLE			_ONEBIT8L(0)
#define SCC_WR14_BRGEN_PCLK				_ONEBIT8L(1)
#define SCC_WR14_DTR_REQUEST			_ONEBIT8L(2)
#define SCC_WR14_AUTO_ECHO				_ONEBIT8L(3)
#define SCC_WR14_LOOPBACK				_ONEBIT8L(4)
#define SCC_WR14_CMD_MASK				_BITFIELD8L(5,7)
#define SCC_WR14_CMD_NULL				_BITFIELD8L(5,0)
#define SCC_WR14_CMD_ENTER_SEARCH_MODE	_BITFIELD8L(5,1)
#define SCC_WR14_CMD_RESET_MISSING_CLK	_BITFIELD8L(5,2)
#define SCC_WR14_CMD_DISABLE_DPLL		_BITFIELD8L(5,3)
#define SCC_WR14_CMD_SRC_BRGEN			_BITFIELD8L(5,4)
#define SCC_WR14_CMD_SRC_RTxC			_BITFIELD8L(5,5)
#define SCC_WR14_CMD_FM_MODE			_BITFIELD8L(5,6)
#define SCC_WR14_CMD_NRZI_MODE			_BITFIELD8L(5,7)

/*
** WR15 -- External/Status Interrupt Control
*/
#define SCC_WR15_WR7PRIME_EN			_ONEBIT8L(0)
#define SCC_WR15_ZEROCOUNT_IEN			_ONEBIT8L(1)
#define SCC_WR15_SDLCSTATUSFIFO_EN		_ONEBIT8L(2)
#define SCC_WR15_DCD_IEN				_ONEBIT8L(3)
#define SCC_WR15_SYNC_HUNT_IEN			_ONEBIT8L(4)
#define SCC_WR15_CTS_IEN				_ONEBIT8L(5)
#define SCC_WR15_TXUNDERRUN_EOM_IEN		_ONEBIT8L(6)
#define SCC_WR15_BREAK_ABORT_IEN		_ONEBIT8L(7)

/*
** RR0 - Transmit/Receive Buffer Status and External Status
*/
#define SCC_RR0_RX_AVAILABLE			_ONEBIT8L(0)
#define SCC_RR0_ZERO_COUNT				_ONEBIT8L(1)
#define SCC_RR0_TX_EMPTY				_ONEBIT8L(2)
#define SCC_RR0_DCD						_ONEBIT8L(3)
#define SCC_RR0_SYNC					_ONEBIT8L(4)
#define SCC_RR0_CTS						_ONEBIT8L(5)
#define SCC_RR0_TX_UNDERRUN				_ONEBIT8L(6)
#define SCC_RR0_BREAK					_ONEBIT8L(7)

/*
** RR1 - Special Receive Condition Bits
*/
#define SCC_RR1_ALL_SENT				_ONEBIT8L(0)
#define SCC_RR1_RESIDUE_CODE2			_ONEBIT8L(1)
#define SCC_RR1_RESIDUE_CODE1			_ONEBIT8L(2)
#define SCC_RR1_RESIDUE_CODE0			_ONEBIT8L(3)
#define SCC_RR1_PARITY					_ONEBIT8L(4)
#define SCC_RR1_RX_OVERRUN				_ONEBIT8L(5)
#define SCC_RR1_CRC_FRAMING				_ONEBIT8L(6)
#define SCC_RR1_END_OF_FRAME			_ONEBIT8L(7)

/*
** RR2 - Read Back of WR2
**		A channel: unaltered
**		B channel: status information added (see status low/high in WR9)
*/

/* Status information */
#define SCC_RR2_STATUS_MASK				_BITFIELD8L(0, 7)
#define SCC_RR2_TX_BUF_EMPTY			_BITFIELD8L(0, 0)
#define SCC_RR2_EXT_STAT_CHANGE			_BITFIELD8L(0, 1)
#define SCC_RR2_RX_AVAILABLE			_BITFIELD8L(0, 2)
#define SCC_RR2_RX_SPECIAL_CON			_BITFIELD8L(0, 3)
#define SCC_RR2_CHANNEL_MASK			_ONEBIT8L(2)
#define SCC_RR2_GET_LOW(n)				(n>>1)
#define SCC_RR2_GET_HIGH(n)				(n>>4)


/*
** RR3 - Interrupt Pending Bits (only available on A channel)
*/
#define SCC_RR3_STATUS_B				_ONEBIT8L(0)
#define SCC_RR3_TX_B					_ONEBIT8L(1)
#define SCC_RR3_RX_B					_ONEBIT8L(2)
#define SCC_RR3_STATUS_A				_ONEBIT8L(3)
#define SCC_RR3_TX_A					_ONEBIT8L(4)
#define SCC_RR3_RX_A					_ONEBIT8L(5)
	
/*
** RR4 - Read Back of WR4 if Extended Read enabled, otherwise RR0
*/

/*
** RR5 - Read Back of WR5 if Extended Read enabled, otherwise RR1
*/

/*
** RR6 - least significant byte of frame byte count in status FIFO
*/

/*
** RR7 - most significant 6 bits of frame byte count in status FIFO
*/

/*
** RR8 - Receive Data Register
*/

/*
** RR9 - Read Back of WR3 if Extended Read enabled, otherwise RR13
*/

/*
** RR10 - Misc. Status Bits
*/
#define SCC_RR10_ON_LOOP				_ONEBIT8L(1)
#define SCC_RR10_LOOP_SENDING			_ONEBIT8L(4)
#define SCC_RR10_TWO_CLOCKS_MISSING		_ONEBIT8L(6)
#define SCC_RR10_ONE_CLOCK_MISSING		_ONEBIT8L(7)

/*
** RR11 - Read Back of WR10 if Extended Read enabled, otherwise RR15
*/

/*
** RR12 - Read Back of WR12
*/

/*
** RR13 - Read Back of WR13
*/

/*
** RR14 - Read Back of WR7 Prime if Extended Read enabled, otherwise RR10
*/

/*
** RR15 - Read Back of WR15
*/

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/public/hw/zscc.h $ $Rev: 680332 $")
#endif
