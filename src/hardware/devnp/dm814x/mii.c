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

#include	"ti814x.h"


/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
uint16_t ti814x_mdi_read (void *hdl, uint8_t phyid, uint8_t reg)
{
	ti814x_dev_t	*ti814x = (ti814x_dev_t *) hdl;
	int				timeout = 1000;
	uint16_t		data = 0;
	uint32_t		mdio_useraccess_reg;

	if (ti814x->emu_phy != -1 && ti814x->emu_phy == ti814x->cfg.device_index) {
		return (emu_phy_read (ti814x, phyid, reg));
		}

	switch(ti814x->cfg.device_index) {
		case 0: mdio_useraccess_reg = MDIOUSERACCESS0; break;
		case 1: mdio_useraccess_reg = MDIOUSERACCESS1; break;
		default: return(0xffff);
	}

	while (timeout) {
		if (! (inle32 (ti814x->cpsw_regs + mdio_useraccess_reg) & GO))
			break;
		nanospin_ns (1000);
		timeout--;
	}
	
	if (timeout <= 0) {
		slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d timeout waiting for ready",	__FUNCTION__, __LINE__);
		return (0xffff);
	}
	
	outle32 (ti814x->cpsw_regs + mdio_useraccess_reg, GO | REGADR(reg) | PHYADR(phyid));

	while (timeout) {
		if (! (inle32 (ti814x->cpsw_regs + mdio_useraccess_reg) & GO))
			break;
		nanospin_ns (1000);
		timeout--;
	}
	
	if (timeout <= 0) {
		slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d timeout waiting for data",	__FUNCTION__, __LINE__);
		return (0xffff);
	}

	// if (!(inle32(mdio+mdio_useraccess_reg) & ACK))
	//	 slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d ACK BIT NOT SET: BAD",	__FUNCTION__, __LINE__);
	data = ((uint16_t) inle32 (ti814x->cpsw_regs + mdio_useraccess_reg) & 0xffff);
	if (ti814x->cfg.verbose & DEBUG_MII_RW) {
		slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d devid=%d phyid=%d reg=%d data=0x%04x",
			  __FUNCTION__, __LINE__, ti814x->cfg.device_index, phyid, reg, data);
	}
	return data;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void ti814x_mdi_write (void *hdl, uint8_t phyid, uint8_t reg, uint16_t data)
{
	ti814x_dev_t	*ti814x = (ti814x_dev_t *) hdl;
	int				timeout = 1000;
	uint32_t		mdio_useraccess_reg;

	if (ti814x->cfg.verbose & DEBUG_MII_RW) {
		slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d - devid=%d phyid=%d reg=%d data=0x%04x",
			__FUNCTION__, __LINE__, ti814x->cfg.device_index, phyid, reg, data);
	}

	if (ti814x->emu_phy != -1 && ti814x->emu_phy == ti814x->cfg.device_index) {
		return;
		}

	switch(ti814x->cfg.device_index) {
		case 0: mdio_useraccess_reg = MDIOUSERACCESS0; break;
		case 1: mdio_useraccess_reg = MDIOUSERACCESS1; break;
		default:
			slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d -Invalid device index %d",
						__FUNCTION__, __LINE__, ti814x->cfg.device_index);
			return;
	}

	while (timeout) {
		if (! (inle32 (ti814x->cpsw_regs + mdio_useraccess_reg) & GO))
			break;
		nanospin_ns (1000);
		timeout--;
	}
	if (timeout <= 0)
		return;

	outle32 (ti814x->cpsw_regs + mdio_useraccess_reg, GO | WRITE | REGADR(reg) | PHYADR(phyid) | data);

	timeout = 1000;
	while (timeout) {
		if (!(inle32(ti814x->cpsw_regs + mdio_useraccess_reg) & GO))
			break;
		nanospin_ns(1000);
		timeout--;
	}

	if (timeout <= 0) {
		 slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d - write operation timeout",
		  __FUNCTION__, __LINE__);
	}
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int	ti814x_phy_funcs (ti814x_dev_t *ti814x, struct ifdrv *ifd)

{
    phy_access_t phy;
    int	         timeout;

	if (! ti814x->phy_init) {
		outle32 (ti814x->cpsw_regs + MDIOCONTROL, MDIO_ENABLE | MDIO_FAULT | MDIO_FAULTENB | MDIO_CLK_DIVISOR);
		timeout = 1000;
		while (timeout) {
			if (!(inle32 (ti814x->cpsw_regs + MDIOCONTROL) & MDIO_IDLE))
				break;
			nanospin_ns (1000);
			timeout--;
			}
		if (timeout <= 0) {
			slogf (_SLOGC_NETWORK, _SLOG_DEBUG1, "PHY init timeout");
			return (-1);
			}
		ti814x->phy_init = 1;
		}
	if (copyin ((((uint8_t *)ifd) + sizeof(*ifd)), &phy, sizeof(phy))) {
	    return (EINVAL);
		}
	if (strcmp (ifd->ifd_name, "dm0") && strcmp (ifd->ifd_name, "dm1")) {
		return (EINVAL);
		}
	if (ifd->ifd_data == NULL) {
		return (EINVAL);
		}
	if (ifd->ifd_len != sizeof (phy_access_t)) {
		return (EINVAL);
		}
	if (phy.phy_id >= 32) {
		return (EINVAL);
		}
	if (ifd->ifd_cmd == READ_PHY_REG)
		phy.phy_data = ti814x_mdi_read (ti814x, phy.phy_id, phy.phy_reg);
	else
		ti814x_mdi_write (ti814x, phy.phy_id, phy.phy_reg, phy.phy_data);
	copyout (&phy, (((uint8_t *)ifd) + sizeof(*ifd)), sizeof(phy));
	return (EOK);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

void ti814x_mdi_callback (void *handle, uint8_t phyaddr, uint8_t linkstate)

{
	ti814x_dev_t		*ti814x = handle;
	int					i, mode, gig_en=0;
	uint32_t			reg, mac_control_reg;
	char				*s = 0;
	struct ifnet		*ifp = &ti814x->ecom.ec_if;
	nic_config_t		*cfg = &ti814x->cfg;
	uint16_t		advert, lpadvert;
	uint32_t		q1_send, q2_send, q3_send;
	uint32_t		port_bw, percent_reg, value;

	if (cfg->verbose & DEBUG_MII) {
		slogf(_SLOGC_NETWORK, _SLOG_INFO, "%s:%d phyaddr=%d linkstate=%d devidx=%d",
				__FUNCTION__, __LINE__, phyaddr, linkstate, cfg->device_index);
	}
	
	switch (linkstate) {
		case	MDI_LINK_UP:
		if ((i = MDI_GetActiveMedia (ti814x->mdi, cfg->phy_addr, &mode)) != MDI_LINK_UP) {
			if (cfg->verbose & DEBUG_MII)
				slogf(_SLOGC_NETWORK, _SLOG_INFO, "MDI_GetActiveMedia returned %x phy_addr: %d", i, cfg->phy_addr);
			mode = 0;
		}   

		switch (mode) {
			case	MDI_10bTFD:
				cfg->media_rate = 10000L;
				cfg->duplex = 1;
				break;
			case	MDI_10bT:
				s = "10 BaseT Half Duplex";
				cfg->duplex = 0;
				cfg->media_rate = 10000L;
				break;
			case	MDI_100bTFD:
				s = "100 BaseT Full Duplex";
				cfg->duplex = 1;
				cfg->media_rate = 100000L;
				break;
			case	MDI_100bT:
				s = "100 BaseT Half Duplex";
				cfg->duplex = 0;
				cfg->media_rate = 100000L;
				break;
			case	MDI_100bT4:
				s = "100 BaseT4";
				cfg->duplex = 0;
				cfg->media_rate = 100000L;
				break;
			case MDI_1000bT:
				s = "1000 BaseT Half Duplex !!!NOT SUPPORTED!!!";
				cfg->duplex = 0;
				cfg->media_rate = 1000 * 1000L;
				gig_en=1;
				break;
			case MDI_1000bTFD:
				s = "1000 BaseT Full Duplex";
				cfg->duplex = 1;
				cfg->media_rate = 1000 * 1000L;
				gig_en=1;
				break;
			default:
				s = "Unknown";
				cfg->duplex = 0;
				cfg->media_rate = 0L;
				break;
			}
		if (cfg->verbose & DEBUG_MASK) {
			slogf (_SLOGC_NETWORK, _SLOG_INFO, "%s:%d - Link up (%s) if=%d", __FUNCTION__, __LINE__, s, cfg->device_index);
		}

		/* Set Duplex on MAC */
		switch (ti814x->cfg.device_index) {
			case 0: mac_control_reg = ti814x->cpsw_regs + SL1_MAC_CONTROL; break;
			case 1: mac_control_reg = ti814x->cpsw_regs + SL2_MAC_CONTROL; break;
			default: 
				slogf (_SLOGC_NETWORK, _SLOG_INFO, "%s:%d - Invalid device index (%d)",
								__FUNCTION__, __LINE__, ti814x->cfg.device_index );
				return;
		}
		reg = inle32 (mac_control_reg);
		reg &= ~(FULLDUPLEX | EXT_EN | GIG_MODE);

		if (cfg->duplex)
			reg |= FULLDUPLEX;
		else
			reg &= ~FULLDUPLEX;

		if (cfg->media_rate == 100000L)
			reg |= MACIFCTL_A;
		else
			reg &= ~MACIFCTL_A;

		if (cfg->media_rate == 10000L)
			reg |= EXT_EN;

		if (gig_en)
#ifndef	J6
			reg |= (GIG_MODE | EXT_EN);
#else
			reg |= (GIG_MODE);
#endif
		
		if (ti814x->flow == -1) {
		    /* Flow control was autoneg'd, set what we got in the MAC */
		    advert = ti814x_mdi_read(ti814x, cfg->phy_addr, MDI_ANAR);
		    lpadvert = ti814x_mdi_read(ti814x, cfg->phy_addr, MDI_ANLPAR);
		    if (advert & MDI_FLOW) {
			if (lpadvert & MDI_FLOW) {
			    /* Enable Tx and Rx of Pause */
			    ti814x->flow_status = IFM_ETH_RXPAUSE |
						  IFM_ETH_TXPAUSE;
			} else if ((advert & MDI_FLOW_ASYM) &&
				   (lpadvert & MDI_FLOW_ASYM)) {
			    /* Enable Rx of Pause */
			    ti814x->flow_status = IFM_ETH_RXPAUSE;
			} else {
			    /* Disable all pause */
			    ti814x->flow_status = 0;
			}
		    } else if ((advert & MDI_FLOW_ASYM) &&
			       (lpadvert & MDI_FLOW) &&
			       (lpadvert & MDI_FLOW_ASYM)) {
			/* Enable Tx of Pause */
			ti814x->flow_status = IFM_ETH_TXPAUSE;
		    } else {
			/* Disable all pause */
			ti814x->flow_status = 0;
		    }
		} else if (ti814x->flow == IFM_FLOW) {
		    ti814x->flow_status = IFM_ETH_RXPAUSE | IFM_ETH_TXPAUSE;
		} else {
		    ti814x->flow_status = ti814x->flow;

		}
		reg &= ~(RX_FLOW_EN | TX_FLOW_EN);
		if (ti814x->flow_status & IFM_ETH_RXPAUSE) {
		    reg |= TX_FLOW_EN;
		}
		if (ti814x->flow_status & IFM_ETH_TXPAUSE) {
		    reg |= RX_FLOW_EN;
		}

		outle32 (mac_control_reg, reg);

		/*
		 * Recalculate the send percentage
		 * If link is down then ignore for now.
		 * Send percent will be set on link speed change
		 * when link comes up.
		 */
		port_bw = ti814x->cfg.media_rate / 1000; /* In Mbps */
		if (port_bw) {
		    if (!ti814x->cfg.device_index) {
			percent_reg = P1_SEND_PERCENT;
			q1_send = (in32(ti814x->cpsw_regs + TX_PRI0_RATE +
					(1 * 2 * sizeof(uint32_t)))) >>
					PRI_SEND_SHIFT;
			q2_send = (in32(ti814x->cpsw_regs + TX_PRI0_RATE +
					(2 * 2 * sizeof(uint32_t)))) >>
					PRI_SEND_SHIFT;
			q3_send = (in32(ti814x->cpsw_regs + TX_PRI0_RATE +
					(3 * 2 * sizeof(uint32_t)))) >>
					PRI_SEND_SHIFT;
		    } else {
			percent_reg = P2_SEND_PERCENT;
			q1_send = (in32(ti814x->cpsw_regs + TX_PRI0_RATE +
					(((1 * 2) + 1) * sizeof(uint32_t)))) >>
					PRI_SEND_SHIFT;
			q2_send = (in32(ti814x->cpsw_regs + TX_PRI0_RATE +
					(((2 * 2) + 1) * sizeof(uint32_t)))) >>
					PRI_SEND_SHIFT;
			q3_send = (in32(ti814x->cpsw_regs + TX_PRI0_RATE +
					(((3 * 2) + 1) * sizeof(uint32_t)))) >>
					PRI_SEND_SHIFT;
		    }
		    value = (((q3_send * 100) + port_bw - 1) / port_bw) << 16;
		    value |= (((q2_send * 100) + port_bw - 1) / port_bw) << 8;
		    value |= ((q1_send * 100) + port_bw - 1) / port_bw;
		    out32(ti814x->cpsw_regs + percent_reg, value);
		}

		cfg->flags &= ~NIC_FLAG_LINK_DOWN;
		ti814x->linkup = 1;
		if_link_state_change(ifp, LINK_STATE_UP);
                if (ti814x->tx_q_len[0]) {
		    /*
		     * There were some Tx descriptors in use
		     * when the link went down. Kick the start
		     * to ensure things keep running.
		     */
		    NW_SIGLOCK(&ifp->if_snd_ex, ti814x->iopkt);
		    ti814x_start(ifp);
		}
		break;

	case	MDI_LINK_DOWN:
		cfg->media_rate = cfg->duplex = -1;
		MDI_AutoNegotiate(ti814x->mdi, cfg->phy_addr, NoWait);
		cfg->flags |= NIC_FLAG_LINK_DOWN;
		ti814x->linkup = 0;

		if (cfg->verbose & DEBUG_MASK) {
			slogf(_SLOGC_NETWORK, _SLOG_INFO, "%s:%d: Link down %d",
				  __FUNCTION__, __LINE__, cfg->lan);
		}

		if_link_state_change(ifp, LINK_STATE_DOWN);
		break;

	default:
		slogf (_SLOGC_NETWORK, _SLOG_INFO, "%s:%d: Unknown link state %hhu",
			   __FUNCTION__, __LINE__, linkstate);
		break;
		}
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

void ti814x_MDI_MonitorPhy (void *arg)

{
	ti814x_dev_t	*ti814x = arg;
	struct  ifnet	*ifp;

	ifp = &ti814x->ecom.ec_if;

	/* Reap if not recently done */
	if (!ti814x->tx_reaped) {
	    NW_SIGLOCK(&ifp->if_snd_ex, ti814x->iopkt);
	    ti814x_reap_pkts(ti814x, 0);
	    NW_SIGUNLOCK(&ifp->if_snd_ex, ti814x->iopkt);
	}
	ti814x->tx_reaped = 0;

#ifndef SWITCHMODE
	/*
	 * Only monitor the link if it's the 2nd link - the first uses
	 * interrupts, unless emu_phy is specified or it is an ECO
	 *
	 * And:
	 *   1) the user forces it, or
	 *   2) the link state is unknown, or
	 *   3) there's no traffic.
	 */

	if (
#ifndef J5_ECO
	    (ti814x->cfg.device_index || (ti814x->emu_phy != -1)) &&
#endif
	    (ti814x->probe_phy ||
	     (ti814x->cfg.flags & NIC_FLAG_LINK_DOWN) ||
	     (ti814x->cfg.media_rate <= 0) ||
	     !ti814x->pkts_received)) {
		MDI_MonitorPhy(ti814x->mdi);
	}
	ti814x->pkts_received = 0;
#endif

	if (ti814x->stop_miimon) {
	    callout_stop(&ti814x->mii_callout);
	} else {
	    callout_msec(&ti814x->mii_callout, 3 * 1000,
			 ti814x_MDI_MonitorPhy, ti814x);
	}
}

/*****************************************************************************/
/* MII PHY Interface Routines...                                             */
/*****************************************************************************/
#define NIC_MDI_PRIORITY 10 // mii timer

int ar8031_phy_setup(ti814x_dev_t *ti814x)
{
    nic_config_t        *cfg         = &ti814x->cfg;
    int                 phy_idx     = cfg->phy_addr;
    unsigned short val;

    /* Enable AR8031 to output a 125MHz clk from CLK_25M */
    ti814x_mdi_write(ti814x, phy_idx, 0xd, 0x7);
    ti814x_mdi_write(ti814x, phy_idx, 0xe, 0x8016);
    ti814x_mdi_write(ti814x, phy_idx, 0xd, 0x4007);
    val = ti814x_mdi_read(ti814x, phy_idx, 0xe);

    val &= 0xffe3;
    val |= 0x18;
    ti814x_mdi_write(ti814x, phy_idx, 0xe, val);

    /* Introduce tx clock delay to meet data setup and hold times */
    ti814x_mdi_write(ti814x, phy_idx, 0x1d, 0x5);
    val = ti814x_mdi_read(ti814x, phy_idx, 0x1e);
    val |= 0x0100;
    ti814x_mdi_write(ti814x, phy_idx, 0x1e, val);

    /*
     * Disable SmartEEE
     * The Tx delay can mean late pause and bad timestamps.
     */
    ti814x_mdi_write(ti814x, phy_idx, 0xd, 0x3);
    ti814x_mdi_write(ti814x, phy_idx, 0xe, 0x805d);
    ti814x_mdi_write(ti814x, phy_idx, 0xd, 0x4003);
    val = ti814x_mdi_read(ti814x, phy_idx, 0xe);
    val &= ~(1 << 8);
    ti814x_mdi_write(ti814x, phy_idx, 0xe, val);

    /* As above for EEE (802.3az) */
    ti814x_mdi_write(ti814x, phy_idx, 0xd, 0x7);
    ti814x_mdi_write(ti814x, phy_idx, 0xe, 0x3c);
    ti814x_mdi_write(ti814x, phy_idx, 0xd, 0x4007);
    ti814x_mdi_write(ti814x, phy_idx, 0xd,0);

    return 0;
}

int ti814x_findphy(ti814x_dev_t *ti814x)

{
	int					timeout, i;
	int					phy_found = 0;
	uint16_t			reg, reg1;
	volatile uint32_t	phyid;
	int					devidx;
	nic_config_t		*cfg = &ti814x->cfg;

	outle32 (ti814x->cpsw_regs + MDIOCONTROL, MDIO_ENABLE | MDIO_FAULT | MDIO_FAULTENB | MDIO_CLK_DIVISOR);
	ti814x->phy_init = 1;

	/* Wait for MDIO to be IDLE */
	timeout = 1000;
	while (timeout) {
		if (!(inle32 (ti814x->cpsw_regs + MDIOCONTROL) & MDIO_IDLE))
			break;
		nanospin_ns (1000);
		timeout--;
	}
	if (timeout <= 0) {
		slogf (_SLOGC_NETWORK, _SLOG_DEBUG1, "PHY init timeout");
		return (-1);
	}

	/* Look for an active PHY */
	devidx = cfg->device_index;

	if (ti814x->emu_phy != -1 && ti814x->emu_phy == devidx) {
		emu_phy_init (ti814x);
	}

	/* 4.3 mdio_fs_v1p5p1: Each of the 32 bits of this register (ALIVE) is set if the
	 * most recent access to the PHY with address corresponding to the register
	 * bit number was acknowledged by the PHY, the bit is reset if the PHY
	 * fails to acknowledge the access.
	 */
	/* As such we need to iterate through all 32 possible PHYS, read from them
	 * and only then will the ALIVE bits be representative of what's actually
	 * present on the board */
	for (i = 0; i < 32; i++) {
		ti814x_mdi_read(ti814x, i, MDI_BMSR);
		nanospin_ns (50000);
	}

	phyid = inle32 (ti814x->cpsw_regs + MDIOALIVE);

	if (cfg->verbose & DEBUG_MII) {
		slogf (_SLOGC_NETWORK, _SLOG_DEBUG1, "MDIOALIVE=0x%08x", phyid);
	}
	if (ti814x->emu_phy == -1) {
		if ((phyid) && (phyid != 0xffffffff)) {
			for (i = ti814x->phy_idx; i < 32; i++) {
				if (phyid & (1 << i)) {
					if (!devidx)	/* Phy matched device index */
						break;
					else
						devidx--;   /* decrement phy index for next loop */
				}
			}
			phyid = i;
			cfg->phy_addr = phyid;
			phy_found = 1;
		}
	}
	else {
		phy_found = 1;
		phyid = cfg->phy_addr;
	}
	if (phy_found) {
		if (cfg->verbose & DEBUG_MII) {
			slogf(_SLOGC_NETWORK, _SLOG_ERROR,
				"%s:%d - MII transceiver found at address %d",
				__FUNCTION__, __LINE__, cfg->phy_addr);
		}
	}

	if ((!phy_found) || (phyid==32)) {
		slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d - No PHY found",
			__FUNCTION__, __LINE__);
		return (-1);
	}

	reg = ti814x_mdi_read(ti814x, phyid, MDI_BMSR);
	reg1 = ti814x_mdi_read(ti814x, phyid, MDI_EMSR);
	if (reg == 0xffff)
		return (-1);
	if (reg & BMSR_EXT_STATUS) {
		if (!(reg1 & 0xf000))
			ti814x->no_gig = 1;
	}
	else
		ti814x->no_gig = 1;

	return (EOK);
}	

int ti814x_initphy(ti814x_dev_t *ti814x)

{
	int					an_capable, status, rc, i;
	uint16_t			reg, reg1;
	volatile uint32_t	phyid;
	nic_config_t		*cfg = &ti814x->cfg;

	if (ti814x->mdi) {
		MDI_DeRegister ((mdi_t **)&ti814x->mdi);
	}

	phyid = cfg->phy_addr;

	if ((rc = MDI_Register_Extended (ti814x, ti814x_mdi_write, ti814x_mdi_read,
		ti814x_mdi_callback, (mdi_t **)&ti814x->mdi, NULL,
		0, 0)) != MDI_SUCCESS) {
		slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d - MDI_Register_Extended failed rc=%d",
			__FUNCTION__, __LINE__, rc);
		return(-1);
	}


	cfg->connector = NIC_CONNECTOR_MII;
	an_capable = ti814x_mdi_read(ti814x, cfg->phy_addr, MDI_BMSR) & BMSR_AN_ABILITY;

	if (cfg->verbose & DEBUG_MASK) {
		slogf (_SLOGC_NETWORK, _SLOG_INFO,
			"%s:%d: an_capable 0x%X, force_link %d, media_rate %d, duplex %d",
			__FUNCTION__, __LINE__, an_capable, ti814x->force_link, 
		ti814x->cfg.media_rate, ti814x->cfg.duplex);
	}

	/* Permit the MDIO State machine to monitor the link status */
	if (ti814x->emu_phy == -1) {
		switch (ti814x->cfg.device_index) {
			case 0:
				outle32 (ti814x->cpsw_regs + MDIOUSERPHYSEL0, LINKINT_ENABLE | phyid);
				break;
			case 1:
				outle32 (ti814x->cpsw_regs + MDIOUSERPHYSEL1, LINKINT_ENABLE | phyid);
				break;
			default:
				slogf (_SLOGC_NETWORK, _SLOG_ERROR, "Invalid Device Index");
				break;
		}
	}
	

	if (MDI_InitPhy(ti814x->mdi, phyid) != MDI_SUCCESS) {
		slogf (_SLOGC_NETWORK, _SLOG_ERROR, "InitPhy failed");
		return (-1);
	}

	if (ti814x->force_link == 0) {
		MDI_IsolatePhy(ti814x->mdi, phyid);
		MDI_PowerdownPhy(ti814x->mdi, phyid);
		MDI_DisableMonitor(ti814x->mdi);
	} else {
		MDI_DeIsolatePhy(ti814x->mdi, phyid);
		MDI_PowerupPhy(ti814x->mdi, phyid);
		delay(10);

		if (ti814x->force_link != -1 || !an_capable) {
		reg = ti814x_mdi_read(ti814x, ti814x->cfg.phy_addr, MDI_BMCR);

		reg &= ~(BMCR_RESTART_AN | BMCR_SPEED_100 | BMCR_SPEED_1000 | BMCR_FULL_DUPLEX);
		if (an_capable) {
		   /*
			* If we force the speed, but the link partner
			* is autonegotiating, there is a greater chance
			* that everything will work if we advertise with
			* the speed that we are forcing to.
			*/
			MDI_SetAdvert(ti814x->mdi, ti814x->cfg.phy_addr, ti814x->force_link);

			reg |= BMCR_RESTART_AN | BMCR_AN_ENABLE;

			if (cfg->verbose & DEBUG_MASK) {
				slogf (_SLOGC_NETWORK, _SLOG_INFO,
					"%s:%d: Restricted autonegotiate (%dMbps only)",
					__FUNCTION__, __LINE__, 
					ti814x->cfg.media_rate / 1000);
			}
			ti814x_mdi_write (ti814x, cfg->phy_addr, MDI_BMCR, reg);
			MDI_EnableMonitor (ti814x->mdi, 1);
		} else {
			/* No autoneg available */
			reg &= ~BMCR_AN_ENABLE;

			if (cfg->verbose & DEBUG_MASK) {
				slogf (_SLOGC_NETWORK, _SLOG_INFO,
				"%s:%d: Forcing the link (%dMbps %s Duplex)", __FUNCTION__, __LINE__,
				ti814x->cfg.media_rate/1000, (cfg->duplex > 0) ? "Full" : "Half");
			}

			switch (cfg->media_rate) {
			case (100 * 1000):
				reg |= BMCR_SPEED_100;
				break;
			case (1000 * 1000):
				reg |= BMCR_SPEED_1000;
				break;
			default:
				/* 10Mb is no bits set */
				break;
			}

			if (cfg->duplex) {
				reg |= BMCR_FULL_DUPLEX;
			}

			if (cfg->verbose & DEBUG_MII) {
				slogf (_SLOGC_NETWORK, _SLOG_INFO,
				"%s:%d: writing reg 0x%X to MDI_BMCR", __FUNCTION__, __LINE__, reg);
			}
			ti814x_mdi_write (ti814x, cfg->phy_addr, MDI_BMCR, reg);
			MDI_DisableMonitor (ti814x->mdi);
		}
		} else {  // not forcing the link
		cfg->flags |= NIC_FLAG_LINK_DOWN;

		/* Enable Pause in autoneg */
		MDI_GetMediaCapable(ti814x->mdi, cfg->phy_addr, &i);
		if ((ti814x->mdi->PhyData[cfg->phy_addr]->VendorOUI == KENDIN) &&
			(ti814x->mdi->PhyData[cfg->phy_addr]->Model == KSZ9031)) {	/* Bug in KSZ9031 PHY */
			i |= MDI_FLOW;
		} else {
			i |= MDI_FLOW | MDI_FLOW_ASYM;
		}
		MDI_SetAdvert(ti814x->mdi, cfg->phy_addr, i);
		MDI_AutoNegotiate(ti814x->mdi, cfg->phy_addr, NoWait);

		status = MDI_EnableMonitor(ti814x->mdi, 1);
		if (status != MDI_SUCCESS) {
			slogf (_SLOGC_NETWORK, _SLOG_INFO, "%s:%d: MDI_EnableMonitor returned %d",
			__FUNCTION__, __LINE__, status);
		}
		}
	}

	 if (ti814x->fix_mii_clk) {
		reg = ti814x_mdi_read (ti814x, phyid, MDI_PHYID_1);
		reg1 = ti814x_mdi_read (ti814x, phyid, MDI_PHYID_2);
		if (reg == 0x22 && reg1 == 0x1556) { /* Micrel RMII PHY */
			reg = ti814x_mdi_read (ti814x, phyid, 0x1f);
			reg |= (1<<7);	/* Set 50Mhz clock bit */
			ti814x_mdi_write (ti814x, phyid, 0x1f, reg);
			if (cfg->verbose) {
				reg = ti814x_mdi_read (ti814x, phyid, 0x1f);
				slogf (_SLOGC_NETWORK, _SLOG_DEBUG1, "PHY Reg 0x1f set to - %x", reg);
			}
		}
	}

	if((ti814x->mdi->PhyData[cfg->phy_addr]->VendorOUI == KENDIN) &&
		(ti814x->mdi->PhyData[cfg->phy_addr]->Model == KSZ8051)) {
		ti814x_mdi_write(ti814x, phyid, 0x16, 0x2);
	}

#ifdef AM335X
	if((ti814x->mdi->PhyData[cfg->phy_addr]->VendorOUI == ATHEROS) &&
		(ti814x->mdi->PhyData[cfg->phy_addr]->Model == AR8031)) {

		uint32_t	link_mode;

		slogf (_SLOGC_NETWORK, _SLOG_DEBUG1, "AM335X board with Atheros AR8031 PHY");

		/* Set the AM335x to generate the RMII ref clock. */
		link_mode = inle32(ti814x->cmr_base + GMII_SET);
		link_mode &= ~(GMII0_RMII_CLKIN | GMII1_RMII_CLKIN);
		outle32(ti814x->cmr_base + GMII_SET, link_mode);

		/*
		 * As per TI SPRZ360F errata advisory 1.0.10, the AM335X does not support internal delay mode
		 * so the Atheros phy must be configured to add an internal delay on its tx clk input.
		 */
		ar8031_phy_setup(ti814x);
	}
#endif
	return(EOK);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/io-pkt/sys/dev_qnx/dm814x/mii.c $ $Rev: 758089 $")
#endif
