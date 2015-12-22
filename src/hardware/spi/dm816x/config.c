/*
 * $QNXLicenseC:
 * Copyright 2009,2014, QNX Software Systems.
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

unsigned int speed_divisor[13] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096};

int omap3_cfg(void *hdl, spi_cfg_t *cfg, uint16_t* ext_div)
{
	omap3_spi_t	*dev = hdl;
	uint32_t	ctrl;
	uint32_t	i = 0;
	uint16_t	divisor = 0, ext_divisor = 0 ;
	uint8_t		clkg_bit=1;
	uint8_t		clkd_divisor = 0;

	if (cfg == NULL)
		return 0;

	i = (cfg->mode & SPI_MODE_CHAR_LEN_MASK) - 1;

	if (i > 32 || i < 4)
		return 0;

	ctrl = (i << 7);

	if (!(cfg->mode & SPI_MODE_CSPOL_HIGH))
		ctrl |= OMAP3_MCSPI_CSPOL_ACTIVE_LOW;	/* Bit Order */

        if (!(cfg->mode & SPI_MODE_CKPOL_HIGH))
		ctrl |= OMAP3_MCSPI_POLARIY;	/* Active low polarity */

	if (cfg->mode & SPI_MODE_CKPHASE_HALF)
		ctrl |= OMAP3_MCSPI_PHASE;	/* CPHA 1 Phase */

	/* Since all the modules here will be configured as SPI masters,
	 * the "somi" line is to be configured as "input/reception" line
	 * and "simo" has to be configured as "output/transmission" line
	 * we determine which line used as somi and which line as simo here.
	*/
	if (dev->somi) {		// SOMI: D1; SIMO; D0:
		ctrl |= OMAP3_MCSPI_IS_SOMI_INPUT_D1;
		ctrl &= OMAP3_MCSPI_DPE0_SIMO_OUTPUT;
		ctrl |= OMAP3_MCSPI_DPE1_TX_DISABLE;
	} else {				// SOMI: D0; SIMO; D1:
		ctrl &= OMAP3_MCSPI_IS_SOMI_INPUT;
		ctrl &= OMAP3_MCSPI_DPE1_SIMO_OUTPUT;
		ctrl |= OMAP3_MCSPI_DPE0_TX_DISABLE;
	}

	// Set the CS to start of data delay, default is 0.5 clock cycle
	ctrl &= OMAP3_MCSPI_CS_DELAY_MASK;
	ctrl |= (dev->cs_delay << 25 ); // 0, 1, 2, 3  bit 26:25

	/* Calculate the SPI target operational speed.
	* The SPI module is supplied with a 48MHz reference clock.
	*/
	divisor = dev->clock / cfg->clock_rate;
	if (dev->clock % cfg->clock_rate)
		divisor += 1;

	//check if need to set Clock divider granularity: CLKG as 0
	for(i = 0; i < 13; i++)
	{
		if((divisor == speed_divisor[i]) && ((dev->clock % cfg->clock_rate) ==0) ){
			clkg_bit = 0;
			clkd_divisor = i;
			break;
		}
	}

	// set Clock divider granularity -- MCSPI_CHnCONF:CLKG bit
	ctrl &= ~OMAP3_MCSPI_CLKG;
	ctrl |= (clkg_bit << OMAP3_MCSPI_CLKG_OFF); // 0, 1

	if(clkg_bit == 1){
		// calculate EXTCLK value
		for( i=0 ; i < 0xfe; i++)
		{
			if((divisor >= (i << OMAP3_MCSPI_CLKD_LEN)) && (divisor < ((i + 1) << OMAP3_MCSPI_CLKD_LEN))){
				ext_divisor = i;
				break;
			}
		}
		//cacluate CLKD value
		clkd_divisor = divisor - (ext_divisor << OMAP3_MCSPI_CLKD_LEN);
		if(clkd_divisor != 0)
			clkd_divisor -= 1;
	}
	// set MCSPI_CHnCONF: CLKD bits
	ctrl &= ~OMAP3_MCSPI_CLKD;
	ctrl |= (clkd_divisor << OMAP3_MCSPI_CLKD_OFF);

	// update cfg->clock_rate value as SPI_SCLK
	if(clkg_bit == 0){
		cfg->clock_rate = dev->clock / speed_divisor[clkd_divisor] ;
	}else{
		cfg->clock_rate = dev->clock / (clkd_divisor + 1 + (ext_divisor << OMAP3_MCSPI_CLKD_LEN));
	}

	*ext_div = ext_divisor;
	return ctrl;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/dm816x/config.c $ $Rev: 740148 $")
#endif


