/*
 * $QNXLicenseC:
 * Copyright 2007-2008, QNX Software Systems.
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
 *
 *    mcasp_dll.c
 */

#include <mcasp.h>

#define MIN(A, B)              ((A)<(B)?(A):(B))

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static
unsigned get_num_list_from_string(unsigned values[], unsigned limit, char *str, int sep)
{
	unsigned cnt = 0;

	while (str && cnt < limit)
	{
		if (cnt > 0) str++;  // skip over separator
		values[cnt++] = strtoul(str, &str, 0);
		str = strchr(str, sep);  // find next separator
	}
	return cnt;
}

/**
 * This function returns the number of open capture subchannels
 */
uint32_t
num_open_capture_subchn(mcasp_cap_strm_t *strm)
{
	uint32_t num; /* number of capture subchn open*/
	uint32_t idx = 0;

	num = 0;
	for(idx=0; idx < strm->nsubchn; idx++)
	{
		if(NULL != strm->subchn[idx].pcm_subchn)
			num++;
	}
	return num;
}

static
int any_active_capture_streams(struct mcasp_cap_interfaces *cap_aif)
{
	unsigned i;

	// check all capture streams to see if any of them are active
	for (i = 0; i < cap_aif->serializer_cnt; i++)
	{
		if (num_open_capture_subchn(&cap_aif->cap_strm[i]) != 0)
		{
			return 1;
		}
	}
	return 0;
}

static
struct mcasp_cap_strm *find_capture_stream_with_pcm(struct mcasp_cap_interfaces *cap_aif, ado_pcm_t *pcm)
{
	unsigned cnt;
	for (cnt = 0; cnt < cap_aif->serializer_cnt; cnt++)
	{
		struct mcasp_cap_strm *cap_strm = &cap_aif->cap_strm[cnt];
		if (cap_strm->pcm == pcm)
			return cap_strm;
	}
	return NULL;
}

static inline int multiple_rates_supported(HW_CONTEXT_T *mcasp_card, int channel)
{
	if (channel == SND_PCM_CHANNEL_PLAYBACK)
		return mcasp_card->play_strm.sample_rate_min < mcasp_card->play_strm.sample_rate_max;
	else
		return mcasp_card->cap_aif.sample_rate_min < mcasp_card->cap_aif.sample_rate_max;
}

int32_t mcasp_capabilities(HW_CONTEXT_T * mcasp_card, ado_pcm_t *pcm, snd_pcm_channel_info_t * info)
{
	int chn_avail = 1;

	if (info->channel == SND_PCM_CHANNEL_PLAYBACK)
	{
		if (mcasp_card->play_strm.subchn.pcm_subchn)
		{
			chn_avail = 0;
		}
		else if (multiple_rates_supported(mcasp_card, SND_PCM_CHANNEL_PLAYBACK))
		{
			ado_mutex_lock(&mcasp_card->hw_lock);
			if (mcasp_card->async_clks == 0 && any_active_capture_streams(&mcasp_card->cap_aif))
			{
				/* Playback and Capture are Rate locked, so adjust rate capabilities
				 * if the other side has been aquried. When async is disabled the capture side
				 * also uses the play_strm clock configurations.
				 */
				info->min_rate = info->max_rate = mcasp_card->play_strm.sample_rate;
				info->rates = ado_pcm_rate2flag(mcasp_card->play_strm.sample_rate);
			}
			ado_mutex_unlock(&mcasp_card->hw_lock);
		}
	}
	else if (info->channel == SND_PCM_CHANNEL_CAPTURE)
	{
		// Find the capture stream for this pcm handle
		struct mcasp_cap_strm *cap_strm = find_capture_stream_with_pcm(&mcasp_card->cap_aif, pcm);

        if (mcasp_card->cap_aif.serializer_cnt > 1 || cap_strm->nsubchn > 1)
        {
            /* When multiple subchannels or serializers are used the DMA buffer is separate from the subchannels PCM buffer.
             * Make sure the subchannls PCM buffer is aligned with the DMA buffer.
             */
            info->fragment_align = mcasp_card->cap_aif.frag_size;
        }

		if (cap_strm)
		{
			// check to see if there are any subchns available
			if (num_open_capture_subchn(cap_strm) == cap_strm->nsubchn)
			{
				chn_avail = 0;
			}
			else if (multiple_rates_supported(mcasp_card, SND_PCM_CHANNEL_CAPTURE))
			{
				ado_mutex_lock(&mcasp_card->hw_lock);
				if (mcasp_card->async_clks == 0 && (mcasp_card->play_strm.subchn.pcm_subchn ||
						any_active_capture_streams(&mcasp_card->cap_aif)))
				{
					/* Playback and Capture are Rate locked, so adjust rate capabilities
					 * if the other side has been aquried. When async is disabled the capture
					 * side uses the play_strm clock configurations.
					 */
					info->min_rate = info->max_rate = mcasp_card->play_strm.sample_rate;
					info->rates = ado_pcm_rate2flag(mcasp_card->play_strm.sample_rate);
				}
				else if (mcasp_card->async_clks == 1 && any_active_capture_streams(&mcasp_card->cap_aif))
				{
					/* Async is enabled, so capture side uses it's own clock configurations */
					info->min_rate = info->max_rate = mcasp_card->cap_aif.sample_rate;
					info->rates = ado_pcm_rate2flag(mcasp_card->cap_aif.sample_rate);
				}
				ado_mutex_unlock(&mcasp_card->hw_lock);
			}
		}
		else chn_avail = 0;
	}

	if (chn_avail == 0)
	{
		info->formats = 0;
		info->rates = 0;
		info->min_rate = 0;
		info->max_rate = 0;
		info->min_voices = 0;
		info->max_voices = 0;
		info->min_fragment_size = 0;
		info->max_fragment_size = 0;
	}

	return (0);
}

// Check if there is a high frequency ahclk that is evenly divisible by the requested rate.
// Save the index of the first high frequency that was evenly divisible in freq_idx.
static
int sample_rate_divides_ahclk(struct ahclk *ahclk, unsigned rate)
{
	unsigned i;
	for (i = 0; i < ahclk->num_freqs; i++)
	{
		unsigned freq = ahclk->freqs[i];

		if (freq > 0 && freq % rate == 0)
		{
			ahclk->freq_idx = i;
			return freq;
		}
	}
	return 0;
}

#define MASK(width) ((1 << (width)) - 1)
#define SET_FIELD(regval, fieldwidth, fieldpos, fieldval) \
	(((regval) & ~(MASK(fieldwidth) << (fieldpos))) | ((MASK(fieldwidth) & (fieldval)) << (fieldpos)))

static
void set_play_bit_clock_rate(HW_CONTEXT_T *mcasp_card, unsigned rate)
{
	if (mcasp_card->clk_mode == SLAVE)
		return;

	mcasp_card->play_strm.sample_rate = rate;

	struct ahclk *ahclkx = &mcasp_card->ahclkx;
	unsigned hi_freq = sample_rate_divides_ahclk(ahclkx, rate);
	if (ahclkx->mux_select) ahclkx->mux_select(mcasp_card);

	if (mcasp_card->play_strm.protocol == PROTOCOL_SPDIF)
	{
		/* Calculate BCLK based on high frequency xclk
		 * SPDIF clock = 128 * FS
		 */
		unsigned divider = (hi_freq > 0) ? (hi_freq / (rate * 128)) : 1;
		mcasp_card->mcasp->ACLKXCTL  = SET_FIELD(mcasp_card->mcasp->ACLKXCTL, 5, 0, divider-1);

		ado_debug(DB_LVL_DRIVER, "mcasp: %s: SPDIF CLKXDIV %u, rate %u", __func__, divider, rate);
	}
	else
	{
		/* Calculate BCLK based on high frequency xclk */
		unsigned divider = (hi_freq > 0) ? (hi_freq / (rate * mcasp_card->slot_size * mcasp_card->slot_num)) : 1;

		// This is to support the 8 kHz sample rate from a 18.432 MHz master clock
		// Because we cannot divide by 36, use 24=36*2/3 and fudge XMOD from 2 to 3
		{
			unsigned afs_ctl = ahclkx->afs_ctl;  // original setting

			if (divider > 32 && divider % 3 == 0)
			{
				divider = 2 * divider / 3;
				afs_ctl |= AFSXCTL_XMOD_1CH;
			}
			mcasp_card->mcasp->AFSXCTL = afs_ctl;
		}

		mcasp_card->mcasp->ACLKXCTL  = SET_FIELD(mcasp_card->mcasp->ACLKXCTL, 5, 0, divider-1);

		ado_debug(DB_LVL_DRIVER, "mcasp: %s: CLKXDIV %u, rate %u", __func__, divider, rate);
	}
}

static
void set_cap_bit_clock_rate(HW_CONTEXT_T *mcasp_card, unsigned rate)
{
	if (mcasp_card->clk_mode == SLAVE)
		return;

	mcasp_card->cap_aif.sample_rate = rate;

	struct ahclk *ahclkr = &mcasp_card->ahclkr;
	unsigned hi_freq = sample_rate_divides_ahclk(ahclkr, rate);
	if (ahclkr->mux_select) ahclkr->mux_select(mcasp_card);

	/* Calculate BCLK based on high frequency clk */
	unsigned divider = (hi_freq > 0) ? (hi_freq / (rate * mcasp_card->slot_size * mcasp_card->slot_num)) : 1;
	mcasp_card->mcasp->ACLKRCTL  = SET_FIELD(mcasp_card->mcasp->ACLKRCTL, 5, 0, divider-1);

	ado_debug(DB_LVL_DRIVER, "mcasp: %s: CAP CLKRDIV %u, rate %u", __func__, divider, rate);
}

static
struct mcasp_cap_strm *find_capture_stream(struct mcasp_cap_interfaces *cap_aif, ado_pcm_subchn_t *subchn)
{
	unsigned cnt;

	for (cnt = 0; cnt < cap_aif->serializer_cnt; cnt++)
	{
		struct mcasp_cap_strm *cap_strm = &cap_aif->cap_strm[cnt];

		if (ado_pcm_subchn_is_channel(subchn, cap_strm->pcm, ADO_PCM_CHANNEL_CAPTURE))
		{
			return cap_strm;
		}
	}
	return NULL;
}

int32_t
mcasp_playback_aquire(HW_CONTEXT_T * mcasp_card, PCM_SUBCHN_CONTEXT_T ** pc,
					  ado_pcm_config_t * config, ado_pcm_subchn_t * subchn, uint32_t * why_failed)
{

	ado_mutex_lock(&mcasp_card->hw_lock);
	if (mcasp_card->play_strm.subchn.pcm_subchn)
	{
		*why_failed = SND_PCM_PARAMS_NO_CHANNEL;
		ado_mutex_unlock(&mcasp_card->hw_lock);
		return (EAGAIN);
	}

	/* Ensure ASP and DMA are disable */
	CLR_EER(mcasp_card->play_strm.dma_idx);
	mcasp_card->mcasp->XGBLCTL &= ~(XGBLCTL_XSMRST);

	if (multiple_rates_supported(mcasp_card, SND_PCM_CHANNEL_PLAYBACK))
	{
		// Is rate switch necessary?
		if (config->format.rate != mcasp_card->play_strm.sample_rate)
		{
			// check all capture streams to see if any of them are active
			if (mcasp_card->async_clks == 0 && any_active_capture_streams(&mcasp_card->cap_aif))
			{
				ado_mutex_unlock(&mcasp_card->hw_lock);
				return (EBUSY);
			}

			set_play_bit_clock_rate(mcasp_card, config->format.rate);
		}
	}

	if ((config->dmabuf.addr = ado_shm_alloc(config->dmabuf.size, config->dmabuf.name,
											 ADO_SHM_DMA_SAFE, &config->dmabuf.phys_addr)) == NULL)
	{
		ado_mutex_unlock(&mcasp_card->hw_lock);
		return (errno);
	}

	memset(&mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx], 0x00,
		   sizeof (mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx]));
	mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx].OPT = OPT_SYNCDIM | OPT_TCINTEN | OPT_TCC(mcasp_card->play_strm.dma_idx);
	mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx].ACNT = mcasp_card->sample_size;						/* Sample size in bytes */
	mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx].BCNT = mcasp_card->play_strm.serializer_cnt; 		/* Number of Tx_serializer */
	mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx].DST = mcasp_card->mcasp_dmax;
	mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx].SRCBIDX =
		mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx].ACNT;
	mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx].DSTBIDX = 0;
	mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx].LINK = 0x4000 + 0x20 * (DMA_RELOAD + mcasp_card->play_strm.dma_idx);
	mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx].BCNTRLD =
		mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx].BCNT;
	mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx].SRCCIDX = mcasp_card->sample_size * mcasp_card->play_strm.serializer_cnt;
	mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx].DSTCIDX = 0;
	mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx].CCNT =
		ado_pcm_dma_int_size(config) / (mcasp_card->sample_size * mcasp_card->play_strm.serializer_cnt);	/* Number of frame/time_slot per param set */

	/* Reload Param set */
	memcpy(&mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx + DMA_RELOAD],
		   &mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx],
		   sizeof (mcasp_card->edma3->PaRAM[mcasp_card->play_strm.dma_idx]));

	mcasp_card->play_strm.subchn.pcm_config = config;
	mcasp_card->play_strm.subchn.pcm_subchn = subchn;
	mcasp_card->play_strm.subchn.strm = &mcasp_card->play_strm;
	*pc = &mcasp_card->play_strm.subchn;
	ado_mutex_unlock(&mcasp_card->hw_lock);
	return (EOK);
}

int32_t mcasp_playback_release(HW_CONTEXT_T * mcasp_card, PCM_SUBCHN_CONTEXT_T * pc, ado_pcm_config_t * config)
{

	ado_mutex_lock(&mcasp_card->hw_lock);
	pc->pcm_subchn = NULL;
	ado_shm_free(config->dmabuf.addr, config->dmabuf.size, config->dmabuf.name);
	ado_mutex_unlock(&mcasp_card->hw_lock);
	return (0);
}

int32_t
mcasp_capture_aquire(HW_CONTEXT_T * mcasp_card, PCM_SUBCHN_CONTEXT_T ** pc,
					 ado_pcm_config_t * config, ado_pcm_subchn_t * subchn, uint32_t * why_failed)
{
	int idx, opened_subchns = 0;
	PCM_SUBCHN_CONTEXT_T *ctx = NULL;
	mcasp_cap_strm_t *strm = find_capture_stream(&mcasp_card->cap_aif, subchn);

	ado_mutex_lock(&mcasp_card->hw_lock);
	if (strm == NULL)
	{
		*why_failed = SND_PCM_PARAMS_NO_CHANNEL;
		ado_mutex_unlock(&mcasp_card->hw_lock);
		return (EAGAIN);
	}

	/* check for available sub-channel */
	for(idx = 0; idx < strm->nsubchn; idx++)
	{
		if(NULL == strm->subchn[idx].pcm_subchn)
		{
			ctx = &strm->subchn[idx];
			break;
		}
	}
	if(NULL == ctx)
	{
		*why_failed = SND_PCM_PARAMS_NO_CHANNEL;
		ado_mutex_unlock(&mcasp_card->hw_lock);
		return (EAGAIN);
	}

	if (multiple_rates_supported(mcasp_card, SND_PCM_CHANNEL_CAPTURE))
	{
		// Is rate switch necessary?
		if (config->format.rate != mcasp_card->cap_aif.sample_rate)
		{
			if (mcasp_card->async_clks == 0 && mcasp_card->play_strm.subchn.pcm_subchn)
			{
				ado_mutex_unlock(&mcasp_card->hw_lock);
				return (EBUSY);
			}

			// check all other capture streams to see if any of them are active
			if (any_active_capture_streams(&mcasp_card->cap_aif))
			{
				ado_mutex_unlock(&mcasp_card->hw_lock);
				return (EBUSY);
			}

			if (mcasp_card->async_clks)
				set_cap_bit_clock_rate(mcasp_card, config->format.rate);
			else
				set_play_bit_clock_rate(mcasp_card, config->format.rate);
		}
	}

	if((config->dmabuf.addr = ado_shm_alloc(config->dmabuf.size, config->dmabuf.name, ADO_SHM_DMA_SAFE, &config->dmabuf.phys_addr)) == NULL)
	{
		ado_mutex_unlock(&mcasp_card->hw_lock);
		return (errno);
	}

	/* Only setup DMA for the first active subchn */
	for (idx = 0; idx < mcasp_card->cap_aif.serializer_cnt; idx++)
		opened_subchns += num_open_capture_subchn(&mcasp_card->cap_aif.cap_strm[idx]);
	if(0 == opened_subchns)
	{
		/* If configured for only 1 serializer and 1 subchn, use the subchannls pcm buffer as the DMA  buffer */
		if (mcasp_card->cap_aif.serializer_cnt == 1 && strm->nsubchn == 1)
		{
			mcasp_card->cap_aif.dmabuf = &config->dmabuf;
			mcasp_card->cap_aif.frag_size = ado_pcm_dma_int_size(config);
		}

		/* Ensure ASP and DMA are disable */
		CLR_EER(strm->dma_idx);
		mcasp_card->mcasp->RGBLCTL &= ~(RGBLCTL_RSMRST);

		memset(&mcasp_card->edma3->PaRAM[strm->dma_idx], 0x00,
			   sizeof (mcasp_card->edma3->PaRAM[strm->dma_idx]));
		mcasp_card->edma3->PaRAM[strm->dma_idx].OPT = OPT_SYNCDIM | OPT_TCINTEN | OPT_TCC(strm->dma_idx);
		mcasp_card->edma3->PaRAM[strm->dma_idx].SRC = mcasp_card->mcasp_dmax;
		mcasp_card->edma3->PaRAM[strm->dma_idx].ACNT = mcasp_card->sample_size;	/* Sample size in bytes */
		mcasp_card->edma3->PaRAM[strm->dma_idx].BCNT = mcasp_card->cap_aif.serializer_cnt;
		mcasp_card->edma3->PaRAM[strm->dma_idx].SRCBIDX = 0;
		mcasp_card->edma3->PaRAM[strm->dma_idx].DSTBIDX = mcasp_card->edma3->PaRAM[strm->dma_idx].ACNT;
		mcasp_card->edma3->PaRAM[strm->dma_idx].LINK = 0x4000 + 0x20 * (DMA_RELOAD + strm->dma_idx);
		mcasp_card->edma3->PaRAM[strm->dma_idx].BCNTRLD = mcasp_card->edma3->PaRAM[strm->dma_idx].BCNT;
		mcasp_card->edma3->PaRAM[strm->dma_idx].SRCCIDX = 0;
		mcasp_card->edma3->PaRAM[strm->dma_idx].DSTCIDX = mcasp_card->sample_size * mcasp_card->cap_aif.serializer_cnt;
		mcasp_card->edma3->PaRAM[strm->dma_idx].CCNT =
			mcasp_card->cap_aif.frag_size / (mcasp_card->sample_size * mcasp_card->cap_aif.serializer_cnt);    /* Number of frame/time_slot per param set */
		/* Reload Param set */
		memcpy(&mcasp_card->edma3->PaRAM[strm->dma_idx + DMA_RELOAD],
			   &mcasp_card->edma3->PaRAM[strm->dma_idx],
			   sizeof (mcasp_card->edma3->PaRAM[strm->dma_idx]));
	}

	ctx->pcm_config = config;
	ctx->pcm_subchn = subchn;
	ctx->strm = strm;
	*pc = ctx;
	ado_mutex_unlock(&mcasp_card->hw_lock);
	return (EOK);
}

int32_t mcasp_capture_release(HW_CONTEXT_T * mcasp_card, PCM_SUBCHN_CONTEXT_T * pc, ado_pcm_config_t * config)
{
	ado_mutex_lock(&mcasp_card->hw_lock);
	pc->pcm_subchn = NULL;
	ado_shm_free(config->dmabuf.addr, config->dmabuf.size, config->dmabuf.name);
	ado_mutex_unlock(&mcasp_card->hw_lock);
	return (0);
}


int32_t mcasp_prepare(HW_CONTEXT_T * mcasp_card, PCM_SUBCHN_CONTEXT_T * subchn, ado_pcm_config_t * config)
{
	if (subchn->strm == &mcasp_card->play_strm)
	{
		mcasp_play_strm_t *strm = subchn->strm;
		/* Set the phys DMA adder in the two param sets */
		strm->pcm_cur_frag = 0;
		mcasp_card->edma3->PaRAM[strm->dma_idx].SRC =
			config->dmabuf.phys_addr + strm->pcm_cur_frag++ * ado_pcm_dma_int_size(config);
		mcasp_card->edma3->PaRAM[strm->dma_idx + DMA_RELOAD].SRC =
			config->dmabuf.phys_addr + strm->pcm_cur_frag++ * ado_pcm_dma_int_size(config);
		/* Reset the CCNT (Frames per param set) */
		mcasp_card->edma3->PaRAM[strm->dma_idx].CCNT =
			(ado_pcm_dma_int_size(config) / (mcasp_card->sample_size * strm->serializer_cnt));
	}
	else
	{
		/* Only configure DMA on first active subchannel */
		ado_mutex_lock(&mcasp_card->hw_lock);
		if(mcasp_card->cap_aif.active == 0)
		{
			mcasp_cap_strm_t *strm = subchn->strm;
			/* Set the phys DMA adder in the two param sets */
			mcasp_card->cap_aif.pcm_cur_frag = 0;
			mcasp_card->edma3->PaRAM[strm->dma_idx].DST =
				mcasp_card->cap_aif.dmabuf->phys_addr + mcasp_card->cap_aif.pcm_cur_frag++ * mcasp_card->cap_aif.frag_size;
			mcasp_card->edma3->PaRAM[strm->dma_idx + DMA_RELOAD].DST =
				mcasp_card->cap_aif.dmabuf->phys_addr + mcasp_card->cap_aif.pcm_cur_frag++ * mcasp_card->cap_aif.frag_size;
			/* Reset the CCNT (Frames per param set) */
			mcasp_card->edma3->PaRAM[strm->dma_idx].CCNT = 
				(mcasp_card->cap_aif.frag_size / (mcasp_card->sample_size * mcasp_card->cap_aif.serializer_cnt));
		}
		ado_mutex_unlock(&mcasp_card->hw_lock);
		mcasp_card->cap_aif.pcm_completed_frag = 0;
		subchn->pcm_offset = 0; /* reset pcm offset */
	}
	return (0);
}


int32_t mcasp_trigger(HW_CONTEXT_T * mcasp_card, PCM_SUBCHN_CONTEXT_T * pc, uint32_t cmd)
{
	int timeout, i;

	ado_mutex_lock(&mcasp_card->hw_lock);
	if (cmd == ADO_PCM_TRIGGER_GO)
	{
		/* Enable ASP and DMA Channel */
		if (pc == &mcasp_card->play_strm.subchn)
		{
#if !defined(VARIANT_j2_pcm3168a)
			/* AFIFO ENABLE
			 * WNUMDMA must be equal to the number of the serializers enabled for transmit
			 * WNUMEVT must be a non-zero multiple of the serializers enabled for transmit
			 */
			mcasp_card->mcasp->WFIFOCTL = WFIFOCTL_WNUMEVT(mcasp_card->play_strm.serializer_cnt) | WFIFOCTL_WNUMDMA(mcasp_card->play_strm.serializer_cnt);
			mcasp_card->mcasp->WFIFOCTL |= WFIFOCTL_WENA;
#endif
			/* Set Tx serializer */
			for (i=0; i<NUMBER_OF_SERIALIZER; i++)
			{
				if (mcasp_card->play_strm.serializer[i])
				{
					mcasp_card->mcasp->SRCTL[i] = SRCTL_XMIT;
				}
			}

			/* Enable Tx serializer */
			timeout = 1000;
			mcasp_card->mcasp->XGBLCTL |= XGBLCTL_XSRCLR;
			while (((mcasp_card->mcasp->GBLCTL & XGBLCTL_XSRCLR) != XGBLCTL_XSRCLR) && timeout--)
				nanospin_ns(100);
			if (timeout <= 0)
				ado_error("mcasp_trigger: tx serializer not enabled");

			/* Enable DMA */
			SET_EER(mcasp_card->play_strm.dma_idx);

#if !defined(VARIANT_j2_pcm3168a)
			/* Wait for TX AFIFO to fill to ensure the hardware does not underrun when we turn it on */
			timeout = 1000;
			while (((mcasp_card->mcasp->WFIFOSTS & WFIFOSTS_WLVL) < 64) && timeout--)
				nanospin_ns(100);
			if (timeout <= 0)
				ado_error("mcasp_trigger: AFIFO is not full");
#endif
			/* Wait for data to be moved from XBUF to XRSR (shift register) before turning it on */ 
			timeout = 1000;
			while (((mcasp_card->mcasp->XSTAT & XSTAT_XDATA)) && timeout--)
				nanospin_ns(100);
			if (timeout <= 0)
				ado_error("mcasp_trigger: TX FIFO is never filled by EDMA");

			/* Clear underrun flag */
			mcasp_card->mcasp->XSTAT |= XSTAT_XUNDRN;

			/* Enable underrun interrupt */
			mcasp_card->mcasp->XINTCTL = XINTCTL_XUNDRN;

			/* Release TX State machine from reset */
			timeout = 1000;
			mcasp_card->mcasp->XGBLCTL |= XGBLCTL_XSMRST;
			while (((mcasp_card->mcasp->GBLCTL & XGBLCTL_XSMRST) != XGBLCTL_XSMRST) && timeout--)
				nanospin_ns(100);
			if (timeout <= 0)
				ado_error("mcasp_trigger: unable to release TX State Machine from reset");
		}
		else
		{
			if (mcasp_card->cap_aif.active == 0)
			{
				int cnt = 0;
#if !defined(VARIANT_j2_pcm3168a)
				/* AFIFO ENABLE 
				 * RNUMDMA must be equal to the number of the serializers enabled for receive 
				 * RNUMEVT must be a non-zero multiple of the serializers enabled for receive
				 */
				mcasp_card->mcasp->RFIFOCTL = RFIFOCTL_RNUMEVT(mcasp_card->cap_aif.serializer_cnt) | RFIFOCTL_RNUMDMA(mcasp_card->cap_aif.serializer_cnt);
				mcasp_card->mcasp->RFIFOCTL |= RFIFOCTL_RENA;
#endif
				for (cnt = 0; cnt < mcasp_card->cap_aif.serializer_cnt; cnt++)
					mcasp_card->mcasp->SRCTL[mcasp_card->cap_aif.cap_strm[cnt].serializer] = SRCTL_RECV;

				/* Enable serializer */
				timeout = 1000;
				mcasp_card->mcasp->RGBLCTL |= RGBLCTL_RSRCLR;
				while (((mcasp_card->mcasp->GBLCTL & RGBLCTL_RSRCLR) != RGBLCTL_RSRCLR) && timeout--)
					nanospin_ns(100);
				if (timeout <= 0)
					ado_error("mcasp_trigger: rx serializer not enabled");

				/* Enable DMA */
				SET_EER(mcasp_card->cap_aif.cap_strm[0].dma_idx);

				/* Clear overrun flag */
				mcasp_card->mcasp->RSTAT |= RSTAT_ROVRN;

				/* Enable overrun interrupt */
				mcasp_card->mcasp->RINTCTL = RINTCTL_ROVRN;

				/* Release RX State machine from reset */
				timeout = 1000;
				mcasp_card->mcasp->RGBLCTL |= RGBLCTL_RSMRST;
				while (((mcasp_card->mcasp->GBLCTL & RGBLCTL_RSMRST) != RGBLCTL_RSMRST) && timeout--)
					nanospin_ns(100);
				if (timeout <= 0)
					ado_error("mcasp_trigger: unable to release RX State Machine from reset");
			}
			mcasp_card->cap_aif.active++;	/* Total active subchannel cnt */
			pc->go = 1;	/* Subchannel is active */
		}
	}
	else
	{
		/* Disable ASP and DMA Channel */
		if (pc == &mcasp_card->play_strm.subchn)
		{
			/* Disable underrun interrupt */
			mcasp_card->mcasp->XINTCTL &= ~XINTCTL_XUNDRN;

			/* Disable DMA */
			CLR_EER(mcasp_card->play_strm.dma_idx);

#if !defined(VARIANT_j2_pcm3168a)
			/* Ensure fifo is empty */
			timeout = 50000;
			while (mcasp_card->mcasp->WFIFOSTS && timeout--)
				nanospin_ns(10);
			if (timeout <= 0)
				ado_debug(DB_LVL_DRIVER, "TX AFIFO was not emptied!");
#endif
			/* Ensure TXBUF is empty */
			timeout = 1000;
			while ((!(mcasp_card->mcasp->XSTAT & XSTAT_XDATA)) && timeout--)
				nanospin_ns(100);

			if (timeout <= 0)
				ado_error("mcasp_trigger: TXBUF was not emptied");

			/* Disable TX State Machine */
			mcasp_card->mcasp->XGBLCTL &= ~(XGBLCTL_XSMRST);

			/* Reset TX serializer */
			mcasp_card->mcasp->XGBLCTL &= ~XGBLCTL_XSRCLR;
			for (i=0; i<NUMBER_OF_SERIALIZER; i++)
			{
				if (mcasp_card->play_strm.serializer[i])
				{
					mcasp_card->mcasp->SRCTL[i] = 0;
				}
			}

#if !defined(VARIANT_j2_pcm3168a)
			/* AFIFO disable */
			mcasp_card->mcasp->WFIFOCTL = 0;
#endif
		}
		else
		{
			int cnt;
			if(--mcasp_card->cap_aif.active == 0)
			{
				/* Disable overrun interrupt */
				mcasp_card->mcasp->RINTCTL &= ~RINTCTL_ROVRN;
				/* Reset RX serializer */
				mcasp_card->mcasp->RGBLCTL &= ~RGBLCTL_RSRCLR;
				for (cnt = 0; cnt < mcasp_card->cap_aif.serializer_cnt; cnt++)
					mcasp_card->mcasp->SRCTL[mcasp_card->cap_aif.cap_strm[cnt].serializer] = 0;
				/* Disable RX State Machine */
				mcasp_card->mcasp->RGBLCTL &= ~(RGBLCTL_RSMRST);
				/* Disable DMA */
				CLR_EER(mcasp_card->cap_aif.cap_strm[0].dma_idx);
#if !defined(VARIANT_j2_pcm3168a)
				/* AFIFO disable */
				mcasp_card->mcasp->RFIFOCTL = 0;
#endif
			}
			pc->go = 0;
		}
	}
	ado_mutex_unlock(&mcasp_card->hw_lock);
	return (0);
}

uint32_t mcasp_position(HW_CONTEXT_T * mcasp_card, PCM_SUBCHN_CONTEXT_T * pc, ado_pcm_config_t * config)
{
	uint32_t pos;

	ado_mutex_lock(&mcasp_card->hw_lock);
	
	if (pc->pcm_subchn == mcasp_card->play_strm.subchn.pcm_subchn)
	{
		mcasp_play_strm_t *strm = pc->strm;

		/* AB-Synchronized Transfers: position = Frag size - (samples left * tx_serializer_count * sample size) */
		pos = ado_pcm_dma_int_size(config) - (mcasp_card->edma3->PaRAM[strm->dma_idx].CCNT * mcasp_card->edma3->PaRAM[strm->dma_idx].SRCCIDX);
	}
	else
	{
		mcasp_cap_strm_t *strm = pc->strm;
	
		/* AB-Synchronized Transfers: position = Frag size - (samples left * rx_serializer_count * sample size) */
		pos = ado_pcm_dma_int_size(config) - (mcasp_card->edma3->PaRAM[strm->dma_idx].CCNT * mcasp_card->edma3->PaRAM[strm->dma_idx].SRCCIDX);
	}
	ado_mutex_unlock(&mcasp_card->hw_lock);
	return (pos);
}

void mcasp_rx_interrupt(mcasp_card_t * mcasp_card, int32_t irq)
{
	ado_error("mcasp_card: RX Overrun occured!");
	mcasp_card->mcasp->RSTAT |= RSTAT_ROVRN;
}

void mcasp_tx_interrupt(mcasp_card_t * mcasp_card, int32_t irq)
{
#if !defined(VARIANT_j2_pcm3168a)
	ado_error("mcasp_card: TX Underrun occured! - TX AFIFO count = %d", mcasp_card->mcasp->WFIFOSTS & WFIFOSTS_WLVL);
#else
	ado_error("mcasp_card: TX Underrun occured!");
#endif
	mcasp_card->mcasp->XSTAT |= XSTAT_XUNDRN;
}

void mcasp_play_interrupt(mcasp_card_t * mcasp_card, int32_t irq)
{
	ado_pcm_config_t *config;
	int status = 0;

	ado_mutex_lock(&mcasp_card->hw_lock);
	/* Setup next param set */
	if (mcasp_card->play_strm.subchn.pcm_subchn)
	{
		config = mcasp_card->play_strm.subchn.pcm_config;
		if (mcasp_card->play_strm.pcm_cur_frag * ado_pcm_dma_int_size(config) >= config->dmabuf.size)
			mcasp_card->play_strm.pcm_cur_frag = 0;

		mcasp_card->edma3->PaRAM[DMA_RELOAD + mcasp_card->play_strm.dma_idx].SRC =
			config->dmabuf.phys_addr + mcasp_card->play_strm.pcm_cur_frag++ * ado_pcm_dma_int_size(config);
		status = mcasp_card->play_strm.dma_idx;
	}

	ado_mutex_unlock(&mcasp_card->hw_lock);

	if (status == mcasp_card->play_strm.dma_idx)
		dma_interrupt(mcasp_card->play_strm.subchn.pcm_subchn);
}

/**
 * This function is used when more than 1 capture subchannel have been enabled. It is called from
 * rx interrupt handler (or pulse). It manually copies data from global dma buffer to client
 * PCM buffer and notifies io-audio when a complete fragment has been transferred.
 */
static void
subchn_dmacapture(HW_CONTEXT_T *mcasp_card, uint8_t *srcDMAAddr, uint32_t size)
{
	int32_t idx               = 0;
	int32_t bytesToTransfer   = 0;
	int32_t bytesTransferred  = 0;
	mcasp_subchn_t *ctx;
	ado_pcm_config_t *config = NULL;

	if((NULL == srcDMAAddr) || (0 == size))
	{
		ado_error("mcasp: subchn_dmacapture invalid data or size");
		return;
	}

	for(idx = 0; idx < mcasp_card->cap_aif.cap_strm[0].nsubchn; idx++)
	{
		ctx = &mcasp_card->cap_aif.cap_strm[0].subchn[idx];
		config = ctx->pcm_config;
		if((NULL != ctx->pcm_subchn) && (1 == ctx->go))
		{
			bytesTransferred = 0;
			bytesToTransfer  = 0;
			while(bytesTransferred < size)
			{
				/* Make sure we interrupt for each fragment. Below check ensures that we
				 * copy enough bytes to complete one fragment at a time and interrupts io-audio.
				 */
				if(ctx->pcm_offset < ado_pcm_dma_int_size(config))
					bytesToTransfer = MIN((ado_pcm_dma_int_size(config) - ctx->pcm_offset), (size - bytesTransferred));
				else
					bytesToTransfer = MIN((config->dmabuf.size - ctx->pcm_offset), (size - bytesTransferred));
				memcpy((uint8_t *)(config->dmabuf.addr + ctx->pcm_offset), &srcDMAAddr[bytesTransferred], bytesToTransfer);
				ctx->pcm_offset = (ctx->pcm_offset + bytesToTransfer) % config->dmabuf.size;
				if ((ctx->pcm_offset % ado_pcm_dma_int_size(config)) == 0)
				{
					/* Signal io-audio (DMA transfer was completed) */
					dma_interrupt(ctx->pcm_subchn);
				}
				bytesTransferred += bytesToTransfer;
			}
		}
	}
}

/**
 * This function is used when more than 1 capture serializers have been enabled. It is called from
 * rx interrupt handler (or pulse). Data from each serializer is interleaved into a single DMA buffer
 * this routine splits the interleaved data into separate "virtual" device buffers/streams and notifies
 * io-audio when a full fragment worth of data is transfered per "virtual" device/stream. 
 */
static void
serializer_dmacapture(HW_CONTEXT_T *mcasp_card, uint8_t *srcDMAAddr, uint32_t size)
{
	int32_t idx, sample_offset = 0, cnt;
	mcasp_subchn_t *ctx;
	uint16_t *buf16;
	uint32_t *buf32;
	uint16_t *dmaptr16 = (uint16_t*) srcDMAAddr;
	uint32_t *dmaptr32 = (uint32_t*) srcDMAAddr;

	if((NULL == srcDMAAddr) || (0 == size))
	{
		ado_error("mcasp: subchn_dmacapture invalid data or size");
		return;
	}
	
	if (mcasp_card->cap_aif.serializer_cnt == 1)
		subchn_dmacapture(mcasp_card, srcDMAAddr, size);
	else
	{
		/* Note: pcm_offset is in samples not bytes, so we must convert to bytes by multiplying by the sample size */
		while ((sample_offset * mcasp_card->sample_size) < size)
		{
			for (cnt = 0; cnt < mcasp_card->cap_aif.serializer_cnt; cnt++ )
			{
				for(idx = 0; idx < mcasp_card->cap_aif.cap_strm[cnt].nsubchn; idx++)
				{
					ctx = &mcasp_card->cap_aif.cap_strm[cnt].subchn[idx];
					if((NULL != ctx->pcm_subchn) && (1 == ctx->go))
					{
						if ((ctx->pcm_offset * mcasp_card->sample_size ) >= ctx->pcm_config->dmabuf.size)
							ctx->pcm_offset = 0;
						if (mcasp_card->sample_size == 2)
						{
							buf16 = (uint16_t*) ctx->pcm_config->dmabuf.addr + ctx->pcm_offset;
							*buf16 = dmaptr16[sample_offset]; 
						}
						else
						{
							buf32 = (uint32_t*) ctx->pcm_config->dmabuf.addr + ctx->pcm_offset;
							*buf32 = dmaptr32[sample_offset]; 
						}
						ctx->pcm_offset++;
						if (((ctx->pcm_offset * mcasp_card->sample_size) % ado_pcm_dma_int_size(ctx->pcm_config)) == 0)
						{
							// Signal to io-audio (DMA transfer was completed)
							dma_interrupt(ctx->pcm_subchn);
						}
					}
				}
				sample_offset++;
			}
		}
	}
}

void mcasp_cap_interrupt(mcasp_card_t * mcasp_card, int32_t irq)
{
	ado_pcm_config_t *config;

	ado_mutex_lock(&mcasp_card->hw_lock);
	/* Setup next param set */
	if (mcasp_card->cap_aif.active > 0)
	{ 
		if (mcasp_card->cap_aif.serializer_cnt == 1 && mcasp_card->cap_aif.cap_strm[0].nsubchn == 1)
		{
			config = mcasp_card->cap_aif.cap_strm[0].subchn[0].pcm_config;
			if (mcasp_card->cap_aif.pcm_cur_frag * ado_pcm_dma_int_size(config) >= config->dmabuf.size)
				mcasp_card->cap_aif.pcm_cur_frag = 0;

			mcasp_card->edma3->PaRAM[DMA_RELOAD + mcasp_card->cap_aif.cap_strm[0].dma_idx].DST =
				config->dmabuf.phys_addr + mcasp_card->cap_aif.pcm_cur_frag++ * ado_pcm_dma_int_size(config);
		}
		else
		{
			if (mcasp_card->cap_aif.pcm_cur_frag * mcasp_card->cap_aif.frag_size >= (mcasp_card->cap_aif.frag_size * NUM_CAPTURE_DMA_FRAGS))
			{
				mcasp_card->cap_aif.pcm_cur_frag = 0;
			}
			mcasp_card->edma3->PaRAM[DMA_RELOAD + mcasp_card->cap_aif.cap_strm[0].dma_idx].DST =
				mcasp_card->cap_aif.dmabuf->phys_addr + mcasp_card->cap_aif.pcm_cur_frag++ * mcasp_card->cap_aif.frag_size;
		}
	}

	ado_mutex_unlock(&mcasp_card->hw_lock);

	if(mcasp_card->cap_aif.active > 0)
	{
		if(mcasp_card->cap_aif.serializer_cnt == 1 && mcasp_card->cap_aif.cap_strm[0].nsubchn == 1)
			dma_interrupt(mcasp_card->cap_aif.cap_strm[0].subchn[0].pcm_subchn);
		else
		{
			int offset = mcasp_card->cap_aif.pcm_completed_frag++ * mcasp_card->cap_aif.frag_size;
			if (mcasp_card->cap_aif.pcm_completed_frag >= NUM_CAPTURE_DMA_FRAGS)
				mcasp_card->cap_aif.pcm_completed_frag = 0;
			serializer_dmacapture(mcasp_card, (uint8_t*)(&mcasp_card->cap_aif.dmabuf->addr[offset]), mcasp_card->cap_aif.frag_size);
		}
	}
}
void configure_default_protocol_flags(HW_CONTEXT_T * mcasp, int channel)
{
	switch (channel)
	{
		case SND_PCM_CHANNEL_PLAYBACK:
			switch (mcasp->play_strm.protocol)
			{
				case PROTOCOL_TDM_PCM:                  // PCM Mode 1
					mcasp->play_strm.clk_pol = 1;       // tx on rise
					mcasp->play_strm.bit_delay = 0;     // 0 bit delay
					mcasp->play_strm.fsync_pol = 1;     // tx frame sync active high
					break;
				case PROTOCOL_TDM_I2S:
					mcasp->play_strm.clk_pol = 0;       // tx on fall
					mcasp->play_strm.bit_delay = 1;     // 1 bit delay
					if (mcasp->slot_num == 2)
						mcasp->play_strm.fsync_pol = 0; // tx frame sync active low
					else
						mcasp->play_strm.fsync_pol = 1; // tx frame sync active high
					break;
				case PROTOCOL_TDM_LJ:
					mcasp->play_strm.clk_pol = 0;       // tx on fall
					mcasp->play_strm.bit_delay = 0;     // 1 bit delay
					mcasp->play_strm.fsync_pol = 1;     // tx frame sync active high
					break;
				default:
					break;
			}
			break;
		case SND_PCM_CHANNEL_CAPTURE:
			switch (mcasp->cap_aif.protocol)
			{
				case PROTOCOL_TDM_PCM:                  // PCM Mode 1
					mcasp->cap_aif.clk_pol = 0;         // rx on fall
					mcasp->cap_aif.bit_delay = 0;       // 0 bit delay
					mcasp->cap_aif.fsync_pol = 1;       // rx frame sync active high
					break;
				case PROTOCOL_TDM_I2S:
					mcasp->cap_aif.clk_pol = 1;         // rx on rise
					mcasp->cap_aif.bit_delay = 1;       // 1 bit delay
					if (mcasp->slot_num == 2)
						mcasp->cap_aif.fsync_pol = 0;   // rx frame sync active low
					else
						mcasp->cap_aif.fsync_pol = 1;   // rx frame sync active high
					break;
				case PROTOCOL_TDM_LJ:
					mcasp->cap_aif.clk_pol = 1;         // rx on rise
					mcasp->cap_aif.bit_delay = 0;       // 1 bit delay
					mcasp->cap_aif.fsync_pol = 1;       // rx frame sync active high
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

int
mcasp_parse_commandline(HW_CONTEXT_T * mcasp_card, char *args)
{
	int opt = 0, i;
	int xbit_delay = -1, xfsync_pol = -1, xclk_pol = -1;
	int rbit_delay = -1, rfsync_pol = -1, rclk_pol = -1;
	char *value;
	int rx_fragsize = DEFAULT_CAPTURE_DMA_FRAG_SIZE;
	char *opts[] = {
		"i2c_addr",          // 0
		"clk_mode",          // 1
		"mcasp",             // 2
		"i2c_dev",           // 3
		"tx_voices",         // 4
		"rx_voices",         // 5
		"xprotocol",         // 6
		"rprotocol",         // 7
		"sample_size",       // 8
		"ahxclk",            // 9
		"ahrclk",            // 10
		"mclk",              // 11
		"tx_serializer",     // 12
		"rx_serializer",     // 13
		"async_clks",        // 14
		"fs_active_width",   // 15
		"sample_rate",       // 16
		"xrate",             // 17
		"rrate",             // 18
		"slot_size",         // 19
		"slot_num",          // 20
		"capture_subchn",    // 21
		"dma_fragsize",      // 22
		"xbit_delay",        // 23
		"rbit_delay",        // 24
		"xfsync_pol",        // 25
		"rfsync_pol",        // 26
		"xclk_pol",          // 27
		"rclk_pol",          // 28
		"loopback",          // 29
		NULL
	};

	mcasp_card->clk_mode = CLOCK_MODE;
	mcasp_card->mcasp_baseaddr = MCASP0_BASEADDR;
	mcasp_card->mcasp_dmax = MCASP0_dMAX;
	mcasp_card->play_strm.dma_idx = MCASP0_DMA_PLAY;
	mcasp_card->cap_aif.cap_strm[0].dma_idx = MCASP0_DMA_CAP;
	mcasp_card->codec_i2c_addr = CODEC_SLAVE_ADDR1;
	mcasp_card->i2c_dev = 0xff;					 // board-codec layer will overwrite this value
	mcasp_card->play_strm.voices = TX_VOICES;
	mcasp_card->cap_aif.cap_strm[0].voices = RX_VOICES;
	mcasp_card->mclk = MCLK;
	mcasp_card->ahclkx.io = AHCLKX_DIR;
	mcasp_card->ahclkr.io = AHCLKR_DIR;
	mcasp_card->ahclkx.num_freqs = 1;
	mcasp_card->ahclkr.num_freqs = 1;
	mcasp_card->ahclkx.freqs[0] = AHCLKX_FREQ;
	mcasp_card->ahclkr.freqs[0] = AHCLKR_FREQ;
	mcasp_card->play_strm.sample_rate = PLAYBACK_SAMPLE_RATE;
	mcasp_card->play_strm.sample_rate_min = PLAYBACK_SAMPLE_RATE;
	mcasp_card->play_strm.sample_rate_max = PLAYBACK_SAMPLE_RATE;
	mcasp_card->cap_aif.sample_rate = CAPTURE_SAMPLE_RATE;
	mcasp_card->cap_aif.sample_rate_min = CAPTURE_SAMPLE_RATE;
	mcasp_card->cap_aif.sample_rate_max = CAPTURE_SAMPLE_RATE;
	mcasp_card->play_strm.protocol = PROTOCOL;
	mcasp_card->cap_aif.protocol = PROTOCOL;
	mcasp_card->sample_size = SAMPLE_SIZE;
	for (i=0; i<NUMBER_OF_SERIALIZER; i++)
	{
		mcasp_card->play_strm.serializer[i] = 0;
	}
	mcasp_card->cap_aif.serializer_cnt = 1;	
	mcasp_card->cap_aif.cap_strm[0].serializer = RX_SERIALIZER;
	mcasp_card->async_clks = ASYNC_CLKS;
	mcasp_card->fs_active_width = FS_ACTIVE_WIDTH;
	mcasp_card->slot_size = TDM_SLOT_SIZE;
	mcasp_card->slot_num = TDM_NSLOTS;
	for (i=0; i < NUMBER_OF_SERIALIZER; i++)
		mcasp_card->cap_aif.cap_strm[i].nsubchn = 1;

	while (args != NULL && args[0] != 0)
	{
		switch ((opt = getsubopt(&args, opts, &value)))
		{
			case 0:
				if (value != NULL)
				{
					mcasp_card->codec_i2c_addr = strtoul(value, NULL, 0);
					ado_debug(DB_LVL_DRIVER, "mcasp_card: i2c_addr %d", mcasp_card->codec_i2c_addr);
				}
				break;
			case 1:
				if (value != NULL)
				{
					if (strcmp(value, "master") == 0)
						mcasp_card->clk_mode = MASTER;
					else if (strcmp(value, "slave") == 0)
						mcasp_card->clk_mode = SLAVE;
					ado_debug(DB_LVL_DRIVER, "mcasp_card: clk mode = %s",
							  (mcasp_card->clk_mode == 0) ? "Master" : "Slave");
				}
				break;
			case 2:
				if (value != NULL)
				{
					mcasp_card->mcasp_idx = atoi(value);
#if defined (VARIANT_J6)
					if (mcasp_card->mcasp_idx < 0 || mcasp_card->mcasp_idx > 7)
#else
					if (mcasp_card->mcasp_idx < 0 || mcasp_card->mcasp_idx > 5)
#endif
					{
						mcasp_card->mcasp_idx = 2;
						ado_debug(DB_LVL_DRIVER, "mcasp_card: Invalid MCASP index, using default index (MCASP2)");
					}
					else
						ado_debug(DB_LVL_DRIVER, "mcasp_card: Using MCASP%d", mcasp_card->mcasp_idx);
					switch (mcasp_card->mcasp_idx)
					{
						case 0:
							mcasp_card->mcasp_baseaddr = MCASP0_BASEADDR;
							mcasp_card->mcasp_dmax = MCASP0_dMAX;
							mcasp_card->play_strm.dma_idx = MCASP0_DMA_PLAY;
							mcasp_card->cap_aif.cap_strm[0].dma_idx = MCASP0_DMA_CAP;
							mcasp_card->play_strm.irq = MCASP0_TX_IRQ;
							mcasp_card->cap_aif.cap_strm[0].irq = MCASP0_RX_IRQ;
							break;
						case 1:
							mcasp_card->mcasp_baseaddr = MCASP1_BASEADDR;
							mcasp_card->mcasp_dmax = MCASP1_dMAX;
							mcasp_card->play_strm.dma_idx = MCASP1_DMA_PLAY;
							mcasp_card->cap_aif.cap_strm[0].dma_idx = MCASP1_DMA_CAP;
							mcasp_card->play_strm.irq = MCASP1_TX_IRQ;
							mcasp_card->cap_aif.cap_strm[0].irq = MCASP1_RX_IRQ;
							break;
						case 2:
							mcasp_card->mcasp_baseaddr = MCASP2_BASEADDR;
							mcasp_card->mcasp_dmax = MCASP2_dMAX;
							mcasp_card->play_strm.dma_idx = MCASP2_DMA_PLAY;
							mcasp_card->cap_aif.cap_strm[0].dma_idx = MCASP2_DMA_CAP;
							mcasp_card->play_strm.irq = MCASP2_TX_IRQ;
							mcasp_card->cap_aif.cap_strm[0].irq = MCASP2_RX_IRQ;
							break;
						case 3:
							mcasp_card->mcasp_baseaddr = MCASP3_BASEADDR;
							mcasp_card->mcasp_dmax = MCASP3_dMAX;
							mcasp_card->play_strm.dma_idx = MCASP3_DMA_PLAY;
							mcasp_card->cap_aif.cap_strm[0].dma_idx = MCASP3_DMA_CAP;
							mcasp_card->play_strm.irq = MCASP3_TX_IRQ;
							mcasp_card->cap_aif.cap_strm[0].irq = MCASP3_RX_IRQ;
							break;
						case 4:
							mcasp_card->mcasp_baseaddr = MCASP4_BASEADDR;
							mcasp_card->mcasp_dmax = MCASP4_dMAX;
							mcasp_card->play_strm.dma_idx = MCASP4_DMA_PLAY;
							mcasp_card->cap_aif.cap_strm[0].dma_idx = MCASP4_DMA_CAP;
							mcasp_card->play_strm.irq = MCASP4_TX_IRQ;
							mcasp_card->cap_aif.cap_strm[0].irq = MCASP4_RX_IRQ;
							break;
						case 5:
							mcasp_card->mcasp_baseaddr = MCASP5_BASEADDR;
							mcasp_card->mcasp_dmax = MCASP5_dMAX;
							mcasp_card->play_strm.dma_idx = MCASP5_DMA_PLAY;
							mcasp_card->cap_aif.cap_strm[0].dma_idx = MCASP5_DMA_CAP;
							mcasp_card->play_strm.irq = MCASP5_TX_IRQ;
							mcasp_card->cap_aif.cap_strm[0].irq = MCASP5_RX_IRQ;
							break;
#if defined (VARIANT_J6)
						case 6:
							mcasp_card->mcasp_baseaddr = MCASP6_BASEADDR;
							mcasp_card->mcasp_dmax = MCASP6_dMAX;
							mcasp_card->play_strm.dma_idx = MCASP6_DMA_PLAY;
							mcasp_card->cap_aif.cap_strm[0].dma_idx = MCASP6_DMA_CAP;
							mcasp_card->play_strm.irq = MCASP6_TX_IRQ;
							mcasp_card->cap_aif.cap_strm[0].irq = MCASP6_RX_IRQ;
							break;
						case 7:
							mcasp_card->mcasp_baseaddr = MCASP7_BASEADDR;
							mcasp_card->mcasp_dmax = MCASP7_dMAX;
							mcasp_card->play_strm.dma_idx = MCASP7_DMA_PLAY;
							mcasp_card->cap_aif.cap_strm[0].dma_idx = MCASP7_DMA_CAP;
							mcasp_card->play_strm.irq = MCASP7_TX_IRQ;
							mcasp_card->cap_aif.cap_strm[0].irq = MCASP7_RX_IRQ;
							break;
#endif
						default:
							break;
					}
				}
				break;
			case 3:
				if (value != NULL)
				{
					mcasp_card->i2c_dev = atoi(value);
					if (mcasp_card->i2c_dev < 0 || mcasp_card->i2c_dev > 2)
					{
						mcasp_card->i2c_dev = 0xff;	// let board-codec layer determine the default value
						ado_error("mcasp: Invalid i2c_dev value (card will decide what to connect to)");
					}
				}
				break;
			case 4:
				if (value != NULL)
				{
					mcasp_card->play_strm.voices = atoi(value);
					if (mcasp_card->play_strm.voices > 8)
					{
						mcasp_card->play_strm.voices = TX_VOICES;
						ado_debug(DB_LVL_DRIVER, "mcasp: Invalid tx channels, using tx_voices=%d",
								  mcasp_card->play_strm.voices);
					}
				}
				break;
			case 5:
				if (value != NULL)
				{
					mcasp_card->cap_aif.cap_strm[0].voices = atoi(value);
					if (mcasp_card->cap_aif.cap_strm[0].voices > 8)
					{
						mcasp_card->cap_aif.cap_strm[0].voices = RX_VOICES;
						ado_debug(DB_LVL_DRIVER, "mcasp: Invalid rx channels, using rx_voices=%d",
									mcasp_card->cap_aif.cap_strm[0].voices);
					}
				}
				break;
			case 6:
				if (value != NULL && *value != NULL)
				{
					if (strcmp(value, "tdm_i2s") == 0)
					{
						mcasp_card->play_strm.protocol = PROTOCOL_TDM_I2S;
						ado_debug(DB_LVL_DRIVER, "mcasp: Transmit protocol set to tdm_i2s");
					}
					else if (strcmp(value, "tdm_lj") == 0)
					{
						mcasp_card->play_strm.protocol = PROTOCOL_TDM_LJ;
						ado_debug(DB_LVL_DRIVER, "mcasp: Transmit protocol set to tdm_lj");
					}
					else if (strcmp(value, "tdm_pcm") == 0)
					{
						mcasp_card->play_strm.protocol = PROTOCOL_TDM_PCM;
						ado_debug(DB_LVL_DRIVER, "mcasp: Transmit protocol set to tdm_pcm");
					}

					else if (strcmp(value, "spdif") == 0)
					{
						mcasp_card->play_strm.protocol = PROTOCOL_SPDIF;
						mcasp_card->async_clks = 1;	/* Async clocks must be enabled for spdif */
						ado_debug(DB_LVL_DRIVER, "mcasp: Transmit protocol set to spdif");
					}
					else
					{
						ado_error("mcasp: Transmit protocol not supported");
						return (EINVAL);
					}
				}
				break;
			case 7:
				if (value != NULL && *value != NULL)
				{
					if (strcmp(value, "tdm_i2s") == 0)
					{
						mcasp_card->cap_aif.protocol = PROTOCOL_TDM_I2S;
						ado_debug(DB_LVL_DRIVER, "mcasp: Receive protocol set to tdm_i2s");
					}
					else if (strcmp(value, "tdm_lj") == 0)
					{
						mcasp_card->cap_aif.protocol = PROTOCOL_TDM_LJ;
						ado_debug(DB_LVL_DRIVER, "mcasp: Receive protocol set to tdm_lj");
					}
					else if (strcmp(value, "tdm_pcm") == 0)
					{
						mcasp_card->cap_aif.protocol = PROTOCOL_TDM_PCM;
						ado_debug(DB_LVL_DRIVER, "mcasp: Receive protocol set to tdm_pcm");
					}
					else
					{
						ado_error("mcasp: Receive protocol not supported");
						return (EINVAL);
					}
				}
				break;

			case 8:
				if (value != NULL)
				{
					mcasp_card->sample_size = atoi(value);
				}
				break;
			case 9:
				if (value && *value != NULL)
				{
					struct ahclk *ahclkx = &mcasp_card->ahclkx;
					if (strncmp(value, "input", 5) == 0)
						ahclkx->io = INPUT;
					else if (strncmp(value, "output", 6) == 0)
						ahclkx->io = OUTPUT;

					char *value2 = strchr(value, ':');
					if (value2)
					{
						ahclkx->num_freqs = get_num_list_from_string(ahclkx->freqs, ARRAY_SIZE(ahclkx->freqs), value2+1, ':');
					}
				}
				break;
			case 10:
				if (value && *value != NULL)
				{
					struct ahclk *ahclkr = &mcasp_card->ahclkr;
					if (strncmp(value, "input", 5) == 0)
						ahclkr->io = INPUT;
					else if (strncmp(value, "output", 6) == 0)
						ahclkr->io = OUTPUT;

					char *value2 = strchr(value, ':');
					if (value2)
					{
						ahclkr->num_freqs = get_num_list_from_string(ahclkr->freqs, ARRAY_SIZE(ahclkr->freqs), value2+1, ':');
					}
				}
				break;
			case 11:
				if (value != NULL)
				{
					mcasp_card->mclk = atoi(value);
				}
				break;
			case 12:
				if (value != NULL)
				{
					char *value2;
					int idx;

					idx = strtoul(value, 0, 0);
					if ((idx >= 0) && (idx < NUMBER_OF_SERIALIZER))
					{
						mcasp_card->play_strm.serializer[idx] = SERIALIZER_ENABLED;
						mcasp_card->play_strm.serializer_cnt = 1;
					}
					while ((value2 = strchr(value, ':')) != NULL)
					{
						idx = strtoul(value2 + 1, 0, 0);
						if ((idx >= 0) && (idx < NUMBER_OF_SERIALIZER))
						{
							mcasp_card->play_strm.serializer[idx] = SERIALIZER_ENABLED;
							mcasp_card->play_strm.serializer_cnt++;
						}
						value = value2+1;
					}
				}
				break;
			case 13:
				if (value != NULL)
				{
					char *value2;
					int idx = 1;

					mcasp_card->cap_aif.cap_strm[0].serializer = strtoul(value, 0, 0);
					mcasp_card->cap_aif.serializer_cnt = 1;
					while (idx < NUMBER_OF_SERIALIZER && (value2 = strchr(value, ':')) != NULL)
					{
						mcasp_card->cap_aif.cap_strm[idx++].serializer = strtoul(value2 + 1, 0, 0);
						mcasp_card->cap_aif.serializer_cnt++;
						value = value2+1;
					}
				}
				break;
			case 14:
				if (value != NULL)
				{
					mcasp_card->async_clks = atoi(value);
					if (mcasp_card->play_strm.protocol == PROTOCOL_SPDIF && mcasp_card->async_clks == 0)
					{
						ado_error("mcasp: SPDIF protocol requries async clocks, forcing async enabled");
						mcasp_card->async_clks = 1;
					}
				}
				break;
			case 15:
				if (value && *value != NULL)
				{
					if (strcmp(value, "word") == 0)
						mcasp_card->fs_active_width = FS_WORD;
					else if (strcmp(value, "bit") == 0)
						mcasp_card->fs_active_width = FS_BIT;
				}
				break;
			case 16:
				if (value != NULL)	/* Legacy sample_rate option, value applies to both playback and capture */
				{
					unsigned rates[2];
					unsigned n = get_num_list_from_string(rates, ARRAY_SIZE(rates), value, ':');

					mcasp_card->play_strm.sample_rate_min = rates[0];
					mcasp_card->play_strm.sample_rate_max = (n > 1) ? rates[1] : mcasp_card->play_strm.sample_rate_min;

					if (n < 1 || ado_pcm_rate2flag(mcasp_card->play_strm.sample_rate_min) == 0)
					{
						ado_error("Invalid min_rate in sample_rate: %s", value);
						return EINVAL;
					}

					if (mcasp_card->play_strm.sample_rate_max < mcasp_card->play_strm.sample_rate_min
						|| ado_pcm_rate2flag(mcasp_card->play_strm.sample_rate_max) == 0)
					{
						ado_error("Invalid max_rate in sample_rate: %s", value);
						return EINVAL;
					}
					mcasp_card->cap_aif.sample_rate_min = mcasp_card->play_strm.sample_rate_min;
					mcasp_card->cap_aif.sample_rate_max = mcasp_card->play_strm.sample_rate_max;
				}
				break;
			case 17:
				if (value != NULL)
				{
					unsigned rates[2];
					unsigned n = get_num_list_from_string(rates, ARRAY_SIZE(rates), value, ':');

					mcasp_card->play_strm.sample_rate_min = rates[0];
					mcasp_card->play_strm.sample_rate_max = (n > 1) ? rates[1] : mcasp_card->play_strm.sample_rate_min;

					if (n < 1 || ado_pcm_rate2flag(mcasp_card->play_strm.sample_rate_min) == 0)
					{
						ado_error("Invalid playback min_rate in sample_rate: %s", value);
						return EINVAL;
					}

					if (mcasp_card->play_strm.sample_rate_max < mcasp_card->play_strm.sample_rate_min
						|| ado_pcm_rate2flag(mcasp_card->play_strm.sample_rate_max) == 0)
					{
						ado_error("Invalid playback max_rate in sample_rate: %s", value);
						return EINVAL;
					}
				}
				break;
			case 18:
				if (value != NULL)
				{
					unsigned rates[2];
					unsigned n = get_num_list_from_string(rates, ARRAY_SIZE(rates), value, ':');

					mcasp_card->cap_aif.sample_rate_min = rates[0];
					mcasp_card->cap_aif.sample_rate_max = (n > 1) ? rates[1] : mcasp_card->cap_aif.sample_rate_min;

					if (n < 1 || ado_pcm_rate2flag(mcasp_card->cap_aif.sample_rate_min) == 0)
					{
						ado_error("Invalid capture min_rate in sample_rate: %s", value);
						return EINVAL;
					}

					if (mcasp_card->cap_aif.sample_rate_max < mcasp_card->cap_aif.sample_rate_min
						|| ado_pcm_rate2flag(mcasp_card->cap_aif.sample_rate_max) == 0)
					{
						ado_error("Invalid capture max_rate in sample_rate: %s", value);
						return EINVAL;
					}
				}
				break;
			case 19:
				if (value != NULL)
				{
					mcasp_card->slot_size = atoi(value);
				}
				break;
			case 20:
				if (value != NULL)
				{
					mcasp_card->slot_num = atoi(value);
				}
				break;
			case 21:
				if (value != NULL)
				{
					char *value2;
					int idx = 1;

					mcasp_card->cap_aif.cap_strm[0].nsubchn = strtoul(value, 0, 0);
					if(mcasp_card->cap_aif.cap_strm[0].nsubchn < 1 || mcasp_card->cap_aif.cap_strm[0].nsubchn > MAX_CAP_SUBCHN_COUNT)
					{
						ado_debug(DB_LVL_DRIVER, "mcasp: Invalid number of capture subchannels (must be between 1 and %d) (default to 1)", MAX_CAP_SUBCHN_COUNT);
						mcasp_card->cap_aif.cap_strm[0].nsubchn = 1;
					}
					while (idx < NUMBER_OF_SERIALIZER && (value2 = strchr(value, ':')) != NULL)
					{
						mcasp_card->cap_aif.cap_strm[idx].nsubchn = strtoul(value2 + 1, 0, 0);
						value = value2+1;
						if(mcasp_card->cap_aif.cap_strm[idx].nsubchn < 1 || mcasp_card->cap_aif.cap_strm[idx].nsubchn > MAX_CAP_SUBCHN_COUNT)
						{
							ado_debug(DB_LVL_DRIVER, "mcasp: Invalid number of capture subchannels (must be between 1 and %d) (default to 1)", MAX_CAP_SUBCHN_COUNT);
							mcasp_card->cap_aif.cap_strm[idx].nsubchn = 1;
						}
						idx++;
					}
				}
				break;
			case 22:
				if (value != NULL)
				{
					rx_fragsize = atoi(value);
					if (rx_fragsize <= 0)
					{
						rx_fragsize = DEFAULT_CAPTURE_DMA_FRAG_SIZE;
						ado_debug(DB_LVL_DRIVER, "mcasp: Invalid rx_fragsize, defaulting to %d", DEFAULT_CAPTURE_DMA_FRAG_SIZE);
					}
				}
				break;
			case 23:
				xbit_delay = atoi (value);
				if (xbit_delay > 2 || xbit_delay < 0)
				{
					ado_error ("Invalid TX bit_delay value (0, 1 or 2)");
					return EINVAL;
				}
				break;
			case 24:
				rbit_delay = atoi (value);
				if (rbit_delay > 2 || rbit_delay < 0)
				{
					ado_error ("Invalid RX bit_delay value (0, 1 or 2)");
					return EINVAL;
				}
				break;
			case 25:
				xfsync_pol = atoi (value);
				if (xfsync_pol > 1 || xfsync_pol < 0)
				{
					ado_error ("Invalid xfsync_pol value");
					return EINVAL;
				}
				break;
			case 26:
				rfsync_pol = atoi (value);
				if (rfsync_pol > 1 || rfsync_pol < 0)
				{
					ado_error ("Invalid rfsync_pol value");
					return EINVAL;
				}
				break;
			case 27:
				xclk_pol = atoi (value);
				if (xclk_pol > 1 || xclk_pol < 0)
				{
					ado_error ("Invalid xclk polarity mode");
					return EINVAL;
				}
				break;
			case 28:
				rclk_pol = atoi (value);
				if (rclk_pol > 1 || rclk_pol < 0)
				{
					ado_error ("Invalid rclk polarity mode");
					return EINVAL;
				}
				break;
			case 29:
				mcasp_card->loopback = 1;
				break;
			default:
				break;
		}
	}

	/* Validate sample size and convert to bytes */
	switch ( mcasp_card->sample_size )
	{
		case 2:
		case 16:
			mcasp_card->sample_size = 2;
			break;
		case 4:
		case 32:
			mcasp_card->sample_size = 4;
			break;
		default:
			ado_error("Invalid sample size defaulting to %d-bit", (SAMPLE_SIZE == 2 || SAMPLE_SIZE == 16) ? 16 : 32);
			fprintf(stderr, "Invalid sample size defaulting to %d-bit\n", (SAMPLE_SIZE == 2 || SAMPLE_SIZE == 16) ? 16 : 32);
			mcasp_card->sample_size = (SAMPLE_SIZE == 2 || SAMPLE_SIZE == 16) ? 2 : 4;
			break;
	}

	/* Validate slot size and convert to bits */
	switch ( mcasp_card->slot_size )
	{
		case 2:
		case 16:
			mcasp_card->slot_size = 16;
			break;
		case 4:
		case 32:
			mcasp_card->slot_size = 32;
			break;
		default:
			ado_error("Invalid slot size defaulting to %d-bit", (TDM_SLOT_SIZE == 2 || TDM_SLOT_SIZE == 16) ? 16 : 32);
			fprintf(stderr, "Invalid slot size defaulting to %d-bit\n", (TDM_SLOT_SIZE == 2 || TDM_SLOT_SIZE == 16) ? 16 : 32);
			mcasp_card->slot_size = (TDM_SLOT_SIZE == 2 || TDM_SLOT_SIZE == 16) ? 16 : 32;
			break;
	}

	if ((mcasp_card->sample_size * _BITS_BYTE) > mcasp_card->slot_size)
	{
		ado_error("Invalid configuration: sample size cannot be greater then TDM slot size");
		fprintf(stderr, "Invalid configuration: sample size cannot be greater then TDM slot size\n");
		return (EINVAL);
	}

	if (mcasp_card->play_strm.voices > mcasp_card->slot_num)
	{
		ado_error("Invalid configuration: tx_voices cannot be greater then slot_num");
		fprintf(stderr, "Invalid configuration: tx_voices cannot be greater then slot_num\n");
		return (EINVAL);
	}

	/* Validate clocks */
	if (mcasp_card->async_clks == 0)
	{
		if (mcasp_card->play_strm.sample_rate_min != mcasp_card->cap_aif.sample_rate_min || mcasp_card->play_strm.sample_rate_max != mcasp_card->cap_aif.sample_rate_max)
		{
			ado_error("Invalid configuration: In synchronous clock mode playback sample rate must equal the capture sample rate");
			fprintf(stderr, "Invalid configuration: In synchronous clock mode playback sample rate must equal the capture sample rate\n");
			return (EINVAL);
		}
	}

	configure_default_protocol_flags(mcasp_card, SND_PCM_CHANNEL_PLAYBACK);
	/* Override protocol parameters with command line values */
	if (xclk_pol != -1)
		mcasp_card->play_strm.clk_pol = xclk_pol;
	if (xbit_delay != -1)
		mcasp_card->play_strm.bit_delay = xbit_delay;
	if (xfsync_pol != -1)
		mcasp_card->play_strm.fsync_pol = xfsync_pol;

	configure_default_protocol_flags(mcasp_card, SND_PCM_CHANNEL_CAPTURE);
	/* Override protocol parameters with command line values */
	if (rclk_pol != -1)
		mcasp_card->cap_aif.clk_pol = rclk_pol;
	if (rbit_delay != -1)
		mcasp_card->cap_aif.bit_delay = rbit_delay;
	if (rfsync_pol != -1)
		mcasp_card->cap_aif.fsync_pol = rfsync_pol;

	/* If TX serializer was not set via command line then apply the default as per variant.h definition */
	if (mcasp_card->play_strm.serializer_cnt == 0)
	{
		mcasp_card->play_strm.serializer_cnt = 1;
		mcasp_card->play_strm.serializer[TX_SERIALIZER] = SERIALIZER_ENABLED;
	}

	if (mcasp_card->cap_aif.serializer_cnt > 1 || mcasp_card->cap_aif.cap_strm[0].nsubchn > 1)
	{
		/* Align the DMA buffer to the audio frame size */
		int frame_size = mcasp_card->sample_size * mcasp_card->cap_aif.cap_strm[0].voices * mcasp_card->cap_aif.serializer_cnt;

		mcasp_card->cap_aif.frag_size = rx_fragsize / frame_size * frame_size;

		ado_debug(DB_LVL_DRIVER, "%s: Requested RX fragsize = %d, Frame size alignment = %d, Actual RX Fragsize = %d",
					__FUNCTION__, rx_fragsize, frame_size, mcasp_card->cap_aif.frag_size);
	}

	return(EOK);
}

int32_t
mcasp_playback_reconstitute16 (HW_CONTEXT_T * mcasp_card, PCM_SUBCHN_CONTEXT_T * pc, int8_t * dmaptr, size_t size)
{
	int16_t i, src_idx, dst_idx, iter, *ptr;
	uint16_t *buffer = mcasp_card->play_strm.reconstitute_buffer;

	ptr = (int16_t *) dmaptr;

	/* 
	 * When using multiple serializers, each sample shifted out will go
	 * to the next active serializer, so we must re-organize the samples 
	 * to get the expected output (i.e. Left and right samples on the same serializer
	 * rather then slpit over multiple serializers).
	 */
	for (i = 0; i < (size / sizeof (int16_t)); i+=(mcasp_card->play_strm.voices * mcasp_card->play_strm.serializer_cnt))
	{
		dst_idx = 0;
		for (iter = 0; dst_idx < (mcasp_card->play_strm.voices * mcasp_card->play_strm.serializer_cnt); iter++)
		{	
			for(src_idx = iter; src_idx < (mcasp_card->play_strm.voices * mcasp_card->play_strm.serializer_cnt); src_idx+=2, dst_idx++)
				buffer[dst_idx] = ptr[i + src_idx];
		}
		memcpy (&ptr[i], &buffer[0], (mcasp_card->play_strm.voices * mcasp_card->play_strm.serializer_cnt)*sizeof(int16_t));
	}

	return (0);
}

int32_t
mcasp_playback_reconstitute32 (HW_CONTEXT_T * mcasp_card, PCM_SUBCHN_CONTEXT_T * pc, int8_t * dmaptr, size_t size)
{
	int32_t i, src_idx, dst_idx, iter, *ptr;
	uint32_t *buffer = mcasp_card->play_strm.reconstitute_buffer;

	ptr = (int32_t *) dmaptr;

	/* 
	 * When using multiple serializers, each sample shifted out will go
	 * to the next active serializer, so we must re-organize the samples 
	 * to get the expected output (i.e. Left and right samples on the same serializer
	 * rather then slpit over multiple serializers).
	 */
	for (i = 0; i < (size / sizeof (int32_t)); i+=(mcasp_card->play_strm.voices * mcasp_card->play_strm.serializer_cnt))
	{
		dst_idx = 0;
		for (iter = 0; dst_idx < (mcasp_card->play_strm.voices * mcasp_card->play_strm.serializer_cnt); iter++)
		{	
			for(src_idx = iter; src_idx < (mcasp_card->play_strm.voices * mcasp_card->play_strm.serializer_cnt); src_idx+=2, dst_idx++)
				buffer[dst_idx] = ptr[i + src_idx];
		}
		memcpy (&ptr[i], &buffer[0], (mcasp_card->play_strm.voices * mcasp_card->play_strm.serializer_cnt)*sizeof(int32_t));
	}

	return (0);
}

void mcasp_init(mcasp_card_t * mcasp_card)
{
	uint16_t divider;
	int i;
	uint32_t regVal;
	int timeout;

	/* Setup MCASP for Playback */
	mcasp_card->mcasp->GBLCTL = 0;				 // Reset McASP
	mcasp_card->mcasp->PWRDEMU = PWRDEMU_FREE;	 // Free-running mode

	if (mcasp_card->play_strm.protocol == PROTOCOL_SPDIF)
	{
		/* SPDIF is only supported on TX so only hit the X registers */
		/* SPDIF is fixed at 32-bit slot size */
		mcasp_card->mcasp->XFMT = XFMT_XSSZ_32BIT;

		if (mcasp_card->sample_size == 2)
		{
			/* SPDIF is right aligned but audio data must always be in bit positions 23-0, 
			 * so for 16bit we must shift to the left by 8bits (i.e. rotate right by 24bits)
			 */
			mcasp_card->mcasp->XMASK = 0x0000ffff;	/* Mask upper 16 bits */
			mcasp_card->mcasp->XFMT |= (0x6); /* Rotate right by 24bits (i.e. shift left by 8bits) */
		}
		else
		{
			/* SPDIF is right aligned, but McASP in DIT mode does not support 32bit data
			 * so convert to 24bit by masking the 8 LSB bits and rotating right by 8bits.
			 */
			mcasp_card->mcasp->XMASK = 0xffffff00;	 /* mask upper 8 bits */
			mcasp_card->mcasp->XFMT |= (0x2); /* Rotate right by 8bits */
		}

		/* XMOD for SPDIF (DIT) mode, Rising edge FS, 1-bit frame sync pulse width */
		mcasp_card->mcasp->AFSXCTL = AFSXCTL_XMOD_384_DIT;
		
		/* Enable DIT Mode */
		mcasp_card->mcasp->DITCTL = DITCTL_DITEN;
	}
	else
	{
		if (mcasp_card->slot_size == 16)
		{
			/* 16-bit */
			mcasp_card->mcasp->XMASK = 0x0000ffff;	 /* Mask upper 16 bits */
			mcasp_card->mcasp->RMASK = 0x0000ffff;	 /* Mask upper 16 bits */
			mcasp_card->mcasp->XFMT = XFMT_XRVRS_MSB | XFMT_XSSZ_16BIT | (0x4);	/* I2S is left aligned so XROT = word size of 16 bits */
			mcasp_card->mcasp->RFMT = RFMT_RRVRS_MSB | RFMT_RSSZ_16BIT | (0x0);	/* I2S is left aligned so RROT = (slot - word) = 16 - 16 = 0 bits */
		}
		else
		{
			/* 32-bit */
			mcasp_card->mcasp->XMASK = 0xffffffff;	 /* Don't mask any bits */
			mcasp_card->mcasp->RMASK = 0xffffffff;	 /* Don't mask any bits */
			mcasp_card->mcasp->XFMT = XFMT_XRVRS_MSB | XFMT_XSSZ_32BIT;
			mcasp_card->mcasp->RFMT = RFMT_RRVRS_MSB | RFMT_RSSZ_32BIT;

			/* If only using 16 of the 32bits in the slot then rotate by 16 bits
			 * to get the sample left justified within the slot
			 */
			if (mcasp_card->sample_size == 2)
			{
				mcasp_card->mcasp->XMASK = 0x0000ffff;	/* Mask upper 16 bits */
				mcasp_card->mcasp->RMASK = 0x0000ffff;	/* Mask upper 16 bits */
				mcasp_card->mcasp->XFMT |= 0x4;		 /* I2S is left algined so XROT = word size (16 bits) */
				mcasp_card->mcasp->RFMT |= 0x4;		 /* I2S is left algibed so RROT = (slot - word) = 32 - 16 = 16 */
			}
		}

		if (mcasp_card->slot_num == 4)
		{
			mcasp_card->mcasp->AFSXCTL = AFSXCTL_XMOD_4CH;	//FS period = 4 words
			mcasp_card->mcasp->AFSRCTL = AFSRCTL_RMOD_4CH;
		}
		else if (mcasp_card->slot_num == 6)
		{
			mcasp_card->mcasp->AFSXCTL = AFSXCTL_XMOD_6CH;	// FS period = 6 words
			mcasp_card->mcasp->AFSRCTL = AFSRCTL_RMOD_6CH;
		}
		else if (mcasp_card->slot_num == 8)
		{
			mcasp_card->mcasp->AFSXCTL = AFSXCTL_XMOD_8CH;	// FS period = 8 words
			mcasp_card->mcasp->AFSRCTL = AFSRCTL_RMOD_8CH;
		}
		else
		{
			mcasp_card->mcasp->AFSXCTL = AFSXCTL_XMOD_2CH;	// FS period = 2 words
			mcasp_card->mcasp->AFSRCTL = AFSRCTL_RMOD_2CH;
		}
		
		if (mcasp_card->fs_active_width == FS_WORD)
		{
			/* 1-word frame sync pulse width */
			mcasp_card->mcasp->AFSXCTL |= AFSXCTL_FXWID;
			mcasp_card->mcasp->AFSRCTL |= AFSRCTL_FRWID;
		}
		else
		{
			/* 1-bit frame sync pulse width */
			mcasp_card->mcasp->AFSXCTL &= ~(AFSXCTL_FXWID);
			mcasp_card->mcasp->AFSRCTL &= ~(AFSRCTL_FRWID);
		}

		if (mcasp_card->play_strm.bit_delay == 0)
			mcasp_card->mcasp->XFMT &= ~(XFMT_XDATDLY_1BIT | XFMT_XDATDLY_2BIT);
		else if (mcasp_card->play_strm.bit_delay == 1)
			mcasp_card->mcasp->XFMT |= XFMT_XDATDLY_1BIT;
		else
			mcasp_card->mcasp->XFMT |= XFMT_XDATDLY_2BIT;

		if (mcasp_card->cap_aif.bit_delay == 0)
			mcasp_card->mcasp->RFMT &= ~(RFMT_RDATDLY_1BIT | RFMT_RDATDLY_2BIT);
		else if (mcasp_card->cap_aif.bit_delay == 1)
			mcasp_card->mcasp->RFMT |= RFMT_RDATDLY_1BIT;
		else
			mcasp_card->mcasp->RFMT |= RFMT_RDATDLY_2BIT;
	
		if (mcasp_card->play_strm.fsync_pol == 0)
			mcasp_card->mcasp->AFSXCTL |= AFSXCTL_FSXP;	
		else
			mcasp_card->mcasp->AFSXCTL &= ~(AFSXCTL_FSXP);

		if (mcasp_card->cap_aif.fsync_pol == 0)
			mcasp_card->mcasp->AFSRCTL |= AFSRCTL_FSRP;
		else
			mcasp_card->mcasp->AFSRCTL &= ~(AFSRCTL_FSRP);

		if (mcasp_card->play_strm.clk_pol == 0)
			mcasp_card->mcasp->ACLKXCTL = ACLKXCTL_CLKXP;
		else
			mcasp_card->mcasp->ACLKXCTL &= ~(ACLKXCTL_CLKXP);

		if (mcasp_card->cap_aif.clk_pol == 0)
			mcasp_card->mcasp->ACLKRCTL &= ~(ACLKRCTL_CLKRP);
		else
			mcasp_card->mcasp->ACLKRCTL = ACLKRCTL_CLKRP;
	}

	/* High frequency xclk is an output so calculate clock rate and set pin direction */
	if (mcasp_card->ahclkx.io == OUTPUT)
	{
		/* Must program 1 less then desired value for clock dividers */
		divider = (mcasp_card->mclk / mcasp_card->ahclkx.freqs[0]) - 1;
		mcasp_card->mcasp->AHCLKXCTL = AHCLKXCTL_HCLKXM | divider;	/*  High Frequency Clock */
	}
	else
		mcasp_card->mcasp->AHCLKXCTL = 0x0;		 /* External High Frequency Clock */

	/* High frequency xclk is an output so calculate clock rate and set pin direction */
	if (mcasp_card->ahclkr.io == OUTPUT && mcasp_card->async_clks)
	{
		divider = (mcasp_card->mclk / mcasp_card->ahclkr.freqs[0]) - 1;
		mcasp_card->mcasp->AHCLKRCTL = AHCLKRCTL_HCLKRM | divider;	/* Internal High Frequency Clock */
	}
	else
	{
		/* External high frequency clock or RX clks sourced from TX clks */
		mcasp_card->mcasp->AHCLKRCTL = 0;
	}

	if (mcasp_card->clk_mode == SLAVE)
	{
		if (mcasp_card->async_clks == 1)
			mcasp_card->mcasp->ACLKXCTL |= ACLKXCTL_ASYNC;
		else
			mcasp_card->mcasp->ACLKXCTL &= ~(ACLKXCTL_ASYNC);

		/* External Frame sync */
		mcasp_card->mcasp->AFSXCTL &= ~(AFSXCTL_FSXM);
		mcasp_card->mcasp->AFSRCTL &= ~(AFSRCTL_FSRM);

		/* External Bit clk */
		mcasp_card->mcasp->ACLKXCTL &= ~(ACLKXCTL_CLKXM);
		mcasp_card->mcasp->ACLKRCTL &= ~(ACLKRCTL_CLKRM);
	}
	else  // mcasp_card->clk_mode == MASTER
	{
		if (mcasp_card->play_strm.protocol == PROTOCOL_SPDIF)
		{
			mcasp_card->mcasp->ACLKXCTL  = ACLKXCTL_CLKXM;	// Internal BCLK
			mcasp_card->mcasp->AFSXCTL  |= AFSXCTL_FSXM;	// Internal Frame Sync
		}
		else
		{
			mcasp_card->mcasp->ACLKXCTL |= ACLKXCTL_CLKXM;	// Internal BCLK
			mcasp_card->mcasp->AFSXCTL  |= AFSXCTL_FSXM;	// Internal Frame Sync
		}

		mcasp_card->ahclkx.afs_ctl = mcasp_card->mcasp->AFSXCTL;  // save original settings

		set_play_bit_clock_rate(mcasp_card, mcasp_card->play_strm.sample_rate_max);

		if (mcasp_card->async_clks == 1)
		{
			mcasp_card->mcasp->ACLKXCTL |= ACLKXCTL_ASYNC;

			if (mcasp_card->cap_aif.cap_strm[0].voices > 0)
			{
				mcasp_card->mcasp->ACLKRCTL |= ACLKRCTL_CLKRM;	// Internal BCLK
				mcasp_card->mcasp->AFSRCTL  |= AFSRCTL_FSRM;	// Internal Frame Sync

				set_cap_bit_clock_rate(mcasp_card, mcasp_card->cap_aif.sample_rate_max);
			}
		}
		else
		{
			mcasp_card->mcasp->ACLKXCTL &= ~(ACLKXCTL_ASYNC);
		}
	}

	/* Enable TX slots */
	if (mcasp_card->play_strm.protocol == PROTOCOL_SPDIF)
	{
		regVal = 0xFFFFFFFF; /* All active slots for DIT transfers */
		mcasp_card->mcasp->XTDM = regVal;
		mcasp_card->mcasp->XINTCTL = 0;
	}
	else
	{
		if (mcasp_card->play_strm.voices > 0)
		{
			/* Enable TX slots 
			 * The configuration of the XTDM register to enable slots applies to all enabled serializers.
			 * (i.e. if you have 2 channels on serialzer 0, you must have 2 channels on serializer 1)
			 * We use the tx_voices driver configuration to determine the number of voices/channels per serializer,
			 * we will then multiply the tx_voices by the serializer count to determine the overall number of 
			 * voices/channels for the audio stream down in the ctrl_init.
			 */
			regVal = 0;
			for (i = TX_TDM_SLOT_START; i < (TX_TDM_SLOT_START + mcasp_card->play_strm.voices); i++)
			{
				regVal |= TDMS(i);
			}
			mcasp_card->mcasp->XTDM = regVal;
			mcasp_card->mcasp->XINTCTL = 0;
		}

		if (mcasp_card->cap_aif.cap_strm[0].voices > 0)
		{
			/* Enable RX slots
			 * The configuration of the RTDM register to enable slots applies to all enabled serializers.
			 * (i.e. if you have 2 channels on serialzer 0, you must have 2 channels on serializer 1)
			 * We use the rx_voices driver configuration to determine the number of voices/channels per serializer,
			 * we will then multiply the rx_voices by the serializer count to determine the overall number of 
			 * voices/channels for the audio stream down in the ctrl_init.
			 */
			regVal = 0;
			for (i = RX_TDM_SLOT_START; i < (RX_TDM_SLOT_START + mcasp_card->cap_aif.cap_strm[0].voices); i++)
			{
				regVal |= TDMS(i);
			}
			mcasp_card->mcasp->RTDM = regVal;
			mcasp_card->mcasp->RINTCTL = 0;
		}
	}

	/* Configure all pins to function as a MCASP pin */
	mcasp_card->mcasp->PFUNC = PFUNC_MCASP;
	mcasp_card->mcasp->PDIR = 0;
	/* Set TX pin direction */
	for (i=0; i<NUMBER_OF_SERIALIZER; i++)
	{
		if (mcasp_card->play_strm.serializer[i])
		{
			mcasp_card->mcasp->PDIR |= PDIR_AXR_OUT(i);
		}
	}
	if (mcasp_card->ahclkx.io == OUTPUT)
		mcasp_card->mcasp->PDIR |= (PDIR_AHCLKX_OUT);	/* set AHCLK as output */
	if (mcasp_card->ahclkr.io == OUTPUT && mcasp_card->async_clks)
		mcasp_card->mcasp->PDIR |= (PDIR_AHCLKR_OUT);	/* set AHCLK as output */
	if (mcasp_card->clk_mode == MASTER)
	{
		/* BCLK an FSYNC lines are output */
		mcasp_card->mcasp->PDIR |= (PDIR_ACLKX_OUT | PDIR_AFSX_OUT);
		if (mcasp_card->async_clks == 1 && mcasp_card->cap_aif.cap_strm[0].voices > 0)
			mcasp_card->mcasp->PDIR |= (PDIR_ACLKR_OUT | PDIR_AFSR_OUT);
	}

	/* If High Frequency xclk is an output then enable High Frequency xclk generator */
	if (mcasp_card->ahclkx.io == OUTPUT)
	{
		/* Release TX High Frequency clk divider from reset */
		timeout = 1000;
		mcasp_card->mcasp->XGBLCTL |= XGBLCTL_XHCLKRST;
		while (((mcasp_card->mcasp->GBLCTL & XGBLCTL_XHCLKRST) != XGBLCTL_XHCLKRST) && timeout--)
			nanospin_ns(100);
		if (timeout <= 0)
			ado_error("%s: unable to release TX clk divider from reset", __func__);
	}

	/* If High Frequency xclk is an output then enable High Frequency rclk generator */
	if (mcasp_card->ahclkr.io == OUTPUT && mcasp_card->async_clks && mcasp_card->cap_aif.cap_strm[0].voices > 0)
	{
		/* Enable RX high frequency clock (required even with external clock) */
		timeout = 1000;
		mcasp_card->mcasp->RGBLCTL |= RGBLCTL_RHCLKRST;
		while (((mcasp_card->mcasp->GBLCTL & RGBLCTL_RHCLKRST) != RGBLCTL_RHCLKRST) && timeout--)
			nanospin_ns(100);
		if (timeout <= 0)
			ado_error("%s: unable to enable HF clock", __func__);
	}

	/* If McASP is clk master then enable BCLK and FSYNC generators */
	if (mcasp_card->clk_mode == MASTER)
	{
		if (mcasp_card->async_clks == 1)
		{	
			/* Release TX and RX BCLK Generators from reset */
			timeout = 1000;
			mcasp_card->mcasp->GBLCTL |= (XGBLCTL_XCLKRST | RGBLCTL_RCLKRST);
			while (((mcasp_card->mcasp->GBLCTL & (XGBLCTL_XCLKRST | RGBLCTL_RCLKRST)) != (XGBLCTL_XCLKRST | RGBLCTL_RCLKRST)) && timeout--)
				nanospin_ns(100);
			if (timeout <= 0)
				ado_error("%s: unable to release BCLK generators from reset", __func__);

			/* Release TX and RX Frame Sync Generators from reset (even with external supplied clock ) */
			timeout = 1000;
			mcasp_card->mcasp->GBLCTL |= (XGBLCTL_XFRST | RGBLCTL_RFRST);
			while (((mcasp_card->mcasp->GBLCTL & (XGBLCTL_XFRST | RGBLCTL_RFRST)) != (XGBLCTL_XFRST | RGBLCTL_RFRST)) && timeout--)
				nanospin_ns(100);
			if (timeout <= 0)
				ado_error("%s: unable to release FSYNC generators from reset", __func__);
		}
		else
		{
			/* Reset TX BCLK divider */
			timeout = 1000;
			mcasp_card->mcasp->XGBLCTL |= XGBLCTL_XCLKRST;
			while (((mcasp_card->mcasp->GBLCTL & XGBLCTL_XCLKRST) != XGBLCTL_XCLKRST) && timeout--)
				nanospin_ns(100);
			if (timeout <= 0)
				ado_error("%s: unable to release TX BCLK generator from reset", __func__);

			/* Release TX Frame Sync Generator from reset (even with external supplied clock ) */
			timeout = 1000;
			mcasp_card->mcasp->XGBLCTL |= XGBLCTL_XFRST;
			while (((mcasp_card->mcasp->GBLCTL & XGBLCTL_XFRST) != XGBLCTL_XFRST) && timeout--)
				nanospin_ns(100);
			if (timeout <= 0)
				ado_error("%s: unable to release TX FSYNC generator from reset", __func__);
		}
	}

	if (mcasp_card->loopback)
	{
		int idx;
		/* Find enabled TX serializer so that we can properly set the DLBCTL_ORD bit
		 * based on whether it is an even or odd serializer.
		 * NOTE: Loopback will only loopback adjacent serializers:
		 *       Odd = (N+1 loopback to N)
		 *       Even = (N loopback to N+1)
		 */
		for(idx = 0; idx < NUMBER_OF_SERIALIZER; idx++)
		{
			if (mcasp_card->play_strm.serializer[idx] == SERIALIZER_ENABLED)
				break;
		}
		if (idx >= NUMBER_OF_SERIALIZER)
		{
			ado_error("%s: Lookback enable failed - Invalid TX serializer", __FUNCTION__);
			mcasp_card->mcasp->DLBCTL = 0;
		}
		else
			mcasp_card->mcasp->DLBCTL = (DLBCTL_SYNC | DLBCTL_DLBEN | ((idx % 2 == 0) ? DLBCTL_ORD : 0));
	}
	else
	{
		mcasp_card->mcasp->DLBCTL = 0;
	}
}

ado_dll_version_t ctrl_version;
void ctrl_version(int *major, int *minor, char *date)
{
	*major = ADO_MAJOR_VERSION;
	*minor = 1;
	date = __DATE__;
}

static const unsigned supported_rate_list[] = {
	SND_PCM_RATE_8000,
	SND_PCM_RATE_16000,
	SND_PCM_RATE_32000,
	SND_PCM_RATE_48000,
	SND_PCM_RATE_96000,

	SND_PCM_RATE_11025,
	SND_PCM_RATE_22050,
	SND_PCM_RATE_44100,
	SND_PCM_RATE_88200,
};

static
unsigned find_supported_rates(struct ahclk *ahclk, unsigned rate_min, unsigned rate_max)
{
	unsigned rates = 0;
	int i;
	for (i = 0; i < ARRAY_SIZE(supported_rate_list); i++)
	{
		unsigned sr   = supported_rate_list[i];
		unsigned rate = ado_pcm_flag2rate(sr);

		if (rate_min <= rate && rate <= rate_max)
			if (sample_rate_divides_ahclk(ahclk, rate))
				rates |= sr;
	}
	return rates;
}

static
unsigned supported_rates(HW_CONTEXT_T *mcasp_card, int channel)
{
	if (mcasp_card->clk_mode == MASTER)
	{
		if (channel == SND_PCM_CHANNEL_PLAYBACK)
			return find_supported_rates(&mcasp_card->ahclkx, mcasp_card->play_strm.sample_rate_min, mcasp_card->play_strm.sample_rate_max);
		else
			return find_supported_rates(&mcasp_card->ahclkr, mcasp_card->cap_aif.sample_rate_min, mcasp_card->cap_aif.sample_rate_max);
	}
	else
	{
		if (channel == SND_PCM_CHANNEL_PLAYBACK)
			return ado_pcm_rate2flag(mcasp_card->play_strm.sample_rate_min);
		else
			return ado_pcm_rate2flag(mcasp_card->cap_aif.sample_rate_min);
	}
}

ado_ctrl_dll_init_t ctrl_init;
int ctrl_init(HW_CONTEXT_T ** hw_context, ado_card_t * card, char *args)
{
	int cnt = 0;
	mcasp_card_t *mcasp_card;

	ado_debug(DB_LVL_DRIVER, "CTRL_DLL_INIT: mcasp_card");
	
	if ((mcasp_card = ado_calloc(1, sizeof (mcasp_card_t))) == NULL)
	{
		ado_error("Unable to allocate memory for mcasp_card (%s)", strerror(errno));
		return -1;
	}
	*hw_context = mcasp_card;
	mcasp_card->card = card;

	ado_card_set_shortname(card, "mcasp_card");
	ado_card_set_longname(card, "mcasp_card", 0x1000);

	if (mcasp_parse_commandline(mcasp_card, args) != EOK)
	{
		ado_free(mcasp_card);
		return -1;
	}

	if (supported_rates(mcasp_card, SND_PCM_CHANNEL_PLAYBACK) == 0)
	{
		ado_error("Invalid sample rate range %u:%u - does not divide ahxclk",
				mcasp_card->play_strm.sample_rate_min, mcasp_card->play_strm.sample_rate_max);
		ado_free(mcasp_card);
		return -1;
	}

	if (mcasp_card->async_clks && supported_rates(mcasp_card, SND_PCM_CHANNEL_CAPTURE) == 0)
	{
		ado_error("Invalid sample rate range %u:%u - does not divide ahrclk",
				mcasp_card->cap_aif.sample_rate_min, mcasp_card->cap_aif.sample_rate_max);
		ado_free(mcasp_card);
		return -1;
	}

	if ((mcasp_card->mcasp = mmap_device_memory(0, sizeof (mcasp_t),
												PROT_READ | PROT_WRITE | PROT_NOCACHE, 0,
												mcasp_card->mcasp_baseaddr)) == MAP_FAILED)
	{
		ado_error("Unable to mmap mcasp (%s) \n", strerror(errno));
		ado_free(mcasp_card);
		return -1;
	}

	if ((mcasp_card->edma3 = mmap_device_memory(0, sizeof (edma3_t),
												PROT_READ | PROT_WRITE | PROT_NOCACHE, 0, EDMA3CC_ADDR)) == MAP_FAILED)
	{
		ado_error("Unable to mmap DMA (%s) \n", strerror(errno));
		ado_device_munmap(mcasp_card->mcasp, sizeof (mcasp_t));
		ado_free(mcasp_card);
		return -1;
	}

	ado_mutex_init(&mcasp_card->hw_lock);

	/* Initialize the card */
	SET_DRAE0(mcasp_card->play_strm.dma_idx);
	SET_DRAE0(mcasp_card->cap_aif.cap_strm[0].dma_idx);
	CLR_EER(mcasp_card->play_strm.dma_idx);
	CLR_EER(mcasp_card->cap_aif.cap_strm[0].dma_idx);

	mcasp_init(mcasp_card);

	/* If multiple capture subchannels and/or multiple capture serializers then allocate underlying DMA buffer */
	if (mcasp_card->cap_aif.serializer_cnt > 1 || mcasp_card->cap_aif.cap_strm[0].nsubchn > 1)
	{
		if ((mcasp_card->cap_aif.dmabuf = ado_calloc(1, sizeof(ado_pcm_dmabuf_t))) == NULL)
		{
			ado_error("Failed to allocated capture dmabuf structure - (%s)", strerror(errno));
			ado_mutex_destroy(&mcasp_card->hw_lock);
			ado_device_munmap(mcasp_card->mcasp, sizeof (mcasp_t));
			ado_device_munmap(mcasp_card->edma3, sizeof (edma3_t));
			ado_free(mcasp_card);
			return (errno);
		}
		mcasp_card->cap_aif.dmabuf->size = mcasp_card->cap_aif.frag_size * NUM_CAPTURE_DMA_FRAGS;

		if((mcasp_card->cap_aif.dmabuf->addr = ado_shm_alloc(mcasp_card->cap_aif.dmabuf->size, mcasp_card->cap_aif.dmabuf->name, ADO_SHM_DMA_SAFE, &mcasp_card->cap_aif.dmabuf->phys_addr)) == NULL)
		{
			ado_error("Failed to allocated capture DMA buffer - (%s)", strerror(errno));
			ado_mutex_destroy(&mcasp_card->hw_lock);
			ado_device_munmap(mcasp_card->mcasp, sizeof (mcasp_t));
			ado_device_munmap(mcasp_card->edma3, sizeof (edma3_t));
			ado_free(mcasp_card->cap_aif.dmabuf);
			ado_free(mcasp_card);
			return (errno);
		}
	}

	if (ado_attach_interrupt(card, (EDMA_BASE_IRQ + mcasp_card->play_strm.dma_idx), mcasp_play_interrupt, mcasp_card) != 0)
	{
		ado_error("Unable to attach playback interrupt (%s)", strerror(errno));
		ado_mutex_destroy(&mcasp_card->hw_lock);
		ado_device_munmap(mcasp_card->mcasp, sizeof (mcasp_t));
		ado_device_munmap(mcasp_card->edma3, sizeof (edma3_t));
		if (mcasp_card->cap_aif.serializer_cnt > 1 || mcasp_card->cap_aif.cap_strm[0].nsubchn > 1)
		{
			ado_shm_free(mcasp_card->cap_aif.dmabuf->addr, mcasp_card->cap_aif.dmabuf->size, mcasp_card->cap_aif.dmabuf->name);
			ado_free(mcasp_card->cap_aif.dmabuf);
		}
		ado_free(mcasp_card);
		return (-1);
	}

	if (ado_attach_interrupt(card, (EDMA_BASE_IRQ + mcasp_card->cap_aif.cap_strm[0].dma_idx), mcasp_cap_interrupt, mcasp_card) != 0)
	{
		ado_error("Unable to attach capture interrupt (%s)", strerror(errno));
		ado_mutex_destroy(&mcasp_card->hw_lock);
		ado_device_munmap(mcasp_card->mcasp, sizeof (mcasp_t));
		ado_device_munmap(mcasp_card->edma3, sizeof (edma3_t));
		if (mcasp_card->cap_aif.serializer_cnt > 1 || mcasp_card->cap_aif.cap_strm[0].nsubchn > 1)
		{
			ado_shm_free(mcasp_card->cap_aif.dmabuf->addr, mcasp_card->cap_aif.dmabuf->size, mcasp_card->cap_aif.dmabuf->name);
			ado_free(mcasp_card->cap_aif.dmabuf);
		}
		ado_free(mcasp_card);
		return (-1);
	}

	if (ado_attach_interrupt(card, mcasp_card->play_strm.irq, mcasp_tx_interrupt, mcasp_card) != 0)
	{
		ado_error("Unable to attach tx interrupt (%s)", strerror(errno));
		ado_mutex_destroy(&mcasp_card->hw_lock);
		ado_device_munmap(mcasp_card->mcasp, sizeof (mcasp_t));
		ado_device_munmap(mcasp_card->edma3, sizeof (edma3_t));
		if (mcasp_card->cap_aif.serializer_cnt > 1 || mcasp_card->cap_aif.cap_strm[0].nsubchn > 1)
		{
			ado_shm_free(mcasp_card->cap_aif.dmabuf->addr, mcasp_card->cap_aif.dmabuf->size, mcasp_card->cap_aif.dmabuf->name);
			ado_free(mcasp_card->cap_aif.dmabuf);
		}
		ado_free(mcasp_card);
		return (-1);
	}

	if (ado_attach_interrupt(card, mcasp_card->cap_aif.cap_strm[0].irq, mcasp_rx_interrupt, mcasp_card) != 0)
	{
		ado_error("Unable to attach rx interrupt (%s)", strerror(errno));
		ado_mutex_destroy(&mcasp_card->hw_lock);
		ado_device_munmap(mcasp_card->mcasp, sizeof (mcasp_t));
		ado_device_munmap(mcasp_card->edma3, sizeof (edma3_t));
		if (mcasp_card->cap_aif.serializer_cnt > 1 || mcasp_card->cap_aif.cap_strm[0].nsubchn > 1)
		{
			ado_shm_free(mcasp_card->cap_aif.dmabuf->addr, mcasp_card->cap_aif.dmabuf->size, mcasp_card->cap_aif.dmabuf->name);
			ado_free(mcasp_card->cap_aif.dmabuf);
		}
		ado_free(mcasp_card);
		return (-1);
	}

	mcasp_card->play_strm.pcm_caps.chn_flags = SND_PCM_CHNINFO_BLOCK | SND_PCM_CHNINFO_STREAM |
		SND_PCM_CHNINFO_INTERLEAVE | SND_PCM_CHNINFO_BLOCK_TRANSFER | SND_PCM_CHNINFO_MMAP | SND_PCM_CHNINFO_MMAP_VALID;
	if (mcasp_card->sample_size == 2)
		mcasp_card->play_strm.pcm_caps.formats = SND_PCM_FMT_S16_LE;
	else
		mcasp_card->play_strm.pcm_caps.formats = SND_PCM_FMT_S32_LE;

	mcasp_card->play_strm.pcm_caps.rates    = supported_rates(mcasp_card, SND_PCM_CHANNEL_PLAYBACK);
	mcasp_card->play_strm.pcm_caps.min_rate = mcasp_card->play_strm.sample_rate_min;
	mcasp_card->play_strm.pcm_caps.max_rate = mcasp_card->play_strm.sample_rate_max;
	/* Each serializer must look identical in terms of number of active slots/channels,
	 * so treat the tx_voices configuration as the number of channels per serializer.
	 */
	mcasp_card->play_strm.pcm_caps.min_voices = mcasp_card->play_strm.voices * mcasp_card->play_strm.serializer_cnt;
	mcasp_card->play_strm.pcm_caps.max_voices = mcasp_card->play_strm.voices * mcasp_card->play_strm.serializer_cnt;
	mcasp_card->play_strm.pcm_caps.min_fragsize = 64;
	mcasp_card->play_strm.pcm_caps.max_fragsize = (64 * 1024);

	memcpy(&mcasp_card->cap_aif.cap_strm[0].pcm_caps, &mcasp_card->play_strm.pcm_caps,
		   sizeof (mcasp_card->cap_aif.cap_strm[0].pcm_caps));
	if (mcasp_card->async_clks)
	{
		mcasp_card->cap_aif.cap_strm[0].pcm_caps.rates  = supported_rates(mcasp_card, SND_PCM_CHANNEL_CAPTURE);
	}
	mcasp_card->cap_aif.cap_strm[0].pcm_caps.min_voices = mcasp_card->cap_aif.cap_strm[0].voices;
	mcasp_card->cap_aif.cap_strm[0].pcm_caps.max_voices = mcasp_card->cap_aif.cap_strm[0].voices;

	/* Align the min and max capture PCM fragsizes to the capture DMA fragsize */
	if (mcasp_card->cap_aif.serializer_cnt > 1 || mcasp_card->cap_aif.cap_strm[0].nsubchn > 1)
	{
		mcasp_card->cap_aif.cap_strm[0].pcm_caps.min_fragsize = mcasp_card->cap_aif.frag_size;
		mcasp_card->cap_aif.cap_strm[0].pcm_caps.max_fragsize = (mcasp_card->cap_aif.cap_strm[0].pcm_caps.max_fragsize /
			 mcasp_card->cap_aif.frag_size) * mcasp_card->cap_aif.frag_size;
	}

	mcasp_card->play_strm.pcm_funcs.capabilities2 = mcasp_capabilities;
	mcasp_card->play_strm.pcm_funcs.aquire = mcasp_playback_aquire;
	mcasp_card->play_strm.pcm_funcs.release = mcasp_playback_release;
	mcasp_card->play_strm.pcm_funcs.prepare = mcasp_prepare;
	mcasp_card->play_strm.pcm_funcs.trigger = mcasp_trigger;
	mcasp_card->play_strm.pcm_funcs.position = mcasp_position;

	mcasp_card->cap_aif.cap_strm[0].pcm_funcs.capabilities2 = mcasp_capabilities;
	mcasp_card->cap_aif.cap_strm[0].pcm_funcs.aquire = mcasp_capture_aquire;
	mcasp_card->cap_aif.cap_strm[0].pcm_funcs.release = mcasp_capture_release;
	mcasp_card->cap_aif.cap_strm[0].pcm_funcs.prepare = mcasp_prepare;
	mcasp_card->cap_aif.cap_strm[0].pcm_funcs.trigger = mcasp_trigger;
	mcasp_card->cap_aif.cap_strm[0].pcm_funcs.position = mcasp_position;

	/* If using multiple serializers we must allocate a reconstitute buffer and set the reconstitute callback */
	if (mcasp_card->play_strm.serializer_cnt > 1)
	{
		mcasp_card->play_strm.reconstitute_buffer = ado_calloc ( 1, mcasp_card->sample_size * mcasp_card->play_strm.voices * mcasp_card->play_strm.serializer_cnt);
		if (mcasp_card->play_strm.reconstitute_buffer != NULL)
		{
			if (mcasp_card->sample_size == 2)
				mcasp_card->play_strm.pcm_funcs.reconstitute2 = mcasp_playback_reconstitute16;
			else
				mcasp_card->play_strm.pcm_funcs.reconstitute2 = mcasp_playback_reconstitute32;
		}
	}

	if (ado_pcm_create(card, "McASP PCM 0", 0, "mcasp-0",
					(mcasp_card->play_strm.voices > 0) ? 1 : 0,
					(mcasp_card->play_strm.voices > 0) ? &mcasp_card->play_strm.pcm_caps : NULL,
					(mcasp_card->play_strm.voices > 0) ? &mcasp_card->play_strm.pcm_funcs : NULL,
					(mcasp_card->cap_aif.cap_strm[0].voices > 0) ? mcasp_card->cap_aif.cap_strm[0].nsubchn : 0,
					(mcasp_card->cap_aif.cap_strm[0].voices > 0) ? &mcasp_card->cap_aif.cap_strm[0].pcm_caps : NULL,
					(mcasp_card->cap_aif.cap_strm[0].voices > 0) ? &mcasp_card->cap_aif.cap_strm[0].pcm_funcs : NULL,
					&mcasp_card->pcm))
	{
		ado_error("Unable to create pcm devices (%s)", strerror(errno));
		ado_mutex_destroy(&mcasp_card->hw_lock);
		ado_device_munmap(mcasp_card->mcasp, sizeof (mcasp_t));
		ado_device_munmap(mcasp_card->edma3, sizeof (edma3_t));
		if (mcasp_card->cap_aif.serializer_cnt > 1 || mcasp_card->cap_aif.cap_strm[0].nsubchn > 1)
		{
			ado_shm_free(mcasp_card->cap_aif.dmabuf->addr, mcasp_card->cap_aif.dmabuf->size, mcasp_card->cap_aif.dmabuf->name);
			ado_free(mcasp_card->cap_aif.dmabuf);
		}
		if (mcasp_card->play_strm.reconstitute_buffer != NULL)
		{	
			ado_free(mcasp_card->play_strm.reconstitute_buffer);
			mcasp_card->play_strm.reconstitute_buffer = NULL;
		}
		ado_free(mcasp_card);
		return (-1);
	}
	/* Make the pcm handle of the first RX serializer/device point to the base PCM device/handle */
	mcasp_card->cap_aif.cap_strm[0].pcm = mcasp_card->pcm;

	/* We treat every RX serializer as a separate capature device */
	for (cnt = 1; cnt < mcasp_card->cap_aif.serializer_cnt; cnt++)
	{
		char string1[20], string2[20];
		int tmp_serializer = mcasp_card->cap_aif.cap_strm[cnt].serializer;
		int tmp_nsubchns = mcasp_card->cap_aif.cap_strm[cnt].nsubchn;
				
		/* Fill in the capture stream structure based on the values from capture stream 0 */
		memcpy(&mcasp_card->cap_aif.cap_strm[cnt], &mcasp_card->cap_aif.cap_strm[0], sizeof (mcasp_card->cap_aif.cap_strm[0]));
		mcasp_card->cap_aif.cap_strm[cnt].serializer = tmp_serializer;
		mcasp_card->cap_aif.cap_strm[cnt].nsubchn = tmp_nsubchns;

		snprintf(string1, 20, "McASP PCM %d", cnt);
		snprintf(string2, 20, "mcasp-%d", cnt);

		if (ado_pcm_create(card, string1, 0, string2,
							0,
							NULL,
							NULL,
							mcasp_card->cap_aif.cap_strm[cnt].nsubchn,
							&mcasp_card->cap_aif.cap_strm[cnt].pcm_caps,
							&mcasp_card->cap_aif.cap_strm[cnt].pcm_funcs,
							&mcasp_card->cap_aif.cap_strm[cnt].pcm))
		{
			ado_error("Unable to create pcm devices (%s)", strerror(errno));
			ado_mutex_destroy(&mcasp_card->hw_lock);
			ado_device_munmap(mcasp_card->mcasp, sizeof (mcasp_t));
			ado_device_munmap(mcasp_card->edma3, sizeof (edma3_t));
			/* We are only in this loop is there are multiple capture serializers so the dma buffer is in existence */
			ado_shm_free(mcasp_card->cap_aif.dmabuf->addr, mcasp_card->cap_aif.dmabuf->size, mcasp_card->cap_aif.dmabuf->name);
			ado_free(mcasp_card->cap_aif.dmabuf);
			if (mcasp_card->play_strm.reconstitute_buffer != NULL)
			{	
				ado_free(mcasp_card->play_strm.reconstitute_buffer);
				mcasp_card->play_strm.reconstitute_buffer = NULL;
			}
			ado_free(mcasp_card);
			return (-1);
		}
	}

	if (codec_mixer(card, mcasp_card))
	{
		ado_error("Unable to create a mixer");
		ado_mutex_destroy(&mcasp_card->hw_lock);
		ado_device_munmap(mcasp_card->mcasp, sizeof (mcasp_t));
		ado_device_munmap(mcasp_card->edma3, sizeof (edma3_t));
		if (mcasp_card->cap_aif.serializer_cnt > 1 || mcasp_card->cap_aif.cap_strm[0].nsubchn > 1)
		{
			ado_shm_free(mcasp_card->cap_aif.dmabuf->addr, mcasp_card->cap_aif.dmabuf->size, mcasp_card->cap_aif.dmabuf->name);
			ado_free(mcasp_card->cap_aif.dmabuf);
		}
		if (mcasp_card->play_strm.reconstitute_buffer != NULL)
		{	
			ado_free(mcasp_card->play_strm.reconstitute_buffer);
			mcasp_card->play_strm.reconstitute_buffer = NULL;
		}
		ado_free(mcasp_card);
		return -1;
	}

#if defined(VARIANT_DM814X)
	if (setup_mux_select_funcs(mcasp_card))
		return (-1);
#endif

	if (mcasp_card->play_strm.voices > 0)
	{
		if (ado_pcm_sw_mix(card, mcasp_card->pcm, mcasp_card->mixer))
		{
			ado_error("Unable to create a pcm sw mixer");
			ado_mutex_destroy(&mcasp_card->hw_lock);
			ado_device_munmap(mcasp_card->mcasp, sizeof (mcasp_t));
			ado_device_munmap(mcasp_card->edma3, sizeof (edma3_t));
			if (mcasp_card->cap_aif.serializer_cnt > 1 || mcasp_card->cap_aif.cap_strm[0].nsubchn > 1)
			{
				ado_shm_free(mcasp_card->cap_aif.dmabuf->addr, mcasp_card->cap_aif.dmabuf->size, mcasp_card->cap_aif.dmabuf->name);
				ado_free(mcasp_card->cap_aif.dmabuf);
			}
			if (mcasp_card->play_strm.reconstitute_buffer != NULL)
			{
				ado_free(mcasp_card->play_strm.reconstitute_buffer);
				mcasp_card->play_strm.reconstitute_buffer = NULL;
			}
			ado_free(mcasp_card);
			return (-1);
		}
	}
	return 0;
}

ado_ctrl_dll_destroy_t ctrl_destroy;
int ctrl_destroy(HW_CONTEXT_T * mcasp_card)
{
	ado_debug(DB_LVL_DRIVER, "CTRL_DLL_DESTROY: mcasp_card");

	CLR_DRAE0(mcasp_card->play_strm.dma_idx);
	CLR_DRAE0(mcasp_card->cap_aif.cap_strm[0].dma_idx);
	ado_device_munmap(mcasp_card->mcasp, sizeof (mcasp_t));
	ado_device_munmap(mcasp_card->edma3, sizeof (edma3_t));
	if (mcasp_card->cap_aif.serializer_cnt > 1 || mcasp_card->cap_aif.cap_strm[0].nsubchn > 1)
	{
		ado_shm_free(mcasp_card->cap_aif.dmabuf->addr, mcasp_card->cap_aif.dmabuf->size, mcasp_card->cap_aif.dmabuf->name);
		ado_free(mcasp_card->cap_aif.dmabuf);
	}
	if (mcasp_card->play_strm.reconstitute_buffer != NULL)
	{
		ado_free(mcasp_card->play_strm.reconstitute_buffer);
		mcasp_card->play_strm.reconstitute_buffer = NULL;
	}
	ado_mutex_destroy(&mcasp_card->hw_lock);
	ado_free(mcasp_card);
	return (0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/deva/ctrl/mcasp/mcasp_dll.c $ $Rev: 747351 $")
#endif
