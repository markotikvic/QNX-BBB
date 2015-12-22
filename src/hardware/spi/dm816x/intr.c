/*
 * $QNXLicenseC:
 * Copyright 2009,2013 QNX Software Systems.
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

/*
 * We use the same buffer for transmit and receive
 * For exchange, that's exactly what we wanted
 * For Read, it doesn't matter what we write to SPI, so we are OK.
 * For transmit, the receive data is put at the buffer we just transmitted, we are still OK.
 */

static const struct sigevent *spi_intr(void *area, int id)
{
	int channel_id = 0;
	uint32_t reg_value = 0;
	uintptr_t	base;
	int 		i, expected;
	omap3_spi_t	*dev = area;
	base = dev->vbase;
	
	reg_value = in32(base + OMAP3_MCSPI_IRQ_STATUS_OFFSET);

	/* The code below detects on which channel the interrupt
	 * has occured.
	 */
	if(reg_value & 0x0F)
		channel_id = OMAP3_SPI_MASTER_CHANNEL_ONE;
	else {
		if(reg_value & 0x70)
			channel_id = OMAP3_SPI_MASTER_CHANNEL_TWO;
		else {
			if(reg_value & 0x700)
				channel_id = OMAP3_SPI_MASTER_CHANNEL_THREE;
			else 
				channel_id = OMAP3_SPI_MASTER_CHANNEL_FOUR;
		}
	}

	/* Clear the interupt */
	out32(base + OMAP3_MCSPI_IRQ_STATUS_OFFSET, reg_value);

	if (channel_id >= NUM_OF_SPI_DEVS) {
		return NULL;
	}

	/* The remaining bytes are read from the omap3_xfer function */
	if (reg_value & INTR_TYPE_EOWKE)
		return (&dev->spievent);

	/* Is Rx Full? also implies that EOW event is not raised and more data needs to be transfered */
	if (reg_value & INTR_TYPE_RX0_FULL << (channel_id * OMAP3_INTERRUPT_BITS_PER_SPI_CHANNEL)) {

		/* Bytes available in the Rx FIFO */
 		expected = OMAP3_SPI_FIFOLEN * dev->dlen;
		for(i = 0; i < expected; i += dev->dlen, dev->rlen += dev->dlen) {
			switch (dev->dlen) {
			case 1:
				dev->pbuf[dev->rlen] = in32(base + OMAP3_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * channel_id));
				break;
			case 2:
				*(uint16_t *)(&dev->pbuf[dev->rlen]) = in32(base + OMAP3_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * channel_id));
				break;
			default:
				*(uint32_t *)(&dev->pbuf[dev->rlen]) = in32(base + OMAP3_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * channel_id));
				break;
			}
		}

		/* number of bytes to be sent */
		expected = min(OMAP3_SPI_FIFOLEN * dev->dlen, dev->xlen - dev->tlen);
		if (expected) {	
			for (i = 0; i < expected; i += dev->dlen) {
				switch(dev->dlen) {
					case 1:
						out32(base + OMAP3_MCSPI_CH1_TX_BUFFER_OFFSET + OMAP3_SPI_DEVICE_OFFSET * channel_id, dev->pbuf[dev->tlen + i]);
						break;
					case 2:
						out32(base + OMAP3_MCSPI_CH1_TX_BUFFER_OFFSET + OMAP3_SPI_DEVICE_OFFSET * channel_id, *(uint16_t *)(&dev->pbuf[dev->tlen + i]));
						break;
					default:
						out32(base + OMAP3_MCSPI_CH1_TX_BUFFER_OFFSET + OMAP3_SPI_DEVICE_OFFSET * channel_id, *(uint32_t *)(&dev->pbuf[dev->tlen + i]));
						break;
				}
			}

			dev->tlen += expected;
		}
	}

	return NULL;
}

int omap3_attach_intr(omap3_spi_t *omap3)
{
	if ((omap3->chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK)) == -1)
		return -1;

	if ((omap3->coid = ConnectAttach(0, 0, omap3->chid, _NTO_SIDE_CHANNEL, 0)) == -1) 
		goto fail0;

	omap3->spievent.sigev_notify   = SIGEV_PULSE;
	omap3->spievent.sigev_coid	 = omap3->coid;
	omap3->spievent.sigev_code	 = OMAP3_SPI_EVENT;
	omap3->spievent.sigev_priority = omap3->prio;

	/*
	 * Attach SPI interrupt
	 */
	omap3->iid_spi = InterruptAttach(omap3->irq_spi, spi_intr, omap3, 0, _NTO_INTR_FLAGS_TRK_MSK);

	if (omap3->iid_spi != -1)
		return 0;

	ConnectDetach(omap3->coid);
fail0:
	ChannelDestroy(omap3->chid);

	return -1;
}


int omap3_edma_attach_intr(omap3_spi_t *omap3)
{
	struct sigevent		event;

	if ((omap3->edma_coid = ConnectAttach(0, 0, omap3->chid, _NTO_SIDE_CHANNEL, 0)) == -1) 
		goto fail0;

	event.sigev_notify   = SIGEV_PULSE;
	event.sigev_coid	 = omap3->edma_coid;
	event.sigev_code	 = OMAP3_EDMA_EVENT;
	event.sigev_priority = omap3->prio;

	omap3->iid_edma = InterruptAttachEvent(omap3->irq_edma, &event, _NTO_INTR_FLAGS_TRK_MSK);
	
	if (omap3->iid_edma != -1)
		return 0;

	ConnectDetach(omap3->edma_coid);
fail0:

	return -1;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/dm816x/intr.c $ $Rev: 716134 $")
#endif
