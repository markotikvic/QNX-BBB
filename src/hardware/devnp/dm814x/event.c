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

#include    "bpfilter.h"
#include    "ti814x.h"
#include    <netinet/in.h>
#include    <sys/neutrino.h>
#include    <quiesce.h>

#if NBPFILTER > 0
#include    <net/bpf.h>
#include    <net/bpfdesc.h>
#endif

extern	ti814x_dev_t	*ti_dev [2];
intrspin_t spinner; /* For misc enable register as no separate set/clear */

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int ti814x_process_interrupt (void *arg, struct nw_work_thread *wtp)

{
    ti814x_dev_t	*ti814x = arg;
    struct ifnet	*ifp;
    struct mbuf		*m;

    ifp = &ti814x->ecom.ec_if;

    while(1) {
	pthread_mutex_lock(&ti814x->rx_mutex);
	IF_DEQUEUE(&ti814x->rx_queue, m);
	if (m != NULL) {
	    pthread_mutex_unlock(&ti814x->rx_mutex);
	    (*ifp->if_input)(ifp, m);
	} else {
	    /* Leave mutex locked to prevent any enqueues, unlock in enable */
	    break;
	}
    }
    return 1;
}

int ti814x_enable_interrupt (void *arg)
{
    ti814x_dev_t	*ti814x = arg;

    ti814x->rx_running = 0;
    if (ti814x->rx_full) {
		outle32(ti814x->cpsw_regs + RX_INTMASK_SET, ti814x->rx_full);
    }
    pthread_mutex_unlock(&ti814x->rx_mutex);

    return 1;
}

/**************************************************************************/
/*                                                                        */
/**************************************************************************/

const struct sigevent *ti814x_isr (void *arg, int iid)

{
	attach_args_t				*attach_args = arg;
	uint32_t				mask, chan;

	mask = in32(attach_args->cpsw_base + RX_STAT);
	chan = ffs(mask) - 1;

	outle32(attach_args->cpsw_base + RX_INTMASK_CLEAR, 1 << chan);
	outle32(attach_args->cpsw_base + CPDMA_EOI_VECTOR, 0x1);

	return &attach_args->isr_event[chan];
}

void *ti814x_rx_thread (void *arg)
{
    attach_args_t	*attach_args = arg;
    int			rcvid, chan;
    struct _pulse	pulse;
    iov_t		msg;

    SETIOV(&msg, &pulse, sizeof(pulse));

    while (1) {
      rcvid = MsgReceivev(attach_args->chid, &msg, 1, NULL);
	if (rcvid == 0) {
	    switch (pulse.code) {
	    case TI814X_RX_PULSE:
		chan = pulse.value.sival_int;
		if (ti814x_receive(attach_args, WTP, chan)) {
		    outle32(attach_args->cpsw_base + RX_INTMASK_SET, 1 << chan);
		}
		break;
	    case TI814X_QUIESCE_PULSE:
		quiesce_block(pulse.value.sival_int);
		break;
	    default:
		slogf(_SLOGC_NETWORK, _SLOG_ERROR,
		      "dm814x Rx Unknown pulse %d received",
		    pulse.code);
		break;
	    }
	} else {
	    slogf(_SLOGC_NETWORK, _SLOG_ERROR, "dm814x Rx MsgReceive error");
	}
    }
    return NULL;
}




/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int ti814x_process_link_interrupt (void *arg, struct nw_work_thread *wtp)

{
	attach_args_t	*attach_args = arg;

	if (ti_dev[0] != NULL) {
		if (attach_args->mdio_raw & 1) {
			MDI_MonitorPhy(ti_dev[0]->mdi);
			attach_args->mdio_raw &= ~1;
			}
		}
	if (ti_dev[1] != NULL) {
		if (attach_args->mdio_raw & 2) {
			MDI_MonitorPhy(ti_dev[1]->mdi);
			attach_args->mdio_raw &= ~2;
			}
		}

	return (1);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int ti814x_process_stat_interrupt (void *arg, struct nw_work_thread *wtp)

{
ti814x_dev_t	*ti814x = NULL;

	if (ti_dev[0] != NULL) {
		if (ti_dev[0]->get_stats)
			ti814x = ti_dev[0];
		}
	if (ti_dev[1] != NULL) {
		if (ti_dev[1]->get_stats)
			ti814x = ti_dev[1];
		}
	if (ti814x != NULL)
		ti814x_read_stats (ti814x);
	return (1);
}

int ti814x_process_tx_interrupt (void *arg, struct nw_work_thread *wtp)
{
	ti814x_dev_t	*ti814x;
	uint32_t	i, cp, hdp, start_phys, chan, queue, offset;
	cppi_desc_t	*desc;

	chan = (uint32_t)arg;
	ti814x = ti_dev[chan % 2];
	queue = chan / 2;
	cp = TX0_CP + (sizeof(uint32_t) * chan);
	hdp = TX0_HDP + (sizeof(uint32_t) * chan);
	offset = queue * 2 * NUM_TX_PKTS;

	NW_SIGLOCK_P(&ti814x->ecom.ec_if.if_snd_ex, ti814x->iopkt, wtp);

	/* Clear the interrupt */
	outle32(ti814x->cpsw_regs + cp, in32(ti814x->cpsw_regs + cp));

	/* If DMA stalled from a misqueue then kick it */
	if (in32(ti814x->cpsw_regs + hdp) == 0) {
	    i = ti814x->tx_cidx[queue];
	    while (i != ti814x->tx_pidx[queue]) {
		desc = &ti814x->meminfo.tx_desc[i + offset];
		if (desc->flag_len & DESC_FLAG_OWN) {
		    start_phys = ti814x->meminfo.tx_phys +
		      (sizeof (cppi_desc_t) * (i + offset));
		    out32(ti814x->cpsw_regs + hdp, start_phys);
		    break;
		}
		i = (i + 1) % NUM_TX_PKTS;
	    }
	}

	/* If was out of tx descriptors call start to reap and Tx more */
	if ((queue == 0) && (ti814x->ecom.ec_if.if_flags_tx & IFF_OACTIVE)) {
	    ti814x_start(&ti814x->ecom.ec_if);
	} else {
	    NW_SIGUNLOCK_P(&ti814x->ecom.ec_if.if_snd_ex, ti814x->iopkt, wtp);
	}

	return 1;
}

/**************************************************************************/
/*                                                                        */
/**************************************************************************/

const struct sigevent *ti814x_isr_link (void *arg, int iid)

{
	attach_args_t			*attach_args = arg;
	struct _iopkt_inter		*link_ent = &attach_args->inter_link;
	struct _iopkt_inter		*stat_ent = &attach_args->inter_stat;
	uint32_t			misc_stat, val;
	const struct sigevent		*rc = NULL;

	misc_stat = in32(attach_args->cpsw_base + MISC_STAT);
	if (misc_stat & LINKSTAT) {
	    attach_args->mdio_raw = in32(attach_args->cpsw_base +
					 MDIOLINKINTRAW);
	    val = in32(attach_args->cpsw_base + C0_MISC_EN);
	    val &= ~LINKINT_EN;
	    out32(attach_args->cpsw_base + C0_MISC_EN, val);
	    out32(attach_args->cpsw_base + MDIOLINKINTRAW,
		  attach_args->mdio_raw);
	    rc = interrupt_queue(attach_args->iopkt, link_ent);
	}

	if (misc_stat & EVNTPEND) {
	    val = in32(attach_args->cpsw_base + C0_MISC_EN);
	    val &= ~EVENTINT_EN;
	    out32(attach_args->cpsw_base + C0_MISC_EN, val);
	    ti814x_process_ptp_interrupt(attach_args);
	    val |= EVENTINT_EN;
	    out32(attach_args->cpsw_base + C0_MISC_EN, val);
	}

	if (misc_stat & STATPEND) {
	    val = in32(attach_args->cpsw_base + C0_MISC_EN);
	    val &= ~STAT_PEND_EN;
	    out32(attach_args->cpsw_base + C0_MISC_EN, val);
	    rc = interrupt_queue(attach_args->iopkt, stat_ent);
	}

	out32(attach_args->cpsw_base + CPDMA_EOI_VECTOR, 0x3);
	return rc;
}

const struct sigevent *ti814x_isr_tx (void *arg, int iid)
{
	attach_args_t			*attach_args = arg;
	struct _iopkt_inter		*tx_ent;
	uint32_t			mask, chan;

	mask = in32(attach_args->cpsw_base + TX_STAT);
	chan = ffs(mask) - 1;

	tx_ent = &attach_args->inter_tx[chan];

	outle32 (attach_args->cpsw_base + TX_INTMASK_CLEAR, 1 << chan);
	outle32(attach_args->cpsw_base + CPDMA_EOI_VECTOR, 0x2);
	return interrupt_queue(attach_args->iopkt, tx_ent);
}

/**************************************************************************/
/* device_enable_interrupt - enables rx interrupts on the J5 platform      */
/**************************************************************************/

int ti814x_enable_link_interrupt (void *arg)

{
	uint32_t val;
	attach_args_t *attach_args = arg;

	InterruptLock(&spinner);
	val = inle32(attach_args->cpsw_base + C0_MISC_EN);
	val |= LINKINT_EN;
	outle32(attach_args->cpsw_base + C0_MISC_EN, val);
	InterruptUnlock(&spinner);
	return 1;
}

int ti814x_enable_stat_interrupt (void *arg)

{
	uint32_t val;
	attach_args_t *attach_args = arg;

	InterruptLock(&spinner);
	val = inle32(attach_args->cpsw_base + C0_MISC_EN);
	val |= STAT_PEND_EN;
	outle32(attach_args->cpsw_base + C0_MISC_EN, val);
	InterruptUnlock(&spinner);
	val = inle32 (attach_args->cpsw_base + DMA_INTSTAT_SET);
	val |= STAT_PEND_INT_MASK;
	outle32 (attach_args->cpsw_base + DMA_INTSTAT_SET, val);
	return 1;
}

int ti814x_enable_tx_interrupt (void *arg)
{
	ti814x_dev_t	*ti814x;
	uint32_t	chan, val;

	chan = (uint32_t)arg;
	ti814x = ti_dev[chan % 2];

	val  = 1 << chan;

	outle32 (ti814x->cpsw_regs + TX_INTMASK_SET, val);
	return 1;
}
#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/io-pkt/sys/dev_qnx/dm814x/event.c $ $Rev: 706942 $")
#endif
