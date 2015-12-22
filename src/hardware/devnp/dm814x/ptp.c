/*
 * $QNXLicenseC:
 * Copyright 2012, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0.
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

#include "ti814x.h"
#include <sys/syslog.h>
#include <netdrvr/ptp.h>

#define TI814X_TS_BUF_SZ 64

typedef struct {
    uint64_t	timestamp;
    uint16_t	seq;
    uint8_t	type;
    uint8_t	port;
} ether_event_t;

ether_event_t tx_buf[TI814X_TS_BUF_SZ];
uint32_t tx_idx = 0;
ether_event_t rx_buf[TI814X_TS_BUF_SZ];
uint32_t rx_idx = 0;
uint64_t ts_push_buf[TI814X_TS_BUF_SZ];
volatile uint32_t ts_push_cidx = 0;
volatile uint32_t ts_push_pidx = 0;
pthread_mutex_t ts_mutex = PTHREAD_MUTEX_INITIALIZER;

volatile uint32_t	upper_ptp_time;
intrspin_t		spinner;

uint32_t	ti814x_clock_period = 4;	/* in nanoseconds */
uint32_t	ti814x_clock_freq = 250000000;	/* in Hz */

void ti814x_process_ptp_interrupt (attach_args_t *attach_args)
{
    uint32_t		reg, type;
    ether_event_t	*buf;
    uint32_t		*idx;
    uint64_t		time = 0;
    static uint8_t	half_rollover = 0;

    InterruptLock(&spinner);
    reg = in32(attach_args->cpsw_base + CPTS_EVENT_HIGH);
    type = (reg & EVENT_TYPE_MASK) >> EVENT_TYPE_SHIFT;

    /* Fetch timestamp for timestamp events */
    if ((type == EVENT_TYPE_PUSH) ||
	(type == EVENT_TYPE_TX) ||
	(type == EVENT_TYPE_RX)) {

	time = in32(attach_args->cpsw_base + CPTS_EVENT_LOW);

	if (half_rollover || ((time & 0x80000000) == 0)) {
	    /* Timestamp is good, add upper part */
	    time |= (uint64_t)upper_ptp_time << 32;
	} else {
	    /* Misqueued see 9.2.2.5.3 p1497 in TRM sprugz8b */
	    time |= (uint64_t)(upper_ptp_time - 1) << 32;
	}
    }

    switch (type) {
    case EVENT_TYPE_PUSH:
	if (((ts_push_cidx == 0) &&
	     (ts_push_pidx == (TI814X_TS_BUF_SZ - 1))) ||
	    (ts_push_pidx == (ts_push_cidx - 1))) {
	    /* Buffer is full but still need to pop to clear the IRQ */
	    break;
	}
        ts_push_buf[ts_push_pidx] = time;
	ts_push_pidx = (ts_push_pidx + 1) % TI814X_TS_BUF_SZ;
	break;

    case EVENT_TYPE_TX:
    case EVENT_TYPE_RX:
	if (type == EVENT_TYPE_TX) {
	    idx = &tx_idx;
	    buf = &tx_buf[*idx];
	} else {
	    idx = &rx_idx;
	    buf = &rx_buf[*idx];
	}
	buf->timestamp = time;
	buf->seq = reg & SEQUENCE_ID_MASK;
	buf->type = (reg & MESSAGE_TYPE_MASK) >> MESSAGE_TYPE_SHIFT;
	buf->port = (reg & PORT_NUMBER_MASK) >> PORT_NUMBER_SHIFT;
	(*idx)++;
	if (*idx >= TI814X_TS_BUF_SZ) {
	    *idx = 0;
	}
	break;

    case EVENT_TYPE_ROLLOVER:
        upper_ptp_time++;
	half_rollover = 0;
	break;

    case EVENT_TYPE_HALF:
	half_rollover = 1;
	break;
    }

    out32(attach_args->cpsw_base + CPTS_EVENT_POP, EVENT_POP);
    InterruptUnlock(&spinner);
}

uint64_t ti814x_get_push_ts (ti814x_dev_t *ti814x)
{
    uint64_t ts = 0;
    /* Normally first time through but just in case */
    int32_t loop = PLL_MAX_LOOP;

    /* May need to wait for the ISR to populate the data */
    do {
	/* Busy spin, will be short */
	if (ts_push_pidx != ts_push_cidx) {
	    ts = ts_push_buf[ts_push_cidx];
	    ts_push_cidx = (ts_push_cidx + 1) % TI814X_TS_BUF_SZ;
	    break;
	}
	loop--;
    } while (loop > 0);
    return ts;
}

void ti814x_set_time (ti814x_dev_t *ti814x, ptp_time_t time)
{
    uint64_t count;
    uint32_t load_val;

    count = ((time.sec * 1000LL * 1000LL * 1000LL) + time.nsec) /
		ti814x_clock_period;
    InterruptLock(&spinner);
    upper_ptp_time = count >> 32;
    load_val = count & 0xFFFFFFFF;
    out32(ti814x->cpsw_regs + CPTS_TS_LOAD_VAL, load_val);
    out32(ti814x->cpsw_regs + CPTS_TS_LOAD_EN, TS_LOAD_EN);
    InterruptUnlock(&spinner);
}

void ti814x_get_time (ti814x_dev_t *ti814x, ptp_time_t *time)
{
    uint64_t time_ns;

    pthread_mutex_lock(&ts_mutex);
    out32(ti814x->cpsw_regs + CPTS_TS_PUSH, TS_PUSH);
    time_ns = ti814x_get_push_ts(ti814x) * ti814x_clock_period;
    time->sec = time_ns / (1000LL * 1000LL * 1000LL);
    time->nsec = time_ns % (1000LL * 1000LL * 1000LL);
    pthread_mutex_unlock(&ts_mutex);
}

void ti814x_get_timestamp (ptp_extts_t *ts, uint32_t port, uint8_t tx)
{
    uint32_t idx;
    ether_event_t *buf;

    if (tx) {
	buf = tx_buf;
    } else {
	buf = rx_buf;
    }
    for (idx = 0; idx < TI814X_TS_BUF_SZ; idx++) {
	if ((buf[idx].timestamp != 0) && (buf[idx].seq == ts->sequence_id) &&
	    (buf[idx].type == ts->msg_type) && (buf[idx].port == port)) {
	    ts->ts.sec = (buf[idx].timestamp * ti814x_clock_period) / 
	      (1000LL * 1000LL * 1000LL);
	    ts->ts.nsec = (buf[idx].timestamp * ti814x_clock_period) % 
	      (1000LL * 1000LL * 1000LL);
	    return;
	}
    }
    ts->ts.sec = 0;
    ts->ts.nsec = 0;
}

void ti814x_set_compensation (ti814x_dev_t *ti814x, ptp_comp_t comp)
{
    uint32_t val, m, mfrac;
    double frac;

#ifdef J6
    uint32_t mdiv_val = 1000000;
#else
    uint32_t mdiv_val = 500000;
#endif

    /* M = (1billion +- compensation) / mdiv_val */

    val = 1000000000;
    if (comp.positive) {
        val += comp.comp;
    } else {
	val -= comp.comp;
    }

    m = val / mdiv_val;
    frac = ((double)val / mdiv_val) - m;
    mfrac = frac * (1 << 18);

#ifdef J6
    out32(ti814x->pllss_base + HDMIPLL_CONF1,
	  (9 << PLL_REGN_SHIFT | m << PLL_REGM_SHIFT));
    out32(ti814x->pllss_base + HDMIPLL_CONF4, 8 << PLL_REGM2_SHIFT | mfrac);
    out32(ti814x->pllss_base + HDMIPLL_GO, PLL_GO);
#else
    out32(ti814x->pllss_base + VIDEO0PLL_MN2DIV, m);
    out32(ti814x->pllss_base + VIDEO0PLL_FRACDIV, (8 << REGSD_SHIFT) | mfrac);
    out32(ti814x->pllss_base + VIDEO0PLL_TENABLE, 0);
    out32(ti814x->pllss_base + VIDEO0PLL_TENABLE, 1);
    out32(ti814x->pllss_base + VIDEO0PLL_TENABLE, 0);
#endif
}


int ti814x_ptp_ioctl (ti814x_dev_t *ti814x, struct ifdrv *ifd)
{
    ptp_time_t		time;
    ptp_comp_t		comp;
    ptp_extts_t		ts;
    uint8_t		tx;

    if (ifd != NULL) {
	switch(ifd->ifd_cmd) {

	case PTP_GET_TX_TIMESTAMP:
	case PTP_GET_RX_TIMESTAMP:
	    if (ifd->ifd_len != sizeof(ts)) {
		return EINVAL;
	    }

	    if (ISSTACK) {
		if (copyin((((uint8_t *)ifd) + sizeof(*ifd)),
			   &ts, sizeof(ts))) {
		    return EINVAL;
		}
	    } else {
		memcpy(&ts, (((uint8_t *)ifd) + sizeof(*ifd)), sizeof(ts));
	    }

	    if (ifd->ifd_cmd == PTP_GET_TX_TIMESTAMP) {
		tx = 1;
	    } else {
		tx = 0;
	    }

	    ti814x_get_timestamp(&ts, ti814x->cfg.device_index + 1, tx);
	    if (ISSTACK) {
		return (copyout(&ts, (((uint8_t *)ifd) + sizeof(*ifd)),
				sizeof(ts)));
	    } else {
		memcpy((((uint8_t *)ifd) + sizeof(*ifd)), &ts, sizeof(ts));
		return EOK;
	    }
	    break;

	case PTP_GET_TIME:
	    if (ifd->ifd_len != sizeof(time)) {
		return EINVAL;
	    }
	    ti814x_get_time(ti814x,&time);
	    if (ISSTACK) {
		return (copyout(&time, (((uint8_t *)ifd) + sizeof(*ifd)),
				sizeof(time)));
	    } else {
		memcpy((((uint8_t *)ifd) + sizeof(*ifd)), &time, sizeof(time));
		return EOK;
	    }
	    break;

	case PTP_SET_TIME:
	    if (ifd->ifd_len != sizeof(time)) {
		return EINVAL;
	    }
	    if (ISSTACK) {
		if (copyin((((uint8_t *)ifd) + sizeof(*ifd)),
			   &time, sizeof(time))) {
		    return EINVAL;
		}
	    } else {
		memcpy(&time, (((uint8_t *)ifd) + sizeof(*ifd)), sizeof(time));
	    }
	    ti814x_set_time(ti814x, time);
	    /* Clock has changed so all old ts are invalid */
	    memset(tx_buf, 0, sizeof(tx_buf));
	    memset(rx_buf, 0, sizeof(rx_buf));
	    return EOK;
	    break;

	case PTP_SET_COMPENSATION:
	    if (ifd->ifd_len != sizeof(comp)) {
		return EINVAL;
	    }
	    if (ISSTACK) {
		if (copyin((((uint8_t *)ifd) + sizeof(*ifd)),
			   &comp, sizeof(comp))) {
		    return EINVAL;
		}
	    } else {
		memcpy(&comp, (((uint8_t *)ifd) + sizeof(*ifd)), sizeof(comp));
	    }
	    ti814x_set_compensation(ti814x, comp);
	    return EOK;
	    break;

	default:
	    log(LOG_ERR, "Unknown PTP ioctl 0x%lx", ifd->ifd_cmd);
	    break;
	}
    }
    return EINVAL;
}

#ifdef J6
int ti814x_ptp_init (ti814x_dev_t *ti814x)
{
    uint32_t loop, val;
    uintptr_t dss_base;

    upper_ptp_time = 0;

    /* Don't touch the PLL unless asked on the command line */
    if (!ti814x->ptp_enable) {
	return EOK;
    }
    log(LOG_INFO, "Enabling PTP using HDMI DPLL");

    /*
     * Use the HDMI PLL on J6 to provide GMAC_RFT_CLK, it is the only
     * PLL that has fractional-M capability to fine tune the frequency.
     */
    val = in32(ti814x->cmr_base + GMAC_CLKCTRL);
    val &= ~GMAC_CLKCTRL_RFT_MASK;
    val |= GMAC_CLKCTRL_RFT_HDMI;
    out32(ti814x->cmr_base + GMAC_CLKCTRL, val);

    /* Access the PLL via the L4 so disable L3 access */
    val = in32(ti814x->cmr_base + DSS_PLL_CONTROL);
    val |= HDMI_DSS_DISABLE;
    out32(ti814x->cmr_base + DSS_PLL_CONTROL, val);

    /* Enable DESHDCP clock, needs a readback for timing */
    val = in32(ti814x->cmr_base + CONTROL_IO_2);
    val |= DESHDCP_CLK;
    out32(ti814x->cmr_base + CONTROL_IO_2, val);
    val = in32(ti814x->cmr_base + CONTROL_IO_2);

    /* Power up PLL and wait for it */
    dss_base = mmap_device_io(DSS_SIZE, DSS_BASE);
    if (dss_base == (uintptr_t)MAP_FAILED) {
	log(LOG_ERR, "Failed to map DSS registers: %s", strerror(errno));
	return errno;
    }
    out32(dss_base + HDMI_WP_PWR_CTRL, PLL_PWR_ON);
    loop = 0;
    do {
	if ((in32(dss_base + HDMI_WP_PWR_CTRL) & PLL_STATUS_MASK) ==
	    PLL_STATUS_ON) {
		break;
	}
	loop++;
    } while (loop < PLL_MAX_LOOP);

    if (loop >= PLL_MAX_LOOP) {
	log(LOG_ERR, "PLL didn't power on in time, status 0x%x",
	in32(dss_base + HDMI_WP_PWR_CTRL));
	munmap_device_io(dss_base, DSS_SIZE);
	return ENODEV;
    }
    munmap_device_io(dss_base, DSS_SIZE);

    /* Reset the PLL to a known state */
    out32(ti814x->pllss_base + HDMIPLL_CONTROL, 0);
    out32(ti814x->pllss_base + HDMIPLL_CONTROL,
	  HSDIV_SYSRESETN | PLL_SYSRESETN);
    loop = 0;
    do {
	if (in32(ti814x->pllss_base + HDMIPLL_STATUS) &
	    PLLCTRL_RESET_DONE) {
	    break;
	}
	loop++;
    } while (loop < PLL_MAX_LOOP);

    if (loop >= PLL_MAX_LOOP) {
	log(LOG_ERR, "PLL not reset in time, status 0x%x",
	in32(ti814x->pllss_base + HDMIPLL_STATUS));
	return ENODEV;
    }

    /*
     * Set up the PLL
     * freq = (20MHz * (M/N+1)) / M2
     * Constraints are:
     * 0.62MHz <= CLKINP <= 60MHz - OK SYSCLK1 is 20MHz
     * 0.62MHz <= REFCLK <= 2.5MHz - OK N = 9 => REFCLK = 2MHz
     * 20 <= M <= 2045 - OK M = 1000 => DCO = 2000MHz
     * 1 <= M2 <= 127 - OK M2 = 8 => 250MHz PTP Clock
     * PLL_SD <= (M / (N + 1)) * (CLKINP in MHz / 256) i.e. 7
     */
    out32(ti814x->pllss_base + HDMIPLL_CONF1,
	  (9 << PLL_REGN_SHIFT | 1000 << PLL_REGM_SHIFT));
    out32(ti814x->pllss_base + HDMIPLL_CONF3, 7 << PLL_SD_SHIFT);
    out32(ti814x->pllss_base + HDMIPLL_CONF4, 8 << PLL_REGM2_SHIFT);
    out32(ti814x->pllss_base + HDMIPLL_GO, PLL_GO);
    loop = 0;
    do {
	if (in32(ti814x->pllss_base + HDMIPLL_STATUS) & PLL_LOCK) {
	    break;
	}
	loop++;
    } while (loop < PLL_MAX_LOOP);

    if (loop >= PLL_MAX_LOOP) {
	log(LOG_ERR, "PLL not locked in time status 0x%x",
	in32(ti814x->pllss_base + HDMIPLL_STATUS));
	return ENODEV;
    }

    /* Start the timer and enable interrupts */
    out32(ti814x->cpsw_regs + CPTS_CONTROL, CPTS_EN);
    val = in32(ti814x->cpsw_regs + C0_MISC_EN);
    val |= EVENTINT_EN;
    out32(ti814x->cpsw_regs + C0_MISC_EN, val);
    out32(ti814x->cpsw_regs + CPTS_INT_ENABLE, TS_PEND_EN);

    /* Enable Timestamping on ports */
    out32(ti814x->cpsw_regs + TS_LTYPE, 0x88f7);
    out32(ti814x->cpsw_regs + P1_TS_SEQ_MTYPE,
	  (0x1e << TS_SEQ_ID_OFFSET_SHIFT) |
	  TS_TYPE_SYNC | TS_TYPE_DELAY_REQ | TS_TYPE_PDELAY_REQ |
	  TS_TYPE_PDELAY_RESP);
    out32(ti814x->cpsw_regs + P1_CONTROL,
	  TS_107 | TS_320 | TS_319 | TS_132 | TS_131 | TS_130 | TS_129 |
	  TS_TTL_NONZERO | TS_UNI | TS_ANNEX_F | TS_ANNEX_E | TS_ANNEX_D |
	  TS_LTYPE1_EN | TS_TX_EN | TS_RX_EN);
    out32(ti814x->cpsw_regs + P2_TS_SEQ_MTYPE,
	  (0x1e << TS_SEQ_ID_OFFSET_SHIFT) |
	  TS_TYPE_SYNC | TS_TYPE_DELAY_REQ | TS_TYPE_PDELAY_REQ |
	  TS_TYPE_PDELAY_RESP);
    out32(ti814x->cpsw_regs + P2_CONTROL,
	  TS_107 | TS_320 | TS_319 | TS_132 | TS_131 | TS_130 | TS_129 |
	  TS_TTL_NONZERO | TS_UNI | TS_ANNEX_F | TS_ANNEX_E | TS_ANNEX_D |
	  TS_LTYPE1_EN | TS_TX_EN | TS_RX_EN);

    return EOK;
}
#else
int ti814x_ptp_init (attach_args_t *attach_args)
{
    uint32_t loop, val;

    upper_ptp_time = 0;

    /*
     * VIDEO0PLL freq = (20MHz * (M/N+1)) / M2
     * Run PLL at 500MHz with M2=2 to give 250MHz PTP clock
     * Use M=2000 and N+1=80 for finer granularity in adjustment
     */
    out32(attach_args->pllss_base + VIDEO0PLL_M2NDIV, (2 << M2_SHIFT) + 79);
    out32(attach_args->pllss_base + VIDEO0PLL_MN2DIV, 2000);
    out32(attach_args->pllss_base + VIDEO0PLL_TENABLE, 0);
    out32(attach_args->pllss_base + VIDEO0PLL_TENABLE, 1);
    out32(attach_args->pllss_base + VIDEO0PLL_TENABLE, 0);
    out32(attach_args->pllss_base + VIDEO0PLL_TENABLEDIV, 0);
    out32(attach_args->pllss_base + VIDEO0PLL_TENABLEDIV, 1);
    out32(attach_args->pllss_base + VIDEO0PLL_TENABLEDIV, 0);
    out32(attach_args->pllss_base + VIDEO0PLL_CLKCTRL,
	  CYCLESLIPEN | CLKOUTEN | M2PWDNZ | SELFREQDCO_HS2);
    out32(attach_args->pllss_base + VIDEO0PLL_CLKCTRL,
	  CYCLESLIPEN | CLKOUTEN | M2PWDNZ | SELFREQDCO_HS2 | TINITZ);

    loop = 0;
    do {
	if ((in32(attach_args->pllss_base + VIDEO0PLL_STATUS) &
	     (PHASELOCK | FREQLOCK)) == (PHASELOCK | FREQLOCK)) {
	    break;
	}
	delay(PLL_DELAY);
	loop++;
    } while (loop < PLL_MAX_LOOP);

    if (loop >= PLL_MAX_LOOP) {
	log(LOG_ERR, "PLL not locked in time status 0x%x",
	in32(attach_args->pllss_base + VIDEO0PLL_STATUS));
    }

    /* Start the timer and enable interrupts */
    out32(attach_args->cpsw_base + CPTS_CONTROL, CPTS_EN);
    val = in32(attach_args->cpsw_base + C0_MISC_EN);
    val |= EVENTINT_EN;
    out32(attach_args->cpsw_base + C0_MISC_EN, val);
    out32(attach_args->cpsw_base + CPTS_INT_ENABLE, TS_PEND_EN);

    /* Enable Timestamping on ports */
#ifndef J5_ECO
    out32(attach_args->cpsw_base + P1_TS_CTL_CPSW_3GF,
	  TS_TYPE_SYNC | TS_TYPE_DELAY_REQ | TS_TYPE_PDELAY_REQ |
	  TS_TYPE_PDELAY_RESP | TS_TX_EN | TS_RX_EN);
    out32(attach_args->cpsw_base + P1_TS_SEQ_LTYPE,
	  (0x1e << TS_SEQ_ID_OFFSET_SHIFT) | 0x88f7);
    out32(attach_args->cpsw_base + P2_TS_CTL_CPSW_3GF,
	  TS_TYPE_SYNC | TS_TYPE_DELAY_REQ | TS_TYPE_PDELAY_REQ |
	  TS_TYPE_PDELAY_RESP | TS_TX_EN | TS_RX_EN);
    out32(attach_args->cpsw_base + P2_TS_SEQ_LTYPE,
	  (0x1e << TS_SEQ_ID_OFFSET_SHIFT) | 0x88f7);
#else
    out32(attach_args->cpsw_base + TS_LTYPE, 0x88f7);
    out32(attach_args->cpsw_base + P1_TS_SEQ_MTYPE,
	  (0x1e << TS_SEQ_ID_OFFSET_SHIFT) |
	  TS_TYPE_SYNC | TS_TYPE_DELAY_REQ | TS_TYPE_PDELAY_REQ |
	  TS_TYPE_PDELAY_RESP);
    out32(attach_args->cpsw_base + P1_CONTROL,
	  TS_LTYPE1_EN | TS_TX_EN | TS_RX_EN);
    out32(attach_args->cpsw_base + P2_TS_SEQ_MTYPE,
	  (0x1e << TS_SEQ_ID_OFFSET_SHIFT) |
	  TS_TYPE_SYNC | TS_TYPE_DELAY_REQ | TS_TYPE_PDELAY_REQ |
	  TS_TYPE_PDELAY_RESP);
    out32(attach_args->cpsw_base + P2_CONTROL,
	  TS_LTYPE1_EN | TS_TX_EN | TS_RX_EN);
#endif
    return EOK;
}
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/io-pkt/sys/dev_qnx/dm814x/ptp.c $ $Rev: 754989 $")
#endif
