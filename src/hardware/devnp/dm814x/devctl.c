/*
 * $QNXLicenseC:
 * Copyright 2009, QNX Software Systems. 
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

#include    <ti814x.h>
#include    <net/ifdrvcom.h>
#include    <sys/sockio.h>
#include    <avb.h>
#include    <netdrvr/ptp.h>

extern	ti814x_dev_t	*ti_dev [2];

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int		ti814x_set_stats (ti814x_dev_t *ti814x, struct ifdrv *ifd)

{
ti814x_dev_t	*ti814n = NULL;
int				i;
uintptr_t		regs = ti814x->cpsw_regs;

	if (ifd == NULL)
		return (EINVAL);
	if (ti814x == ti_dev[0])
		ti814n = ti_dev[1];
	else
		ti814n = ti_dev[0];
	ti814x->get_stats = 1;
	if (ti814x->cfg.device_index == 0) {
		out32 (regs + CPSW_STAT_PORT_EN, P1_STAT_EN);
		}
	else {
		out32 (regs + CPSW_STAT_PORT_EN, P2_STAT_EN);
		}
	memset ((char *) &ti814x->stats.un.estats, 0, sizeof (struct _nic_ethernet_stats));
	ti814x->stats.un.estats.valid_stats = VALID_STATS;
	for (i = RXGOODFRAMES; i <= RXDMAOVERRUNS; i += 4) {
		if ((i == MISSING_REG1) || (i == MISSING_REG2))	/* Missing registers */
			continue;
		out32 (regs + i, 0xffffffff);	/* Clear out all registers */
		}
	if (ti814n != NULL) {
		ti814n->get_stats = 0;
		memset ((char *) &ti814n->stats.un.estats, 0, sizeof (struct _nic_ethernet_stats));
		}
	return (0);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

void	ti814x_read_stats (ti814x_dev_t *ti814x)

{
nic_ethernet_stats_t	*estats = &ti814x->stats.un.estats;
uintptr_t				regs = ti814x->cpsw_regs;
int						i;
uint32_t				stat;

	if (!ti814x->get_stats)
		return;
	for (i = RXGOODFRAMES; i <= RXDMAOVERRUNS; i += 4) {
		if ((i == MISSING_REG1) || (i == MISSING_REG2))	/* Missing registers */
			continue;
		stat = in32 (regs + i);
		if (!stat)
			continue;
		out32 (regs + i, stat);
		switch (i) {
			case	RXGOODFRAMES:
			case	RXBROADCASTFRAMES:
			case	RXMULTICASTFRAMES:
			case	RXOCTETS:
			case	TXGOODFRAMES:
			case	TXBROADCASTFRAMES:
			case	TXMULTICASTFRAMES:
			case	TXOCTETS:
				break;			/* We already have these statistics */
			case	RXPAUSEFRAMES:
			case	OCTETFRAMES64:
			case	OCTETFRAMES65TO127:
			case	OCTETFRAMES128TO255:
			case	OCTETFRAMES256TO511:
			case	OCTETFRAMES512TO1023:
			case	OCTETFRAMES1024TUP:
			case	TXPAUSEFRAMES:
			case	NETOCTETS:
				break;			/* We don't report these statistics */
			case	RXCRCERRORS:
				estats->fcs_errors += stat;
				break;
			case	RXALIGNCODEERRORS:
				estats->align_errors += stat;
				break;
			case	RXOVERSIZEDFRAMES:
				estats->oversized_packets += stat;
				break;
			case	RXJABBERFRAMES:
				estats->jabber_detected += stat;
				break;
			case	RXUNDERSIZEDFRAMES:
			case	RXFRAGMENTS:
				estats->short_packets += stat;
				break;
			case	TXDEFERREDFRAMES:
				estats->tx_deferred += stat;
				break;
			case	TXCOLLISIONFRAMES:
			case	TXSINGLECOLLFRAMES:
				estats->single_collisions += stat;
				break;
			case	TXMULTCOLLFRAMES:
				estats->multi_collisions += stat;
				break;
			case	TXEXCESSIVECOLLISIONS:
				estats->xcoll_aborted += stat;
				break;
			case	TXLATECOLLISION:
				estats->late_collisions += stat;
				break;
			case	TXUNDERRUN:
				estats->internal_tx_errors += stat;
				break;
			case	TXCARRIERSENSEERRORS:
				estats->no_carrier += stat;
				break;
			case	RXSOFOVERRUNS:
			case	RXMOFOVERRUNS:
			case	RXDMAOVERRUNS:
				estats->internal_rx_errors += stat;
				break;
			default:
				break;
			}
		}
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

void ti814x_filter(ti814x_dev_t *ti814x)

{
    struct ethercom		*ec;
    struct ether_multi	*enm;
    struct ether_multistep	step;
    struct ifnet		*ifp;
    int			mask, loop;
    uint16_t		vlan;

    ec = &ti814x->ecom;
    ifp = &ec->ec_if;

#ifndef SWITCHMODE
    if (ti814x->cfg.device_index == 0) {
	mask = PORT0 | PORT1;
	vlan = PORT1_VLAN;
    } else {
	mask = PORT0 | PORT2;
	vlan = PORT2_VLAN;
    }
#else
    mask = PORT0 | PORT1 | PORT2;
    vlan = 0;
#endif

    ifp->if_flags &= ~IFF_ALLMULTI;
    ti814x_ale_flood_unreg_mcast(ti814x->cpsw_regs, vlan, 0);
    if (ti814x->join_vlan != NULL) {
	loop = 0;
	while (ti814x->join_vlan[loop] != 0) {
	    ti814x_ale_flood_unreg_mcast(ti814x->cpsw_regs,
					 ti814x->join_vlan[loop], 0);
	    loop++;
	}
    }

    ETHER_FIRST_MULTI (step, ec, enm);
    while (enm != NULL) {
	if (memcmp (enm->enm_addrlo, enm->enm_addrhi, ETHER_ADDR_LEN) != 0) {
	    ifp->if_flags |= IFF_ALLMULTI;
	    slogf (_SLOGC_NETWORK, _SLOG_ERROR,
		   "%s: IFF_ALLMULTI", __FUNCTION__);
	    /*
	     * Change port vlan entry to flood all multicast
	     * Del all programmed mcast for this port
	     */
	    ti814x_ale_flood_unreg_mcast(ti814x->cpsw_regs, vlan, 1);
	    ti814x_ale_del_all_vlan_mcast(ti814x->cpsw_regs, vlan, ec);
	    if (ti814x->join_vlan != NULL) {
		loop = 0;
		while (ti814x->join_vlan[loop] != 0) {
		    ti814x_ale_flood_unreg_mcast(ti814x->cpsw_regs,
						 ti814x->join_vlan[loop], 1);
		    ti814x_ale_del_all_vlan_mcast(ti814x->cpsw_regs,
						  ti814x->join_vlan[loop], ec);
		    loop++;
		}
	    }
	    break;
	}

	ti814x_ale_add_vlan_mcast(ti814x->cpsw_regs, enm->enm_addrlo,
				  mask, vlan);
	if (ti814x->join_vlan != NULL) {
	    loop = 0;
	    while (ti814x->join_vlan[loop] != 0) {
		ti814x_ale_add_vlan_mcast(ti814x->cpsw_regs, enm->enm_addrlo,
					  mask, ti814x->join_vlan[loop]);
		loop++;
	    }
	}
	ETHER_NEXT_MULTI (step, enm);
    }

    /*
     * Clear out stale entries
     */
    ti814x_ale_check_vlan_mcast(ti814x->cpsw_regs, vlan, ec);
    if (ti814x->join_vlan != NULL) {
	loop = 0;
	while (ti814x->join_vlan[loop] != 0) {
	    ti814x_ale_check_vlan_mcast(ti814x->cpsw_regs,
					ti814x->join_vlan[loop], ec);
	    loop++;
	}
    }
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int ti814x_ioctl(struct ifnet * ifp, unsigned long cmd, caddr_t data)

{
	int						error = 0;
	ti814x_dev_t			*ti814x = ifp->if_softc;
	struct drvcom_config	*dcfgp;
	struct drvcom_stats		*dstp;
	struct ifdrv_com		*ifdc;
	struct ifdrv			*ifd;

	switch (cmd) {
		case	SIOCGDRVCOM:
			ifdc = (struct ifdrv_com *)data;
			switch (ifdc->ifdc_cmd) {
				case	DRVCOM_CONFIG:
					dcfgp = (struct drvcom_config *)ifdc;

					if (ifdc->ifdc_len != sizeof(nic_config_t)) {
						error = EINVAL;
						break;
					}
					memcpy(&dcfgp->dcom_config, &ti814x->cfg, sizeof(ti814x->cfg));
					break;

				case	DRVCOM_STATS:
					dstp = (struct drvcom_stats *)ifdc;

					if (ifdc->ifdc_len != sizeof(nic_stats_t)) {
						error = EINVAL;
						break;
						}
					ti814x_read_stats (ti814x);
					memcpy(&dstp->dcom_stats, &ti814x->stats, sizeof(ti814x->stats));
					break;

				default:
					error = ENOTTY;
				}
			break;

		case	SIOCSIFMEDIA:
		case	SIOCGIFMEDIA: {
#ifndef SWITCHMODE
			struct ifreq *ifr = (struct ifreq *)data;

			error = ifmedia_ioctl(ifp, ifr, &ti814x->bsd_mii.mii_media, cmd);
#else
			error = EOPNOTSUPP;
#endif
			break;
			}

		case SIOCSDRVSPEC:
		case SIOCGDRVSPEC:
		    ifd = (struct ifdrv *)data;
		    switch (ifd->ifd_cmd) {
#if	!defined(AM335X)
			    case PTP_GET_TX_TIMESTAMP:
			    case PTP_GET_RX_TIMESTAMP:
			    case PTP_GET_TIME:
			    case PTP_SET_TIME:
			    case PTP_SET_COMPENSATION:
					error = ti814x_ptp_ioctl(ti814x, ifd);
					break;
#endif
#if	!defined(AM335X) && !defined(J6)
			    case AVB_SET_BW:
					error = ti814x_set_tx_bw(ti814x, ifd);
					break;
			    case PRECISE_TIMER_DELAY:
					error = ti814x_timer_delay(ti814x, ifd);
					break;
				case READ_PHY_REG:
				case WRITE_PHY_REG:
					error = ti814x_phy_funcs (ti814x, ifd);
					break;
#endif
			    case IOCTL_STAT:
					error = ti814x_set_stats (ti814x, ifd);
					break;

			    default:
					error = ENOTTY;
					break;
			    }
		    break;

		case SIOCSIFFLAGS:
			if (ti814x->ecom.ec_capenable & ETHERCAP_VLAN_MTU) {
#ifndef SWITCHMODE
				if (ti814x->cfg.device_index == 0) {
					out32(ti814x->cpsw_regs + SL1_RX_MAXLEN,
						 ifp->if_mtu + ETHER_HDR_LEN +
						 ETHER_CRC_LEN +
						 ETHER_VLAN_ENCAP_LEN);
				} else {
					out32(ti814x->cpsw_regs + SL2_RX_MAXLEN,
						 ifp->if_mtu + ETHER_HDR_LEN +
						 ETHER_CRC_LEN +
						 ETHER_VLAN_ENCAP_LEN);
				}
#else
				out32(ti814x->cpsw_regs + SL1_RX_MAXLEN,
				      ifp->if_mtu + ETHER_HDR_LEN +
				      ETHER_CRC_LEN + ETHER_VLAN_ENCAP_LEN);
				out32(ti814x->cpsw_regs + SL2_RX_MAXLEN,
				      ifp->if_mtu + ETHER_HDR_LEN +
				      ETHER_CRC_LEN + ETHER_VLAN_ENCAP_LEN);
#endif
			} else {
#ifndef SWITCHMODE
				if (ti814x->cfg.device_index == 0) {
					out32(ti814x->cpsw_regs + SL1_RX_MAXLEN,
					      ifp->if_mtu + ETHER_HDR_LEN +
					      ETHER_CRC_LEN);
				} else {
					out32(ti814x->cpsw_regs + SL2_RX_MAXLEN,
					      ifp->if_mtu + ETHER_HDR_LEN +
					      ETHER_CRC_LEN);
				}
#else
				out32(ti814x->cpsw_regs + SL1_RX_MAXLEN,
				      ifp->if_mtu + ETHER_HDR_LEN +
				      ETHER_CRC_LEN);
				out32(ti814x->cpsw_regs + SL2_RX_MAXLEN,
				      ifp->if_mtu + ETHER_HDR_LEN +
				      ETHER_CRC_LEN);
#endif
			}
			/*
			 * Deliberate fallthrough for other flags
			 * handling in ether_ioctl.
			 */
		default:
			error = ether_ioctl(ifp, cmd, data);
			if (error == ENETRESET) {
				/*
				* Multicast list has changed; set the
				* hardware filter accordingly.
				*/
				if ((ifp->if_flags_tx & IFF_RUNNING) == 0) {
					/* Interface is currently down */
				} else {
					ti814x_filter(ti814x);
				}
				error = 0;
			}
			break;
		}
	return error;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/io-pkt/sys/dev_qnx/dm814x/devctl.c $ $Rev: 757246 $")
#endif
