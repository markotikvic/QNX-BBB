/*
 * $QNXLicenseC:
 * Copyright 2013, QNX Software Systems.
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

#include "startup.h"
#include "hw/omap_i2c.h"

int omap_i2c_debug = 1;

/* dbg = 0 : disable kprint info */
void omap_i2c_set_debug(int dbg)
{
	omap_i2c_debug = dbg;
}

void omap_i2c_deinit(omap_i2c_dev_t *dev)
{
	out32(dev->base + I2C_IE, 0);
	out32(dev->base + I2C_CON, 0);
}

int omap_i2c_init(omap_i2c_dev_t *dev)
{
	unsigned	scll;
	unsigned	iclk;
	unsigned	timeout = 2000;

	/* Disable and place module in reset */
	out32(dev->base + I2C_IE, 0);
	out32(dev->base + I2C_BUF, 0);
	out32(dev->base + I2C_SYSTEST, 0);

	/* Disable I2C module */
	out32(dev->base + I2C_CON, 0);

	/* Reset and enable */
	out32(dev->base + I2C_SYSC, 0x2);
	out32(dev->base + I2C_CON, I2C_CON_EN);

	/* wait for reset to complete */
	while (0 == (in32(dev->base + I2C_SYSS) & I2C_SYSS_RDONE) && timeout--);

	if (timeout <= 0) {
		kprintf("Failed to init I2C\n");
		return -1;
	}

	/* Smart idle mode, Fclock, Enable Wakeup, autoidle */
	out32(dev->base + I2C_SYSC, 0x215);

	/* enable all wakeup sources */
	out32(dev->base + I2C_WE, 0x636f);

	out32(dev->base + I2C_CON, 0);

	/* assuming functional and interface clocks are enabled */
	if (dev->clock <= 100000) {
		iclk = 4000000UL;
	} else {
		iclk = 9600000UL;
	}

	scll = iclk / (dev->clock << 1) - 7;

	/* Set clock prescaler */
	out32(dev->base + I2C_PSC, I2C_FUNCTION_CLK / iclk - 1);

	/* Set clock for "speed" bps */
	out32(dev->base + I2C_SCLL, scll);
	out32(dev->base + I2C_SCLH, scll + 2);

	/* Set own address */
	out32(dev->base + I2C_OA0, 1);

	/* Take module out of reset */
	out32(dev->base + I2C_CON, I2C_CON_EN);

	/* Check if the I2C bus is held by a slave device*/
	if (in32(dev->base + I2C_STAT) & (I2C_STAT_BB)) {
		out32(dev->base + I2C_CON, (I2C_CON_EN | I2C_CON_STP));
	}

	/*
	 * Enable interrupt in order to bring I2C_STAT register bits visible
	 * Please make sure that coresponding interrupt is enabled
	 * from the interrupt controller
	 */
	out32(dev->base + I2C_IE, I2C_STAT_XDR | I2C_STAT_RDR | I2C_STAT_BB
			| I2C_STAT_XRDY | I2C_STAT_RRDY | I2C_STAT_ROVR | I2C_STAT_NACK | I2C_STAT_ARDY);
	return 0;
}

static int omap_i2c_wait_fifo(omap_i2c_dev_t *dev, uint16_t mask)
{
	unsigned tries = 10000;

	while ( tries-- && ((in32(dev->base + I2C_BUFSTAT) & mask) == 0));

	if (tries == 0) {
		return -1;
	}
	return 0;
}

static int omap_i2c_wait(omap_i2c_dev_t *dev, uint16_t val, uint16_t mask)
{
	unsigned tries = 100000;
	unsigned stat = -1U;

	/* After issuing reset command to slave, the content of I2C_STAT could be zero */
	while ((stat = in32(dev->base + I2C_STAT)) && ((stat & mask) != val)) {
		if (tries-- == 0) {
			/* reset the controller to see if it's able to recover */
			omap_i2c_init(dev);

			/* try again to see if it's OK now after reset */
			if ((in32(dev->base + I2C_STAT) & mask) != val) {
				if(omap_i2c_debug) kprintf("omap_i2c_wait failed, I2C_STAT 0x%x\n", in32(dev->base + I2C_STAT));
				return -1;
			} else
			break;
		}
	}
	return 0;
}

/* Assume address length alen <= 4 */
int omap_i2c_read (omap_i2c_dev_t *dev, uint32_t reg, uint8_t alen, uint8_t* buffer, int len)
{
	int i;
	uint16_t stat;

	/* Wait until the I2C controller is not busy */
	if (-1 == omap_i2c_wait(dev, 0, I2C_STAT_BB)) {
		return -1;
	}

	/* Begin address phase */
	if (alen) {
		out32 (dev->base + I2C_CNT, alen);

		/* set slave address */
		out32 (dev->base + I2C_SA, dev->slave);

		/* Clear FIFO Buffer */
		out32 (dev->base + I2C_BUF, in32(dev->base + I2C_BUF) | I2C_BUF_RXFIFO_CLR | I2C_BUF_TXFIFO_CLR);

		/* no stop bit needed here */
		out32 (dev->base + I2C_CON, I2C_CON_EN | I2C_CON_MST | I2C_CON_TRX | I2C_CON_STT );

		/* poll for transmit ready */
		if (-1 == omap_i2c_wait(dev, I2C_STAT_XRDY, I2C_STAT_XRDY))
			return -1;

		while (alen) {

			/* Output register address */
			alen--;
			out8 (dev->base + I2C_DATA, (reg >> (alen * 8)) & 0xFF);
		}

		/* Wait for register access ready */
		if (-1 == omap_i2c_wait(dev, I2C_STAT_ARDY, I2C_STAT_ARDY)) {
			 if(omap_i2c_debug) kprintf("omap_i2c_read: register access not ready\n");
			return -1;
		}
	}

	/*
	 * Before writing address to buffer, TXSTAT in I2C_BUFSTAT equals to DCOUNT in I2C_CNT,
	 * But TXSTAT goes to 0 at the moment all address bytes are written to the buffer.
	 * After a while, TXSTAT == DCOUNT again when it's ready for 'restart'
	 */
	if (-1 == omap_i2c_wait_fifo(dev, I2C_BUFSTAT_TXSTAT))
		return -1;

	/* clear status */
	out32(dev->base + I2C_STAT, in32(dev->base + I2C_STAT));

	/*
	 * It seems no need to wait for BB, since between I2C_CON_EN is set and stop condition,
	 * BB is supposed to be asserted.
	 * But as per TRM: "BB=1 and STT=1, a restart condition is generated,
	 * We keep this statement here
	 */
	if (-1 == omap_i2c_wait(dev, I2C_STAT_BB, I2C_STAT_BB))
		return -1;

	/* Address phase complete, begin data phase */
	/* set slave address, assuming 7bit slave address */
	out32 (dev->base + I2C_SA, dev->slave);

	/* set bytes to rx */
	out32 (dev->base + I2C_CNT, len);

	/* Clear FIFO Buffer */
	out32 (dev->base + I2C_BUF, in32(dev->base + I2C_BUF) | I2C_BUF_RXFIFO_CLR | I2C_BUF_TXFIFO_CLR);

	/* Clear TRX to start receiving on SDA, stop bit set */
	out32 (dev->base + I2C_CON, I2C_CON_EN | I2C_CON_MST | I2C_CON_STP | I2C_CON_STT);

	/* Bytewise read */
	for (i = 0; i < len; i++) {

		/* poll for receive ready */
		if (-1 == omap_i2c_wait_fifo(dev, I2C_BUFSTAT_RXSTAT))
			return -1;

		stat = in32 (dev->base + I2C_STAT);
		if (stat & I2C_STAT_ROVR) {
			out32 (dev->base + I2C_CON, 0);
			if(omap_i2c_debug) kprintf("omap_i2c_read: rx overrun!\n");
			return -1;
		}

		if (stat & I2C_STAT_NACK) {
			out32 (dev->base + I2C_CON, 0);
			if(omap_i2c_debug) kprintf("omap_i2c_read: NAK!\n");
			return -1;
		}

		buffer[i] = in8(dev->base + I2C_DATA);
	}

	/* Wait for register access ready */
	if (-1 == omap_i2c_wait(dev, I2C_STAT_ARDY, I2C_STAT_ARDY)) {
			if(omap_i2c_debug) kprintf("omap_i2c_read: register access not ready\n");
			return -1;
	}

	/* clear status */
	out32(dev->base + I2C_STAT, in32(dev->base + I2C_STAT));

	return 0;
}

int omap_i2c_write(omap_i2c_dev_t *dev, uint8_t *buf, int len)
{
	int numbytes = len;

	if (-1 == omap_i2c_wait(dev, 0, I2C_STAT_BB)) {
		return -1;
	}

	/* set slave address, assuming 7bit slave address */
	out32 (dev->base + I2C_CON, in32(dev->base + I2C_CON) & (~I2C_CON_XA));
	out32(dev->base + I2C_SA, dev->slave);

	/* set data count */
	out32(dev->base + I2C_CNT, len);

	/* Clear FIFO Buffer */
	out32 (dev->base + I2C_BUF, in32(dev->base + I2C_BUF) | I2C_BUF_RXFIFO_CLR | I2C_BUF_TXFIFO_CLR);

	/* set start condition */
	out32(dev->base + I2C_CON, I2C_CON_EN | /* module enabled */
					I2C_CON_MST| /* master mode */
					I2C_CON_TRX | /* transmitter mode */
					I2C_CON_STT | /* start condition */
					I2C_CON_STP); /* stop condition */

	/* poll for transmit ready */
	if (-1 == omap_i2c_wait(dev, I2C_STAT_XRDY, I2C_STAT_XRDY))
		return -1;

	/*
	 * We assume the data to be written is less than FIFO depth,
	 * which is 16 bytes in J5
	 */
	while (numbytes--) {
		out8(dev->base + I2C_DATA, *buf++);
	}

	/* Wait until it's sent */
	if (-1 == omap_i2c_wait(dev, I2C_STAT_ARDY, I2C_STAT_ARDY)) {
		return -1;
	}

	/* clear status */
	out32(dev->base + I2C_STAT, in32(dev->base + I2C_STAT));

	return 0;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/omap_i2c.c $ $Rev: 735269 $")
#endif
