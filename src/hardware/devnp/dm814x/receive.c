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

#include <net/if_vlanvar.h>
#include <net/netbyte.h>
#include <avb.h>
#include "bpfilter.h"
#include "ti814x.h"

#if NBPFILTER > 0
#include <net/bpf.h>
#include <net/bpfdesc.h>
#endif

extern	ti814x_dev_t	*ti_dev [2];

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

static  int ti814x_add_rx_desc(attach_args_t *attach_args, int idx,
			       struct mbuf *new)

{
	off64_t		 phys;

	attach_args->meminfo.rx_mbuf[idx] = new;
	phys = mbuf_phys(new);
	CACHE_INVAL (&attach_args->meminfo.cachectl, new->m_data, phys, new->m_ext.ext_size);

	attach_args->meminfo.rx_desc[idx].buffer = (unsigned int) phys;
	attach_args->meminfo.rx_desc[idx].off_len = MAX_PKT_SIZE;
	attach_args->meminfo.rx_desc[idx].flag_len = DESC_FLAG_OWN;

	return (EOK);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int ti814x_receive(attach_args_t *attach_args, struct nw_work_thread *wtp,
		   uint32_t chan)

{
	struct mbuf			*new, *m;
	int				pkt_len;
	struct ifnet			*ifp;
	int				cidx, idx, nidx, eidx;
	uint32_t			offset, status = 0;
	uint32_t			verbose = attach_args->cfg.verbose;
	nic_stats_t			*stats;
	uint8_t				*dptr, eoq = 0;
	struct ether_vlan_header	*vlan_hdr;
	const struct sigevent		*evp;

	eidx = -1;
	offset = chan * NUM_RX_PKTS;

	while (1) {
		cidx = attach_args->rx_cidx[chan];
		status = attach_args->meminfo.rx_desc[cidx + offset].flag_len;
#ifndef SWITCHMODE
		idx = ((status >> 16) & 3) - 1;
#else
		idx = 0;
#endif
		stats = &ti_dev[idx]->stats;
		ifp = &ti_dev[idx]->ecom.ec_if;

		if (status & DESC_FLAG_EOQ) {
			eoq = 1;
		}

		/* Descriptor is still owned by the port... */
		if (status & DESC_FLAG_OWN) {
			break;
		}

		pkt_len = status & 0xffff;
		m = attach_args->meminfo.rx_mbuf[cidx + offset];
		stats->octets_rxed_ok += pkt_len;
		stats->rxed_ok++;
		dptr = mtod (m, uint8_t *);
		if (dptr[0] & 1) {
			if (IS_BROADCAST (dptr))
				stats->rxed_broadcast++;
			else
				stats->rxed_multicast++;
			}

		/* advance consumer index for the next loop */
		attach_args->rx_cidx[chan] = (cidx + 1) % NUM_RX_PKTS;

		/* Get a packet/buffer to replace the one that was filled */
		new = m_getcl_wtp (M_DONTWAIT, MT_DATA, M_PKTHDR, wtp);
		if (new == NULL) {
			if (verbose & DEBUG_MASK) {
				slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d m_getcl_wtp returned NULL",  __FUNCTION__, __LINE__);
			}
			ti814x_add_rx_desc(attach_args, cidx + offset, m);
			ifp->if_ierrors++;
			ti_dev[idx]->stats.rx_failed_allocs++;
			goto next;
		}
		CACHE_INVAL (&attach_args->meminfo.cachectl, m->m_data,
			     attach_args->meminfo.rx_desc[cidx].buffer, m->m_ext.ext_size);
		m->m_pkthdr.len = pkt_len;
		m->m_len = pkt_len;
		m->m_pkthdr.rcvif = ifp;
		m->m_flags |= M_HASFCS;	 /* length includes 4 byte crc */

#if NBPFILTER > 0
		/* Pass this up to any BPF listeners. */
		if (ifp->if_bpf)
			bpf_mtap (ifp->if_bpf, m);
#endif

		if (verbose & DEBUG_DUMP_RX) {
			int i;
			fprintf(stderr, "\npkt dump for m=0x%p\n", m);
			for (i=0; i<m->m_len; i++) {
				fprintf(stderr, "%02X ", (uint8_t)m->m_data[i]);
				if (i && !((i + 1) % 8)) fprintf(stderr, "   ");
				if (i && !((i + 1) % 16)) fprintf(stderr, "\n");
			}
		}

		/* pass rxd mbuf up to io-pkt */
		ifp->if_ipackets++;

		vlan_hdr = mtod(m, struct ether_vlan_header*);
		if ((ntohs(vlan_hdr->evl_encap_proto) == ETHERTYPE_VLAN) &&
		    (ntohs(vlan_hdr->evl_proto) == ETHERTYPE_1722)) {
		    /* 1722 packet, send it straight up for minimum latency */
		    (*ifp->if_input)(ifp, m);
		} else {
		    /*
		     * Send it up from a stack thread so bridge and
		     * fastforward work. Without this we get logs of "no flow"
		     */
		    pthread_mutex_lock(&ti_dev[idx]->rx_mutex);
		    if (IF_QFULL(&ti_dev[idx]->rx_queue)) {
			m_freem(m);
			ifp->if_ierrors++;
			ti_dev[idx]->stats.rx_failed_allocs++;
		    } else {
			IF_ENQUEUE(&ti_dev[idx]->rx_queue, m);
		    }
		    if (!ti_dev[idx]->rx_running) {
			ti_dev[idx]->rx_running = 1;
			evp = interrupt_queue(ti_dev[idx]->iopkt,
					      &ti_dev[idx]->inter);
			if (evp != NULL) {
			    MsgSendPulse(evp->sigev_coid, evp->sigev_priority,
					 evp->sigev_code,
					 (int)evp->sigev_value.sival_ptr);
			}
		    }
		    pthread_mutex_unlock(&ti_dev[idx]->rx_mutex);
		}
		ti814x_add_rx_desc(attach_args, cidx + offset, new);
		ti_dev [idx]->pkts_received = 1;

		next:
		eidx = cidx;
		/* Tell the DMA engine that we're done with this descriptor */
		outle32 (attach_args->cpsw_base + RX0_CP +
			 (chan * sizeof(uint32_t)),
			 CPPI_DESC_PHYS +
			 (sizeof (cppi_desc_t) * (cidx + offset)));
		/*
		 * If flow control is enabled then don't overflow the queue,
		 * instead stop reading and wait for process_interrupt to
		 * drain it. This will cause the descriptors to fill, the
		 * switch queues to fill and hence backpressure to be sent
		 * from the ingress MAC.
		 * If flow control is not enabled, these would overflow a queue
		 * in the switch anyway, so don't bother with this test. This
		 * will also ensure AVB traffic doesn't get held up as AVB
		 * requires flow control disabled.
		 */
		if ((ti_dev[idx]->flow_status & IFM_ETH_TXPAUSE) &&
		    (ti_dev[idx]->rx_queue.ifq_len >=
		     (ti_dev[idx]->rx_queue.ifq_maxlen - 1))) {
			pthread_mutex_lock(&ti_dev[idx]->rx_mutex);
			ti_dev[idx]->rx_full |= 1 << chan;
			pthread_mutex_unlock(&ti_dev[idx]->rx_mutex);

			if (eoq) {
			    /* Ran out of descriptors, start DMA again */
			    outle32(attach_args->cpsw_base + RX0_HDP +
				    (chan * sizeof(uint32_t)),
				    CPPI_DESC_PHYS +
				    ((attach_args->rx_cidx[chan] + offset) *
				     sizeof (cppi_desc_t)));
			}
			return 0;
		}

	} // while

	if (eidx != -1) {
	    /* Processed some packets, may need to shuffle the descriptor chain */
	    nidx = (attach_args->rx_tail[chan] + 1) % NUM_RX_PKTS;

	    /*
	     * rx_tail = end of current descriptor chain
	     * nidx = start of new descriptor chain to add
	     * eidx = end of new descriptor chain to add
	     */
	    if (attach_args->rx_tail[chan] != eidx) {
		attach_args->meminfo.rx_desc[eidx + offset].next = 0;
		attach_args->meminfo.rx_desc[attach_args->rx_tail[chan]
					     + offset].next =
		  CPPI_DESC_PHYS + ((nidx + offset) * sizeof (cppi_desc_t));
		attach_args->rx_tail[chan] = eidx;
	    }
	}
	if (eoq) {
	    /* Ran out of descriptors, start DMA again */
	    outle32(attach_args->cpsw_base + RX0_HDP +
		    (chan * sizeof(uint32_t)),
		    CPPI_DESC_PHYS +
		    ((attach_args->rx_cidx[chan] + offset) *
		     sizeof (cppi_desc_t)));
	}

	return 1;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/io-pkt/sys/dev_qnx/dm814x/receive.c $ $Rev: 739726 $")
#endif
