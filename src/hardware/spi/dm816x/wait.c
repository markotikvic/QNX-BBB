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


int omap3_wait(omap3_spi_t *dev, int len)
{
	struct _pulse	 pulse;
	edma_t	*param;

	while (1) {
		if (len) {
			uint64_t	to = dev->dtime * 1000 * 50; /* 50 times for time out */
			to *= len ;	
			TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, NULL, &to, NULL);
		}
		if (MsgReceivePulse(dev->chid, &pulse, sizeof(pulse), NULL) == -1){
			fprintf(stderr, "omap3_wait: errono %s(%d), status %x\n", strerror(errno), errno, in32(dev->vbase+ OMAP3_MCSPI_IRQ_STATUS_OFFSET));
			param = (edma_t *)(dev->edmavbase + DM6446_EDMA_PARAM_BASE + (0x20 * dev->edma_rx_chid));
			fprintf(stderr, "omap3_wait: %x %x\n", param->abcnt , param->ccnt);
			return -1;
		}
//		fprintf(stderr, "omap3_wait receive pulse %x\n", pulse.code);

		switch (pulse.code) {
			case OMAP3_SPI_EVENT:
					return 0;
			case OMAP3_EDMA_EVENT:
				{
					param = (edma_t *)(dev->edmavbase + DM6446_EDMA_PARAM_BASE + (0x20 * dev->edma_rx_chid));
					/* Unmask the Interrupt */
					InterruptUnmask(dev->irq_edma, dev->iid_edma);
//					fprintf(stderr, "omap3_wait(%d): %x %x\n", __LINE__, param->abcnt , param->ccnt);
					if (param->abcnt != 0 || param->ccnt != 0){
//						fprintf(stderr, "omap3_wait(%d): %x %x\n", __LINE__, param->abcnt , param->ccnt);
						continue;
					}
					return 0;
				}
		}
	}

	return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/dm816x/wait.c $ $Rev: 680332 $")
#endif
