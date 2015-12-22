/*
 * $QNXLicenseC:
 * Copyright 2009,2014 QNX Software Systems.
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
		
static char *omap3_opts[] = {
	"base",		/* Base address for this CSPI controller */
	"bitrate",	/* defines teh bitrate to to assigned to the spi */
	"clock",	/* defined the value of the clock source of the SPI */
	"channel",	/* defines the channel connected */
	"irq",		/* IRQ for this CSPI intereface */
	"force",	/* Force The default CS level */
	"num_cs",	/* number of devices supported */
	"edma",		/* Disable/enable SDMA for SPI */ 
	"cs_delay",	/* Disable/enable 1 1/2 clock CS to data delay */
	"edmabase",	/* Base address for EDMA */
	"edmairq",	/* IRQ for EDMA channel */
	"edmachannel",	/* EDMA channel for this SPI interface receive event, Tx channel = rxchannel - 1 */
	"somi",			/* determine which pin use for somi: 0 for D0, 1 for D1 */
	"cspol",		/* set CS polarity, =0 for active low, =1 for active high */
	"prio",			/* sig_event priority */
	"fifo",		 	/* FIFO control. 0:disable FIFO 1:enable RxFIFO 2:enable TxFIFO 3:enable both */
	"clk_ratio",	/* 0:power of two, 1:one clock cycle */
	"clk_clkd",	 	/* 0-F: Frequency divider for SPI_SCLK */
	"clk_extclk",	/* 00-FF: Clock ratio extension, used to concatenate with CLKD */
	"clk_activity", /* 0-3: Clocks activity during wake-up mode period */
	"pwr",			/* Power management */
	"ocp",			/* Internal OCP Clock gating strategy */
	"pin",			/* 0:use, 1:not-use PIN34: pin mode selection */
	NULL
};

spi_funcs_t spi_drv_entry = {
	sizeof(spi_funcs_t),
	omap3_init,		/* init() */
	omap3_dinit,	/* fini() */
	omap3_drvinfo,	/* drvinfo() */
	omap3_devinfo,	/* devinfo() */
	omap3_setcfg,	/* setcfg() */
	omap3_xfer,		/* xfer() */
	omap3_dmaxfer	/* dma_xfer() */
};

/*
 * Note:
 * The devices listed are just examples, users should change
 * this according to their own hardware spec.
 */
static spi_devinfo_t devlist[4] = {
	{
		0x00,			// Device ID, for SS0
		"TSC-2046",		// Description
		{ 
			(16 | SPI_MODE_CKPOL_HIGH), // data length 8bit, MSB
			125000						// TSC-2046 will fucntion at 125000
		},
	},
	{
		0x01,			// Device ID, for SS1
		"TSC-2046", 	// Description
		{ 
			(16 | SPI_MODE_CKPOL_HIGH), // data length 16bit, MSB
			125000						// TSC-2046 will fucntion at 125000
		},
	},
	{
		0x02,			// Device ID, for SS2
		"TSC-2046", 	// Description
		{ 
			(16 | SPI_MODE_CKPOL_HIGH), // data length 16bit, MSB
			125000						// TSC-2046 will fucntion at 125000
		},
	},
	{
		0x03,			// Device ID, for SS3
		"TSC-2046", 	// Description
		{ 
			(16 | SPI_MODE_CKPOL_HIGH), // data length 16bit, MSB
			12500						// TSC-2046 will fucntion at 125000
		}
	}
};

static uint32_t devctrl[NUM_OF_SPI_DEVS];

void set_port(unsigned port, unsigned mask, unsigned data) 
{
	uint32_t c;

	c = read_omap(port);
	write_omap(port, (c & ~mask) | (data & mask));
}

static int omap3_options(omap3_spi_t *spi, char *optstring)
{
	int	opt, rc = 0;
	char	*options, *freeptr, *c, *value;
	int		device_id;
	int		prio;
	int		val;

	if (optstring == NULL)
		return 0;

	freeptr = options = strdup(optstring);
	while (options && *options != '\0') {
		c = options;
		if ((opt = getsubopt(&options, omap3_opts, &value)) == -1)
			goto error;

		switch (opt) {
			case 0:
				spi->pbase = strtoull(value, 0, 0);
				continue;
			case 1:
				if (value && *value != NULL) {
					char *value2;
					device_id= strtoul(value, NULL, 0);
					if ((value2 = strchr(value, ':')) != NULL)
						devlist[device_id].cfg.clock_rate = strtoul(value2 + 1, 0, 0);
				}			
				continue;	
			case 2:
				spi->clock = strtoul(value, NULL, 0);
				continue;	
			case 3:
				val = strtoul(value, NULL, 0);
				if(val < 1 || val > 4) {
					fprintf(stderr, "channel number is out of range (1,2,3 or 4), using default of 1\n");
					spi->channel_num = 1;
				}
				else
					spi->channel_num = val;
				continue;	
			case 4:
				spi->irq_spi = strtoul(value, 0, 0);
				continue;
			case 5:
				spi->force = strtoul(value, 0, 0);
				continue;	
			case 6:
				spi->num_cs = strtoul(value, 0, 0);
				continue;
			case 7:
				spi->edma = strtoul(value, 0, 0);
				continue;	
			case 8:
				val = strtoul(value, 0, 0);
				if(val > 3) fprintf(stderr, "spi-dm816x: spi->cs_delay is out of range, use default(0)\n");
				else spi->cs_delay = val;
				continue;
			case 9:
				spi->edmapbase = strtoul(value, 0, 0);
				continue;
			case 10:
				spi->irq_edma = strtoul(value, 0, 0);
				continue;
			case 11:
				spi->edma_rx_chid = strtoul(value, NULL, 0);
				spi->edma_tx_chid = spi->edma_rx_chid - 1;
				continue;	
			case 12:
				spi->somi = strtoul(value, 0, 0);
				continue;
			case 13:
				val = strtoul(value, 0, 0);
				if(val != 0 && val != 1) {
					fprintf(stderr, "spi-dm816x: cspol is out of range (0 or 1), using default (0 - active low)\n");
					//omap3_cfg() will set it low by default
					continue;
				}
				if(val)
						devlist[spi->channel_num].cfg.mode |= SPI_MODE_CSPOL_HIGH;
				continue;
			case 14:
				prio = strtol(value, NULL, 0);
				if ((prio >= OMAP3_SPI_PRIORITY_MIN && prio <= OMAP3_SPI_PRIORITY_MAX) || (prio == SIGEV_PULSE_PRIO_INHERIT)) 
					spi->prio = prio;
				else
					fprintf(stderr, "spi-dm816x: sigev priority set is out of range, uses default (21)\n");
				continue;						
			case 15:
				val = strtoul(value, 0, 0);
				if(val > 3) fprintf(stderr, "spi-dm816x: spi->fifo is out of range, use default(0)\n");
				else spi->fifo = val;
				continue;
			case 16:
				spi->clk_ratio = strtoul(value, 0, 0);
				continue;
			case 17:
				val = strtoul(value, 0, 0);
				if(val > 0xf) fprintf(stderr, "spi-dm816x: spi->clk_clkd is out of range, use default(0)\n");
				else spi->clk_clkd = val;
				continue;
			case 18:
				val = strtoul(value, 0, 0);
				if(val > 0xf) fprintf(stderr, "spi-dm816x: spi->clk_extclk is out of range, use default(0)\n");
				else spi->clk_extclk = val;
				continue;
			case 19:
				val = strtoul(value, 0, 0);
				if(val > 3) fprintf(stderr, "spi-dm816x: spi->clk_activity is out of range, use default(0)\n");
				else spi->clk_activity= val;
				continue;
			case 20:
				val = strtoul(value, 0, 0);
				if(val > 2) fprintf(stderr, "spi-dm816x: spi->pwr is out of range, use default(2)\n");
				else spi->pwr= val;
				continue;
			case 21:
				val = strtoul(value, 0, 0);
				if (0 != val && 1 != val) fprintf(stderr, "spi-dm816x: spi->ocp is out of range, use default(1)\n");
				else spi->ocp = val;
				continue;
			case 22:
				val = strtoul(value, 0, 0);
				if(0!=val && 1!=val) fprintf(stderr, "spi-dm816x: spi->pin is out of range, use default(0)\n");
				else spi->pin = val;
				continue;
		}
error:
		fprintf(stderr, "spi-dm816x: unknown option %s\n", c);
		rc = -1;
	}

	free(freeptr);

	return rc;
}

void *omap3_init(void *hdl, char *options)
{
	omap3_spi_t	*dev;
	uintptr_t	base;
	int			i;
	uint32_t	reg;
	uint16_t	ext_div;

	dev = calloc(1, sizeof(omap3_spi_t));
	if (dev == NULL)
		return NULL;

	/* Set defaults */
	dev->pbase = OMAP3_SPI1_BASE;
	dev->irq_spi = OMAP3_SPI_SPI1_INTR;
	dev->clock = OMAP3_SPI_INPUT_CLOCK;
	dev->channel_num = 1;
	dev->force = 0;
	dev->edmapbase = DM816x_EDMA_BASE;
	dev->irq_edma = 0;	/* We use interrupt of receive channel	*/
	dev->edma = 0;
	dev->num_cs = 1;
	dev->cs_delay = 0;
	dev->somi = 0;		/* SOMI: D0; SIMO; D1 */
	dev->edma_tx_chid = dev->edma_rx_chid = -1;
	dev->prio = OMAP3_SPI_PRIORITY;
	dev->fifo = 0;
	dev->clk_ratio = 0;
	dev->clk_clkd= 0;
	dev->clk_extclk = 0;
	dev->clk_activity = 0;
	dev->pwr = 2;
	dev->ocp = 1;
	dev->pin = 0;

 	if (omap3_options(dev, options))
		goto fail0;

	/*
	 * Map in SPI registers
	 */
	if ((base = mmap_device_io(OMAP3_SPI_REGLEN, dev->pbase)) == (uintptr_t)MAP_FAILED)
		goto fail0;

	dev->vbase = base;
	
	/* Reset the SPI interface
	 * and wait for the reset to complete
	 */
	set_port(base + OMAP3_MCSPI_SYS_CONFIG, OMAP3_MCSPI_CONFIG_SOFT_RESET, OMAP3_MCSPI_CONFIG_SOFT_RESET);
	while (!(in32(base + OMAP3_MCSPI_SYS_CONFIG) & 1)) {
		nanospin_ns(20);
	}

	/* Set Master mode -- single channel moade */
	out32((base + OMAP3_MCSPI_MODCTRL_OFFSET), (in32(base + OMAP3_MCSPI_MODCTRL_OFFSET) & OMAP3_MCSPI_MODCTRL_MASTER_SEL) | OMAP3_MCSPI_MODCTRL_MULTI_CH_SEL);
	if (dev->pin)
		out32(base + OMAP3_MCSPI_MODCTRL_OFFSET, in32(base + OMAP3_MCSPI_MODCTRL_OFFSET) | OMAP3_MCSPI_MODCTRL_PIN34);
	else
		out32(base + OMAP3_MCSPI_MODCTRL_OFFSET, in32(base + OMAP3_MCSPI_MODCTRL_OFFSET) & (~OMAP3_MCSPI_MODCTRL_PIN34));

	/*
	 * Calculate all device configuration here
	 */
	for (i = 0; i < dev->num_cs; i++) { //we have just one device defined in the driver till now....
		devctrl[i] = omap3_cfg(dev, &devlist[i].cfg, &ext_div);
		if (dev->force) {
			/* if we need to set the default CSx level to other than defaul low, we need to kick it */
			out32((base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * i), devctrl[i] | SPI_COMM_TX_RX << 12);
			set_port(base + OMAP3_MCSPI_CH1_CTRL_OFFSET + OMAP3_SPI_DEVICE_OFFSET * i, OMAP3_MCSPI_CHANNEL_ENABLE, OMAP3_MCSPI_CHANNEL_ENABLE);
			//set register MCSPI_CH0CTRL: EXTCLK bit
			set_port((base + OMAP3_MCSPI_CH1_CTRL_OFFSET + OMAP3_SPI_DEVICE_OFFSET * i), OMAP3_MCSPI_CTRL_EXTCLK, (ext_div << OMAP3_MCSPI_CTRL_EXTCLK_OFF));

			/* force CS */
			set_port((base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * i),OMAP3_MCSPI_FORCE_MODE_ONE, OMAP3_MCSPI_FORCE_MODE_ONE);
			delay(1);

			/* un force CS */
			set_port((base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * i),OMAP3_MCSPI_FORCE_MODE_ONE, 0);
			set_port(base + OMAP3_MCSPI_CH1_CTRL_OFFSET + OMAP3_SPI_DEVICE_OFFSET * i, OMAP3_MCSPI_CHANNEL_ENABLE, 0);
		}

		if (0 == dev->clk_ratio) {
			set_port((base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * i), OMAP3_MCSPI_CLKG, 0);
			set_port((base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * i), OMAP3_MCSPI_CLKD, (dev->clk_clkd) << 2);
		}
		else if (1 == dev->clk_ratio) {
			set_port((base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * i), OMAP3_MCSPI_CLKG, OMAP3_MCSPI_CLKG);
			set_port((base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * i), OMAP3_MCSPI_CLKD, (dev->clk_clkd) << 2);
			set_port((base + OMAP3_MCSPI_CH1_CTRL_OFFSET + OMAP3_SPI_DEVICE_OFFSET * i), OMAP3_MCSPI_CTRL_EXTCLK, (dev->clk_extclk) << 8);
		}
	
		set_port(base + OMAP3_MCSPI_SYS_CONFIG, OMAP3_MCSPI_CONFIG_CLOCKACTIVITY, (dev->clk_activity) << 8);		
		set_port(base + OMAP3_MCSPI_SYS_CONFIG, OMAP3_MCSPI_CONFIG_SIDLEMODE, (dev->pwr) << 3);		
		set_port(base + OMAP3_MCSPI_SYS_CONFIG, OMAP3_MCSPI_CONFIG_AUTOIDLE, (dev->ocp) << 0);		
	}

	/*
	 * Attach SPI interrupt
	 */
	if (omap3_attach_intr(dev))
		goto fail1;
	
	dev->spi.hdl = hdl;

	/* Clear the appropriate Interrupts if any*/
	reg = in32(base + OMAP3_MCSPI_IRQ_STATUS_OFFSET);
	out32((base + OMAP3_MCSPI_IRQ_STATUS_OFFSET), reg);

	if (dev->edma) {
		if (omap3_init_edma(dev))
			dev->edma = 0;

		/*
		 * Attach SDMA interrupt
	 	*/
		if (dev->edma && omap3_edma_attach_intr(dev)) {
			printf("%s(%d): omap3_edma_attach_intr failed\n", __func__, __LINE__);
			goto fail2;
		}

	}
	return dev;
	
fail2:
	munmap_device_memory ((void *)dev->edmavbase, DM6446_EDMA_SIZE);
fail1:
	munmap_device_io(dev->vbase, OMAP3_SPI_REGLEN);
fail0:
	free(dev);
	return NULL;
}

void omap3_setup(omap3_spi_t	*dev , uint32_t device)
{
	uintptr_t	base = dev->vbase;
	uint32_t	id;

	id = device & SPI_DEV_ID_MASK;
	out32((base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id), (SPI_COMM_TX_RX << 12) | devctrl[id]);
}

void omap3_dinit(void *hdl)
{
	omap3_spi_t	*dev = hdl;
	
	/*
	 * unmap the register, detach the interrupt
	 */
	InterruptDetach(dev->iid_spi);
	munmap_device_io(dev->vbase, OMAP3_SPI_REGLEN);
	
	/*
	 * Disable EDMA
	 */
	if (dev->edma) {
		omap3_edma_disablespi(dev);
		InterruptDetach(dev->iid_edma);
		omap3_edma_detach (dev);
		munmap_device_memory((void *)dev->edmavbase, DM6446_EDMA_SIZE);
		munmap(dev->dmabuf, OMAP3_SPI_MAXDMALEN);
		ConnectDetach(dev->edma_coid);
	}
	ConnectDetach(dev->coid);
	ChannelDestroy(dev->chid);
	
	free(hdl);
}

int omap3_drvinfo(void *hdl, spi_drvinfo_t *info)
{
	info->version = (SPI_VERSION_MAJOR << SPI_VERMAJOR_SHIFT) | (SPI_VERSION_MINOR << SPI_VERMINOR_SHIFT) | (SPI_REVISION << SPI_VERREV_SHIFT);
	strcpy(info->name, "DM816X SPI");
	info->feature = SPI_FEATURE_DMA;	/* DMA supported, buffer address alignemnt is 1 */
	
	return (EOK);
}

int omap3_setcfg(void *hdl, uint16_t device, spi_cfg_t *cfg)
{
	uint32_t	control;
	omap3_spi_t	*dev = hdl;
	uintptr_t	base = dev->vbase;
	uint16_t	ext_div;

	if (device >= dev->num_cs)
		return (EINVAL);

	memcpy(&devlist[device].cfg, cfg, sizeof(spi_cfg_t));
	control = omap3_cfg(hdl, &devlist[device].cfg, &ext_div);
	if (control == 0)
		return (EINVAL);

	//set register MCSPI_CH0CTRL: EXTCLK bit
	set_port((base + OMAP3_MCSPI_CH1_CTRL_OFFSET + OMAP3_SPI_DEVICE_OFFSET * device), OMAP3_MCSPI_CTRL_EXTCLK, (ext_div << OMAP3_MCSPI_CTRL_EXTCLK_OFF));

	devctrl[device] = control;

	return (EOK);
}

int omap3_devinfo(void *hdl, uint32_t device, spi_devinfo_t *info)
{
	omap3_spi_t	*dev = hdl;
	int		id = device & SPI_DEV_ID_MASK;

	if (device & SPI_DEV_DEFAULT) {
		/*
		 * Info of this device
		 */
		if (id >= 0 &&	id < dev->num_cs)
			memcpy(info, &devlist[id], sizeof(spi_devinfo_t));
		else
			return (EINVAL);
	} else {
		/*
		 * Info of next device 
		 */
		if (id == SPI_DEV_ID_NONE)
			id = -1;
		if (id < (dev->num_cs-1))
			memcpy(info, &devlist[id + 1], sizeof(spi_devinfo_t));
		else
			return (EINVAL);
	}
	
	return (EOK);
}

void *omap3_xfer(void *hdl, uint32_t device, uint8_t *buf, int *len)
{
	omap3_spi_t	*dev = hdl;
	uintptr_t	base = dev->vbase;
	uint32_t	id;
	int 		i;
	int 		timeout, expected;
	uint32_t 	reg_value = 0;

	id = device & SPI_DEV_ID_MASK;
	if (id >= dev->num_cs) {
		*len = -1;
		return buf;
	}

	dev->xlen = *len;
	dev->dlen = ((devlist[id].cfg.mode & SPI_MODE_CHAR_LEN_MASK) + 7) >> 3;

	/* 
	 * Cannot set more than 64KB of data at one time 
	 * due to the limitation of WCNT field in MCSPI_XFERLEVEL register
	 */
	if (dev->xlen >= (64 * 1024)) {
		*len = -1;
		return buf;
	}

	/* The transfer len must be integer multiple of the word width */
	if (dev->xlen % dev->dlen) {
		*len = -1;
		return buf;
	}

	dev->rlen = 0;
	dev->tlen = min(OMAP3_SPI_FIFOLEN * dev->dlen, dev->xlen);
	dev->pbuf = buf;

	// Estimate transfer time in us... The calculated dtime is only used for
	// the timeout, so it doesn't have to be that accurate. At higher clock
	// rates, a calcuated dtime of 0 would mess-up the timeout calculation, so
	// round up to 1 us
	dev->dtime = dev->dlen * 1000 * 1000 / devlist[id].cfg.clock_rate;
	if (dev->dtime == 0)
		dev->dtime = 1;

	omap3_setup(dev, device);

	/* force CS */
	set_port((base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id),OMAP3_MCSPI_FORCE_MODE_ONE, OMAP3_MCSPI_FORCE_MODE_ONE);

	/* set FIFO */
	set_port((base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id), OMAP3_MCSPI_FFER | OMAP3_MCSPI_FFEW, OMAP3_MCSPI_FFER | OMAP3_MCSPI_FFEW);

	/* 
	 * Set FIFO transfer level
	 * we rely on EOW interrupt to indicate the end of the tranfer
	 * RXx_FULL interrupt as an indication to fill more data to the the Tx FIFO
	 */
	out32(base + OMAP3_MCSPI_XFERLEVEL_OFFSET, ((dev->xlen / dev->dlen) << OMAP3_MCSPI_XFERLEVEL_WCNT_OFFSET) 
			| (OMAP3_SPI_FIFOLEN * dev->dlen - 1) << OMAP3_MCSPI_XFERLEVEL_AFL_OFFSET);

	/* Clear any pending interrupts in this channel */
	out32(base + OMAP3_MCSPI_IRQ_STATUS_OFFSET, OMAP3_MCSPI_IRQ_RESET_CHANNEL(id));

	/* Configue the SPI control register to enable the corresponding channel of the SPI */
 	set_port(base + OMAP3_MCSPI_CH1_CTRL_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id, OMAP3_MCSPI_CHANNEL_ENABLE, OMAP3_MCSPI_CHANNEL_ENABLE);

	/* Enable Interrupts */
	out32(base + OMAP3_MCSPI_IRQ_ENABLE_OFFSET, INTR_TYPE_EOWKE | (INTR_TYPE_RX0_FULL << (id * OMAP3_INTERRUPT_BITS_PER_SPI_CHANNEL)));

	/* start the data transmit */
	for (i = 0; i < dev->tlen; i += dev->dlen) {
		switch (dev->dlen) {
			case 1:
				out32(base + OMAP3_MCSPI_CH1_TX_BUFFER_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id, dev->pbuf[i]);
				break;
			case 2:
				out32(base + OMAP3_MCSPI_CH1_TX_BUFFER_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id, *(uint16_t *)(&dev->pbuf[i]));
				break;
			default:
				out32(base + OMAP3_MCSPI_CH1_TX_BUFFER_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id, *(uint32_t *)(&dev->pbuf[i]));
				break;
		}
	}

	/*
	* Wait for exchange to finish
	*/
	if (omap3_wait(dev, dev->xlen * 10)) {
		fprintf(stderr, "spi-dm816x: XFER Timeout!!!\n");
		dev->rlen = -1;
	}

	/* Read the last spi words when EOW interrupt is raised */
	if (dev->rlen < dev->xlen && dev->rlen != -1) {
		reg_value = in32(base + OMAP3_MCSPI_CH1_STATUS_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * id));
		timeout = 1000;
		while( timeout-- && ((reg_value & OMAP3_MCSPI_CH_RX_REG_FULL) == 0) ) {
			nanospin_ns(100);
			reg_value = in32(base + OMAP3_MCSPI_CH1_STATUS_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * id));
		}
		
		if(timeout <= 0) {
			dev->rlen = -1;
		} else {
			/* last words to read from buffer */
			expected = dev->xlen - dev->rlen;

			for(i = 0; i < expected; i += dev->dlen, dev->rlen += dev->dlen) {
				switch (dev->dlen) {
				case 1:
					dev->pbuf[dev->rlen] = in32(base + OMAP3_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * id));
					break;
				case 2:
					*(uint16_t *)(&dev->pbuf[dev->rlen]) = in32(base + OMAP3_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * id));
					break;
				default:
					*(uint32_t *)(&dev->pbuf[dev->rlen]) = in32(base + OMAP3_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * id));
					break;
				}
			}
		}
	}

	/* disable interrupts */
	out32(base + OMAP3_MCSPI_IRQ_ENABLE_OFFSET, 0);	

	/* un-force CS */
	set_port((base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id), OMAP3_MCSPI_FORCE_MODE_ONE, 0);

	set_port(base + OMAP3_MCSPI_CH1_CTRL_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id, OMAP3_MCSPI_CHANNEL_ENABLE, 0);
	set_port(base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id, OMAP3_MCSPI_FFER | OMAP3_MCSPI_FFEW, 0);
	out32(base + OMAP3_MCSPI_XFERLEVEL_OFFSET , 0);
	
	*len = dev->rlen;

	return buf;
}

int omap3_dmaxfer(void *hdl, uint32_t device, spi_dma_paddr_t *paddr, int len)
{
	omap3_spi_t	*dev = hdl;
	int	id = device & SPI_DEV_ID_MASK;
	uintptr_t	base = dev->vbase;

	/* Is the EDMA disabled? */
	if (dev->edma == 0 || id >= dev->num_cs)
		return -1;

	dev->dlen = ((devlist[id].cfg.mode & SPI_MODE_CHAR_LEN_MASK) + 7) >> 3;

	/* The transfer len must be integer multiple of the word width */
	if (len % dev->dlen) {
		fprintf(stderr, "spi-dm816x: transfer length must be integer multiple of the word length!\n");
		return -1;
	}

	// Estimate transfer time in us... The calculated dtime is only used for
	// the timeout, so it doesn't have to be that accurate. At higher clock
	// rates, a calcuated dtime of 0 would mess-up the timeout calculation, so
	// round up to 1 us
	dev->dtime = dev->dlen * 1000 * 1000 / devlist[id].cfg.clock_rate;
	if (dev->dtime == 0)
		dev->dtime = 1;

	omap3_setup(dev, device);
	
	if (omap3_setup_edma(dev, id, paddr, len)) {
		fprintf(stderr, "spi-dm816x: DMA XFER Timeout!!!\n");
		return -1;
	}

	switch (dev->fifo) {
		case 0:
			set_port(base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * id), OMAP3_MCSPI_FFER | OMAP3_MCSPI_FFEW, 0);	
			break;
		case 1:				
			set_port(base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * id), OMAP3_MCSPI_FFER, OMAP3_MCSPI_FFER);	
			break;
		case 2:
			set_port(base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * id), OMAP3_MCSPI_FFEW, OMAP3_MCSPI_FFEW);	
			break;
		case 3:				
			set_port(base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * id), OMAP3_MCSPI_FFER | OMAP3_MCSPI_FFEW, OMAP3_MCSPI_FFER | OMAP3_MCSPI_FFEW);	
			break;
		default:
			 set_port(base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * id), OMAP3_MCSPI_FFER | OMAP3_MCSPI_FFEW, 0);	
			 break;
	}

	if (dev->fifo >= 1 && dev->fifo <= 3)
		out32(base + OMAP3_MCSPI_MODCTRL_OFFSET, in32(base + OMAP3_MCSPI_MODCTRL_OFFSET) | OMAP3_MCSPI_MODCTRL_FDAA);

	/* Enable edma request	*/
	set_port(base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + (OMAP3_SPI_DEVICE_OFFSET * id), OMAP3_MCSPI_DMAR | OMAP3_MCSPI_DMAW, OMAP3_MCSPI_DMAR | OMAP3_MCSPI_DMAW);

	/* Configue the SPI control register to enable the corresponding channel of the SPI */ 
	set_port(base + OMAP3_MCSPI_CH1_CTRL_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id, OMAP3_MCSPI_CHANNEL_ENABLE, OMAP3_MCSPI_CHANNEL_ENABLE);

	/* force CS */
	set_port((base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id), OMAP3_MCSPI_FORCE_MODE_ONE, OMAP3_MCSPI_FORCE_MODE_ONE);

	if (omap3_wait(dev,len * 10)) {
		fprintf(stderr, "spi-dm816x: DMA XFER Timeout!!!\n");
		len = -1;
	}

	/* un-force CS */
	set_port((base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id), OMAP3_MCSPI_FORCE_MODE_ONE, 0);

	/*
	 * Disable SDMA request and SPI function
	 */
	set_port(base + OMAP3_MCSPI_CH1_CTRL_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id, OMAP3_MCSPI_CHANNEL_ENABLE, 0);
	set_port(base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id, OMAP3_MCSPI_DMAR|OMAP3_MCSPI_DMAW, 0);
	set_port(base + OMAP3_MCSPI_CH1_CONFIG_OFFSET + OMAP3_SPI_DEVICE_OFFSET * id, OMAP3_MCSPI_FFER | OMAP3_MCSPI_FFEW, 0);
	if (dev->fifo >= 1 && dev->fifo <= 3)
		out32(base + OMAP3_MCSPI_MODCTRL_OFFSET, in32(base + OMAP3_MCSPI_MODCTRL_OFFSET) & (~OMAP3_MCSPI_MODCTRL_FDAA));

	omap3_edma_disablespi(dev);

	return len;	
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/dm816x/omap3spi.c $ $Rev: 740148 $")
#endif
