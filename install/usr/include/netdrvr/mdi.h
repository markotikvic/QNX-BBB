/*
 * $QNXLicenseC:
 * Copyright 2007, QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software.   Free development
 * licenses are available for evaluation and non-commercial purposes.  For more
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 * $
 */

#ifndef	_NETDRVR_MDI_H_INCLUDED
#define	_NETDRVR_MDI_H_INCLUDED

#include <inttypes.h>
#include <signal.h>
#include <time.h>

/*
 * Our pulse.code value from the mdi->CallBackTimer in NTO. This timer is
 * used to monitor our PHY interfaces for changes every 3 seconds.
 */
#define	MDI_TIMER	0x0002

/*
 * A typical MDI Protocol Sequence looks like
 *	<PREAMBLE><ST><OP><PHYAD><REGAD><TA><DATA><IDLE>
 */
#define	MDI_ST		0x01
#define	MDI_READ_OP	0x02
#define	MDI_WRITE_OP	0x01
#define	MDI_PHYAD_MASK	0x1F
#define	MDI_REGAD_MASK	0x1F
#define	MDI_WRITE_TA	0x02

/* Media */
#define	MDI_10bT	(1<<0)
#define	MDI_10bTFD	(1<<1)
#define	MDI_100bT	(1<<2)
#define	MDI_100bTFD	(1<<3)
#define	MDI_100bT4	(1<<4)
#define	MDI_100bT2	(1<<5)
#define	MDI_100bT2FD	(1<<6)
#define MDI_UNKNOWN	(1<<7)
#define	MDI_FLOW	(1<<10)
#define	MDI_FLOW_ASYM	(1<<11)
#define	MDI_1000bT	(1<<12)
#define	MDI_1000bTFD	(1<<13)
#define	MDI_1000bX	(1<<14)
#define	MDI_1000bXFD	(1<<15)

/* Basic Mode Control Register */
#define MDI_BMCR			0x00
	#define BMCR_RESET		(1<<15)
	#define BMCR_LOOPBACK		(1<<14)
	#define BMCR_SPEED_100		(1<<13)
	#define BMCR_AN_ENABLE		(1<<12)
	#define BMCR_SLEEP		(1<<11)
	#define BMCR_ISOLATE		(1<<10)
	#define BMCR_RESTART_AN		(1<<9)
	#define BMCR_FULL_DUPLEX	(1<<8)
	#define BMCR_COLLISION_TEST	(1<<7)
	#define BMCR_SPEED_1000		(1<<6)

/* Basic Mode Status Register */
#define MDI_BMSR			0x01
	#define	BMSR_100bT4		(1<<15)
	#define	BMSR_100bXFD		(1<<14)
	#define	BMSR_100bX		(1<<13)
	#define	BMSR_10FD		(1<<12)
	#define	BMSR_10			(1<<11)
	#define	BMSR_100bT2FD		(1<<10)
	#define	BMSR_100bT2		(1<<9)
	#define BMSR_EXT_STATUS		(1<<8)
	#define BMSR_AN_COMPLETE	(1<<5)
	#define BMSR_REMOTE_FAULT	(1<<4)
	#define BMSR_AN_ABILITY		(1<<3)
	#define BMSR_LINK_STATUS	(1<<2)
	#define BMSR_JABBER_DETECT	(1<<1)
	#define BMSR_EXTENDED_CAP	(1<<0)

/* PHY Identifier  Registers */
#define	MDI_PHYID_1			0x02
#define	MDI_PHYID_2			0x03
	#define	PHYID_VENDOR(x)		((uint32_t)(((x) >> 10) & 0x3FFFFF))
	#define	PHYID_MODEL(x)		((uint8_t)(((x) >> 4) & 0x3F))
	#define	PHYID_REV(x)		((uint8_t)((x) & 0xF))

/* Auto-Negotiation Advertisement Register */
#define MDI_ANAR			0x04
	#define ANAR_NEXT_PAGE		(1<<15)
	#define ANAR_ACKNOWLEDGE	(1<<14)
	#define ANAR_REMOTE_FAULT	(1<<13)
	#define	ANAR_FLOW_ASYMMETRIC	(1<<11)
	#define	ANAR_FLOW_CONTROL	(1<<10)
	#define	ANAR_ADVERT_MSK		(0x1fe0)

	#define ANAR_100bT4		(1 << 9)
	#define ANAR_100bTFD		(1 << 8)
	#define ANAR_100bT		(1 << 7)
	#define ANAR_10bTFD		(1 << 6)
	#define ANAR_10bT		(1 << 5)

/* Auto-Negotiation Link Partner Ability Register */
#define MDI_ANLPAR			0x05
	#define ANLPAR_NEXT_PAGE	(1<<15)
	#define ANLPAR_ACKNOWLEDGE	(1<<14)
	#define ANLPAR_MSG_PAGE		(1<<13)
	#define	ANLPAR_ACKNOWLEDGE2	(1<<12)
	#define	ANLPAR_TOGGLE		(1<<11)
	#define	ANAR_ADVERT_MSK		(0x1fe0)

/* Auto-Negotiation Expansion Register */
#define	MDI_ANAE			0x06
	#define	ANAE_PAR_DETECT_FAULT	(1<<4)
	#define	ANAE_LP_NP_ABLE		(1<<3)
	#define	ANAE_NP_ABLE		(1<<2)
	#define	ANAE_PAGE_RX		(1<<1)
	#define	ANAE_LP_AN_ABLE		(1<<0)

/* Auto-Negotiation Next Page Transmit Register */
#define	MDI_ANPT			0x07
	#define	ANPT_NXT_PAGE		(1<<15)
	#define	ANPT_MSG_PAGE		(1<<13)
	#define	ANPT_ACK2		(1<<12)
	#define	ANPT_TOGGLE		(1<<11)

/* Auto-Negotiation Link Partner Next Page Register */
#define	MDI_ALPNP			0x08

/* MASTER-SLAVE Control Register (100Base-T2 and 1000Base-T only) */
#define	MDI_MSCR			0x09
	#define	MSCR_MANUAL_CONFIG	(1<<12)
	#define	MSCR_CONFIG_VALUE	(1<<11)
	#define	MSCR_PORT_TYPE		(1<<10)
	#define	MSCR_ADV_1000bTFD	(1<<9)
	#define	MSCR_ADV_1000bT		(1<<8)

/* MASTER-SLAVE Status Register (100Base-T2 and 1000Base-T only) */
#define	MDI_MSSR			0x0a
	#define	MSSR_CONFIG_FAULT	(1<<15)
	#define	MSSR_CONFIG_RESULT	(1<<14)
	#define	MSSR_LOCAL_RX_STATUS	(1<<13)
	#define	MSSR_REMOTE_RX_STATUS	(1<<12)
	#define	MSSR_LP_1000bTFD	(1<<11)
	#define	MSSR_LP_1000bT		(1<<10)

/* Extended Mode Status Register (GMII only) */
#define	MDI_EMSR			0x0f
	#define	EMSR_1000bXFD		(1<<15)
	#define	EMSR_1000bX		(1<<14)
	#define	EMSR_1000bTFD		(1<<13)
	#define	EMSR_1000bT		(1<<12)

#define	MDI_VENDOR_BASE			(0x10)

/* OUI IDs for PHY Manufacturers. */
#define	NATIONAL_SEMICONDUCTOR		0x00080017
	#define	DP_83840		0x00
		#define	DP_83840A	0x01
	#define	DP_83843		0x01

	#define	NS83840A_PAR		(MDI_VENDOR_BASE + 0x09)
	#define	NS83840A_DUPLEX_STAT	(1<<7)
	#define	NS83840A_SPEED_10	(1<<6)

	#define	NS83843_PHYSTS		(MDI_VENDOR_BASE)
	#define	NS83843_DUPLEX		(1<<2)
	#define	NS83843_SPEED_10	(1<<1)


#define	LEVEL_ONE			0x001E0400
	#define	LXT_9746		0x00

	#define	L9746_CHIP_STATUS	(MDI_VENDOR_BASE + 0x04)
	#define	L9746_LINK_STATUS	(1<<13)
	#define	L9746_DUPLEX_STAT	(1<<12)
	#define	L9746_SPEED_100		(1<<11)

#define	QUALITY_SEMICONDUCTOR		0x00006051
	#define	QS6612_HUGH		0x00
	#define	QS6612			0x01

	#define	QS6612_INT_MASK		(MDI_VENDOR_BASE + 0x0E)
	#define	QS6612_PHY_CONTROL	(MDI_VENDOR_BASE + 0x0F)
	#define	QS6612_GET_MODE(x)	(uint8_t)((x & (1<<4|1<<3|1<<2)) >> 2)
	#define	QS6612_AUTONEG		0x00
	#define	QS6612_10bT		0x01
	#define	QS6612_100bT		0x02
	#define	QS6612_100bT4		0x04
	#define	QS6612_10bTFD		0x05
	#define	QS6612_100bTFD		0x06
	#define	QS6612_ISOLATE		0x07

#define	ICS				0x0000057D
	#define	ICS1889			0x01
	#define	ICS1890			0x02
		#define	INTERNAL	0x00
		#define	ALPHA_1890	0x01
		#define	GEN_REL		0x02
		#define	J_RELEASE	0x03
	#define	ICS1890_QPOLL		(MDI_VENDOR_BASE + 0x01)
	#define	ICS1890_SPEED_100	(1<<15)
	#define	ICS1890_DUPLEX		(1<<14)

#define	INTEL				0x0000AA00
	#define	I82555			0x15
		#define	I82555_REV	0x00
	#define	I82555_SCTRL		(MDI_VENDOR_BASE + 0x00)
	#define	I82555_SPEED_100	(1<<1)
	#define	I82555_DUPLEX		(1<<0)

#define	DAVICOM				0x0000606E
	#define	DM9101			0x00
		#define	DM9101_REV	0x00
		#define	DM9101_AREV	0x01
	#define	DM_DSCR			(MDI_VENDOR_BASE + 0x00)
	#define	DM_DSCSR		(MDI_VENDOR_BASE + 0x01)
		#define	DSCSR_100FDX	(1<<15)
		#define	DSCSR_100TX	(1<<14)
		#define	DSCSR_10FDX	(1<<13)
		#define	DSCSR_10TX	(1<<12)
	#define	DM_10BTCSR		(MDI_VENDOR_BASE + 0x02)

#define	MYSON				0x0000C0B4
	#define	MTD972			0x00

#define	BROADCOM			0x00001018
    #define BM5202			0x21
    #define BM5202_SPEED_MASK   	(1 << 10 | 1 << 9 | 1 << 8 )
    #define BM5202_10BT			(0 << 10 | 0 << 9 | 1 << 8 )
    #define BM5202_10BTFDX		(0 << 10 | 1 << 9 | 0 << 8 )
    #define BM5202_100BTX		(0 << 10 | 1 << 9 | 1 << 8 )
    #define BM5202_100BT4		(1 << 10 | 0 << 9 | 0 << 8 )
    #define BM5202_100BTXFDX		(1 << 10 | 0 << 9 | 1 << 8 )

#define	BROADCOM2			0x0000d897
	#define BCM89810		0xc
	#define BCM54616		0x11


#define	LSILOGIC			0x5be
	#define	LSI80225		0x8
	#define	LSI_STATOUT		(MDI_VENDOR_BASE + 0x02)
		#define	LSI_100MB	(1<<7)
		#define	LSI_FD		(1<<6)

#define	TDK	0x0000c039
	#define TDK78Q2120		0x14
	/* TDK 78Q2110 has 3 specifice registers */
	#define TDK_MR16		0x10			/* Vendor Specific */
	#define TDK_MR17		0x11			/* Interrupt Control/Status */
	#define TDK_MR18		0x12			/* Diagnostic */
	#define TDK_MR18_DPLX		(1 << 11)		/* set = full duplex */
	#define TDK_MR18_RATE		(1 << 10)		/* set = 100 Base    */
	#define TDK_MR18_RXLOCK		(1 << 8)		/* set we have a locked signal */

#define KENDIN				0x885
	#define KSZ9021			0x21
	#define	KSZ9031			0x22
	#define	KSZ8051			0x15
	#define KSZ8081			0x16

#define ATHEROS				0x1374
	#define AR8031			0x7

#define MARVELL             0x5043
    #define ALASKA          0x1d
/*
 * Determines if the MASTER-SLAVE registers (9 and 10) are valid, based
 * on the media supported by the PHY.
 */
#define MDI_MS_VALID(__media) \
    ((__media) & (MDI_100bT2 | MDI_100bT2FD | \
    MDI_1000bT | MDI_1000bTFD | MDI_1000bX | MDI_1000bXFD))

/**********************************************************
 ** Structures.
 **********************************************************/
struct mdi;
typedef uint16_t	(*MDIReadFunc)(void *, uint8_t, uint8_t);
typedef	void		(*MDIWriteFunc)(void *, uint8_t, uint8_t, uint16_t);
typedef void		(*MDICallBack)(void *, uint8_t, uint8_t);

#define			MDI_CALLBACKTIME	0x3L	/* 3 Seconds */

typedef	int	(*PHYGetMedia)(struct mdi *mdi, int PhyAddr, int *Media);
typedef void	(*PHYDumpVendor)(struct mdi *mdi, int PhyAddr);
typedef void	(*PHYResetComplete)(struct mdi *mdi, int PhyAddr);
typedef	void	(*PHYSpaceHolder)(struct mdi *mdi, int PhyAddr);
typedef enum	{ WaitBusy = 0, NoWait, IrqNoWait } MDIWaitType;
typedef enum	{ MDI_WaitBusy = 0, MDI_NoWait, MDI_IrqNoWait } MDI_WaitType;


typedef struct {
	PHYGetMedia	GetMedia;
	PHYDumpVendor	DumpVendor;
	PHYResetComplete ResetComplete;
	PHYSpaceHolder	Holder1;
	PHYSpaceHolder	Holder2;

	char		*Desc;
	uint32_t	VendorOUI;
	uint8_t		Model;
	uint8_t		Rev;

	uint8_t		Cnt;
	uint8_t		Padding1;
	uint16_t	CurAdvert;
	uint8_t		Padding[2];

	uint16_t	Control;
	uint16_t	StatusReg;

	uint16_t	SetSpeed;
	uint16_t	SetAdvert;
	uint8_t		SetDuplex;
	uint8_t		CurrState;
	uint8_t		Padding2[2];
	int			MediaCapable;
} PhyData_t;

typedef struct mdi {
	PhyData_t	*PhyData[32];
	void	   	*handle;		/* Driver-private handle */

	MDIReadFunc	Read;
	MDIWriteFunc	Write;
	MDICallBack	CallBack;
	timer_t		CallBackTimer;
	struct itimerspec	CBTimer;
	uint8_t		DisableMonitor;
	uint8_t		LDownTest;
	uint8_t		Padding[2];
} mdi_t;

/**********************************************************
 ** Return Codes.
 **********************************************************/

/* MDI Return Codes */
#define	MDI_FAILURE		0
#define	MDI_SUCCESS		1

#define	MDI_LINK_UP		2
#define	MDI_LINK_DOWN		3
#define	MDI_LINK_UNKNOWN	4
#define	MDI_RESET_PHY		5
#define	MDI_AUTONEG		6

#define	MDI_BADPARAM		9
#define	MDI_NOMEM		10
#define	MDI_UNSUPPORTED		11
#define	MDI_INVALID_MEDIA	12
#define	MDI_PROXY_FAILURE	13
#define	MDI_TIMER_FAILURE	14

/**********************************************************
 ** Public Interface.
 **********************************************************/
int MDI_Register(void *handle,
    MDIWriteFunc write, MDIReadFunc read, MDICallBack callback,
    mdi_t **mdi, struct sigevent *event);
int MDI_Register_Extended (void *handle, MDIWriteFunc write, MDIReadFunc read,
     MDICallBack callback, mdi_t **mdi, struct sigevent *event,
     int priority, int callback_interval);
void MDI_DeRegister(mdi_t **mdi);
int MDI_FindPhy(mdi_t *mdi, int PhyAddr);
int MDI_InitPhy(mdi_t *mdi, int PhyAddr);
int MDI_GetLinkStatus(mdi_t *mdi, int PhyAddr);
int MDI_GetActiveMedia(mdi_t *mdi, int PhyAddr, int *Media);
int MDI_SyncPhy(mdi_t *mdi, int PhyAddr);
int MDI_IsolatePhy(mdi_t *mdi, int PhyAddr);
int MDI_DeIsolatePhy(mdi_t *mdi, int PhyAddr);
int MDI_PowerdownPhy(mdi_t *mdi, int PhyAddr);
int MDI_PowerupPhy(mdi_t *mdi, int PhyAddr);
int MDI_ResetPhy(mdi_t *mdi, int PhyAddr, MDIWaitType Wait);
int MDI_SetSpeedDuplex(mdi_t *mdi, int PhyAddr, int Speed, int Duplex);
int MDI_SetAdvert(mdi_t *mdi, int PhyAddr, int MediaCode);
int MDI_GetAdvert(mdi_t *mdi, int PhyAddr, int *Advert);
int MDI_GetMediaCapable(mdi_t *mdi, int PhyAddr, int *Advert);
int MDI_GetPartnerAdvert(mdi_t *mdi, int PhyAddr, int *Advert);
int MDI_AutoNegotiate (mdi_t *mdi, int PhyAddr, int timeout);
int MDI_GetCallBackpid(mdi_t *mdi, pid_t *pid);
int MDI_DisableMonitor(mdi_t *mdi);
int MDI_EnableMonitor(mdi_t *mdi, int LDownTest);
void MDI_MonitorPhy(mdi_t *mdi);
int MDI_DumpRegisters(mdi_t *mdi, int PhyAddr);
int MDI_Autonegotiated_Active_Media(mdi_t *mdi, int PhyAddr, int *Media);

/******************************************************************************
 ** Specific PHY Support Included.
 *****************************************************************************/

/* Broadcom BM5202 */
int GetMedia_BM5202(mdi_t *mdi, int PhyAddr, int *Media);
void DumpVendor_BM5202(mdi_t *mdi, int PhyAddr);
void ResetComplete_BM5202(mdi_t *mdi, int PhyAddr);

/* National SemiConductor DP83840 */
int GetMedia_DP83840A(mdi_t *mdi, int PhyAddr, int *Media);
void DumpVendor_DP83840A(mdi_t *mdi, int PhyAddr);
void ResetComplete_DP83840A(mdi_t *mdi, int PhyAddr);

/* National SemiConductor DP83843 */
int GetMedia_DP83843(mdi_t *mdi, int PhyAddr, int *Media);
void DumpVendor_DP83843(mdi_t *mdi, int PhyAddr);

/* Level One LXT9746 */
int GetMedia_LXT9746(mdi_t *mdi, int PhyAddr, int *Media);
void DumpVendor_LXT9746(mdi_t *mdi, int PhyAddr);

/* Quality SemiConductor QS6612 */
int GetMedia_QS6612(mdi_t *mdi, int PhyAddr, int *Media);
void DumpVendor_QS6612(mdi_t *mdi, int PhyAddr);

/* Integrated Circuit Systems ICS1890 */
int GetMedia_ICS1890(mdi_t *mdi, int PhyAddr, int *Media);
void DumpVendor_ICS1890(mdi_t *mdi, int PhyAddr);
void ResetComplete_ICS1890(mdi_t *mdi, int PhyAddr);

/* INTEL Corp. I82555 */
int GetMedia_I82555(mdi_t *mdi, int PhyAddr, int *Media);
void DumpVendor_I82555(mdi_t *mdi, int PhyAddr);
void ResetComplete_I82555(mdi_t *mdi, int PhyAddr);

/* Davicom Inc. DM9101 */
int GetMedia_DM9101(mdi_t *mdi, int PhyAddr, int *Media);
void DumpVendor_DM9101(mdi_t *mdi, int PhyAddr);
void ResetComplete_DM9101(mdi_t *mdi, int PhyAddr);

/* LSI Logic. 80225 */
int GetMedia_LSI80225(mdi_t *mdi, int PhyAddr, int *Media);

/* TDK 78Q2120 */
void DumpVendor_TDK78Q2120(mdi_t *mdi, int PhyAddr);

#endif /* _NETDRVR_MDI_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/io-pkt/sys/lib/libnetdrvr/public/netdrvr/mdi.h $ $Rev: 764164 $")
#endif
