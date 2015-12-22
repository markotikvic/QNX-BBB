/*
 * $QNXLicenseC:
 * Copyright 2008-2013, QNX Software Systems. 
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

// Module Description:  board specific interface

#include <sim_mmc.h>
#include <sim_omap3.h>
#include <arm/omap3530.h>

/*
 * Board specific options
 * example:  
 *	devb-mmcsd-jacinto5 mmcsd ioport=0x47810100,irq=29,ioport=0x49000000,dma=5,dma=6,bs=emmc:retry=0:pwr_delay=0:bw=8
 */
static char  *opts[] = {
#define NOCD			0
	"nocd",
#define BW8				1		// Enable 8 bits data width, by default it's 4 bits
	"bw8",
#define PWR_RETRY		2		// power up/down retry. Defaults to 3
	"retry",	
#define PWR_DELAY		3		// number of milli seconds delay for power up/down, defaults to 10ms 
	"pwr_delay",	
#define SVOLTAGE		4 		// Supported volatge, default to support 3.0/3.3/1.8v
	"svoltage",
#define CLOCK_ENA_DIS	5		// Enable/disable clock when omap3_clock() is called
	"cen",
#define DTO_CNT			6		// Data timeout counter
	"dto",
#define CDR				7		// Enable or disable card detect recovery.
	"cdr", 
	NULL
};

static int netra_detect(SIM_HBA *hba)
{
	SIM_MMC_EXT	 	*ext;
	omap3_ext_t	 	*omap;
	uintptr_t		base;
	static int		state_cnt=0;
	ext	= (SIM_MMC_EXT *)hba->ext;
	omap = (omap3_ext_t *)ext->handle;
	base = omap->mmc_base;

	if (in32(base + OMAP3_MMCHS_PSTATE) & PSTATE_CINS) {
		if(omap->cdr){
			if(!(ext->eflags & MMC_EFLAG_READY) && state_cnt++ > omap->cdr){
				state_cnt = 0;
				return MMC_FAILURE;
			}
		}
		return MMC_SUCCESS;
	 } 
	 if(omap->cdr) state_cnt = 0;
	 return MMC_FAILURE;
}

/* Use ":" as separator, other than the regular ","  */
static int my_getsubopt(char **optionp, char * const *tokens, char **valuep) {
	char	*p, *opt;
	int		len, index;
	const char  *token;

	*valuep = 0;

	for (opt = p = *optionp, len = 0; *p && *p != ':'; p++, len++) {
		if (*p == '=') {
			for (*valuep = ++p; *p && *p != ':'; p++) {
				/* nothing to do */
			}
			break;
		}
	}
	if (*p) {
		*p++ = '\0';
	}
	*optionp = p;

	for (index = 0; (token = *tokens++); index++) {
		if (*token == *opt && !strncmp(token, opt, len) && token[len] == '\0') {
			return index;
		}
	}
	*valuep = opt;
	return -1;
}

int bs_init(SIM_HBA *hba)
{
	SIM_MMC_EXT		*ext;
	omap3_ext_t		*omap;
	CONFIG_INFO		*cfg;
	int		tmpint;
	char	*value, *options;
	int		opt;
	int		nocd = 0, bw8 = 0;

	cfg = (CONFIG_INFO *)&hba->cfg;

	if (!cfg->NumIOPorts) {
		cfg->IOPort_Base[0]   = 0x48060100;
		cfg->IOPort_Length[0] = OMAP3_MMCHS_SIZE;
		cfg->IOPort_Base[1]   = DRA446_EDMA_BASE;
		cfg->IOPort_Length[1] = DRA446_EDMA_SIZE;
		cfg->NumIOPorts = 2;
	} else if (cfg->NumIOPorts < 2) {
		slogf (_SLOGC_SIM_MMC, _SLOG_ERROR, "OMAP3 MMCSD: DMA4 base address must be specified");
		return MMC_FAILURE;
	}
	
	if (!cfg->NumIRQs) {
		cfg->IRQRegisters[0] = 64;
		cfg->NumIRQs = 1;
	}

	if (!cfg->NumDMAs) {
		cfg->DMALst[0] = 24;	// DMA request line for MMC1 TX
		cfg->DMALst[1] = 25;	// DMA request line for MMC1 RX
		cfg->NumDMAs = 2;
	} else if (cfg->NumDMAs == 1) {
		cfg->DMALst[1] = cfg->DMALst[1]+1;	// DMA request line for MMC1 RX
		cfg->NumDMAs = 2;
	} else if (cfg->NumDMAs < 2) {
		slogf (_SLOGC_SIM_MMC, _SLOG_ERROR, "OMAP3 MMCSD: DMA channel and Tx/Rx request event must be specified");
		return MMC_FAILURE;
	}

	if (omap3_attach(hba) != MMC_SUCCESS)
		return MMC_FAILURE;

	ext  = (SIM_MMC_EXT *)hba->ext;
	omap = (omap3_ext_t *)ext->handle;

	/* Handle board specific options. Our intention is to overwrite the default settings, so need to do it after omap3_attach() */
	options = ext->opts;
	while (options && *options != '\0') {
		if ((opt = my_getsubopt( &options, opts, &value)) == -1) {
			slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "MMCSD: invalid BS options %s", options);
			return MMC_FAILURE;
		}

		switch (opt) {
			case NOCD:
				nocd = 1;
				break;
			case BW8:
				bw8 = 1; 
				break;

			/* Based on the target board, adjust the number of power down/up retries
			 * during startup. If this option is not used, default (3) retries will be used.
			 */
			case PWR_RETRY:
				tmpint = strtol(value, 0, 0);
				if (tmpint < 0 || tmpint > 30)
					slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "MMCSD: Power down/up retries %d is out of range, using default: %d", tmpint, ext->pwr_retry);
				else
					ext->pwr_retry = tmpint;
				break;
			case PWR_DELAY: /* Default to 0 (ms), MMCSD driver will delay this amount of time after calling the power up/down functions */
				ext->pwr_delay = strtol(value, 0, 0);
				break;
			case SVOLTAGE:
				tmpint = strtol(value, 0, 0);
				if (tmpint < 0 || tmpint > 2) {
					slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "MMCSD: Support voltage %d is out of range, using default", tmpint);
				} else {
					ext->hccap &= ~(MMC_HCCAP_33V | MMC_HCCAP_30V | MMC_HCCAP_18V);

					if (tmpint == 1)
						ext->hccap |= MMC_HCCAP_30V;
					else if (tmpint == 2)
						ext->hccap |= MMC_HCCAP_33V;
					else
						ext->hccap |= MMC_HCCAP_18V;
				}
				break;
			case CLOCK_ENA_DIS:
				tmpint = strtol(value, 0, 0);
				if (tmpint < 0 || tmpint > 1) {
					slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "MMCSD: clock %d is out of range, using default %d", tmpint, 1);
				} else {
					if (tmpint)
						omap->dtoc |= SYSCTL_CEN;
					else
						omap->dtoc &= ~SYSCTL_CEN;
				}
				break;
			case DTO_CNT:
				tmpint = strtol(value, 0, 0);
				if (tmpint < 0 || tmpint > 14) {
					slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "MMCSD: dto %d is out of range, using default 0x0E", tmpint);
				} else {
					omap->dtoc &= ~SYSCTL_DTO_MASK;
					omap->dtoc |= tmpint << 16;
				}
				break;
			case CDR:
				tmpint = strtol(value, 0, 0);
				if (tmpint < 0 || tmpint > 20) {
					slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "MMCSD: cdr %d is out of range, using default %d", tmpint, 0);
				} else {
					if (tmpint) {
						if(tmpint >=3) omap->cdr = tmpint;
						else  omap->cdr = 3;
					}
					else  omap->cdr = 0;;
				}
				break;
			default:
				break;
		}
	}

	ext->hccap |= MMC_HCCAP_HS;
	omap->mmc_clock = 192000000;
	ext->clock	 = omap->mmc_clock;

	/* If the board supports card detection, override the function 
	 * pointer detect. Otherwise, just use the default function omap3_detect().
	 */
	if (!nocd) {
		ext->detect = netra_detect;
		ext->hccap |= MMC_HCCAP_CD_INTR;
		/* enable card insertion/removal interrupt */
		out32(omap->mmc_base + OMAP3_MMCHS_ISE, INTR_CINS | INTR_CREM);
	}

	/*
	 * By default, ext->hccap is set to 8 bits enabled in omap3_attach()
	 * But if no "bw8" option specified, 8 bits are not supported for backward compatible
	 */
	if (!bw8)
		ext->hccap &= ~MMC_HCCAP_BW8;

	return MMC_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int bs_dinit(SIM_HBA *hba)
{
	return (CAM_SUCCESS);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devb/mmcsd/arm/jacinto5.le.v7/sim_bs.c $ $Rev: 732094 $")
#endif
