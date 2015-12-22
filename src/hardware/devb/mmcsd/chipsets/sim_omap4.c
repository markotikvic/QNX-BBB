/*
 * $QNXLicenseC:
 * Copyright 2007, 2008, QNX Software Systems. 
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

#include <sim_mmc.h>

#ifdef MMCSD_VENDOR_TI_OMAP4

#include <sim_omap4.h>
#include <sys/syspage.h>
#include <hw/sysinfo.h>

//#define OMAP_DEBUG

int omap_dump( SIM_HBA *hba )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	uintptr_t		base;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;
	base	= oext->mmc_base;

#ifdef OMAP_DEBUG
	mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 3, "%s: HL_REV %x, HL_HWINFO %x, HL_SYSCONFIG %x, SYSSTATUS %x, REV %x, CAPA %x, CUR_CAPA %x, SYSTEST %x, FE %x",
	__FUNCTION__, 
	in32( base + OMAP_MMCHS_HL_REV ),
	in32( base + OMAP_MMCHS_HL_HWINFO ),
	in32( base + OMAP_MMCHS_HL_SYSCONFIG ),
	in32( base + OMAP_MMCHS_SYSSTATUS ),
	in32( base + OMAP_MMCHS_REV ),
	in32( base + OMAP_MMCHS_CAPA ),
	in32( base + OMAP_MMCHS_CUR_CAPA ),
	in32( base + OMAP_MMCHS_SYSTEST ),
	in32( base + OMAP_MMCHS_FE ) );
	
	mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 3, "%s: CSRE %x, CON %x, PWCNT %x, BLK %x, ARG %x, CMD %x, RSP10 %x, RSP32 %x, RSP54 %x,  RSP76 %x",
	__FUNCTION__,
	in32( base + OMAP_MMCHS_CSRE ),
	in32( base + OMAP_MMCHS_CON ),
	in32( base + OMAP_MMCHS_PWCNT ),
	in32( base + OMAP_MMCHS_BLK ),
	in32( base + OMAP_MMCHS_ARG ),
	in32( base + OMAP_MMCHS_CMD ),
	in32( base + OMAP_MMCHS_RSP10 ),
	in32( base + OMAP_MMCHS_RSP32 ),
	in32( base + OMAP_MMCHS_RSP54 ),
	in32( base + OMAP_MMCHS_RSP76 ) );

	mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 3, "%s: PSTATE %x, HCTL %x, SYSCTL %x, STAT %x, IE %x, ISE %x, AC12 %x, ADMAES %x ADMASAL %x",
	__FUNCTION__,
	in32( base + OMAP_MMCHS_PSTATE ),
	in32( base + OMAP_MMCHS_HCTL ),
	in32( base + OMAP_MMCHS_SYSCTL ),
	in32( base + OMAP_MMCHS_STAT ), 
	in32( base + OMAP_MMCHS_IE ),
	in32( base + OMAP_MMCHS_ISE ),
	in32( base + OMAP_MMCHS_AC12 ),
	in32( base + OMAP_MMCHS_ADMAES ),
	in32( base + OMAP_MMCHS_ADMASAL ) );
#endif
	return( MMC_SUCCESS );
}

int omap_waitmask( SIM_HBA *hba, uint32_t reg, uint32_t mask, uint32_t val, uint32_t usec )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	uintptr_t		base;
	uint32_t		cnt;
	int				stat;
	int				rval;
	uint32_t		iter;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;
	base	= oext->mmc_base;
	stat	= MMC_FAILURE;
	rval	= MMC_SUCCESS;

		// fast poll for 1ms - 1us intervals
	for( cnt = min( usec, 1000 ); cnt; cnt-- ) {
		if( ( ( rval = in32( base + reg ) ) & mask ) == val ) {
			stat = MMC_SUCCESS;
			break;
		}
		nanospin_ns( 1000L );
	}

	if( usec > 1000 && stat ) {
		iter	= usec / 1000L;
		for( cnt = iter; cnt; cnt-- ) {
			if( ( ( rval = in32( base + reg ) ) & mask ) == val ) {
				stat = MMC_SUCCESS;
				break;
			}
			delay( 1 );
		}
	}

#ifdef OMAP_DEBUG
	if( !cnt ) {
		mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 3, "%s: reg %x, mask %x, val %x, rval %x", __FUNCTION__, reg, mask, val, in32( base + reg ) ); 
	}
#endif

	return( stat );
}

int omap_reset( SIM_HBA *hba, uint32_t rst )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	uintptr_t		base;
	int				stat;
	uint32_t		sctl;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;
	base	= oext->mmc_base;

#ifdef OMAP_DEBUG
	mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 1, "%s:  rst %x", __FUNCTION__, rst );
#endif

	sctl	= in32( base + OMAP_MMCHS_SYSCTL );

		// wait up to 100 ms for reset to complete
	out32( base + OMAP_MMCHS_SYSCTL, sctl | rst );
	stat = omap_waitmask( hba, OMAP_MMCHS_SYSCTL, rst, rst, 10000 );
	stat = omap_waitmask( hba, OMAP_MMCHS_SYSCTL, rst, 0, 100000 );

	if( stat ) {
		mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 1, "%s:  timeout", __FUNCTION__ );
	}

	return( stat );
}

static int omap_interrupt( SIM_HBA *hba, int irq, int resp_type, uint32_t *resp )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	uint32_t		sts;
	int				intr;
	uintptr_t		base;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;
	base	= oext->mmc_base;
	intr	= 0;

	sts = in32( base + OMAP_MMCHS_STAT );
	sts &= in32( base + OMAP_MMCHS_IE ) | INTR_ERRI;
	out32( base + OMAP_MMCHS_STAT, sts );

	if( sts & INTR_ERRI ) {	// Any errors ?
//		mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 1, "%s:  status %x", __FUNCTION__, sts );
		omap_dump( hba );
		intr |= MMC_INTR_ERROR | MMC_INTR_COMMAND;
		if( sts & INTR_ADMAE ) {	// ADMA error
			mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 1, "%s:  ADMAES %x", __FUNCTION__, in32( base + OMAP_MMCHS_ADMAES ) );
		}

		if( sts & INTR_DTO ) {
			intr |= MMC_ERR_DATA_TO;
			omap_reset( hba, SYSCTL_SRD );
			omap_reset( hba, SYSCTL_SRC );
		}
		if( sts & INTR_DCRC ) {
			intr |= MMC_ERR_DATA_CRC;
		}
		if( sts & INTR_DEB ) {
			intr |= MMC_ERR_DATA_END;
		}
		if( sts & INTR_CTO ) {
			intr |= MMC_ERR_CMD_TO;
			omap_reset( hba, SYSCTL_SRC );
		}
		if( sts & INTR_CCRC ) {
			intr |= MMC_ERR_CMD_CRC;
		}
		if( sts & INTR_CEB ) {
			intr |= MMC_ERR_CMD_END;
		}
		if( sts & INTR_CIE ) {
			intr |= MMC_ERR_CMD_IDX;
		}
		if( sts & INTR_ACE ) {
			mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 1, "%s: AC12 Error %x", __FUNCTION__, in32( base + OMAP_MMCHS_AC12 ) );
			intr |= MMC_ERR_DATA_TO | MMC_INTR_ERROR;
			omap_reset( hba, SYSCTL_SRD );
			omap_reset( hba, SYSCTL_SRC );
		}
	}
	else {
		if( sts & INTR_CC ) {
			intr |= MMC_INTR_COMMAND;
			if( resp ) {
				if( resp_type & MMC_RSP_136 ) {
					resp[3] = in32( base + OMAP_MMCHS_RSP76 );
					resp[2] = in32( base + OMAP_MMCHS_RSP54 );
					resp[1] = in32( base + OMAP_MMCHS_RSP32 );
					resp[0] = in32( base + OMAP_MMCHS_RSP10 );
				}
				else if( resp_type & MMC_RSP_PRESENT ) {
					resp[0] = in32( base + OMAP_MMCHS_RSP10 );
				}
			}

				// Busy check?
			if( resp_type & MMC_RSP_BUSY ) {
				if( omap_waitmask( hba, OMAP_MMCHS_PSTATE, PSTATE_DLA, 0, 500000 ) ) {
					intr |= MMC_ERR_CMD_TO | MMC_INTR_ERROR;
					mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 1, "%s:  busy check time out", __FUNCTION__ );
				}
			}
		}

		if( sts & ( INTR_TC | INTR_BWR | INTR_BRR ) ) {
			if( sts & INTR_TC ) {
				intr |= MMC_INTR_DATA;
			}
			if( sts & INTR_BRR ) {
				intr |= MMC_INTR_RBRDY;
			}
			if( sts & INTR_BWR ) {
				intr |= MMC_INTR_WBRDY;
			}
		}
	}

	if( intr ) {
		out32( base + OMAP_MMCHS_IE, 0 );
	}

	return( intr );
}

static int omap_setup_pio( SIM_HBA *hba, int len, int dir )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	int				nblk;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;

	nblk = len / oext->blksz;
	len  = nblk * oext->blksz;

	oext->dcmd = CMD_DP;
	if( nblk > 1 ) {
		oext->dcmd |= CMD_MBS | CMD_BCE;
		if( ext->hccap & MMC_HCCAP_ACMD12 ) {
			oext->dcmd |= CMD_ACEN;
		}
	}
	if( dir == MMC_DIR_IN ) {
		oext->dcmd |= CMD_DDIR;
		oext->dmask = INTR_BRR;
	}
	else {
		oext->dmask = INTR_BWR;
	}

	out32( oext->mmc_base + OMAP_MMCHS_BLK, ( nblk << 16 ) | oext->blksz );

	return( len );
}

static int omap_pio_done( SIM_HBA *hba, char *buf, int len, int dir )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	uintptr_t		base;
	int				cnt;
	int				nbytes;
	uint32_t		*pbuf;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;
	base	= oext->mmc_base;
	pbuf	= (uint32_t *)buf;

	cnt = nbytes = len < MMCHS_FIFO_SIZE ? len : MMCHS_FIFO_SIZE;

	if( dir == MMC_DIR_IN ) {
		if( !( in32( base + OMAP_MMCHS_PSTATE ) & PSTATE_BRE ) ) {
			out32( base + OMAP_MMCHS_IE, INTR_BRR );
			return( 0 );
		}

		for( ; nbytes > 0; nbytes -= 4 ) {
			*pbuf++ = in32( base + OMAP_MMCHS_DATA );
		}

		if( len < MMCHS_FIFO_SIZE ) {
			int		cnt;

			for( cnt = 2000; cnt; cnt-- ) {
				if( !( in32( base + OMAP_MMCHS_PSTATE ) & PSTATE_RTA ) ) {
					break;
				}
				nanospin_ns( 1000 );
			}
		}
	}
	else {
		if( !( in32( base + OMAP_MMCHS_PSTATE ) & PSTATE_BWE ) ) {
			out32( base + OMAP_MMCHS_IE, INTR_BWR );
			return( 0 );
		}

		for( ; nbytes > 0; nbytes -= 4 ) {
			out32( base + OMAP_MMCHS_DATA, *pbuf++ );
		}

		if( len == cnt ) {
			out32( base + OMAP_MMCHS_IE, INTR_TC );
		}
	}

	return( cnt );
}


static int omap_setup_sgdma( SIM_HBA *hba, int dir )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	CCB_SCSIIO		*ccb;
	uintptr_t		base;
	uint32_t		xlen;
	uint32_t		blkcnt;
	sdhci_adma32_t	*adma;
	SG_ELEM			sge;
	SG_ELEM			*sgp;
	int				sgc;
	int				sgi;
	int				acnt;
	int				alen;
	int				sg_count;
	paddr_t			paddr;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;
	adma	= (sdhci_adma32_t *)oext->adma;
	ccb		= ext->nexus;
	base	= oext->mmc_base;
	xlen	= ccb->cam_dxfer_len;

#ifdef OMAP_DEBUG
	mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 1, "%s: ", __FUNCTION__ );
#endif

	if( ccb->cam_ch.cam_flags & CAM_SCATTER_VALID ) {
		sgc = ccb->cam_sglist_cnt;
		sgp = ccb->cam_data.cam_sg_ptr;
	}
	else {
		sgp = &sge;
		sgc = 1;
		sgp->cam_sg_count	= ccb->cam_dxfer_len;
		sgp->cam_sg_address	= ccb->cam_data.cam_data_ptr;
	}

#define OMAP_ADMA_MAX_XFER	(1024 *60)

	for( sgi = 0, acnt = 0; sgi < sgc; sgi++, sgp++ ) {
		paddr		= mmc_mphys( ccb, sgp->cam_sg_address, sgi );
		sg_count	= sgp->cam_sg_count;
		while( sg_count ) {
			alen		= min( sg_count, OMAP_ADMA_MAX_XFER );
			adma->attr	= SDHCI_ADMA2_VALID | SDHCI_ADMA2_TRAN;
			adma->addr	= paddr;
			adma->len	= alen;
			sg_count	-= alen; 
			paddr		+= alen;
			adma++;
			if( ++acnt > SDHCI_ADMA_DESC_MAX ) {
				return( 0 );
			}
		}
	}

	adma--;
	adma->attr |= SDHCI_ADMA2_END;

	blkcnt = ccb->cam_dxfer_len / oext->blksz;
	xlen   = oext->blksz * blkcnt;

	if( blkcnt == 0 ) {
		return( 0 );
	}

	oext->dmask	= INTR_TC;
	oext->dcmd	= CMD_DP | CMD_DE;
	if( ccb->cam_ch.cam_flags & CAM_DIR_IN ) {
		oext->dcmd |= CMD_DDIR;
	}

	if( blkcnt > 1 ) {
		oext->dcmd |= CMD_MBS | CMD_BCE;
		if( ext->hccap & MMC_HCCAP_ACMD12 ) {
			oext->dcmd |= CMD_ACEN;
		}
	}

	out32( oext->mmc_base + OMAP_MMCHS_BLK, ( blkcnt << 16 ) | oext->blksz );
	out32( base + OMAP_MMCHS_ADMASAL, oext->admap );

	return( xlen );
}

static int omap_setup_dma( SIM_HBA *hba, paddr_t paddr, int len, int dir )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	uintptr_t		cbase;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;

	if( oext->adma ) {
		return( omap_setup_sgdma( hba, 0 ) );
	}

	cbase = oext->sdma_cbase;
	if( ( len = omap_setup_pio( hba, len, dir ) ) ) {
		out32( cbase + DMA4_CSR, 0x1FFE );		// Clear all status bits
		out32( cbase + DMA4_CEN, len >> 2 );
		out32( cbase + DMA4_CFN, 1 );			// Number of frames
		out32( cbase + DMA4_CSE, 1 );
		out32( cbase + DMA4_CDE, 1 );
		out32( cbase + DMA4_CICR, 0 );			// We don't want any interrupts

		if( dir == MMC_DIR_IN ) {
			// setup receive SDMA channel
			out32( cbase + DMA4_CSDP, (2 <<  0)		// DATA_TYPE = 0x2:  32 bit element
						| (0 <<  2)		// RD_ADD_TRSLT = 0: Not used
						| (0 <<  6)		// SRC_PACKED = 0x0: Cannot pack source data
						| (0 <<  7)		// SRC_BURST_EN = 0x0: Cannot burst source
						| (0 <<  9)		// WR_ADD_TRSLT = 0: Undefined
						| (0 << 13)		// DST_PACKED = 0x0: No packing
						| (3 << 14)		// DST_BURST_EN = 0x3: Burst at 16x32 bits
						| (1 << 16)		// WRITE_MODE = 0x1: Write posted
						| (0 << 18)		// DST_ENDIAN_LOCK = 0x0: Endianness adapt
						| (0 << 19)		// DST_ENDIAN = 0x0: Little Endian type at destination
						| (0 << 20)		// SRC_ENDIAN_LOCK = 0x0: Endianness adapt
						| (0 << 21) );	// SRC_ENDIAN = 0x0: Little endian type at source

			out32( cbase + DMA4_CCR, DMA4_CCR_SYNCHRO_CONTROL( oext->sdma_rreq )	// Synchro control bits
						| (1 <<  5)		// FS = 1: Packet mode with BS = 0x1
						| (0 <<  6)		// READ_PRIORITY = 0x0: Low priority on read side
						| (0 <<  7)		// ENABLE = 0x0: The logical channel is disabled.
						| (0 <<  8)		// DMA4_CCRi[8] SUSPEND_SENSITIVE = 0
						| (0 << 12)		// DMA4_CCRi[13:12] SRC_AMODE = 0x0: Constant address mode
						| (1 << 14)		// DMA4_CCRi[15:14] DST_AMODE = 0x1: Post-incremented address mode
						| (1 << 18)		// DMA4_CCRi[18] BS = 0x1: Packet mode with FS = 0x1
						| (1 << 24)		// DMA4_CCRi[24] SEL_SRC_DST_SYNC = 0x1: Transfer is triggered by the source. The packet element number is specified in the DMA4_CSFI register.
						| (0 << 25) );	// DMA4_CCRi[25] BUFFERING_DISABLE = 0x0

			out32( cbase + DMA4_CSSA, oext->mmc_pbase + OMAP_MMCHS_DATA );
			out32( cbase + DMA4_CDSA, paddr );
			out32( cbase + DMA4_CSF, oext->blksz >> 2 );
		}
		else {
			// setup transmit SDMA channel
			out32( cbase + DMA4_CSDP, (2 <<  0)		// DATA_TYPE = 0x2:  32 bit element
						| (0 <<  2)		// RD_ADD_TRSLT = 0: Not used
						| (0 <<  6)		// SRC_PACKED = 0x0: Cannot pack source data
						| (3 <<  7)		// SRC_BURST_EN = 0x3: Burst at 16x32 bits
						| (0 <<  9)		// WR_ADD_TRSLT = 0: Undefined
						| (0 << 13)		// DST_PACKED = 0x0: No packing
						| (0 << 14)		// DST_BURST_EN = 0x0: Cannot Burst
						| (0 << 16)		// WRITE_MODE = 0x0: Write not posted
						| (0 << 18)		// DST_ENDIAN_LOCK = 0x0: Endianness adapt
						| (0 << 19)		// DST_ENDIAN = 0x0: Little Endian type at destination
						| (0 << 20)		// SRC_ENDIAN_LOCK = 0x0: Endianness adapt
						| (0 << 21) );	// SRC_ENDIAN = 0x0: Little endian type at source

			out32( cbase + DMA4_CCR, DMA4_CCR_SYNCHRO_CONTROL( oext->sdma_treq )
						| (1 <<  5)		// FS = 1: Packet mode with BS = 0x1
						| (0 <<  6)		// READ_PRIORITY = 0x0: Low priority on read side
						| (0 <<  7)		// ENABLE = 0x0: The logical channel is disabled.
						| (0 <<  8)		// DMA4_CCRi[8] SUSPEND_SENSITIVE = 0
						| (1 << 12)		// DMA4_CCRi[13:12] SRC_AMODE = 0x1: Post-incremented address mode
						| (0 << 14)		// DMA4_CCRi[15:14] DST_AMODE = 0x0: Constant address mode
						| (1 << 18)		// DMA4_CCRi[18] BS = 0x1: Packet mode with FS = 0x1
						| (0 << 24)		// DMA4_CCRi[24] SEL_SRC_DST_SYNC = 0x0: Transfer is triggered by the source. The packet element number is specified in the DMA4_CSFI register.
						| (0 << 25) );	// DMA4_CCRi[25] BUFFERING_DISABLE = 0x0

			out32( cbase + DMA4_CSSA, paddr );
			out32( cbase + DMA4_CDSA, oext->mmc_pbase + OMAP_MMCHS_DATA );
			out32( cbase + DMA4_CDF, oext->blksz >> 2 );
		}

		// Enable DMA event
		out32( cbase + DMA4_CCR, in32( cbase + DMA4_CCR ) | 1 << 7 );

		oext->dcmd |= CMD_DE;
		oext->dmask = INTR_TC;		// Use transfer complete interrupt
		oext->dsize = len;
	}

	return( len );
}

static int omap_dma_done( SIM_HBA *hba, int dir )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	uintptr_t		base;
	uintptr_t		cbase;
	int				cnt;
	int				ret;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;
	base	= oext->mmc_base;
	ret		= MMC_SUCCESS;

	if( oext->adma ) {
	}
	else {
		cbase	= oext->sdma_cbase;
		for( cnt = 2000; cnt; cnt-- ) {	// wait a couple ms for xfer completion
			if( in32( cbase + DMA4_CCEN ) == ( oext->dsize >> 2 ) ) {	// xfer complete
				break;
			}
			if( in32( cbase + DMA4_CSR ) & DMA4_CSR_ERROR ) {	// DMA err
				break;
			}
			nanospin_ns( 1000 );
		}

		if( in32( cbase + DMA4_CSR ) & DMA4_CSR_ERROR || !cnt ) {
			ret = MMC_FAILURE;
		}

		out32( cbase + DMA4_CCR, 0 );		// Disable this DMA event
		out32( cbase + DMA4_CSR, 0x1FFE );	// Clear all status bits
	}

	return( ret );
}

static int omap_command( SIM_HBA *hba, mmc_cmd_t *cmd )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	uintptr_t		base;
	uint32_t		con;
	uint32_t		imask;
	uint32_t		command;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;
	base	= oext->mmc_base;
	
	out32( base + OMAP_MMCHS_STAT, 0x117f8033 );	// Clear Status
	con = in32( base + OMAP_MMCHS_CON ) & ~CON_OD;

	if( !( cmd->eflags & MMC_CMD_PPL ) ) {
		con |= CON_OD;
	}

	if( cmd->eflags & MMC_CMD_INIT ) {
		out32( base + OMAP_MMCHS_CON, con | CON_INIT );
		delay( 10 );
	}

	out32( base + OMAP_MMCHS_CON, con );

	imask	= INTR_DFLTS;
	command	= (cmd->opcode) << 24;
	if( cmd->opcode == 12 ) {
		command |= CMD_TYPE_CMD12;
	}

	if( cmd->eflags & MMC_CMD_DATA ) {
		command |= oext->dcmd;
		imask |= INTR_DTO | INTR_DCRC | INTR_DEB;	// Enable all data error interrupts
		imask |= oext->dmask;	// Data complete interrupt or data ready interrupt
	}
	else {
		imask |= INTR_CC;		// Enable command complete interrupt
	}

	if( cmd->resp & MMC_RSP_PRESENT ) {
		if( cmd->resp & MMC_RSP_136 ) {
			command |= CMD_RSP_TYPE_136;
		}
		else if( cmd->resp & MMC_RSP_BUSY ) {	// Response with busy check
			command |= CMD_RSP_TYPE_48b;
		}
		else {
			command |= CMD_RSP_TYPE_48;
		}

		if( cmd->resp & MMC_RSP_OPCODE ) {		// Index check
			command |= CMD_CICE;
		}

		if( cmd->resp & MMC_RSP_CRC ) {			// CRC check
			command |= CMD_CCCE;
		}
	}

	out32( base + OMAP_MMCHS_IE,  imask );
	out32( base + OMAP_MMCHS_ARG, cmd->argument );
	out32( base + OMAP_MMCHS_CMD, command );

	return( MMC_SUCCESS );
}

static int omap_cfg_bus(SIM_HBA *hba, int width, int mmc)
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	uintptr_t		base;
	uint32_t		con;
	uint32_t		hctl;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;
	base	= oext->mmc_base;
	con		= in32( base + OMAP_MMCHS_CON ) & ~( CON_DW8 | CON_DDR );
	hctl	= in32( base + OMAP_MMCHS_HCTL ) & ~( HCTL_DTW4 );

	switch( width ) {
		case 8:
			con |= CON_DW8; break;
		case 4:
			hctl |= HCTL_DTW4; break;
		case 1:
		default:
			break;
	}

	if( ext->eflags & MMC_EFLAG_DDR ) {
		con |= CON_DDR;
		hctl &= ~HCTL_HSE;
	}

	out32( base + OMAP_MMCHS_HCTL, hctl );
	out32( base + OMAP_MMCHS_CON, con );

	return( MMC_SUCCESS );
}

static int omap_clock( SIM_HBA *hba, int *clock, int high_speed )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	uintptr_t		base;
	uint32_t		sysctl;
	int				clkd;
#if 0
	uint32_t		hctl;
#endif

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;
	base	= oext->mmc_base;

	clkd = oext->mmc_clock / (*clock);
	if( oext->mmc_clock != (*clock) * clkd ) {
		clkd++;
	}
	*clock = oext->mmc_clock / clkd;
//mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 1, "%s: clock %d, clk div %d", __FUNCTION__, *clock, clkd );

	sysctl = in32( base + OMAP_MMCHS_SYSCTL );

		// Stop clock
	sysctl &= ~(SYSCTL_ICE | SYSCTL_CEN | SYSCTL_DTO_MASK);
	sysctl |= SYSCTL_DTO_MAX | SYSCTL_SRC | SYSCTL_SRD;
	out32( base + OMAP_MMCHS_SYSCTL, sysctl );

		// Enable internal clock
	sysctl &= ~(0x3FF << 6);
	sysctl |= (clkd << 6) | SYSCTL_ICE;
	out32( base + OMAP_MMCHS_SYSCTL, sysctl );

		// Wait for the clock to be stable
	omap_waitmask( hba, OMAP_MMCHS_SYSCTL, SYSCTL_ICS, SYSCTL_ICS, 10000 );

		// Enable clock to the card
	out32( base + OMAP_MMCHS_SYSCTL, sysctl | SYSCTL_CEN );
#if 0
// TI says not to use HSE
	hctl = in32( base + OMAP_MMCHS_HCTL ) & ~HCTL_HSE;
	if( high_speed ) {
		hctl |= HCTL_HSE;
	}
	out32( base + OMAP_MMCHS_HCTL, hctl );
#endif
	return( MMC_SUCCESS );
}

static int omap_block_size( SIM_HBA *hba, int blksz )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;

	if( blksz > 1024 ) {
		return( MMC_FAILURE );
	}

	oext->blksz = blksz;
	out32( oext->mmc_base + OMAP_MMCHS_BLK, oext->blksz );
	delay( 10 );

	return( MMC_SUCCESS );
}

// Reset host controller/card and set clock to 400KHz.
static int omap_powerup(SIM_HBA *hba)
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	uint32_t		con;
	uint32_t		hctl;
	uint32_t		capa;
	uintptr_t		base;
	int				clock;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;
	base	= oext->mmc_base;

	out32( base + OMAP_MMCHS_IE, 0 );	// Disable all intrs

		// Software reset
#if 0
	out32( base + OMAP_MMCHS_SYSCONFIG, SYSCONFIG_SOFTRESET | SYSCONFIG_AUTOIDLE );
#else
	out32( base + OMAP_MMCHS_SYSCONFIG, in32( base + OMAP_MMCHS_SYSCONFIG ) | SYSCONFIG_SOFTRESET );
#endif
	omap_waitmask( hba, OMAP_MMCHS_SYSSTATUS, SYSSTATUS_RESETDONE, SYSSTATUS_RESETDONE, 10000 );
	omap_reset( hba, SYSCTL_SRA );

	capa = in32( base + OMAP_MMCHS_CAPA ) & ~( CAPA_VS3V3 | CAPA_VS3V0 | CAPA_VS1V8 );

	if( oext->mmc_pbase == OMAP_MMCHS1_BASE ) { // MMC1
		hctl = HCTL_SDVS3V0;
		capa |= CAPA_VS3V3 | CAPA_VS3V0 | CAPA_VS1V8;
	}
	else {
		hctl = HCTL_SDVS1V8;
		capa |= CAPA_VS1V8;
	}

	con = CON_DEBOUNCE_8MS;
	if( oext->adma ) {
		con |= CON_DMA_MNS;
		hctl |= HCTL_DMAS_ADMA2;
	}

	out32( base + OMAP_MMCHS_CON, con );
	out32( base + OMAP_MMCHS_HCTL, hctl );
	out32( base + OMAP_MMCHS_CAPA, capa );

	clock = 400 * 1000;		// 400KHz clock
	omap_clock( hba, (int *)&clock, 0 );

	out32( base + OMAP_MMCHS_HCTL, hctl | HCTL_SDBP );

	out32( base + OMAP_MMCHS_ISE, INTR_ALL );			// Enable intrs

	delay( 10 );
	return( MMC_SUCCESS );
}

static int omap_powerdown( SIM_HBA *hba )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	uintptr_t		base;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;
	base	= oext->mmc_base;

	omap_core_pm( hba, MMC_PM_ACTIVE );

		// Disable all MMCHS interrupt signals
	out32( base + OMAP_MMCHS_ISE, 0 );

		// Disable All interrupts
	out32( base + OMAP_MMCHS_IE, 0 );
	out32( base + OMAP_MMCHS_ISE, 0 );

		// Software reset
	out32( base + OMAP_MMCHS_SYSCONFIG, in32( base + OMAP_MMCHS_SYSCONFIG ) | SYSCONFIG_SOFTRESET );
	omap_waitmask( hba, OMAP_MMCHS_SYSSTATUS, SYSSTATUS_RESETDONE, SYSSTATUS_RESETDONE, 10000 );

	omap_reset( hba, SYSCTL_SRA );
	
	return( MMC_SUCCESS );
}

int omap_suspend( SIM_HBA *hba )
{
//	mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 3, "%s: ", __FUNCTION__ );
//	omap_dump( hba );
	omap_powerdown( hba );

	return( MMC_SUCCESS );
}

int omap_resume( SIM_HBA *hba )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	int				clock;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;

	clock	= ext->speed ? ext->speed : ( 400 * 1000 );

	omap_core_pm( hba, MMC_PM_ACTIVE );
	omap_powerup( hba );
	omap_clock( hba, &clock, 0 );
	omap_cfg_bus( hba, ext->bus_width, 0 );
	omap_block_size( hba, oext->blksz );

//	mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 3, "%s: ", __FUNCTION__ );
//	omap_dump( hba );
	return( MMC_SUCCESS );
}

static int omap_detect(SIM_HBA *hba)
{
		// Need external logic to detect card
	return( MMC_SUCCESS );
}

int omap_core_pm( SIM_HBA *hba, int op )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	int				wait;
	uint32_t		cval;
	uintptr_t		clkctl;
	uintptr_t		context;
	
	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;
	clkctl	= oext->l3init_cm2_base + CM_L3INIT_HSMMC_CLKCTRL( oext->mmc_pbase );
	context	= oext->l3init_prm_base + RM_L3INIT_MMC_CONTEXT( oext->mmc_pbase );

	if( !( oext->oflags & OMAP_OFLAG_PM ) ) {
		return( MMC_SUCCESS );
	}

	if( op ) {
		if( !( oext->oflags & OMAP_OFLAG_ACTIVE ) ) {
#ifdef OMAP_DEBUG
			mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 3, "%s: set %s", __FUNCTION__, op ? "active" : "idle" );
#endif
			oext->oflags	|= OMAP_OFLAG_ACTIVE;
			out32( clkctl, in32( clkctl ) | CM_L3INIT_CLKCTRL_MODULEMODE_ENB | CM_L3INIT_CLKCTRL_CKLSEL_96 );
			for( wait = 10000; wait; wait-- ) {		// wait up to 100ms
				cval = in32( clkctl );
				if( ( cval & CM_L3INIT_CLKCTRL_MODULEMODE_MSK ) &&
						!( cval & CM_L3INIT_CLKCTRL_IDLEST_MSK ) ) {
					break;
				}
				nanospin_ns( 1000L );
			}
				// smart idle
			out32( oext->mmc_base + OMAP_MMCHS_SYSCONFIG,
				( in32( oext->mmc_base + OMAP_MMCHS_SYSCONFIG ) &
					~SYSCONFIG_CONFIG_MSK ) | SYSCONFIG_CONFIG );

			if( in32( context ) & RM_L3INIT_MMC_CONTEXT_LOSTCONTEXT_RFF ) {
				mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 3, "%s: LOSTCONTEXT_RFF", __FUNCTION__ );
				omap_resume( hba );
				out32( context, RM_L3INIT_MMC_CONTEXT_LOSTCONTEXT_RFF );
			}
		}
	}
	else {
		if( ( oext->oflags & OMAP_OFLAG_ACTIVE ) ) {
#ifdef OMAP_DEBUG
			mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 3, "%s: set %s", __FUNCTION__, op ? "active" : "idle" );
#endif
			oext->oflags &= ~OMAP_OFLAG_ACTIVE;

				// force idle
			out32( oext->mmc_base + OMAP_MMCHS_SYSCONFIG,
				in32( oext->mmc_base + OMAP_MMCHS_SYSCONFIG ) & ~SYSCONFIG_CONFIG_MSK );

			out32( clkctl, in32( clkctl ) & ~CM_L3INIT_CLKCTRL_MODULEMODE_MSK );
		}
	}
	return( MMC_SUCCESS );
}

static int omap_shutdown( SIM_HBA *hba )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;

	ext		= (SIM_MMC_EXT *)hba->ext;
	oext	= (omap_ext_t *)ext->handle;

	if( oext->mmc_base ) {
		omap_powerdown( hba );
		if( oext->l3init_cm2_base ) {
			omap_core_pm( hba, MMC_PM_IDLE );
			munmap_device_io( oext->l3init_cm2_base, OMAP_L3INIT_CM2_SIZE );
		}
		if( oext->l3init_prm_base ) {
			munmap_device_io( oext->l3init_prm_base, OMAP_L3INIT_PRM_SIZE );
		}
		munmap_device_io( oext->mmc_base, OMAP_MMCHS_SIZE );
	}

	if( oext->sdma_base ) {
		out32( oext->sdma_cbase + DMA4_CCR, 0 );		// Disable DMA event
		out32( oext->sdma_cbase + DMA4_CSR, 0x1ffe );	// Clear Status
		munmap_device_io( oext->sdma_base, OMAP_DMA4_SIZE );
	}

	if( oext->adma ) {
		munmap( oext->adma, sizeof( sdhci_adma32_t ) * SDHCI_ADMA_DESC_MAX );
	}

	free( oext );

	return( 0 );
}

static unsigned omap_get_syspage_clk( )
{
	unsigned	item;
	unsigned	clock;
	unsigned	offset;
	char		*name;
	hwi_tag		*tag;

	clock = 0;
	if( ( item = hwi_find_item( HWI_NULL_OFF,
			HWI_ITEM_DEVCLASS_DISK, "mmc", NULL ) ) == HWI_NULL_OFF ) {
		return( 0 );
	}

	offset = item;

	while( ( offset = hwi_next_tag( offset, 1 ) ) != HWI_NULL_OFF ) {
		tag = hwi_off2tag( offset );
		name = __hwi_find_string( ((hwi_tag *)tag)->prefix.name );

		if( strcmp( name, HWI_TAG_NAME_inputclk ) == 0 ) {
			clock = ((struct hwi_inputclk *)tag)->clk / ((struct hwi_inputclk *)tag)->div;
		}
	}

	return( clock );
}

int	omap4_attach( SIM_HBA *hba )
{
	SIM_MMC_EXT		*ext;
	omap_ext_t		*oext;
	CONFIG_INFO		*cfg;
	uintptr_t		base;
	uintptr_t		context;

	cfg = (CONFIG_INFO *)&hba->cfg;
	ext = (SIM_MMC_EXT *)hba->ext;

	if( ext->handle == NULL && ( ext->handle = calloc( 1, sizeof( omap_ext_t ) ) ) == NULL ) {
		return( MMC_FAILURE );
	}

	oext = ext->handle;

	if( ( oext->mmc_clock = omap_get_syspage_clk( ) ) == 0 ) {
		oext->mmc_clock = OMAP_MMCHS_CLK_DFLT;
	}

	oext->mmc_pbase = cfg->IOPort_Base[0];

	if( ( base = oext->mmc_base = mmap_device_io( OMAP_MMCHS_SIZE, oext->mmc_pbase ) ) == (uintptr_t)MAP_FAILED ) {
		mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 1, "%s: MMCHS base mmap_device_io %s", __FUNCTION__, strerror( errno ) );
		omap_shutdown( hba );
		return( MMC_FAILURE );
	}

		// Capability
	ext->hccap |= MMC_HCCAP_ACMD12 | MMC_HCCAP_BW1 | MMC_HCCAP_BW4 | MMC_HCCAP_BW8;	// 1 bit / 4 bits bus supported
	ext->hccap |= MMC_HCCAP_33V | MMC_HCCAP_30V | MMC_HCCAP_18V;

	switch( oext->mmc_pbase ) {
		case OMAP_MMCHS1_BASE:			// MMC1
			break;

		case OMAP_MMCHS2_BASE:			// MMC2 / EMMC
			ext->hccap &= ~MMC_HCCAP_33V | MMC_HCCAP_30V;
			break;

		default:
			return( MMC_FAILURE );
			break;
	}

	if( ( oext->l3init_cm2_base = mmap_device_io( OMAP_L3INIT_CM2_SIZE, L3INIT_CM2_BASE ) ) == (uintptr_t)MAP_FAILED ) {
		mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 1, "%s: MMCHS L3INIT_CM2 mmap_device_io %s", __FUNCTION__, strerror( errno ) );
		omap_shutdown( hba );
		return( MMC_FAILURE );
	}

	if( ( oext->l3init_prm_base = mmap_device_io( OMAP_L3INIT_PRM_SIZE, L3INIT_PRM_BASE ) ) == (uintptr_t)MAP_FAILED ) {
		mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 1, "%s: MMCHS L3INIT_PRM mmap_device_io %s", __FUNCTION__, strerror( errno ) );
		omap_shutdown( hba );
		return( MMC_FAILURE );
	}

	if( ( oext->oflags & OMAP_OFLAG_PM ) ) {
		context	= oext->l3init_prm_base + RM_L3INIT_MMC_CONTEXT( oext->mmc_pbase );
		out32( context, RM_L3INIT_MMC_CONTEXT_LOSTCONTEXT_RFF );
		ext->pwrmgnt = omap_core_pm;
		omap_core_pm( hba, MMC_PM_ACTIVE );
	}

	oext->cap = in32( base + OMAP_MMCHS_CAPA );
	if( oext->cap & CAPA_HS ) {
		ext->hccap |= MMC_HCCAP_HS;
	}

	if( ( in32( base + OMAP_MMCHS_HL_HWINFO ) & HWINFO_MADMA_EN ) && ( oext->cap & CAPA_AD2S ) ) {
		if( ( oext->adma = mmap( NULL, sizeof( sdhci_adma32_t ) * SDHCI_ADMA_DESC_MAX, PROT_READ | PROT_WRITE | PROT_NOCACHE, 
				MAP_PRIVATE | MAP_ANON | MAP_PHYS, NOFD, 0)) == MAP_FAILED) {
			mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 1, "%s: ADMA mmap %s", __FUNCTION__, strerror( errno ) );
			omap_shutdown( hba );
			return( MMC_FAILURE );
		}

		oext->admap			= mphys( oext->adma );
		ext->hccap			|= MMC_HCCAP_DMA;
		ext->hccap			|= MMC_HCCAP_DDR50;
		ext->max_sg			= SDHCI_ADMA_DESC_MAX;
		ext->setup_sgdma	= omap_setup_sgdma;
	}
	else if( cfg->NumDMAs > 2 && cfg->NumIOPorts > 1 ) {
		oext->sdma_chnl = cfg->DMALst[0];
		oext->sdma_treq = cfg->DMALst[1];
		oext->sdma_rreq = cfg->DMALst[2];
		if( ( oext->sdma_base = mmap_device_io( OMAP_DMA4_SIZE, cfg->IOPort_Base[1] ) ) == (uintptr_t)MAP_FAILED ) {
			mmc_slogf( hba, _SLOGC_SIM_MMC, _SLOG_ERROR, 1, "%s: DMA base mmap_device_io %s", __FUNCTION__, strerror( errno ) );
			omap_shutdown( hba );
			return( MMC_FAILURE );
		}
		oext->sdma_cbase = oext->sdma_base + DMA4_CH_BASE( oext->sdma_chnl );
		ext->hccap |= MMC_HCCAP_DMA;
	}

	ext->clock		= oext->mmc_clock;
	ext->detect		= omap_detect;
	ext->powerup	= omap_powerup;
	ext->powerdown	= omap_powerdown;
	ext->command	= omap_command;
	ext->cfg_bus	= omap_cfg_bus;
	ext->set_clock	= omap_clock;
	ext->set_blksz	= omap_block_size;
	ext->interrupt	= omap_interrupt;
	ext->setup_dma	= omap_setup_dma;
	ext->dma_done	= omap_dma_done;
	ext->setup_pio	= omap_setup_pio;
	ext->pio_done	= omap_pio_done;
	ext->shutdown	= omap_shutdown;

	if( !cfg->Description[0] ) {
		strncpy( (char *)cfg->Description, "TI OMAP MMCHS", sizeof( cfg->Description ) );
	}

	return( MMC_SUCCESS );
}
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devb/mmcsd/chipsets/sim_omap4.c $ $Rev: 680332 $")
#endif
