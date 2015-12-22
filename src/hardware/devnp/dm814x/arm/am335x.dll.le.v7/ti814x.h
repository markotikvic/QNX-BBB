/*
 * $QNXLicenseC:
 * Copyright 2011, QNX Software Systems.
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

#ifndef TI814X_H
#define TI814X_H

#include <io-pkt/iopkt_driver.h>
#include <stdio.h>
#include <errno.h>
#include <atomic.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/siginfo.h>
#include <sys/syspage.h>
#include <sys/neutrino.h>
#include <sys/mbuf.h>
#include <sys/slogcodes.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/if_ether.h>
#include <net/if_media.h>
#include <sys/io-pkt.h>
#include <sys/cache.h>
#include <sys/callout.h>
#include <sys/device.h>
#include <hw/inout.h>
#include <netdrvr/mdi.h>
#include <netdrvr/eth.h>
#include <netdrvr/nicsupport.h>
#include <hw/nicinfo.h>
#include <sys/device.h>
#define _STDDEF_H_INCLUDED
#include <siglock.h>
#include <dev/mii/miivar.h>

#include <sys/trace.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* Macros */
#define get_mac_hi(mac) (((mac)[0] << 0) | ((mac)[1] << 8) |	\
			 ((mac)[2] << 16) | ((mac)[3] << 24))
#define get_mac_lo(mac) (((mac)[4] << 0) | ((mac)[5] << 8))


/* DEBUG Facility */
#define DEBUG_MII_RW					0x80000000
#define DEBUG_MII						0x40000000
#define DEBUG_RX						0x20000000
#define DEBUG_RX2						0x10000000
#define DEBUG_DUMP_RX					0x08000000
#define DEBUG_TX						0x04000000
#define DEBUG_TX2						0x02000000
#define DEBUG_TRACE						0x01000000
#define DEBUG_DEFRAG					0x00800000
#define DEBUG_MASK						0x000000ff

/* Special IOCTL to set interfaces stats */
#define	IOCTL_STAT						0x50
#define	VALID_STATS		NIC_ETHER_STAT_SINGLE_COLLISIONS | \
					    NIC_ETHER_STAT_MULTI_COLLISIONS | \
					    NIC_ETHER_STAT_TX_DEFERRED | \
					    NIC_ETHER_STAT_XCOLL_ABORTED | \
					    NIC_ETHER_STAT_LATE_COLLISIONS | \
					    NIC_ETHER_STAT_ALIGN_ERRORS | \
					    NIC_ETHER_STAT_FCS_ERRORS | \
					    NIC_ETHER_STAT_OVERSIZED_PACKETS | \
					    NIC_ETHER_STAT_JABBER_DETECTED | \
					    NIC_ETHER_STAT_SHORT_PACKETS | \
						NIC_ETHER_STAT_TOTAL_COLLISION_FRAMES | \
						NIC_ETHER_STAT_NO_CARRIER | \
					    NIC_ETHER_STAT_INTERNAL_RX_ERRORS

/* J5 register address definitions */
#define TI814X_CPSW_BASE				0x4a100000
#define TI814X_CPSW_SIZE				0x4000

#define TI814X_CPSW_MDIO_OFFSET			0x00001000

/* TI814x Interrupt Vectors */
#define TI814x_RX_THRESH_INT0	40	  /* RX Threshold interrupt */
#define TI814x_RX_INT0			41	  /* RX Pulse interrupt */
#define TI814x_TX_INT0			42	  /* TX Pulse interrupt */
#define TI814x_MISC_INT0		43	  /* Misc interrupt */
#define NUM_IRQS			3	  /* Rx Threshold not used */

#define TI814X_QUIESCE_PULSE		_PULSE_CODE_MINAVAIL
#define TI814X_RX_PULSE			(TI814X_QUIESCE_PULSE + 1)

#define NUM_RX_PKTS			96
#define NUM_RX_DMA_CHAN			4
#define NUM_TX_PKTS			16
#define NUM_TX_DMA_CHAN			8
#define NUM_TX_QUEUES			(NUM_TX_DMA_CHAN / 2) /* 2 ports */
#define TOTAL_NUM_TX_PKTS		(NUM_TX_PKTS * NUM_TX_DMA_CHAN)
#define MAX_PKT_SIZE			2047 /* MCLBYTES but off_len smaller */
#define DEFRAG_LIMIT			5
#define PORT1_VLAN			0
#define PORT2_VLAN			1
#define	MII_NUM_REGS			29

/* J5 GMAC offset Definitions */
/* CPSW_SS 0x0000-0x00ff */
#define CPSW_ID_VER				0x00000000
	#define CPSW_ID_VER_ID(x)			((x)&0xffff0000) >> 16
	#define CPSW_ID_VER_RTLVER(x)			((x)&0x0000f800) >> 11
	#define CPSW_ID_VER_MAJOR(x)			((x)&0x00000700) >> 8
	#define CPSW_ID_VER_MINOR(x)			((x)&0x000000ff)
#define CPSW_CONTROL				0x00000004
#define CPSW_SOFT_RESET				0x00000008
	#define CPSW_SOFT_RESET_BIT			0x00000001
#define CPSW_STAT_PORT_EN			0x0000000c
	#define P2_STAT_EN					0x00000004
	#define P1_STAT_EN					0x00000002
	#define P0_STAT_EN					0x00000001
#define CPSW_PTYPE				0x00000010
	#define P2_PRI3_SHAPE_EN			0x00200000
	#define P2_PRI2_SHAPE_EN			0x00100000
	#define P2_PRI1_SHAPE_EN			0x00080000
	#define P1_PRI3_SHAPE_EN			0x00040000
	#define P1_PRI2_SHAPE_EN			0x00020000
	#define P1_PRI1_SHAPE_EN			0x00010000
#define CPSW_SOFT_IDLE				0x00000014
#define CPSW_THRU_RATE				0x00000018
#define CPSW_GAP_THRESH				0x0000001c
#define CPSW_TX_START_WDS			0x00000020
#define CPSW_FLOW_CONTROL			0x00000024
#define VLAN_LTYPE				0x00000028
#define TS_LTYPE				0x0000002C

/* CPSW_PORT 0x100-0x7ff */
#define P0_CONTROL				0x00000100
#define P0_MAX_BLKS				0x00000104
#define P0_BLK_CNT				0x0000010C
#define P0_TX_IN_CTL				0x00000110
	#define NORM_PRI_MODE				0x00000000
	#define DUAL_MAC_MODE				0x00010000
	#define RATE_LIM_MODE				0x00020000
	#define TX_BLKS_REM				0x00004000
	#define TX_PRI_WDS				0x000000c0
#define P0_PORT_VLAN				0x00000114
#define P0_TX_PRI_MAP				0x00000118
#define CPDMA_TX_PRI_MAP			0x0000011c
#define CPDMA_RX_CH_MAP				0x00000120
#define P0_RX_DSCP_PRI_MAP0			0x00000130
#define P0_RX_DSCP_PRI_MAP1			0x00000134
#define P0_RX_DSCP_PRI_MAP2			0x00000138
#define P0_RX_DSCP_PRI_MAP3			0x0000013c
#define P0_RX_DSCP_PRI_MAP4			0x00000140
#define P0_RX_DSCP_PRI_MAP5			0x00000144
#define P0_RX_DSCP_PRI_MAP6			0x00000148
#define P0_RX_DSCP_PRI_MAP7			0x0000014c
#define P1_CONTROL				0x00000200
	#define TS_LTYPE2_EN				0x00000008
	#define TS_LTYPE1_EN				0x00000004
	#define TS_TX_EN				0x00000002
	#define TS_RX_EN				0x00000001
#define P1_MAX_BLKS				0x00000208
#define P1_BLK_CNT				0x0000020c
#define P1_TX_IN_CTL				0x00000210
	#define HOST_BLKS_REM				0x08000000
	#define TX_RATE_EN				0x00e00000
#define P1_PORT_VLAN				0x00000214
#define P1_TX_PRI_MAP				0x00000218
#define P1_TS_SEQ_MTYPE				0x0000021c
	#define TS_SEQ_ID_OFFSET_SHIFT				16
	#define TS_TYPE_MANAGEMENT				(1<<13)
	#define TS_TYPE_SIGNALING				(1<<12)
	#define TS_TYPE_ANNOUNCE				(1<<11)
	#define TS_TYPE_PDELAY_RESP_FOLLOWUP			(1<<10)
	#define TS_TYPE_DELAY_RESP				(1<<9)
	#define TS_TYPE_FOLLOWUP				(1<<8)
	#define TS_TYPE_PDELAY_RESP				(1<<3)
	#define TS_TYPE_PDELAY_REQ				(1<<2)
	#define TS_TYPE_DELAY_REQ				(1<<1)
	#define TS_TYPE_SYNC					(1<<0)
#define SL1_SA_LO				0x00000220
#define SL1_SA_HI				0x00000224
#define P1_SEND_PERCENT				0x00000228
#define P1_RX_DSCP_PRI_MAP0			0x00000230
#define P1_RX_DSCP_PRI_MAP1			0x00000234
#define P1_RX_DSCP_PRI_MAP2			0x00000238
#define P1_RX_DSCP_PRI_MAP3			0x0000023c
#define P1_RX_DSCP_PRI_MAP4			0x00000240
#define P1_RX_DSCP_PRI_MAP5			0x00000244
#define P1_RX_DSCP_PRI_MAP6			0x00000248
#define P1_RX_DSCP_PRI_MAP7			0x0000024c
#define P2_CONTROL				0x00000300
#define P2_MAX_BLKS				0x00000308
#define P2_BLK_CNT				0x0000030c
#define P2_TX_IN_CTL				0x00000310
#define P2_PORT_VLAN				0x00000314
#define P2_TX_PRI_MAP				0x00000318
#define P2_TS_SEQ_MTYPE				0x0000031c
#define SL2_SA_LO				0x00000320
#define SL2_SA_HI				0x00000324
#define P2_SEND_PERCENT				0x00000328
#define P2_RX_DSCP_PRI_MAP0			0x00000330
#define P2_RX_DSCP_PRI_MAP1			0x00000334
#define P2_RX_DSCP_PRI_MAP2			0x00000338
#define P2_RX_DSCP_PRI_MAP3			0x0000033c
#define P2_RX_DSCP_PRI_MAP4			0x00000340
#define P2_RX_DSCP_PRI_MAP5			0x00000344
#define P2_RX_DSCP_PRI_MAP6			0x00000348
#define P2_RX_DSCP_PRI_MAP7			0x0000034c

/* CPSW_CPDMA 0x800-0x8ff */
#define TX_IDVER				0x00000800
#define TX_CONTROL				0x00000804
	#define TX_EN					0x00000001
#define TX_TEARDOWN				0x00000808
#define RX_IDVER				0x00000810
#define RX_CONTROL				0x00000814
	#define RX_EN					0x00000001
#define RX_TEARDOWN				0x00000818
#define DMA_SOFT_RESET				0x0000081c
	#define DMA_SOFT_RESET_BIT			0x00000001
#define DMA_CONTROL				0x00000820
	#define TX_RLIM					0x0000fc00
	#define TX_PTYPE				0x00000001
#define DMA_STATUS				0x00000824
	#define DMA_STAT_RXERR_CH_MSK			0x00000700
	#define DMA_STAT_RXERR_CODE_MSK			0x0000f000
		#define DMA_STAT_RXERR_NOOWN			0x0010
		#define DMA_STAT_RXERR_ZERO_BUF			0x0100
		#define DMA_STAT_RXERR_ZERO_LEN			0x0101
		#define DMA_STAT_RXERR_SOP			0x0110
	#define DMA_STAT_TXERR_CH_MSK			0x00070000
	#define DMA_STAT_TXERR_CODE_MSK			0x00f00000
	#define DMA_STAT_IDLE				0x80000000
#define RX_BUFFER_OFFSET			0x00000828
#define EM_CONTROL				0x0000082c
#define TX_PRI0_RATE				0x00000830
	#define PRI_SEND_SHIFT					16
#define TX_PRI1_RATE				0x00000834
#define TX_PRI2_RATE				0x00000838
#define TX_PRI3_RATE				0x0000083c
#define TX_PRI4_RATE				0x00000840
#define TX_PRI5_RATE				0x00000844
#define TX_PRI6_RATE				0x00000848
#define TX_PRI7_RATE				0x0000084c
#define TX_INTSTAT_RAW				0x00000880
#define TX_INTSTAT_MASKED			0x00000884
#define TX_INTMASK_SET				0x00000888
        #define TX7_PEND_MASK                           0x00000080
        #define TX6_PEND_MASK                           0x00000040
        #define TX5_PEND_MASK                           0x00000020
        #define TX4_PEND_MASK                           0x00000010
        #define TX3_PEND_MASK                           0x00000008
        #define TX2_PEND_MASK                           0x00000004
        #define TX1_PEND_MASK                           0x00000002
        #define TX0_PEND_MASK                           0x00000001
#define TX_INTMASK_CLEAR			0x0000088c
#define CPDMA_IN_VECTOR				0x00000890
#define CPDMA_EOI_VECTOR			0x00000894
#define RX_INTSTAT_RAW				0x000008a0
	#define RX7_THRESH_PEND				0x00008000
	#define RX6_THRESH_PEND				0x00004000
	#define RX5_THRESH_PEND				0x00002000
	#define RX4_THRESH_PEND				0x00001000
	#define RX3_THRESH_PEND				0x00000800
	#define RX2_THRESH_PEND				0x00000400
	#define RX1_THRESH_PEND				0x00000200
	#define RX0_THRESH_PEND				0x00000100
	#define RX7_PEND				0x00000080
	#define RX6_PEND				0x00000040
	#define RX5_PEND				0x00000020
	#define RX4_PEND				0x00000010
	#define RX3_PEND				0x00000008
	#define RX2_PEND				0x00000004
	#define RX1_PEND				0x00000002
	#define RX0_PEND				0x00000001
#define RX_INTSTAT_MASKED			0x000008a4
#define RX_INTMASK_SET				0x000008a8
	#define RX7_THRESH_PEND_MASK			0x00008000
	#define RX6_THRESH_PEND_MASK			0x00004000
	#define RX5_THRESH_PEND_MASK			0x00002000
	#define RX4_THRESH_PEND_MASK			0x00001000
	#define RX3_THRESH_PEND_MASK			0x00000800
	#define RX2_THRESH_PEND_MASK			0x00000400
	#define RX1_THRESH_PEND_MASK			0x00000200
	#define RX0_THRESH_PEND_MASK			0x00000100
	#define RX7_PEND_MASK				0x00000080
	#define RX6_PEND_MASK				0x00000040
	#define RX5_PEND_MASK				0x00000020
	#define RX4_PEND_MASK				0x00000010
	#define RX3_PEND_MASK				0x00000008
	#define RX2_PEND_MASK				0x00000004
	#define RX1_PEND_MASK				0x00000002
	#define RX0_PEND_MASK				0x00000001
#define RX_INTMASK_CLEAR			0x000008ac
#define DMA_INTSTAT_RAW				0x000008b0
#define DMA_INTSTAT_MASKED			0x000008b4
#define DMA_INTSTAT_SET				0x000008b8
	#define HOST_ERROR_INT_MASK			0x00000002
	#define	STAT_PEND_INT_MASK			0x00000001
#define DMA_INTSTAT_CLEAR			0x000008bc
#define RX0_PENDTHRESH				0x000008c0
#define RX1_PENDTHRESH				0x000008c4
#define RX2_PENDTHRESH				0x000008c8
#define RX3_PENDTHRESH				0x000008cc
#define RX4_PENDTHRESH				0x000008d0
#define RX5_PENDTHRESH				0x000008d4
#define RX6_PENDTHRESH				0x000008d8
#define RX7_PENDTHRESH				0x000008dc
#define RX0_FREEBUFFER				0x000008e0
#define RX1_FREEBUFFER				0x000008e4
#define RX2_FREEBUFFER				0x000008e8
#define RX3_FREEBUFFER				0x000008ec
#define RX4_FREEBUFFER				0x000008f0
#define RX5_FREEBUFFER				0x000008f4
#define RX6_FREEBUFFER				0x000008f8
#define RX7_FREEBUFFER				0x000008fc

/* CPSW_STATS 0x900-0x9ff */
#define RXGOODFRAMES				0x00000900
#define RXBROADCASTFRAMES			0x00000904
#define RXMULTICASTFRAMES			0x00000908
#define RXPAUSEFRAMES				0x0000090c
#define RXCRCERRORS				0x00000910
#define RXALIGNCODEERRORS			0x00000914
#define RXOVERSIZEDFRAMES			0x00000918
#define RXJABBERFRAMES				0x0000091c
#define RXUNDERSIZEDFRAMES			0x00000920
#define RXFRAGMENTS				0x00000924
#define RXOCTETS				0x00000930
#define TXGOODFRAMES				0x00000934
#define TXBROADCASTFRAMES			0x00000938
#define TXMULTICASTFRAMES			0x0000093c
#define TXPAUSEFRAMES				0x00000940
#define TXDEFERREDFRAMES			0x00000944
#define TXCOLLISIONFRAMES			0x00000948
#define TXSINGLECOLLFRAMES			0x0000094c
#define TXMULTCOLLFRAMES			0x00000950
#define TXEXCESSIVECOLLISIONS			0x00000954
#define TXLATECOLLISION				0x00000958
#define TXUNDERRUN				0x0000095c
#define TXCARRIERSENSEERRORS			0x00000960
#define TXOCTETS				0x00000964
#define OCTETFRAMES64				0x00000968
#define OCTETFRAMES65TO127			0x0000096c
#define OCTETFRAMES128TO255			0x00000970
#define OCTETFRAMES256TO511			0x00000974
#define OCTETFRAMES512TO1023			0x00000978
#define OCTETFRAMES1024TUP			0x0000097c
#define NETOCTETS				0x00000980
#define RXSOFOVERRUNS				0x00000984
#define RXMOFOVERRUNS				0x00000988
#define RXDMAOVERRUNS				0x0000098c  
#define	MISSING_REG1				0x00000928
#define	MISSING_REG2				0x0000092c

/* CPDMA_STATERAM 0xa00-0xbff */
#define TX0_HDP					0x00000a00
#define TX1_HDP					0x00000a04
#define TX2_HDP					0x00000a08
#define TX3_HDP					0x00000a0c
#define TX4_HDP					0x00000a10
#define TX5_HDP					0x00000a14
#define TX6_HDP					0x00000a18
#define TX7_HDP					0x00000a1c
#define RX0_HDP					0x00000a20
#define RX1_HDP					0x00000a24
#define RX2_HDP					0x00000a28
#define RX3_HDP					0x00000a2c
#define RX4_HDP					0x00000a30
#define RX5_HDP					0x00000a34
#define RX6_HDP					0x00000a38
#define RX7_HDP					0x00000a3c
#define TX0_CP					0x00000a40
#define TX1_CP					0x00000a44
#define TX2_CP					0x00000a48
#define TX3_CP					0x00000a4C
#define TX4_CP					0x00000a50
#define TX5_CP					0x00000a54
#define TX6_CP					0x00000a58
#define TX7_CP					0x00000a5C
#define RX0_CP					0x00000a60
#define RX1_CP					0x00000a64
#define RX2_CP					0x00000a68
#define RX3_CP					0x00000a6C
#define RX4_CP					0x00000a70
#define RX5_CP					0x00000a74
#define RX6_CP					0x00000a78
#define RX7_CP					0x00000a7C

/* CPSW_CPTS 0xc00-0xcff */
#define CPTS_IDVER				0x00000c00
#define CPTS_CONTROL				0x00000c04
	#define CPTS_EN					0x00000001
#define CPTS_TS_PUSH				0x00000c0c
	#define TS_PUSH					0x00000001
#define CPTS_TS_LOAD_VAL			0x00000c10
#define CPTS_TS_LOAD_EN				0x00000c14
	#define TS_LOAD_EN				0x00000001
#define CPTS_INTSTAT_RAW			0x00000c20
#define CPTS_INTSTAT_MASKED			0x00000c24
#define CPTS_INT_ENABLE				0x00000c28
	#define TS_PEND_EN				0x00000001
#define CPTS_EVENT_POP				0x00000c30
	#define EVENT_POP				0x00000001
#define CPTS_EVENT_LOW				0x00000c34
#define CPTS_EVENT_HIGH				0x00000c38
	#define SEQUENCE_ID_MASK			0x0000FFFF
	#define MESSAGE_TYPE_MASK			0x000F0000
	#define MESSAGE_TYPE_SHIFT			16
	#define EVENT_TYPE_MASK				0x00F00000
	#define EVENT_TYPE_SHIFT			20
	#define EVENT_TYPE_PUSH				0
	#define EVENT_TYPE_ROLLOVER			1
	#define EVENT_TYPE_HALF				2
	#define EVENT_TYPE_HW				3
	#define EVENT_TYPE_RX				4
	#define EVENT_TYPE_TX				5
	#define PORT_NUMBER_MASK			0x1F000000
	#define PORT_NUMBER_SHIFT			24

/* ALE_REGS @ CPSW + 0xD00 */
#define ALE_IDVER				0x00000d00
#define ALE_CONTROL				0x00000d08
	#define ENABLE_ALE				0x80000000
	#define CLEAR_TABLE				0x40000000
	#define AGE_OUT_NOW				0x20000000
	#define ALE_BYPASS				0x00000010
	#define ALE_VLAN_AWARE				0x00000004
#define ALE_PRESCALE				0x00000d10
#define ALE_UNKNOWN_VLAN			0x00000d18
#define ALE_TBLCTL					0x00000d20
	#define WRITE_RDZ_WRITE				0x80000000
	#define ENTRY_MASK				0x000003ff
#define ALE_TBLW2				0x00000d34
	#define TBLW2_MASK				0x000000ff
	#define TBLW2_PORT_MASK				0x00000003
	#define TBLW2_START_BIT				64
#define ALE_TBLW1				0x00000d38
	#define ENTRY_TYPE_UNICAST			0x10000000
#define ALE_TBLW0				0x00000d3c
	#define PORT0					0x00000001
	#define PORT1					0x00000002
	#define PORT2					0x00000004
#define ALE_PORTCTL0				0x00000d40
	#define PORT_STATE_FORWARD			0x00000003
	#define PORT_STATE_LEARN			0x00000002
	#define PORT_STATE_BLOCKED			0x00000001
	#define PORT_STATE_DISABLED			0x00000000
#define ALE_PORTCTL1				0x00000d44
#define ALE_PORTCTL2				0x00000d48

/* ALE Table entry definitions */
#define PORT_MASK_BIT_OFFSET		66
/* Table entry types */
#define ALE_TYPE_FREE				0X00000000
#define ALE_TYPE_ADDR				0x00000001
#define ALE_TYPE_VLAN				0x00000002
#define ALE_TYPE_VLAN_ADDR			0x00000003
/* Unicast Types */
#define ALE_UNICAST					0x00000000
#define ALE_UNICAST_AGE_NOT_TOUCHED	0x00000001
#define ALE_UNICAST_OUI				0x00000002
#define ALE_UNICAST_AGE_TOUCHED		0x00000003
/* Multicast/FWD states */
#define ALE_STATE_FWD				0x00000000
#define ALE_STATE_BLK_FWD_LRN		0x00000001
#define ALE_STATE_FWD_LRN			0x00000002
#define ALE_STATE_FWD_2				0x00000003

#define ALE_SECURE_FLAG				0x00000001
#define ALE_BLOCKED_FLAG			0x00000002

#define ALE_ENTRY_WORDS				3
#define PORT1_VLAN_ENTRY				0
#define PORT2_VLAN_ENTRY				1
#define PORT1_UCAST_ENTRY				2
#define PORT2_UCAST_ENTRY				3
#define ALE_DYN_START					4
#define ALE_ENTRIES					1024

/* SLX_REGS @ CPSW + 0xD80 */
#define SL1_IDVER					0x00000d80
#define SL1_MAC_CONTROL				0x00000d84
	#define FULLDUPLEX					0x00000001
	#define RX_FLOW_EN					0x00000008
	#define TX_FLOW_EN					0x00000010
	#define GMII_EN						0x00000020
	#define GIG_MODE					0x00000080
	#define MACIFCTL_A					0x00008000
	#define EXT_EN						0x00040000
#define SL1_MAC_STATUS				0x00000d88
#define SL1_SOFT_RESET				0x00000d8c
	#define SL_SOFT_RESET_BIT			0x00000001
#define SL1_RX_MAXLEN				0x00000d90
#define SL1_BOFFTEST				0x00000d94
#define SL1_RX_PAUSE				0x00000d98
#define SL1_TX_PAUSE				0x00000d9c
#define SL1_EM_CONTROL				0x00000da0
#define SL1_RX_PRI_MAP				0x00000da4

#define SL2_IDVER					0x00000dc0
#define SL2_MAC_CONTROL				0x00000dc4
#define SL2_MAC_STATUS				0x00000dc8
#define SL2_SOFT_RESET				0x00000dcc
#define SL2_RX_MAXLEN				0x00000dd0
#define SL2_BOFFTEST				0x00000dd4
#define SL2_RX_PAUSE				0x00000dd8
#define SL2_TX_PAUSE				0x00000ddc
#define SL2_EM_CONTROL				0x00000de0
#define SL2_RX_PRI_MAP				0x00000de4

/* MDIO REgisters @ 0x1000 */
#define MDIOVER						0x00001000
#define MDIOCONTROL					0x00001004
	#define MDIO_IDLE					0x8000000
	#define MDIO_ENABLE					0x40000000
	#define MDIO_FAULT					0x00800000
	#define MDIO_FAULTENB				0x00400000
	#define MDIO_CLK
	#define MDIO_ENABLE					0x40000000
	#define MDIO_FAULT					0x00800000
	#define MDIO_FAULTENB				0x00400000
	// MDIO Typical: 1Mhz, works up to 2.5Mhz
	// MDIO_CLK frequency = peripheral clock frequency/(CLKDIV + 1).
	#define PER_CLK_FREQ_HZ	 			166000000
	#define MDIO_CLK_FREQ_HZ			1000000
	#define MDIO_CLK_DIVISOR			((PER_CLK_FREQ_HZ / MDIO_CLK_FREQ_HZ) - 1)
#define MDIOALIVE					0x00001008
#define MDIOLINK					0x0000100c
#define MDIOLINKINTRAW				0x00001010
#define MDIOLINKINTMASKED			0x00001014
#define MDIOUSERINTRAW				0x00001020
#define MDIOUSERINTMASKED			0x00001024
#define MDIOUSERINTMASKSET			0x00001028
#define MDIOUSERINTMASKCLR			0x0000102c
#define MDIOUSERACCESS0				0x00001080
	#define GO							0x80000000
	#define WRITE						0x40000000
	#define ACK							0x20000000
	#define DATAMASK					0x0000ffff
	#define REGADR(x)					(x << 21)
	#define PHYADR(x)					(x << 16)
#define MDIOUSERPHYSEL0				0x00001084
	#define	LINKINT_ENABLE				(1 << 6)
#define MDIOUSERACCESS1				0x00001088
#define MDIOUSERPHYSEL1				0x0000108c

/* GMAC Subsystem Register Offset Definitions @ CPSW+0x1200 */
#define CPSW_SS_IDVER				0x00001200
#define CPSW_SS_SOFT_RESET			0x00001204
	#define CPSW_SS_SOFT_RESET_BIT		0x00000001
#define CPSW_SS_CONTROL				0x00001208
#define CPSW_SS_INT_CONTROL			0x0000120c
#define CO_RX_THRES_EN				0x00001210
#define C0_RX_EN					0x00001214
	#define RX_EN_CH0					0x00000001
	#define RX_EN_CH1					0x00000002
	#define RX_EN_CH2					0x00000004
	#define RX_EN_CH3					0x00000008
	#define RX_EN_CH4					0x00000010
	#define RX_EN_CH5					0x00000020
	#define RX_EN_CH6					0x00000040
	#define RX_EN_CH7					0x00000080
#define C0_TX_EN					0x00001218
	#define TX_EN_CH0					0x00000001
	#define TX_EN_CH1					0x00000002
	#define TX_EN_CH2					0x00000004
	#define TX_EN_CH3					0x00000008
	#define TX_EN_CH4					0x00000010
	#define TX_EN_CH5					0x00000020
	#define TX_EN_CH6					0x00000040
	#define TX_EN_CH7					0x00000080
#define C0_MISC_EN					0x0000121c
	#define	MDIO_USERINT_EN				0x00000001
	#define	LINKINT_EN					0x00000002
	#define	HOST_PEND_EN				0x00000004
	#define	STAT_PEND_EN				0x00000008
	#define	EVENTINT_EN					0x00000010
#define C1_RX_THRESH_EN				0x00001220
#define C1_RX_EN					0x00001224
#define C2_RX_EN					0x00001234
#define RX_THRESH_STAT					0x00001240
#define RX_STAT						0x00001244
        #define RX_STAT_CH0					0x00000001
        #define RX_STAT_CH1					0x00000002
        #define RX_STAT_CH2					0x00000004
        #define RX_STAT_CH3					0x00000008
        #define RX_STAT_CH4					0x00000010
        #define RX_STAT_CH5					0x00000020
        #define RX_STAT_CH6					0x00000040
        #define RX_STAT_CH7					0x00000080
#define TX_STAT						0x00001248
        #define TX_STAT_CH0					0x00000001
        #define TX_STAT_CH1					0x00000002
        #define TX_STAT_CH2					0x00000004
        #define TX_STAT_CH3					0x00000008
        #define TX_STAT_CH4					0x00000010
        #define TX_STAT_CH5					0x00000020
        #define TX_STAT_CH6					0x00000040
        #define TX_STAT_CH7					0x00000080
#define	MISC_STAT					0x0000124c
	#define	MDIOUSER					0x00000001
	#define	LINKSTAT					0x00000002
	#define	HOSTPEND					0x00000004
	#define	STATPEND					0x00000008
	#define	EVNTPEND					0x00000010


// mdio clock divide down value.

// RMII Interface Clock: 50 Mhz
// Peripheral Clock Frequency: 166 Mhz
// MDIO Typical: 1Mhz, works up to 2.5Mhz
// MDIO_CLK frequency = peripheral clock frequency/(CLKDIV + 1).
#define PER_CLK_FREQ_HZ				166000000
#define MDIO_CLK_FREQ_HZ			1000000
#define MDIO_CLK_DIVISOR			((PER_CLK_FREQ_HZ / MDIO_CLK_FREQ_HZ) - 1)

/* CPPI Descriptor */
#define	CPPI_BASE					0x00002000
#define CPPI_DESC_PHYS				0x4A102000	  /* Physical address of descriptors in CPPI RAM */
typedef struct _cppi_desc_t {
	uint32_t	next;			/* Pointer to next descriptor */
	uint32_t	buffer;			/* Pointer to data buffer */
	uint32_t	off_len;		/* Buffer offset and length */
	uint32_t	flag_len;		/* Packet flag and length */
	} cppi_desc_t;
#define CPPI_DESC_MEM_SIZE			0x00002000	  /* Size of CPPI RAM memory for descriptors */
#define CPPI_RX_DESC_OFFSET			0x00000000
#define CPPI_TX_DESC_OFFSET (NUM_RX_DMA_CHAN * NUM_RX_PKTS * sizeof(cppi_desc_t))

/* Packet flags */
#define DESC_FLAG_SOP			(1 << 31)   /* Start of packet */
#define DESC_FLAG_EOP			(1 << 30)   /* End of packet */
#define DESC_FLAG_OWN			(1 << 29)   /* Ownership flag */
#define DESC_FLAG_EOQ			(1 << 28)   /* End of queue flag */
#define DESC_FLAG_TDOWNCMPLT	(1 << 27)   /* Teardown complete flag */
#define DESC_FLAG_PASSCRC		(1 << 26)   /* Pass CRC flag */
#define DESC_FLAG_JABBER		(1 << 25)   /* Jabber flag */
#define DESC_FLAG_OVERSIZE		(1 << 24)   /* Oversize flag */
#define DESC_FLAG_FRAGMENT		(1 << 23)   /* Fragment flag */
#define DESC_FLAG_UNDERSIZE		(1 << 22)   /* Undersize flag */
#define DESC_FLAG_CONTROL		(1 << 21)   /* Control flag */
#define DESC_FLAG_OVERRUN		(1 << 20)   /* Overrun flag */
#define DESC_FLAG_TOPORT_EN		(1 << 20)   /* to-port-enable for TX descriptors */
#define DESC_FLAG_CODE_ERR		(1 << 19)   /* Code error flag */
#define DESC_FLAG_ALIGN_ERR		(1 << 18)   /* Alignment error flag */
#define DESC_FLAG_CRC_ERR		(1 << 17)   /* CRC Error flag */
#define DESC_FLAG_NOMATCH		(1 << 16)   /* No match flag */
#define DESC_FLAG_TO_PORT(x)	(x << 16)   /* To port */

#define RX_ERROR	(DESC_FLAG_OVERSIZE | DESC_FLAG_UNDERSIZE | DESC_FLAG_OVERRUN | \
					DESC_FLAG_ALIGN_ERR | DESC_FLAG_CRC_ERR)


#define CMR_BASE					0x44e10000	  /* Control Module Registers */
#define CMR_MODULE_SIZE				0x00002000	  /* only a subset of the regs are needed */
/* Register Definitions */
#define MAC_ID0_LO					0x00000630
#define MAC_ID0_HI					0x00000634
#define MAC_ID1_LO					0x00000638
#define MAC_ID1_HI					0x0000063c
#define GMII_SET					0x00000650
	#define	GMII0_SEL_RMII				0x00000001
	#define	GMII0_SEL_RGMII				0x00000002
	#define	GMII1_SEL_RMII				0x00000004
	#define	GMII1_SEL_RGMII				0x00000008
	#define	GMII0_ID_MODE				0x00000010
	#define	GMII1_ID_MODE				0x00000020
	#define	GMII0_RMII_CLKIN			0x00000040
	#define	GMII1_RMII_CLKIN			0x00000080
	#define	RGMII0_EN				0x00000100
	#define	RGMII1_EN				0x00000200

#define	RMII		1
#define	GMII		2
#define	RGMII		3
	
#define PLLSS_BASE					0x481c5000
#define PLLSS_SIZE					0x400
#define VIDEO0PLL_PWRCTRL				0x1a0
#define VIDEO0PLL_CLKCTRL				0x1a4
	#define CYCLESLIPEN					(1<<31)
	#define ENSSC						(1<<30)
	#define CLKDCOLDOEN					(1<<29)
	#define NWELLTRIM_SHIFT					24
	#define IDLE						(1<<23)
	#define BYPASSACKZ					(1<<22)
	#define STBYRET						(1<<21)
	#define CLKOUTEN					(1<<20)
	#define CLKOUTLDOEN					(1<<19)
	#define ULOWCLKEN					(1<<18)
	#define CLKDCOLDOPWDNZ					(1<<17)
	#define M2PWDNZ						(1<<16)
	#define STOPMODE					(1<<14)
	#define SELFREQDCO_HS1					(4<<10)
	#define SELFREQDCO_HS2					(2<<10)
	#define RELAXED_LOCK					(1<<8)
	#define TINITZ						(1<<0)
#define VIDEO0PLL_TENABLE				0x1a8
#define VIDEO0PLL_TENABLEDIV				0x1ac
#define VIDEO0PLL_M2NDIV				0x1b0
	#define M2_SHIFT					16
#define VIDEO0PLL_MN2DIV				0x1b4
	#define N2_SHIFT					16
#define VIDEO0PLL_FRACDIV				0x1b8
	#define REGSD_SHIFT					24
#define VIDEO0PLL_BWCTRL				0x1bc
#define VIDEO0PLL_FRACCTRL				0x1c0
#define VIDEO0PLL_STATUS				0x1c4
	#define PONOUT						(1<<31)
	#define PGOODOUT					(1<<30)
	#define LDOPWDN						(1<<29)
	#define RECAL_BSTATUS3					(1<<28)
	#define RECAL_OPPIN					(1<<27)
	#define PHASELOCK					(1<<10)
	#define FREQLOCK					(1<<9)
	#define BYPASSACK					(1<<8)
	#define STBYRETACK					(1<<7)
	#define LOSSREF						(1<<6)
	#define CLKOUTENACK					(1<<5)
	#define LOCK2						(1<<4)
	#define M2CHANGEACK					(1<<3)
	#define SSCACK						(1<<2)
	#define HIGHJITTER					(1<<1)
	#define BYPASS						(1<<0)
#define PLL_DELAY	10
#define PLL_MAX_LOOP	10

/* DM Timer base addresses */
#define TIMER_SIZE					0x1000
#define TIMER1_BASE					0x4802E000
#define TIMER2_BASE					0x48040000
#define TIMER3_BASE					0x48042000
#define TIMER4_BASE					0x48044000
#define TIMER5_BASE					0x48046000
#define TIMER6_BASE					0x48048000
#define TIMER7_BASE					0x4804A000
#define TIMER8_BASE					0x481C1000

#define TINT1						67
#define TINT2						68
#define TINT3						69
#define TINT4						92
#define TINT5						93
#define TINT6						94
#define TINT7						95
#define TINT8						11

/* Timer registers */
#define TIMER_TIDR					0x00
#define TIMER_THWINFO					0x04
#define TIMER_TIOCP_CFG					0x10
#define TIMER_IRQ_EOI					0x20
#define TIMER_IRQSTATUS_RAW				0x24
	#define INT_MAT						(1 << 0)
	#define INT_OVF						(1 << 1)
	#define INT_CAP						(1 << 2)
#define TIMER_IRQSTATUS					0x28
#define TIMER_IRQENABLE_SET				0x2C
#define TIMER_IRQENABLE_CLR				0x30
#define TIMER_IRQWAKEEN					0x34
#define TIMER_TCLR					0x38
#define TIMER_TCRR					0x3C
#define TIMER_TLDR					0x40
#define TIMER_TTGR					0x44
#define TIMER_TWPS					0x48
#define TIMER_TMAR					0x4C
#define TIMER_TCAR1					0x50
#define TIMER_TSICR					0x54
#define TIMER_TCAR2					0x58

#define	READ_PHY_REG	0x400
#define	WRITE_PHY_REG	0x401

typedef	struct {
	uint8_t		phy_id;
	uint8_t		phy_reg;
	uint16_t	phy_data;
} phy_access_t;

typedef struct {
	struct cache_ctrl	cachectl;
	struct mbuf			**rx_mbuf;
	cppi_desc_t			*rx_desc;
	cppi_desc_t			*tx_desc;
	uint32_t			tx_phys;
} meminfo_t;

typedef struct {
	struct device			dev;			/* Common device */
	struct ethercom			ecom;		   /* Common Ethernet */
	struct ethercom			*common_ecom[2];
	int						stop_miimon;
	int						tx_reaped;
	int						tx_q_len[NUM_TX_QUEUES];
	int						force_link;
	int						linkup;
	int						link_mode;
	int						speed;
	int						duplex;
	int						fix_mii_clk;
	uint16_t					*join_vlan;
	uint16_t					*switch_vlan;
	int						flow;
	int						flow_status;
	int						dying;
	int						tx_pidx[NUM_TX_QUEUES];
	int						tx_cidx[NUM_TX_QUEUES];
	int						probe_phy;		  // cmd line option to force periodic probing
	int						pkts_received;	  // optimization to not probe phy
	int						phy_idx;
	int						phy_init;
	uint32_t				rx_phys;
	uintptr_t				cpsw_regs;
	uintptr_t				cppi_base;
	uintptr_t				cmr_base;		   // control module registers
	uintptr_t				pllss_base;		   // oscillator registers
	uintptr_t				timer_base;
	mdi_t					*mdi;
	uint16_t				*phy_regs;
	int						emu_phy;
	int						no_gig;
	int						get_stats;
	nic_config_t			cfg;
	nic_stats_t				stats;
	struct mbuf				**tx_mbuf;
	struct callout			mii_callout;
	struct mii_data			bsd_mii;
	struct _iopkt_self		*iopkt;
	struct _iopkt_inter		inter;
	pthread_mutex_t			rx_mutex;
	int				rx_running;
	int				rx_full;
	struct ifqueue			rx_queue;
	int	(*stack_output)(struct ifnet *, struct mbuf *,
				struct sockaddr *, struct rtentry *);
	meminfo_t				meminfo;
} ti814x_dev_t;

struct  ti814x_dev {
		struct  device  sc_dev;
		ti814x_dev_t	*sc_ti814x;
		char			filler [sizeof (ti814x_dev_t) + NET_CACHELINE_SIZE];
};

typedef	struct {
	struct _iopkt_self		*iopkt;
	void				*dll_hdl;
	char				*options;
	nic_config_t			cfg;
	uintptr_t			cpsw_base;
	uintptr_t			cppi_base;
	uintptr_t			cmr_base;
	uintptr_t			pllss_base;
	uintptr_t			timer_base;
	struct ethercom			*common_ecom[2];
	meminfo_t			meminfo;
	int				tid;
	int				chid;
	int				coid;
	int				rx_cidx[NUM_RX_DMA_CHAN];
	int				rx_tail[NUM_RX_DMA_CHAN];
	int				iid[NUM_IRQS];
	struct sigevent			isr_event[NUM_RX_DMA_CHAN];
	struct _iopkt_inter		inter_link;
	struct _iopkt_inter		inter_stat;
	struct _iopkt_inter		inter_tx[NUM_TX_DMA_CHAN];
	const struct sigevent		*(*isrp)(void *, int);
	const struct sigevent		*(*isrp_lnk)(void *, int);
	const struct sigevent		*(*isrp_tx)(void *, int);
	uint32_t			mdio_raw;
	void				*sd_hook;
} attach_args_t;

#define	IS_BROADCAST(dptr) \
	((dptr)[0] == 0xff && (dptr)[1] == 0xff && \
	(dptr)[2] == 0xff && (dptr)[3] == 0xff && \
	(dptr)[4] == 0xff && (dptr)[5] == 0xff)


/******************************************************************************
 *  Prototypes.
 *****************************************************************************/

/* ti814x.c */
int init_pll(void);
int init_prcm_emac(void);
void device_flush_mcast_bits (ti814x_dev_t *);

/* bsd_media.c */
void bsd_mii_initmedia(ti814x_dev_t *);

/* devctl.c */
int ti814x_ioctl (struct ifnet *, unsigned long, caddr_t);
void ti814x_filter(ti814x_dev_t *ti814x);
void ti814x_read_stats (ti814x_dev_t *);

/* event.c */
void *ti814x_rx_thread(void *arg);
int ti814x_process_interrupt (void *, struct nw_work_thread *);
const struct sigevent *ti814x_isr (void *, int);
const struct sigevent *ti814x_misc_isr (void *arg, int iid);
int ti814x_enable_interrupt (void *);
int ti814x_process_link_interrupt (void *arg, struct nw_work_thread *wtp);
int ti814x_process_stat_interrupt (void *arg, struct nw_work_thread *wtp);
const struct sigevent *ti814x_isr_link (void *arg, int iid);
int ti814x_enable_link_interrupt (void *arg);
int ti814x_process_tx_interrupt (void *arg, struct nw_work_thread *wtp);
const struct sigevent *ti814x_isr_tx (void *arg, int iid);
int ti814x_enable_tx_interrupt (void *arg);
int ti814x_enable_stat_interrupt (void *arg);

/* mii.c */
uint16_t ti814x_mdi_read (void *, uint8_t, uint8_t);
void ti814x_mdi_write (void *, uint8_t, uint8_t, uint16_t);
void ti814x_MDI_MonitorPhy (void *);
int ti814x_findphy (ti814x_dev_t *);
int ti814x_initphy (ti814x_dev_t *);
int	ti814x_phy_funcs (ti814x_dev_t *ti814x, struct ifdrv *ifd);

/* receive.c */
int ti814x_receive(attach_args_t *attach_args, struct nw_work_thread *wtp,
		   uint32_t chan);

/* transmit.c */
void ti814x_reap_pkts (ti814x_dev_t *, uint32_t);
int ti814x_output(struct ifnet *, struct mbuf *,
		  struct sockaddr *, struct rtentry *);
void ti814x_start (struct ifnet *);
int ti814x_set_tx_bw(ti814x_dev_t *ti814x, struct ifdrv *ifd);

/* ale.c */
void ti814x_ale_init(uintptr_t cpsw_regs);
int ti814x_ale_add_vlan(uintptr_t cpsw_regs, uint16_t vlan, uint8_t ports,
			int index);
int ti814x_ale_add_vlan_mcast(uintptr_t cpsw_regs, uint8_t *addr,
			      int port_mask, uint16_t vlan);
int ti814x_ale_add_vlan_ucast(uintptr_t cpsw_regs, uint8_t *addr, int index,
			      int port, int flags, uint16_t vlan);
void ti814x_ale_check_vlan_mcast(uintptr_t cpsw_regs, uint16_t vlan,
				 struct ethercom *ec);
void ti814x_ale_del_all_vlan_mcast(uintptr_t cpsw_regs, uint16_t vlan,
				   struct ethercom *ec);
void ti814x_ale_flood_unreg_mcast(uintptr_t cpsw_regs, uint16_t vlan,
				  int flood);

/* emu_phy.c */
uint16_t	emu_phy_read (ti814x_dev_t *, uint8_t, uint8_t);
int emu_phy_init (ti814x_dev_t *);
int emu_phy_dinit (ti814x_dev_t *);

/* ptp.c */
int ti814x_ptp_ioctl(ti814x_dev_t *ti814x, struct ifdrv *ifd);
int ti814x_ptp_init(attach_args_t *attach_args);
void ti814x_process_ptp_interrupt(attach_args_t *attach_args);

/* timer.c */
int ti814x_timer_delay (ti814x_dev_t *ti814x, struct ifdrv *ifd);
void ti814x_timer_init (ti814x_dev_t *ti814x);

#ifdef __cplusplus
};
#endif

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/io-pkt/sys/dev_qnx/dm814x/arm/am335x.dll.le.v7/ti814x.h $ $Rev: 758089 $")
#endif
