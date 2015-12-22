/*
 * $QNXLicenseC:
 * Copyright 2010, QNX Software Systems.
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
#include <arm/am335x.h>
#include "am335x_pinmux.h"

/* Pinmux for BeagleBoard */

static void init_uart0_pin_mux(void)
{
    out32(conf_uart0_rxd        , (MODE(0) | PULLUP_EN | RXACTIVE | PULLUDEN));    /* UART0_RXD  */
    out32(conf_uart0_txd        , (MODE(0) | PULLUDEN ));                          /* UART0_TXD  */
    out32(conf_uart0_ctsn       , (MODE(0) | RXACTIVE | PULLUDEN));                /* UART0_CTSN */
    out32(conf_uart0_rtsn       , (MODE(0) | PULLUP_EN | PULLUDEN));               /* UART0_RTSN */
}

static void init_i2c0_pin_mux(void)
{
    out32(conf_i2c0_sda         , (MODE(0) | RXACTIVE | PULLUDEN | SLEWCTRL));     /* I2C_DATA  */
    out32(conf_i2c0_scl         , (MODE(0) | RXACTIVE | PULLUDEN | SLEWCTRL));     /* I2C_SCLK  */
}

static void init_mmc0_pin_mux(void)
{
    out32(conf_mmc0_dat3        , (MODE(0) | RXACTIVE | PULLUP_EN));               /* MMC0_DAT3  */
    out32(conf_mmc0_dat2        , (MODE(0) | RXACTIVE | PULLUP_EN));               /* MMC0_DAT2  */
    out32(conf_mmc0_dat1        , (MODE(0) | RXACTIVE | PULLUP_EN));               /* MMC0_DAT1  */
    out32(conf_mmc0_dat0        , (MODE(0) | RXACTIVE | PULLUP_EN));               /* MMC0_DAT0  */
    out32(conf_mmc0_clk         , (MODE(0) | RXACTIVE | PULLUP_EN));               /* MMC0_CLK   */
    out32(conf_mmc0_cmd         , (MODE(0) | RXACTIVE | PULLUP_EN));               /* MMC0_CMD   */
    out32(conf_spi0_cs1         , (MODE(5) | RXACTIVE | PULLUP_EN));               /* MMC0_CD    */
}

static void init_leds_pin_mux(void)
{
    out32(conf_gpmc_a5          , MODE(7));                                        /* USR0  */
    out32(conf_gpmc_a6          , MODE(7));                                        /* USR1  */
    out32(conf_gpmc_a7          , MODE(7));                                        /* USR2  */
    out32(conf_gpmc_a8          , MODE(7));                                        /* USR3  */
}

static void init_gmii1_pin_mux(void)
{
    out32(conf_mii1_txclk       , MODE(0) | RXACTIVE);                             /* GMII1_TXCLK  */
    out32(conf_mii1_txd0        , MODE(0));                                        /* GMII1_TXD0   */
    out32(conf_mii1_txd1        , MODE(0));                                        /* GMII1_TXD1   */
    out32(conf_mii1_txd2        , MODE(0));                                        /* GMII1_TXD2   */
    out32(conf_mii1_txd3        , MODE(0));                                        /* GMII1_TXD3   */
    out32(conf_mii1_txen        , MODE(0));                                        /* GMII1_TXEN   */
    out32(conf_mii1_crs         , MODE(0));                                        /* GMII1_CRS    */
    out32(conf_mii1_col         , MODE(0));                                        /* GMII1_COL    */
    out32(conf_mii1_rxclk       , MODE(0) | RXACTIVE);                             /* GMII1_RXCLK  */
    out32(conf_mii1_rxd0        , MODE(0) | RXACTIVE);                             /* GMII1_RXD0   */
    out32(conf_mii1_rxd1        , MODE(0) | RXACTIVE);                             /* GMII1_RXD1   */
    out32(conf_mii1_rxd2        , MODE(0) | RXACTIVE);                             /* GMII1_RXD2   */
    out32(conf_mii1_rxd3        , MODE(0) | RXACTIVE);                             /* GMII1_RXD3   */
    out32(conf_mii1_rxerr       , MODE(0) | RXACTIVE);                             /* GMII1_RXERR  */
    out32(conf_mii1_rxdv        , MODE(0) | RXACTIVE);                             /* GMII1_RXDV   */
    out32(conf_mdio_data        , MODE(0) | RXACTIVE | PULLUP_EN);                 /* MDIO_DATA    */
    out32(conf_mdio_clk         , MODE(0) | PULLUP_EN);                            /* MDIO_CLK     */
}

/* UART1 */
static void init_uart1_pin_mux(void)
{
	out32(conf_uart1_rxd        , (MODE(0) | PULLUP_EN | RXACTIVE));    		   /* [P9 26] UART1_RXD  */
	out32(conf_uart1_txd        , (MODE(0) | PULLUDEN ));           			   /* [P9 24] UART1_TXD  */
}

void init_pinmux()
{
    init_i2c0_pin_mux();
    init_leds_pin_mux();
    init_gmii1_pin_mux(); 
    init_mmc0_pin_mux();
    init_uart0_pin_mux();
    /* ======================== *\
     * BBB Pinmux for UART1		*
    \* ======================== */
    init_uart1_pin_mux();
}

/*
 **********************
 **Pinmuxes for Capes**
 **********************
 */

 /*
  * Note that pinmuxes for capes depend on what cape is used. Different capes need
  * different pinmuxes depending on what devices a cape implements (uart, lcd, etc).
  *
  * The functions below are provided as-is only. They demonstrate as an example how
  * a pinmux for a specific device on a cape could be configured. The functions provided
  * are not a complete set, and you may have to implement your own pinmux for your cape.
  * When no cape is present, a default profile of 0 is assumed.
  */

  
/*
 ****************************
 **          UART          **
 ****************************
 */

/* 
 * UART1 - Note: this multiplex choice cannot be used in conjunction with can1 or mmc1.
 * Untested - example configuration for cape uart1
 */
//static void init_uart1_pin_mux(void)
//{                                                                                /*  [expansion header(P8 or P9)  expansion header pin ]*/
//
//    out32(conf_uart1_rxd        , (MODE(0) | PULLUP_EN | RXACTIVE));             /* [P9 26] UART1_RXD  */
//    out32(conf_uart1_txd        , (MODE(0) | PULLUDEN));                         /* [P9 24] UART1_TXD  */
//}

/*
 * UART2 - Note: this multiplex choice cannot be used in conjunction with spi1.
 * Untested - example configuration for cape uart2
 */
static void init_uart2_pin_mux(void)
{                                                                        
   out32(conf_spi0_sclk        , (MODE(1) | PULLUP_EN | RXACTIVE));              /* [P9 22] UART2_RXD  */
   out32(conf_spi0_d0          , (MODE(1) | PULLUDEN));                          /* [P9 21] UART2_TXD  */
}

/* UART4 */
static void init_uart4_pin_mux(void)
{                                                                        
    out32(conf_gpmc_wait0       , (MODE(6) | PULLUP_EN | RXACTIVE));             /* [P9 11] UART4_RXD  */
    out32(conf_gpmc_wpn         , (MODE(6) | PULLUDEN));                         /* [P9 13] UART4_TXD  */
}

/*
 ****************************
 **          I2C           **
 ****************************
 */

/*
 * I2C1 - Note: this multiplex choice cannot be used in conjunction with spi0.
 * Untested - example configuration for cape i2c1
 */

static void init_i2c1_pin_mux(void)
{                                                                               
    out32(conf_spi0_d1          , (MODE(2) | RXACTIVE | PULLUDEN | SLEWCTRL));    /* [P9 18] I2C1_DATA  */
    out32(conf_spi0_cs0         , (MODE(2) | RXACTIVE | PULLUDEN | SLEWCTRL));    /* [P9 17] I2C1_SCLK  */
}

/* I2C2 */
static void init_i2c2_pin_mux(void)
{                                                                                
    out32(conf_uart1_ctsn       , (MODE(3) | RXACTIVE | PULLUDEN | SLEWCTRL));    /* [P9 20] I2C2_DATA  */
    out32(conf_uart1_rtsn       , (MODE(3) | RXACTIVE | PULLUDEN | SLEWCTRL));    /* [P9 19] I2C2_SCLK  */
}

/*
 ****************************
 **          SPI           **
 ****************************
 */

/*
 * SPI0 - Note: this multiplex choice cannot be used in conjunction with i2c1 and uart2.
 * Untested - example configuration for cape spi0
 */

#if 0
static void init_spi0_pin_mux(void)
{                                                                                
    out32(conf_spi0_sclk        , MODE(0) | PULLUDEN | RXACTIVE);                /* [P9 22] SPI0_SCLK  */
    out32(conf_spi0_d0          , MODE(0) | PULLUDEN | PULLUP_EN | RXACTIVE);    /* [P9 21] SPI0_D0    */
    out32(conf_spi0_d1          , MODE(0) | PULLUDEN | RXACTIVE);                /* [P9 18] SPI0_D1    */
    out32(conf_spi0_cs0         , MODE(0) | PULLUDEN | PULLUP_EN | RXACTIVE);    /* [P9 17] SPI0_CS0   */
}
#endif

/*
 * SPI1 - Note: this multiplex choice cannot be used in conjunction with HDMI.
 */
static void init_spi1_pin_mux(void)
{                                                                                
    out32(conf_mcasp0_aclkx     , MODE(3) | PULLUDEN | RXACTIVE);                /* [P9 31] SPI1_SCLK  */
    out32(conf_mcasp0_fsx       , MODE(3) | PULLUDEN | PULLUP_EN | RXACTIVE);    /* [P9 29] SPI1_D0    */
    out32(conf_mcasp0_axr0      , MODE(3) | PULLUDEN | RXACTIVE);                /* [P9 30] SPI1_D1    */
    out32(conf_mcasp0_ahclkr    , MODE(3) | PULLUDEN | PULLUP_EN | RXACTIVE);    /* [P9 28] SPI1_CS0   */
}


/*
 ****************************
 **          MMC           **
 ****************************
 */

/*
 * MMC1 - Note: this multiplex choice cannot be used in conjunction with uart1  or uart 4.
 * Untested - example configuration for cape mmc1
 */
static void init_mmc1_pin_mux(void)
{                                                                        
    out32(conf_gpmc_ad3         , (MODE(1) | RXACTIVE));                         /* [P8  6] MMC1_DAT3  */
    out32(conf_gpmc_ad2         , (MODE(1) | RXACTIVE));                         /* [P8  5] MMC1_DAT2  */
    out32(conf_gpmc_ad1         , (MODE(1) | RXACTIVE));                         /* [P8 24] MMC1_DAT1  */
    out32(conf_gpmc_ad0         , (MODE(1) | RXACTIVE));                         /* [P8 25] MMC1_DAT0  */
    out32(conf_gpmc_csn1        , (MODE(2) | RXACTIVE | PULLUP_EN));             /* [P8 21] MMC1_CLK   */
    out32(conf_gpmc_csn2        , (MODE(2) | RXACTIVE | PULLUP_EN));             /* [P8 20] MMC1_CMD   */
    out32(conf_uart1_rxd        , (MODE(1) | RXACTIVE | PULLUP_EN));             /* [P9 26] MMC1_WP    */
    out32(conf_gpmc_wait0       , (MODE(4) | RXACTIVE));                         /* [P9 11] MMC1_CD    */
}

/*
 ****************************
 **          LCD           **
 ****************************
 */

/*
 * LCD - Note: this multiplex choice cannot be used in conjunction with hdmi.
 * Untested - example configuration for cape LCD
 */

 static void init_lcd_pin_mux(void)
{                                                            
    out32(conf_lcd_data0        , MODE(0));                                      /* [P8 45] LCD_DATA0      */
    out32(conf_lcd_data1        , MODE(0));                                      /* [P8 46] LCD_DATA1      */
    out32(conf_lcd_data2        , MODE(0));                                      /* [P8 43] LCD_DATA2      */
    out32(conf_lcd_data3        , MODE(0));                                      /* [P8 44] LCD_DATA3      */
    out32(conf_lcd_data4        , MODE(0));                                      /* [P8 41] LCD_DATA4      */
    out32(conf_lcd_data5        , MODE(0));                                      /* [P8 42] LCD_DATA5      */
    out32(conf_lcd_data6        , MODE(0));                                      /* [P8 39] LCD_DATA6      */
    out32(conf_lcd_data7        , MODE(0));                                      /* [P8 40] LCD_DATA7      */
    out32(conf_lcd_data8        , MODE(0));                                      /* [P8 37] LCD_DATA8      */
    out32(conf_lcd_data9        , MODE(0));                                      /* [P8 38] LCD_DATA9      */
    out32(conf_lcd_data10       , MODE(0));                                      /* [P8 36] LCD_DATA10     */
    out32(conf_lcd_data11       , MODE(0));                                      /* [P8 34] LCD_DATA11     */
    out32(conf_lcd_data12       , MODE(0));                                      /* [P8 35] LCD_DATA12     */
    out32(conf_lcd_data13       , MODE(0));                                      /* [P8 33] LCD_DATA13     */
    out32(conf_lcd_data14       , MODE(0));                                      /* [P8 31] LCD_DATA14     */
    out32(conf_lcd_data15       , MODE(0));                                      /* [P8 32] LCD_DATA15     */
    out32(conf_gpmc_ad15        , MODE(1));                                      /* [P8 15] LCD_DATA16     */
    out32(conf_gpmc_ad14        , MODE(1));                                      /* [P8 16] LCD_DATA17     */
    out32(conf_gpmc_ad13        , MODE(1));                                      /* [P8 11] LCD_DATA18     */
    out32(conf_gpmc_ad12        , MODE(1));                                      /* [P8 12] LCD_DATA19     */
    out32(conf_gpmc_ad11        , MODE(1));                                      /* [P8 17] LCD_DATA20     */
    out32(conf_gpmc_ad10        , MODE(1));                                      /* [P8 14] LCD_DATA21     */
    out32(conf_gpmc_ad9         , MODE(1));                                      /* [P8 13] LCD_DATA22     */
    out32(conf_gpmc_ad8         , MODE(1));                                      /* [P8 19] LCD_DATA23     */
    out32(conf_lcd_vsync        , MODE(0) | PULLUP_EN);                          /* [P8 27] LCD_VSYNC      */
    out32(conf_lcd_hsync        , MODE(0) | PULLUP_EN);                          /* [P8 29] LCD_HSYNC      */
    out32(conf_lcd_pclk         , MODE(0) | PULLUP_EN);                          /* [P8 28] LCD_PCLK       */
    out32(conf_lcd_ac_bias_en   , MODE(0) | PULLUDDIS);                          /* [P8 30] LCD_AC_BIAS_EN */
    /* backlight and/or power control */
    out32(conf_ecap0_in_pwm0_out, MODE(7) | PULLUDDIS);                          /* [P9 42] LCD_AC_BIAS_EN */
}

/*
 ****************************
 **          HDMI           **
 ****************************
 */

/*
 * HDMI - Note: this multiplex choice cannot be used in conjunction with lcd or SPI1.
 */

 static void init_hdmi_pin_mux(void)
{                                                            
    uint32_t r;

    out32(conf_xdma_event_intr0 , MODE(3));             	/*  				 		*/
	out32(conf_lcd_data0        , MODE(0) | PULLUDDIS);		/* [P8 45] LCD_DATA0 		*/
	out32(conf_lcd_data1        , MODE(0) | PULLUDDIS);		/* [P8 46] LCD_DATA1  		*/
	out32(conf_lcd_data2        , MODE(0) | PULLUDDIS);		/* [P8 43] LCD_DATA2  		*/
	out32(conf_lcd_data3        , MODE(0) | PULLUDDIS);		/* [P8 44] LCD_DATA3  		*/
	out32(conf_lcd_data4        , MODE(0) | PULLUDDIS);		/* [P8 41] LCD_DATA4  		*/
	out32(conf_lcd_data5        , MODE(0) | PULLUDDIS);		/* [P8 42] LCD_DATA5  		*/
	out32(conf_lcd_data6        , MODE(0) | PULLUDDIS);		/* [P8 39] LCD_DATA6  		*/
	out32(conf_lcd_data7        , MODE(0) | PULLUDDIS);		/* [P8 40] LCD_DATA7  		*/
	out32(conf_lcd_data8        , MODE(0) | PULLUDDIS);		/* [P8 37] LCD_DATA8  		*/     
	out32(conf_lcd_data9        , MODE(0) | PULLUDDIS);		/* [P8 38] LCD_DATA9  		*/     
	out32(conf_lcd_data10       , MODE(0) | PULLUDDIS);		/* [P8 36] LCD_DATA10  		*/    
	out32(conf_lcd_data11       , MODE(0) | PULLUDDIS);		/* [P8 34] LCD_DATA11  		*/    
	out32(conf_lcd_data12       , MODE(0) | PULLUDDIS);		/* [P8 35] LCD_DATA12  		*/    
	out32(conf_lcd_data13       , MODE(0) | PULLUDDIS);		/* [P8 33] LCD_DATA13  		*/    
	out32(conf_lcd_data14       , MODE(0) | PULLUDDIS);		/* [P8 31] LCD_DATA14 		*/
	out32(conf_lcd_data15       , MODE(0) | PULLUDDIS);		/* [P8 32] LCD_DATA15 		*/    
	out32(conf_lcd_vsync        , MODE(0) | PULLUDDIS);		/* [P8 27] LCD_VSYNC  		*/    
	out32(conf_lcd_hsync        , MODE(0) | PULLUDDIS);		/* [P8 29] LCD_HSYNC  		*/    
	out32(conf_lcd_pclk         , MODE(0) | PULLUDDIS);		/* [P8 28] LCD_PCLK   		*/    
	out32(conf_lcd_ac_bias_en   , MODE(0) | PULLUDDIS);		/* [P8 30] LCD_AC_BIAS_EN 	*/

	/* HDMI Audio  */
    out32(conf_mcasp0_aclkx     , MODE(0) | PULLUDEN | RXACTIVE);
    out32(conf_mcasp0_fsx       , MODE(0) | PULLUDEN | RXACTIVE);
    out32(conf_mcasp0_ahclkr    , MODE(2) | PULLUDEN | RXACTIVE);                 /* McASP0_axr2 */
    out32(conf_mcasp0_ahclkx    , MODE(0) | PULLUDEN | RXACTIVE);
    out32(conf_gpmc_a11         , MODE(7) | PULLUDDIS);     /* GPIO1_27 to enable 24.576Mhz Audio clock */

    /* Switch GPIO1_27 to output mode */
    r = in32(AM335X_GPIO1_BASE + GPIO_OE);
    r &= ~(1 << 27); /* set 27 bit low to enable output. */
    out32(AM335X_GPIO1_BASE + GPIO_OE, r);

    /* set GPIO1_13 high to enable 24.576Mhz Audio clock */
    out32(AM335X_GPIO1_BASE + GPIO_SETDATAOUT, 1 << 27);
}

typedef struct cape_profile
{
	int uart1;
	int uart2;
	int uart3;
	int uart4;
	int uart5;
	int i2c1;
	int i2c2;
	int spi0;
	int spi1;
	int mmc1;
	int can0;
	int can1;
	int lcd;
	int hdmi;
} CAPE_PROFILE;

CAPE_PROFILE cape_profiles[] = {
//        uart1
//        | uart2
//        | | uart3
//        | | | uart4
//        | | | | uart5
//        | | | | | i2c1
//        | | | | | | i2c2
//        | | | | | | | spi0
//        | | | | | | | | spi1
//        | | | | | | | | | mmc1
//        | | | | | | | | | | can0
//        | | | | | | | | | | | can1
//        | | | | | | | | | | | | lcd
//        | | | | | | | | | | | | | hdmi
//        | | | | | | | | | | | | | | gpmc
        { 1,0,0,1,1,1,1,0,1,0,0,0,0,0 },	/* 00 - this is the default profile where no cape is present */
        { 1,0,0,1,0,1,1,0,0,1,0,0,0,1 },	/* 01 - BeagleBone Black with HDMI */
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 },	/* 02 - add your own profile here */
};

void init_pinmux_capes(int profile, int showchoices)
{
	// Any UARTS?
	if (cape_profiles[profile].uart1)
	{
		if (showchoices) kprintf("uart1\n");
		init_uart1_pin_mux();
	}
	if (cape_profiles[profile].uart2)
	{
		if (showchoices) kprintf("uart2\n");
		init_uart2_pin_mux();
	}
		
	#if 0
	/* init_uart3_pin_mux() not implemented */
	if (cape_profiles[profile].uart3)
	{
		if (showchoices) kprintf("uart3\n");
		init_uart3_pin_mux();
	}
	#endif

	if (cape_profiles[profile].uart4)
	{
		if (showchoices) kprintf("uart4\n");
		init_uart4_pin_mux();
	}

	#if 0
	/* init_uart5_pin_mux() not implemented */
	if (cape_profiles[profile].uart5)
	{
		if (showchoices) kprintf("uart5\n");
		init_uart5_pin_mux();
	}
	#endif

	// Any i2c?
	if (cape_profiles[profile].i2c1)
	{
		if (showchoices) kprintf("i2c1\n");
		init_i2c1_pin_mux();
	}
	if (cape_profiles[profile].i2c2)
	{
		if (showchoices) kprintf("i2c2\n");
		init_i2c2_pin_mux();
	}
	
	// Any SPI?
	#if 0
	/* init_spi0_pin_mux() not implemented */
	if (cape_profiles[profile].spi0)
	{
		if (showchoices) kprintf("spi0\n");
		init_spi0_pin_mux();
	}
	#endif

	if (cape_profiles[profile].spi1)
	{
		if (showchoices) kprintf("spi1\n");
		init_spi1_pin_mux();
	}

	// MMC1?
	if (cape_profiles[profile].mmc1)
	{
		if (showchoices) kprintf("mmc1\n");
		init_mmc1_pin_mux();
	}

	
	// Any CAN?
	#if 0
	/* init_can0_pin_mux() not implemented */
	if (cape_profiles[profile].can0)
	{
		if (showchoices) kprintf("can0\n");
		init_can0_pin_mux();
	}
	#endif

	#if 0
	/* init_can1_pin_mux() not implemented */
	if (cape_profiles[profile].can1)
	{
		if (showchoices) kprintf("can1\n");
		init_can1_pin_mux();
	}
	#endif

	// LCD?
	if (cape_profiles[profile].lcd)
	{
		if (showchoices) kprintf("lcd\n");
		init_lcd_pin_mux();
	}

	// HDMI?
	if (cape_profiles[profile].hdmi)
	{
		if (showchoices) kprintf("hdmi\n");
		init_hdmi_pin_mux();
	}
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/ti-am335x/beaglebone/init_pinmux.c $ $Rev: 761911 $")
#endif
