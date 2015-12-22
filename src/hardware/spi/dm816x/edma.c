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


#include "omap3spi.h"

static inline void edma_setbit(uintptr_t base, int reg, int bit)
{
	if (bit > 31)
		reg += 4, bit -= 32;

	out32(base + reg, (1 << bit));
}

void omap3_edma_disablespi(omap3_spi_t *omap3)
{
	uintptr_t	edmabase = omap3->edmavbase;
	uintptr_t	region0base = edmabase + DM6446_EDMA_REGION0;

	edma_setbit(region0base, DM6446_EDMA_EECR, omap3->edma_rx_chid);
	edma_setbit(region0base, DM6446_EDMA_EECR, omap3->edma_tx_chid);
	edma_setbit(region0base, DM6446_EDMA_ECR,  omap3->edma_rx_chid);
	edma_setbit(region0base, DM6446_EDMA_ECR,  omap3->edma_tx_chid);
	edma_setbit(region0base, DM6446_EDMA_SECR, omap3->edma_rx_chid);
	edma_setbit(region0base, DM6446_EDMA_SECR, omap3->edma_tx_chid);
	edma_setbit(edmabase,    DM6446_EDMA_EMCR, omap3->edma_rx_chid);
	edma_setbit(edmabase,    DM6446_EDMA_EMCR, omap3->edma_tx_chid);
}

int omap3_setup_edma(omap3_spi_t *omap3, int device, spi_dma_paddr_t *paddr, int len )
{
	uint32_t	acnt, bcnt, ccnt;
	edma_t	*param;

	acnt = omap3->dlen;
	bcnt = (len / omap3->dlen) & 0x7FFF;		/* We use 32KB chunk */
	ccnt = (len / omap3->dlen + 0x7FFF) >> 15;

	if (bcnt == 0)
		bcnt = 0x8000;

	omap3_edma_disablespi(omap3);

	/********************
	 * Setup Tx channel *
	 ********************/
	param = (edma_t *)(omap3->edmavbase + DM6446_EDMA_PARAM_BASE + (0x20 * omap3->edma_tx_chid));

	if (paddr->wpaddr)
		param->src     = (uint32_t)paddr->wpaddr;
	else
		param->src     = (uint32_t)paddr->rpaddr;
	param->opt         = (omap3->edma_tx_chid << 12);	/* ACNT only */
	param->abcnt       = (bcnt << 16) | acnt;
	param->dst         = omap3->pbase+OMAP3_MCSPI_CH1_TX_BUFFER_OFFSET + (0x14 * device) ;
	param->srcdstbidx  = (0 << 16) | acnt;
	param->linkbcntrld = 0xFFFF | (0x8000 << 16);
	param->srcdstcidx  = (0 << 16) | acnt;
	param->ccnt        = ccnt;

	/********************
	 * Setup Rx channel *
	 ********************/
	param = (edma_t *)(omap3->edmavbase + DM6446_EDMA_PARAM_BASE + (0x20 * omap3->edma_rx_chid));

	if (paddr->rpaddr) {
		param->dst        = (uint32_t)paddr->rpaddr;
		param->srcdstbidx = (acnt << 16) | 0;
		param->srcdstcidx = (acnt << 16) | 0;
	}
	else {
		param->dst        = omap3->pdmabuf;
		param->srcdstbidx = (0 << 16) | 0;
		param->srcdstcidx = (0 << 16) | 0;
	}
	param->src            = omap3->pbase+OMAP3_MCSPI_CH1_RX_BUFFER_OFFSET + (0x14 * device) ;
	param->abcnt          = (bcnt << 16) | acnt;
	param->linkbcntrld    = 0xFFFF | (0x8000 << 16);
	param->ccnt           = ccnt;
	param->opt            = (omap3->edma_rx_chid << 12) | (1 << 20);	/* ACNT only, interrupt enable */

	/* Enable EDMA event */
	edma_setbit(omap3->edmavbase, DM6446_EDMA_REGION0 + DM6446_EDMA_EESR, omap3->edma_rx_chid);
	edma_setbit(omap3->edmavbase, DM6446_EDMA_REGION0 + DM6446_EDMA_EESR, omap3->edma_tx_chid);

	return 0;
}


void
omap3_edma_detach(omap3_spi_t *omap3)
{
	munmap_device_memory ((void *)omap3->edmavbase, DM6446_EDMA_SIZE);
}

int32_t
omap3_edma_attach(omap3_spi_t *omap3)
{

	/* Map in DMA registers */
	if ((omap3->edmavbase = (uintptr_t)mmap_device_memory (0, DM6446_EDMA_SIZE,
				PROT_READ | PROT_WRITE | PROT_NOCACHE, 0, omap3->edmapbase)) == (uintptr_t)MAP_FAILED)
	{
		fprintf(stderr, "Unable to mmap DMA (%s) \n", strerror (errno));
		return (-1);
	}

	/*
	*********** SPI: transmit ****************
	*/
	if(omap3->edma_tx_chid == -1){
		rsrc_request_t req = { 0 };
		req.length = 1;
		req.flags = RSRCDBMGR_DMA_CHANNEL;
		if (rsrcdbmgr_attach (&req, 1) == -1)
		{
				fprintf(stderr, "Unable to aquire DMA channels (%s) \n", strerror (errno));
				munmap_device_memory ((void *)omap3->edmavbase, DM6446_EDMA_SIZE);
				return (-1);
		}
		 omap3->edma_tx_chid  = req.start;
	}
	
	/*
	*********** SPI: receive ****************
	*/
	if(omap3->edma_rx_chid == -1){
		rsrc_request_t req = { 0 };
		req.length = 1;
		req.flags = RSRCDBMGR_DMA_CHANNEL;
		if (rsrcdbmgr_attach (&req, 1) == -1)
		{
				fprintf(stderr, "Unable to aquire DMA channels (%s) \n", strerror (errno));
				omap3_edma_detach (omap3);
				return (-1);
		}
		omap3->edma_rx_chid = req.start;
	}

	return 0;
}

int omap3_init_edma(omap3_spi_t *omap3)
{
	/*
	 * Map in DMA buffer
	 */
	if (omap3->edma) {
			omap3->dmabuf = mmap(0, OMAP3_SPI_MAXDMALEN, PROT_READ|PROT_WRITE|PROT_NOCACHE, 
							MAP_ANON|MAP_PHYS|MAP_PRIVATE, NOFD, 0);
			if (omap3->dmabuf != MAP_FAILED) {
				omap3->pdmabuf =  mphys(omap3->dmabuf);
			}
	}

	if(omap3_edma_attach(omap3))  return -1;
	
	omap3_edma_disablespi(omap3);

	return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/dm816x/edma.c $ $Rev: 716134 $")
#endif
