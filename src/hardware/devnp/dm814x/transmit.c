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

#include    "bpfilter.h"
#include    "ti814x.h"
#include    <avb.h>

#if NBPFILTER > 0
#include <net/bpf.h>
#include <net/bpfdesc.h>
#endif

static char ti814x_zero_pad_buff[ETHER_MIN_LEN - ETHER_CRC_LEN];

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

static struct mbuf *ti814x_defrag (struct mbuf *m)

{
	struct mbuf *m2;

	
	MGET (m2, M_DONTWAIT, MT_DATA);
	if (m2 == NULL) {
		m_freem (m);
		return (NULL);
	}

	M_COPY_PKTHDR (m2, m);

	MCLGET (m2, M_DONTWAIT);
	if ((m2->m_flags & M_EXT) == 0) {
		m_freem (m);
		m_freem (m2);
		return (NULL);
	}

	/* Paranoid ? */
	if (m->m_pkthdr.len > m2->m_ext.ext_size) {
		m_freem (m);
		m_freem (m2);
		return (NULL);
	}

	m_copydata (m, 0, m->m_pkthdr.len, mtod(m2, caddr_t));
	m2->m_pkthdr.len = m2->m_len = m->m_pkthdr.len;

	m_freem(m);
	
	return (m2);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

void ti814x_reap_pkts(ti814x_dev_t *ti814x, uint32_t queue)

{
	uint32_t		idx, offset;
	struct mbuf		*m;
	cppi_desc_t		*desc;

	if (!queue) {
	    ti814x->tx_reaped = 1;
	}

	offset = queue * 2 * NUM_TX_PKTS;

	while (ti814x->tx_q_len[queue]) {
		idx = ti814x->tx_cidx[queue];
		desc = &ti814x->meminfo.tx_desc[idx + offset];

		/* Don't free memory owned by DMA */
		if (desc->flag_len & DESC_FLAG_OWN) {
		    break;
		}

		if ((m = ti814x->tx_mbuf[idx + offset]) != NULL) {
			m_freem (m);
			ti814x->tx_mbuf[idx + offset] = NULL;
		}
		ti814x->tx_q_len[queue]--;
		ti814x->tx_cidx[queue] = (ti814x->tx_cidx[queue] + 1) %
						NUM_TX_PKTS;
	}
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int ti814x_tx (ti814x_dev_t *ti814x, struct mbuf *m, uint8_t queue)
{
    struct mbuf		*m2;
    off64_t		phys;
    int			idx, idx_start;
    int			first = 1;
    int			num_frags;
    cppi_desc_t		*desc;
    int			len, next_idx;
    uint32_t		start_phys, hdp_idx, offset;
    uint32_t		devidx = ti814x->cfg.device_index;
    nic_stats_t		*stats = &ti814x->stats;
    uint8_t		*dptr;
    struct ifnet	*ifp = &ti814x->ecom.ec_if;

    if (!devidx) {
	hdp_idx = TX0_HDP + (sizeof(uint32_t) * queue * 2);
    } else {
	hdp_idx = TX1_HDP + (sizeof(uint32_t) * queue * 2);
    }

    /* Pad if necessary, hardware adds CRC */
    if (m->m_pkthdr.len < (ETHER_MIN_LEN - ETHER_CRC_LEN)) {
	m_copyback(m, m->m_pkthdr.len,
		   (ETHER_MIN_LEN - ETHER_CRC_LEN) - m->m_pkthdr.len,
		   ti814x_zero_pad_buff);
    }

    /* Count the number of fragments and total packet length*/
    for (num_frags = 0, m2 = m; m2; num_frags++) {
	m2 = m2->m_next;
    }

    /* If the packet is massively fragmented, defrag it */
    if (num_frags > DEFRAG_LIMIT) {
	if ((m2 = ti814x_defrag (m)) == NULL) {
	    ti814x->stats.tx_failed_allocs++;
	    m_free(m);
	    return E2BIG;
	}

	/* Re-adjust the number of fragments and total length */
	m = m2;
	for (num_frags = 0, m2 = m; m2; num_frags++) {
	    m2 = m2->m_next;
	}
    }

    /* Initialize descriptor tracking variables */
    idx_start = idx = ti814x->tx_pidx[queue];
    first = 1;
    offset = queue * 2 * NUM_TX_PKTS;
    desc = &ti814x->meminfo.tx_desc[idx_start + offset];
    ti814x->tx_mbuf[idx + offset] = m;

    desc->flag_len = desc->off_len = desc->buffer = desc->next = 0;
    start_phys = ti814x->meminfo.tx_phys +
		(sizeof (cppi_desc_t) * (idx + offset));
    m->m_nextpkt = NULL;

    /* Set up the CPPI Descriptors. One loop iteration for every fragment. */
    for (m2 = m; m2; m2 = m2->m_next) {
	/* Skip 0 length fragments */
	if (!m2->m_len) {
	    num_frags--;
	    continue;
	}
	phys = mbuf_phys (m2);
	CACHE_FLUSH (&ti814x->meminfo.cachectl, m2->m_data, phys, m2->m_len);
	/* Build the descriptor chain if there is more than one fragment */
	if (num_frags > 1) {
	    next_idx = (idx + 1) % NUM_TX_PKTS;
	    desc->next = ti814x->meminfo.tx_phys +
			(sizeof (cppi_desc_t) * (next_idx + offset));
	} else {
	    desc->next = 0;
	}

	/* Set the buffer pointer to the actual physical memory*/
	desc->buffer = (unsigned int) phys;

	len = m2->m_len & 0x1fff;
	stats->octets_txed_ok += len;
	desc->off_len = len & 0x0000ffff;

	/* packet length is only valid on the start-of-packet descriptor */
	if (first) {
#ifndef SWITCHMODE
	    desc->flag_len = m2->m_pkthdr.len | DESC_FLAG_OWN | DESC_FLAG_SOP |
				DESC_FLAG_TOPORT_EN | (((devidx+1)&0x3) << 16);
#else
	    desc->flag_len = m2->m_pkthdr.len | DESC_FLAG_OWN | DESC_FLAG_SOP;
#endif
	    /* Do stats */
	    dptr = mtod (m2, uint8_t *);
	    if (dptr[0] & 1) {
		if (IS_BROADCAST (dptr)) {
		    stats->txed_broadcast++;
		} else {
		    stats->txed_multicast++;
		}
	    }
	    ifp->if_opackets++;
	    stats->txed_ok++;

	    first = 0;
	} else {
	    /* Only valid on SOP */
	    desc->flag_len = 0;
	}

	ti814x->tx_q_len[queue]++;
	idx = (idx + 1) % NUM_TX_PKTS;
	desc = &ti814x->meminfo.tx_desc[idx + offset];

    }   /* end for loop - fragment iteration */

    /* Mark the last descrptor as end-of-packet and NULL terminate the descriptor chain */
    ti814x->tx_pidx[queue] = idx;
    idx -= 1;
    if (idx < 0) {
	idx = NUM_TX_PKTS - 1;
    }
    desc = &ti814x->meminfo.tx_desc[idx + offset];
    desc->flag_len |= DESC_FLAG_EOP;
    desc->next = NULL;

    /* Append this chain to the current set */
    idx = idx_start - 1;
    if (idx < 0) {
	idx = NUM_TX_PKTS - 1;
    }
    desc = &ti814x->meminfo.tx_desc[idx + offset];
    desc->next = ti814x->meminfo.tx_phys +
		(sizeof (cppi_desc_t) * (idx_start + offset));

    /* If previous packet is done, kick the DMA */
    if ((in32(ti814x->cpsw_regs + hdp_idx) == 0) &&
	(desc->flag_len & DESC_FLAG_EOQ)) {
	out32(ti814x->cpsw_regs + hdp_idx, start_phys);
    }

#if NBPFILTER > 0
    /* Pass the packet to any BPF listeners */
    if (ifp->if_bpf) {
	bpf_mtap (ifp->if_bpf, m);
    }
#endif
    return 0;
}

void ti814x_start (struct ifnet *ifp)

{
    ti814x_dev_t		*ti814x = ifp->if_softc;
    struct _iopkt_self		*iopkt = ti814x->iopkt;
    struct nw_work_thread	*wtp = WTP;
    uint32_t			num_free;
    struct mbuf			*m;

    if ((ifp->if_flags_tx & IFF_RUNNING) == 0) {
	NW_SIGUNLOCK_P (&ifp->if_snd_ex, ti814x->iopkt, wtp);
	return;
    }

#ifndef SWITCHMODE
    if (!ti814x->linkup) {
	NW_SIGUNLOCK_P (&ifp->if_snd_ex, ti814x->iopkt, wtp);
	return;
    }
#endif

    ifp->if_flags_tx |= IFF_OACTIVE;

    while (1) {
	num_free = NUM_TX_PKTS - ti814x->tx_q_len[0];
	if (num_free <= DEFRAG_LIMIT) {
	    ti814x_reap_pkts(ti814x, 0);

	    num_free = NUM_TX_PKTS - ti814x->tx_q_len[0];
	    if (num_free <= DEFRAG_LIMIT) {
		/* Leave IFF_OACTIVE so the stack doesn't call us again */
		NW_SIGUNLOCK_P (&ifp->if_snd_ex, ti814x->iopkt, wtp);
		return;
	    }
	}

	/* Grab an outbound packet/mbuf chain */
	IFQ_DEQUEUE (&ifp->if_snd, m);
	/* If none are available break out of the loop */
	if (m == NULL) {
	    break;
	}
	ti814x_tx(ti814x, m, 0);

    } /* end while(1) */

    ifp->if_flags_tx &= ~IFF_OACTIVE;
    NW_SIGUNLOCK_P (&ifp->if_snd_ex, iopkt, wtp);
}

int ti814x_output (struct ifnet *ifp, struct mbuf *m,
		   struct sockaddr *dst, struct rtentry *rt)
{
    ti814x_dev_t		*ti814x = ifp->if_softc;
    struct m_tag		*tag;
    uint8_t			priority, queue = 0;
    uint32_t			num_free;
    int				error;

    tag = GET_TXQ_TAG(m);
    if (tag == NULL) {
	priority = 0;
    } else {
	priority = EXTRACT_TXQ_TAG(tag);
    }

    /* Map the eight priorities to the 4 queues */
    switch (priority) {
    case 0:
    case 1:
	/* Do a normal if_output via the stack, will be on TXQ 0 */
      return ti814x->stack_output(ifp, m, dst,rt);
      break;

    case 2:
        /* Send the mbuf on TXQ 1 */
	queue = 1;
	break;

    case 3:
	/* Send the mbuf on TXQ 2 */
	queue = 2;
	break;

    default:
	/* Send the mbuf on TXQ 3 */
	queue = 3;
    }

    NW_SIGLOCK_P(&ti814x->ecom.ec_if.if_snd_ex, ti814x->iopkt, WTP);

    /* Check for space */
    num_free = NUM_TX_PKTS - ti814x->tx_q_len[queue];
    if (num_free <= DEFRAG_LIMIT) {
	ti814x_reap_pkts(ti814x, queue);

	num_free = NUM_TX_PKTS - ti814x->tx_q_len[queue];
	if (num_free <= DEFRAG_LIMIT) {
	    m_freem(m);
	    NW_SIGUNLOCK_P(&ti814x->ecom.ec_if.if_snd_ex, ti814x->iopkt, WTP);
	    return ENOBUFS;
	}
    }

    /*
     * The packet already has an ether header on it but is moved down to let
     * ether_output() do the header as well. As this is going direct, just
     * restore the current header.
     */
    m->m_data -= sizeof(struct ether_header);
    m->m_len += sizeof(struct ether_header);
    m->m_pkthdr.len += sizeof(struct ether_header);

    error = ti814x_tx(ti814x, m, queue);
    NW_SIGUNLOCK_P(&ti814x->ecom.ec_if.if_snd_ex, ti814x->iopkt, WTP);
    return error;
}

int ti814x_set_tx_bw (ti814x_dev_t *ti814x, struct ifdrv *ifd)
{
    avb_bw_t *avb_bw;
    uint32_t q1_send, q1_idle, q1_shape;
    uint32_t q2_send, q2_idle, q2_shape;
    uint32_t q3_send, q3_idle, q3_shape;
    uint32_t devidx = ti814x->cfg.device_index;
    uint32_t port_bw, percent_reg, value;

    avb_bw = (avb_bw_t*)&ifd->ifd_data;

    /* Priority 0 & 1 is queue 0 with no shaping so ignore them */

    /* Priority 2 is queue 1 */
    q1_send = (avb_bw->bandwidth[2] + 1023) / 1024; /* kbps to Mbps round up */
    q1_idle = (32 * 125) - q1_send; /* spugz8c.pdf 9.2.1.1.4 */
    q1_shape = (q1_send << PRI_SEND_SHIFT) | q1_idle;
    out32(ti814x->cpsw_regs + TX_PRI0_RATE +
	  (((1 * 2) + devidx) * sizeof(uint32_t)), q1_shape);

    /* Priority 3 is queue 2 */
    q2_send = (avb_bw->bandwidth[3] + 1023) / 1024;
    q2_idle = (32 * 125) - q2_send;
    q2_shape = (q2_send << PRI_SEND_SHIFT) | q2_idle;
    out32(ti814x->cpsw_regs + TX_PRI0_RATE +
	  (((2 * 2) + devidx) * sizeof(uint32_t)), q2_shape);

    /* Priority 4+ is queue 3 */
    q3_send = (avb_bw->bandwidth[4] + avb_bw->bandwidth[5] +
	       avb_bw->bandwidth[6] +avb_bw->bandwidth[7] + 1023) / 1024;
    q3_idle = (32 * 125) - q3_send;
    q3_shape = (q3_send << PRI_SEND_SHIFT) | q3_idle;
    out32(ti814x->cpsw_regs + TX_PRI0_RATE +
	  (((3 * 2) + devidx) * sizeof(uint32_t)), q3_shape);

#ifndef SWITCHMODE
    port_bw = ti814x->cfg.media_rate / 1000; /* In Mbps */
    /*
     * If link is down then ignore for now.
     * Send percent will be set on link speed change when link comes up.
     */
    if (port_bw) {
	if (!devidx) {
	    percent_reg = P1_SEND_PERCENT;
	} else {
	    percent_reg = P2_SEND_PERCENT;
	}
	value = (((q3_send * 100) + port_bw - 1) / port_bw) << 16;
	value |= (((q2_send * 100) + port_bw - 1) / port_bw) << 8;
	value |= ((q1_send * 100) + port_bw - 1) / port_bw;
	out32(ti814x->cpsw_regs + percent_reg, value);
    }
#else
    port_bw = ti814x->speed & 0xFFFF;
    percent_reg = P1_SEND_PERCENT;
    value = (((q3_send * 100) + port_bw - 1) / port_bw) << 16;
    value |= (((q2_send * 100) + port_bw - 1) / port_bw) << 8;
    value |= ((q1_send * 100) + port_bw - 1) / port_bw;
    out32(ti814x->cpsw_regs + percent_reg, value);

    port_bw = ti814x->speed >> 16;
    percent_reg = P2_SEND_PERCENT;
    value = (((q3_send * 100) + port_bw - 1) / port_bw) << 16;
    value |= (((q2_send * 100) + port_bw - 1) / port_bw) << 8;
    value |= ((q1_send * 100) + port_bw - 1) / port_bw;
    out32(ti814x->cpsw_regs + percent_reg, value);
#endif

    /*
     * If this is the first set bandwidth then we need to enable all
     * the shaping setup.
     * N.B. We need to setup shaping on both ports as they are interleaved
     * in the TX priority. So if the customer runs 2 ports then both need
     * set bandwidth calls.
     */
    value = in32(ti814x->cpsw_regs + CPSW_PTYPE);
    if (!value) {
	outle32(ti814x->cpsw_regs + P1_TX_IN_CTL, HOST_BLKS_REM | TX_RATE_EN |
		RATE_LIM_MODE | TX_BLKS_REM | TX_PRI_WDS);
	outle32(ti814x->cpsw_regs + P2_TX_IN_CTL, HOST_BLKS_REM | TX_RATE_EN |
		RATE_LIM_MODE | TX_BLKS_REM | TX_PRI_WDS);
	outle32(ti814x->cpsw_regs + CPSW_PTYPE,
		P2_PRI3_SHAPE_EN | P2_PRI2_SHAPE_EN | P2_PRI1_SHAPE_EN |
		P1_PRI3_SHAPE_EN | P1_PRI2_SHAPE_EN | P1_PRI1_SHAPE_EN);
	outle32(ti814x->cpsw_regs + DMA_CONTROL, TX_RLIM | TX_PTYPE);
    }

    return EOK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/io-pkt/sys/dev_qnx/dm814x/transmit.c $ $Rev: 744998 $")
#endif
