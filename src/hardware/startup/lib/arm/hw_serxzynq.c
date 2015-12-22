/*
 * $QNXLicenseC:
 * Copyright 2008, QNX Software Systems. 
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
 * Polled serial operations for Xilinx ZYNQ 7000
 */

#include "startup.h"
#include <arm/xzynq.h>

static void parse_line(unsigned channel, const char *line, unsigned *baud,
		unsigned *clk)
{
	/*
	 * Get device base address and register stride
	 */
	if (*line != '.' && *line != '\0') {
		dbg_device[channel].base = strtoul(line, (char **) &line, 16);
		if (*line == '^')
			dbg_device[channel].shift = strtoul(line + 1, (char **) &line, 0);
	}

	/*
	 * Get baud rate
	 */
	if (*line == '.')
		++line;
	if (*line != '.' && *line != '\0')
		*baud = strtoul(line, (char **) &line, 0);

	/*
	 * Get clock rate
	 */
	if (*line == '.')
		++line;
	if (*line != '.' && *line != '\0')
		*clk = strtoul(line, (char **) &line, 0);
}

static void serial_setbrg(unsigned int base, long baud)
{
	/*              master clock
	 * Baud rate = ---------------
	 *              bgen*(bdiv+1)
	 */
	/* Variables to vary. */
	unsigned int bdiv, bgen;

	/* Calculation results. */
	long calc_baud = 0;
	unsigned int calc_bauderror;

	/* Find acceptable values for baud generation. */
	for (bdiv = 4; bdiv < 255; bdiv++) {

		bgen = XZYNQ_PERIPH_CLOCK / (baud * (bdiv + 1));
		if (bgen < 2 || bgen > 65535)
			continue;

		calc_baud = XZYNQ_PERIPH_CLOCK / (bgen * (bdiv + 1));

		/* Use first calculated baudrate with an acceptable
		 * (<3%) error.
		 */
		if (baud > calc_baud)
			calc_bauderror = baud - calc_baud;
		else
			calc_bauderror = calc_baud - baud;
		if (((calc_bauderror * 100) / baud) < 3)
			break;

	}

	out32(base + XZYNQ_XUARTPS_BAUDDIV_REG, bdiv);
	out32(base + XZYNQ_XUARTPS_BAUDGEN_REG, bgen);
}

/*
 * Initialise one of the serial ports
 */
void init_xzynq(unsigned channel, const char *init, const char *defaults)
{
	unsigned int baud, clk, base;

	/*
	 * Default peripheral clock rate is 50MHz
	 */
	clk = XZYNQ_PERIPH_CLOCK;

	parse_line(channel, defaults, &baud, &clk);
	parse_line(channel, init, &baud, &clk);
	base = dbg_device[channel].base;

	if (baud == 0)
		return;

	/* Disable and reset UART (TX & RX) */
	out32(base + XZYNQ_XUARTPS_CR_REG, 0x28 | 0x03);

	/* 8-bit, no-parity, 1 stop bit, no flow */
	out32(base + XZYNQ_XUARTPS_MR_REG, 0x20);

	/* Set clocks */
	serial_setbrg(base, baud);

	/* Enable UART (TX & RX) */
	out32(base + XZYNQ_XUARTPS_CR_REG, 0x14);
}

/*
 * Send a character
 */
void put_xzynq(int c)
{
	unsigned base = dbg_device[0].base;

	while ((in32(base + XZYNQ_XUARTPS_SR_REG) & XZYNQ_XUARTPS_SR_TXFULL) != 0);

	if (c == '\n') {
		out32(base + XZYNQ_XUARTPS_FIFO_REG, '\r');
		while ((in32(base + XZYNQ_XUARTPS_SR_REG) & XZYNQ_XUARTPS_SR_TXFULL)
				!= 0);
	}
	out32(base + XZYNQ_XUARTPS_FIFO_REG, c);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif



