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
#include <arm/am335x.h>
#include "board.h"

#define delay_cy(x) {unsigned _delay = ((x+1)>>1); while (_delay--) asm volatile (""); }  /* ~2 cycles per loop at 500MHz, x in 1/500us units (500MHz clock) */
#define delay_32k(n)	delay_cy((n)*33000)		// delay for n 32KHz cycles

void Print_PLL_Config(char *str, uint32_t CLKIN, uint32_t cm_clkmode, uint32_t cm_clksel, uint32_t cm_div_m2)
{
    uint32_t N,M,M2,mult,freq,clkmode,clksel,div_m2;

    clkmode = in32(cm_clkmode);
    clksel  = in32(cm_clksel);
    div_m2  = in32(cm_div_m2);

    M  = (clksel>>8) & 0x7FF;        /* Multiplier bits 18-8 */
    N  = (clksel>>0) & 0x07F;        /* Divisor bits 6-0 */
    M2 = (div_m2>>0) & 0x1F;         /* Post-divisor bits 4-0 */
    mult = (CLKIN * M) / (N + 1);
    freq = mult / M2;
    switch (clkmode & 7)
    {
    case  4: kprintf("%s DPLL in MN Bypass mode\n", str); break;
    case  5: kprintf("%s DPLL in Idle Bypass Low Power mode\n", str); break;
    case  6: kprintf("%s DPLL in Idle Bypass Fast Relock mode\n", str); break;
    case  7: kprintf("%s DPLL in Lock mode:\n", str);
             kprintf("  %s clock %d Mhz [%d/%d]\n", str, freq, mult, M2);
             break;
    default: kprintf("%s DPLL in Reserved mode\n", str); break;
    }
}

void Print_Core_PLL_Config(char *str, uint32_t CLKIN, uint32_t cm_clkmode, uint32_t cm_clksel)
{
    uint32_t N,M,M4,M5,M6,mult,clkmode,clksel,div_m4,div_m5,div_m6;

    clkmode = in32(cm_clkmode);
    clksel  = in32(cm_clksel);
    div_m4  = in32(AM335X_CM_DIV_M4_DPLL_CORE);
    div_m5  = in32(AM335X_CM_DIV_M5_DPLL_CORE);
    div_m6  = in32(AM335X_CM_DIV_M6_DPLL_CORE);

    M  = (clksel>>8) & 0x7FF;        /* Multiplier bits 18-8  */
    N  = (clksel>>0) & 0x07F;        /* Divisor bits 6-0      */ 
    M4 = (div_m4>>0) & 0x1F;         /* Post-divisor bits 4-0 */
    M5 = (div_m5>>0) & 0x1F;         /* Post-divisor bits 4-0 */
    M6 = (div_m6>>0) & 0x1F;         /* Post-divisor bits 4-0 */
    mult = (CLKIN * M) / (N + 1);
    switch (clkmode & 7)
    {
    case  4: kprintf("%s DPLL in MN Bypass mode\n", str); break;
    case  5: kprintf("%s DPLL in Idle Bypass Low Power mode\n", str); break;
    case  6: kprintf("%s DPLL in Idle Bypass Fast Relock mode\n", str); break;
    case  7: kprintf("%s DPLL in Lock mode:\n", str);
             kprintf("  M4 %s clock %d Mhz [%d/%d]\n", str, mult/M4, mult, M4);
             kprintf("  M5 %s clock %d Mhz [%d/%d]\n", str, mult/M5, mult, M5);
             kprintf("  M6 %s clock %d Mhz [%d/%d]\n", str, mult/M6, mult, M6);
             break;
    default: kprintf("%s DPLL in Reserved mode\n", str); break;
    }
}

void display_PLLs(void)
{
    Print_PLL_Config     ("DDR ", 24, AM335X_CM_CLKMODE_DPLL_DDR , AM335X_CM_CLKSEL_DPLL_DDR , AM335X_CM_DIV_M2_DPLL_DDR );
    Print_PLL_Config     ("Disp", 24, AM335X_CM_CLKMODE_DPLL_DISP, AM335X_CM_CLKSEL_DPLL_DISP, AM335X_CM_DIV_M2_DPLL_DISP);
    Print_PLL_Config     ("MPU ", 24, AM335X_CM_CLKMODE_DPLL_MPU , AM335X_CM_CLKSEL_DPLL_MPU , AM335X_CM_DIV_M2_DPLL_MPU );
    Print_PLL_Config     ("PER ", 24, AM335X_CM_CLKMODE_DPLL_PER , AM335X_CM_CLKSEL_DPLL_PER , AM335X_CM_DIV_M2_DPLL_PER );
    Print_Core_PLL_Config("CORE", 24, AM335X_CM_CLKMODE_DPLL_CORE, AM335X_CM_CLKSEL_DPLL_CORE);
}

/*
 * Peripheral PLL generates clocks such as USB_PHY_CLK, PER_CLKOUT_TM, MMC_CLK, SPI_CLK,
 * UART_CLK, I2C_CLK, CLK_24, CLK_32KHz
 *
 * PLL Frequency Formulas:
 * CLKDCOLDO = USB_PHY_CLK = M / (N+1) * CLKINP
 * CLKOUT = PER_CLKOUTM2 = M / (N+1) * CLKINP * (1/M2)
 * CLKOUT is then further divided to produce MMC clock, SPI clock, etc.
 */

void Peripheral_PLL_Config(uint32_t N, uint32_t M, uint32_t M2)
{
    uint32_t clkmode, clksel, div_m2;

    clkmode = in32(AM335X_CM_CLKMODE_DPLL_PER);
    clksel  = in32(AM335X_CM_CLKSEL_DPLL_PER);
    div_m2  = in32(AM335X_CM_DIV_M2_DPLL_PER);

    /* Step 1 - Set the PLL to Bypass Mode */
    out32(AM335X_CM_CLKMODE_DPLL_PER, DPLL_MN_BYP_MODE);

    /* Step 2 - Wait untill PLL is in Bypass Mode */
    while(in32(AM335X_CM_IDLEST_DPLL_PER) != ST_MN_BYPASS);

    /* Step 3 - Configure Multiply and Divide values */
    clksel = clksel & (~DPLL_MULT_MASK);
    clksel = clksel | (DPLL_MULT(M) | DPLL_DIV(N));
    out32(AM335X_CM_CLKSEL_DPLL_PER, clksel);

    /* Step 4 - Configure M2 divider */
    div_m2 = div_m2 & ~DPLL_CLKOUT_DIV_MASK;
    div_m2 = div_m2 | M2;
    out32(AM335X_CM_DIV_M2_DPLL_PER, div_m2);

    /* Step 5 - Set the PLL to Lock Mode */
    clkmode = clkmode | DPLL_LOCK_MODE;
    out32(AM335X_CM_CLKMODE_DPLL_PER, clkmode);

    /* Step 6 - Wait untill PLL enters Lock Lock Mode */
    while(in32(AM335X_CM_IDLEST_DPLL_PER) != 0x1);

}
/*
 * Core PLL generates clock for MPU, Display, L3, L4 clocks, etc.
 *
 * CLKOUT = (M / (N+1)) * CLKINP * (1/M2)
 * 
 */
void CORE_PLL_Config(uint32_t CLKIN, uint32_t N,uint32_t M,uint32_t M4,uint32_t M5,uint32_t M6)
{
    uint32_t ref_clk,clk_out4 = 0;
    uint32_t clkmode,clksel,div_m4,div_m5,div_m6;
    uint32_t clk_out5,clk_out6;

    if (debug_flag >= STARTUP_DEBUG_LEVEL)
        kprintf("\t **** CORE PLL Config is in progress ..........\n");

    ref_clk  = CLKIN / (N + 1);
    clk_out4 = (ref_clk * M) * 2 / M4;
    clk_out5 = (ref_clk * M) * 2 / M5;
    clk_out6 = (ref_clk * M) * 2 / M6;

    clkmode = in32(AM335X_CM_CLKMODE_DPLL_CORE);
    clksel  = in32(AM335X_CM_CLKSEL_DPLL_CORE);
    div_m4  = in32(AM335X_CM_DIV_M4_DPLL_CORE);
    div_m5  = in32(AM335X_CM_DIV_M5_DPLL_CORE);
    div_m6  = in32(AM335X_CM_DIV_M6_DPLL_CORE);

    /* Step 1 - Set PLL to bypass mode */
    clkmode = (clkmode & 0xfffffff8) | DPLL_MN_BYP_MODE;
    out32(AM335X_CM_CLKMODE_DPLL_CORE, clkmode);

    /* Step 2 - Wait until PLL is in Bypass Mode */
    while((in32(AM335X_CM_IDLEST_DPLL_CORE) & ST_MN_BYPASS ) != ST_MN_BYPASS);

    /* Step 3 - Configure Multiply and Divide values */
    clksel = ((M) <<0x8) | (N );
    out32(AM335X_CM_CLKSEL_DPLL_CORE,clksel);

    /* Step 4 - Configure M4, M5, M6 dividers */
    div_m4 = M4;
    out32(AM335X_CM_DIV_M4_DPLL_CORE,div_m4);
    div_m5 = M5;
    out32(AM335X_CM_DIV_M5_DPLL_CORE,div_m5);
    div_m6 = M6;
    out32(AM335X_CM_DIV_M6_DPLL_CORE,div_m6);

    /* Step 5 - Set the PLL to lock mode */ 
    clkmode = (clkmode & 0xfffffff8) | DPLL_LOCK_MODE;
    out32(AM335X_CM_CLKMODE_DPLL_CORE, clkmode);

    if (debug_flag >= STARTUP_DEBUG_LEVEL)
        kprintf("\t **** CORE PLL Config is in progress being Locked ..........    \n");

    /* Step 6 - Wait until PLL enters Lock Mode */
    while ((in32(AM335X_CM_IDLEST_DPLL_CORE) & 0x00000001) != 0x00000001);

    if (debug_flag >= STARTUP_DEBUG_LEVEL)
    {
        kprintf("CLK_OUT4 : %d \n", clk_out4);
        kprintf("CLK_OUT5 : %d \n", clk_out5);
        kprintf("CLK_OUT6 : %d \n", clk_out6);
        kprintf("\t **** CORE PLL Config is DONE .......... \n");
    }
}
/*
 * Display PLL provides pixel clock used for LCD display
 *
 * CLKOUT = (M / (N+1)) * CLKINP * (1/M2)
 */
void Display_PLL_Config(uint32_t CLKIN, uint32_t N, uint32_t M, uint32_t M2)
{
    uint32_t ref_clk,clk_out2 = 0;
    uint32_t clkmode,clksel,div_m2;

    if (debug_flag >= STARTUP_DEBUG_LEVEL)
        kprintf("\t **** DISP PLL Config is in progress ..........\n");


    ref_clk  = CLKIN / (N + 1);
    clk_out2 = (ref_clk * M) * 2 / M2;

    clkmode = in32(AM335X_CM_CLKMODE_DPLL_DISP);
    clksel  = in32(AM335X_CM_CLKSEL_DPLL_DISP);
    div_m2  = in32(AM335X_CM_DIV_M2_DPLL_DISP);

    clkmode = (clkmode & 0xfffffff8) | 0x00000004;
    out32(AM335X_CM_CLKMODE_DPLL_DISP, clkmode);
    while ((in32(AM335X_CM_IDLEST_DPLL_DISP) & 0x00000100) != 0x00000100);

    clksel = ((M) <<0x8) | (N );
    out32(AM335X_CM_CLKSEL_DPLL_DISP, clksel);

    div_m2 = M2;
    out32(AM335X_CM_DIV_M2_DPLL_DISP, div_m2);

    clkmode =(clkmode & 0xfffffff8) | 0x00000007;
    out32(AM335X_CM_CLKMODE_DPLL_DISP,clkmode);

    if (debug_flag >= STARTUP_DEBUG_LEVEL)
        kprintf("\t **** DISP PLL Config is in progress being Locked ..........    \n");

    while ((in32(AM335X_CM_IDLEST_DPLL_DISP) & 0x00000001) != 0x00000001);

    if (debug_flag >= STARTUP_DEBUG_LEVEL)
    {
        kprintf("CLK_OUT2 : %d \n", clk_out2);
        kprintf("\t **** DISP PLL Config is DONE .......... \n");
    }
}
/*
 * DDR PLL provides clocks required by DDR macros and the EMIF.
 *
 * CLKOUT = (M / (N+1)) * CLKINP * (1/M2) 
 */
void DDR_PLL_Config(uint32_t CLKIN, uint32_t N, uint32_t M, uint32_t M2)
{
    uint32_t ref_clk,clk_out2 = 0;
    uint32_t clkmode,clksel,div_m2;

    if (debug_flag >= STARTUP_DEBUG_LEVEL)
        kprintf("\t **** DDR PLL Config is in progress ..........\n");

    ref_clk  = CLKIN / (N + 1);
    clk_out2 = (ref_clk * M) * 2 / M2;

    clkmode = in32(AM335X_CM_CLKMODE_DPLL_DDR);
    clksel  = in32(AM335X_CM_CLKSEL_DPLL_DDR);
    div_m2  = in32(AM335X_CM_DIV_M2_DPLL_DDR);

    /* Step 1 - Set PLL to Bypass Mode */
    clkmode = (clkmode & 0xfffffff8) | 0x00000004;
    out32(AM335X_CM_CLKMODE_DPLL_DDR, clkmode);

    /* Step 2 - Wait for PLL to enter Bypass Mode */
    while ((in32(AM335X_CM_IDLEST_DPLL_DDR) & 0x00000100) != 0x00000100);

    /* Step 3 - Configure Multiply and Divide registers */
    clksel = ((M) <<0x8) | (N);
    out32(AM335X_CM_CLKSEL_DPLL_DDR, clksel);

    /* Step 4 - Configure M2 divisor */
    div_m2 = M2;
    out32(AM335X_CM_DIV_M2_DPLL_DDR, div_m2);

    /* Step 5 - Set PLL to Lock Mode */
    clkmode =(clkmode & 0xfffffff8) | 0x00000007;
    out32(AM335X_CM_CLKMODE_DPLL_DDR,clkmode);

    if (debug_flag >= STARTUP_DEBUG_LEVEL)
        kprintf("\t **** DDR PLL Config is in progress being Locked ..........    \n");

    /* Step 6 - Wait for PLL to enter Lock Mode */
    while ((in32(AM335X_CM_IDLEST_DPLL_DDR) & 0x00000001) != 0x00000001);

    if (debug_flag >= STARTUP_DEBUG_LEVEL)
    {
        kprintf("CLK_OUT2 : %d \n", clk_out2);
        kprintf("\t **** DDR PLL Config is DONE .......... \n");
    }
}

//#define RTC_DEBUG 1
#ifdef RTC_DEBUG
int	save_rtc_regs[3][8];
#endif

/* enable RTC clock */
static void init_rtc(void)
{
   uint32_t regVal;
#if RTC_DEBUG
    save_rtc_regs[0][0] = in32(AM335X_RTC_BASE+AM335X_RTC_CTRL);
    save_rtc_regs[0][1] = in32(AM335X_RTC_BASE+AM335X_RTC_STATUS);
    save_rtc_regs[0][2] = in32(AM335X_RTC_BASE+AM335X_RTC_OSC);
#endif

   /* Unlock RTC registers */
   out32(AM335X_RTC_BASE + AM335X_RTC_KICK0R, 0x83e70b13); 
   out32(AM335X_RTC_BASE + AM335X_RTC_KICK1R, 0x95a4f1e0);

   regVal = in32(AM335X_RTC_BASE + AM335X_RTC_CTRL);

   /* Check if RTC has been disabled or stopped */
   if(((regVal & AM335X_RTC_CTRL_RTC_DISABLE) != 0) || ((regVal & AM335X_RTC_CTRL_RUN_RTC) == 0 )) {
       /* select the clk source - external clk src*/
       out32(AM335X_RTC_BASE + AM335X_RTC_OSC, in32(AM335X_RTC_BASE + AM335X_RTC_OSC) | AM335X_RTC_OSC_32KCLK_SEL_EXT | AM335X_RTC_OSC_32KCLK_EN );  
       out32(AM335X_RTC_BASE + AM335X_RTC_OSC, in32(AM335X_RTC_BASE + AM335X_RTC_OSC) & (~AM335X_RTC_OSC_OSC32K_GZ_DISABLE));

       /* Enable the RTC */
       out32(AM335X_RTC_BASE + AM335X_RTC_CTRL, in32(AM335X_RTC_BASE + AM335X_RTC_CTRL) & (~AM335X_RTC_CTRL_RTC_DISABLE));
       delay_32k(3);
    
#if RTC_DEBUG
    save_rtc_regs[1][0] = in32(AM335X_RTC_BASE+AM335X_RTC_CTRL);
    save_rtc_regs[1][1] = in32(AM335X_RTC_BASE+AM335X_RTC_STATUS);
    save_rtc_regs[1][2] = in32(AM335X_RTC_BASE+AM335X_RTC_OSC);
#endif

       /* Enable oscillator compensation mode */
       out32(AM335X_RTC_BASE + AM335X_RTC_CTRL, in32(AM335X_RTC_BASE + AM335X_RTC_CTRL) | AM335X_RTC_CTRL_AUTO_COMP); 
       delay_32k(30);
   }
   
   /* make sure RTC is set as 24hr mode */
   if((in32(AM335X_RTC_BASE + AM335X_RTC_CTRL) & AM335X_RTC_CTRL_MODE_12_MODE) != 0) {
       out32(AM335X_RTC_BASE + AM335X_RTC_CTRL, in32(AM335X_RTC_BASE + AM335X_RTC_CTRL) & (~AM335X_RTC_CTRL_MODE_12_MODE)); 
       delay_32k(4);
   }
   
   /* RUN the RTC */
   if((in32(AM335X_RTC_BASE + AM335X_RTC_CTRL) & AM335X_RTC_CTRL_RUN_RTC) == 0) {
      out32(AM335X_RTC_BASE + AM335X_RTC_CTRL, in32(AM335X_RTC_BASE+AM335X_RTC_CTRL) | AM335X_RTC_CTRL_RUN_RTC); 
   }
   /* lock RTC registers */
   out32(AM335X_RTC_BASE + AM335X_RTC_KICK0R, 0);  

#if RTC_DEBUG
   save_rtc_regs[2][0] = in32(AM335X_RTC_BASE+AM335X_RTC_CTRL);
   save_rtc_regs[2][1] = in32(AM335X_RTC_BASE+AM335X_RTC_STATUS);
   save_rtc_regs[2][2] = in32(AM335X_RTC_BASE+AM335X_RTC_OSC);
   kprintf("RTC Config Register Changes:\n");
   kprintf(" DM816X_RTC_CTRL:        0x%x  0x%x  0x%x\n",save_rtc_regs[0][0], save_rtc_regs[1][0], save_rtc_regs[2][0]);
   kprintf(" DM816X_RTC_STATUS:      0x%x  0x%x  0x%x\n",save_rtc_regs[0][1], save_rtc_regs[1][1], save_rtc_regs[2][1]);
   kprintf(" DM816X_RTC_OSC:         0x%x  0x%x  0x%x\n",save_rtc_regs[0][2], save_rtc_regs[1][2], save_rtc_regs[2][2]);
#endif

}

/*
 * Enable clocks
 */
#define GPIO_ENABLE_FUNCTIONAL_CLOCK    (1<<18)
void init_clocks()
{

    init_rtc();
    /* 
     * EDMA3 - enable all clocks - Third-Party Channel Controller (TPCC),
     * and Third-Party Transfer Controllers (TPTC)
     */
    out32(AM335X_CM_PER_TPCC_CLKCTRL,    CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_TPTC0_CLKCTRL,   CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_TPTC1_CLKCTRL,   CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_TPTC2_CLKCTRL,   CM_CLOCK_ENABLE);

    /* GPIO - enable all GPIO modules */
    out32(AM335X_CM_WKUP_GPIO0_CLKCTRL,  (CM_CLOCK_ENABLE | GPIO_ENABLE_FUNCTIONAL_CLOCK));
    out32(AM335X_CM_PER_GPIO1_CLKCTRL,   (CM_CLOCK_ENABLE | GPIO_ENABLE_FUNCTIONAL_CLOCK));
    out32(AM335X_CM_PER_GPIO2_CLKCTRL,   (CM_CLOCK_ENABLE | GPIO_ENABLE_FUNCTIONAL_CLOCK));
    out32(AM335X_CM_PER_GPIO3_CLKCTRL,   (CM_CLOCK_ENABLE | GPIO_ENABLE_FUNCTIONAL_CLOCK));

    /* LCD */
    out32(AM335X_CM_PER_OCPWP_CLKCTRL,   CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_LCDC_CLKCTRL,    CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_LCDC_CLKSTCTRL,  CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_OCMCRAM_CLKCTRL, CM_CLOCK_ENABLE);
    out32(AM335X_CLKSEL_LCDC_PIXEL_CLK,  CM_CLOCK_DISABLE);

    out32(AM335X_CM_PER_L4LS_CLKCTRL,    CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_L4FW_CLKCTRL,    CM_CLOCK_ENABLE);

    /* PWMs */
    out32(AM335X_CM_PER_EPWMSS0_CLKCTRL, CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_EPWMSS1_CLKCTRL, CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_EPWMSS2_CLKCTRL, CM_CLOCK_ENABLE);

    /* IPC */
    out32(AM335X_CM_PER_SPINLOCK_CLKCTRL, CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_MAILBOX0_CLKCTRL, CM_CLOCK_ENABLE);

    /* Timers */
    out32(AM335X_CM_WKUP_TIMER0_CLKCTRL, CM_CLOCK_ENABLE);
    out32(AM335X_CM_WKUP_TIMER1_CLKCTRL, CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_TIMER2_CLKCTRL,  CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_TIMER3_CLKCTRL,  CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_TIMER4_CLKCTRL,  CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_TIMER5_CLKCTRL,  CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_TIMER6_CLKCTRL,  CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_TIMER7_CLKCTRL,  CM_CLOCK_ENABLE);

    /* UART0 */
    out32(AM335X_CM_WKUP_UART0_CLKCTRL, CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_UART1_CLKCTRL,  CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_UART2_CLKCTRL,  CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_UART3_CLKCTRL,  CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_UART4_CLKCTRL,  CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_UART5_CLKCTRL,  CM_CLOCK_ENABLE);

    /* GPMC */
    out32(AM335X_CM_PER_GPMC_CLKCTRL, CM_CLOCK_ENABLE);

    /* MMC0 */
    out32(AM335X_CM_PER_MMC0_CLKCTRL, CM_CLOCK_ENABLE);

    /* MMC1 */
    out32(AM335X_CM_PER_MMC1_CLKCTRL, CM_CLOCK_ENABLE);

    /* USB */
    out32(AM335X_CM_PER_USB0_CLKCTRL, CM_CLOCK_ENABLE);

    /* I2C0,1 */
    out32(AM335X_CM_WKUP_I2C0_CLKCTRL, CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_I2C1_CLKCTRL,  CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_I2C2_CLKCTRL,  CM_CLOCK_ENABLE);
    
    /* Ethernet */
    out32(AM335X_CM_PER_CPSW_CLKSTCTRL,  CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_CPGMAC0_CLKCTRL, CM_CLOCK_ENABLE);

    /* CAN */
    out32(AM335X_CM_PER_DCAN0_CLKCTRL, CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_DCAN1_CLKCTRL, CM_CLOCK_ENABLE);

    /* SPI */
    out32(AM335X_CM_PER_SPI0_CLKCTRL, CM_CLOCK_ENABLE);
    out32(AM335X_CM_PER_SPI1_CLKCTRL, CM_CLOCK_ENABLE);

    /* MCASP0 */
    out32(AM335X_CM_PER_MCASP0_CLKCTRL,   CM_CLOCK_ENABLE);

    /* MCASP1 */
    out32(AM335X_CM_PER_MCASP1_CLKCTRL,   CM_CLOCK_ENABLE);

    /* TSCADC */
    out32(AM335X_CM_WKUP_ADC_TSC_CLKCTRL, CM_CLOCK_ENABLE);

    Display_PLL_Config(24000000, 1, 25, 1);

    /* Enable 960MHz clock from peripheral PLL to USB PHY:
     * CLKDCOLDO = (M / (N+1)) * CLKINP
     * Master oscillator is 24MHz so:
     * 960000000 = (960 / (24)) * 24000000
     *
     * Use M2 of 5 to get divided output of 192MHz
     */
    Peripheral_PLL_Config(23, 960, 5);
}


void display_clocks(char *text)
{
    uint32_t r;

    kprintf(text);

    r = in32(AM335X_CM_PER_L4LS_CLKSTCTRL);
    kprintf("AM335X_CM_PER_L4LS_CLKSTCTRL %x\n", r);
    r = in32(AM335X_CM_PER_L3S_CLKSTCTRL);
    kprintf("AM335X_CM_PER_L3S_CLKSTCTRL %x\n", r);
    r = in32(AM335X_CM_PER_L3_CLKSTCTRL);
    kprintf("AM335X_CM_PER_L3_CLKSTCTRL %x\n", r);
    r = in32(AM335X_CM_PER_CPGMAC0_CLKCTRL);
    kprintf("AM335X_CM_PER_CPGMAC0_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_LCDC_CLKCTRL);
    kprintf("AM335X_CM_PER_LCDC_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_USB0_CLKCTRL);
    kprintf("AM335X_CM_PER_USB0_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_TPTC0_CLKCTRL);
    kprintf("AM335X_CM_PER_TPTC0_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_EMIF_CLKCTRL);
    kprintf("AM335X_CM_PER_EMIF_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_OCMCRAM_CLKCTRL);
    kprintf("AM335X_CM_PER_OCMCRAM_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_GPMC_CLKCTRL);
    kprintf("AM335X_CM_PER_GPMC_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_MCASP0_CLKCTRL);
    kprintf("AM335X_CM_PER_MCASP0_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_UART5_CLKCTRL);
    kprintf("AM335X_CM_PER_UART5_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_MMC0_CLKCTRL);
    kprintf("AM335X_CM_PER_MMC0_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_ELM_CLKCTRL);
    kprintf("AM335X_CM_PER_ELM_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_I2C2_CLKCTRL);
    kprintf("AM335X_CM_PER_I2C2_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_I2C1_CLKCTRL);
    kprintf("AM335X_CM_PER_I2C1_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_SPI0_CLKCTRL);
    kprintf("AM335X_CM_PER_SPI0_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_SPI1_CLKCTRL);
    kprintf("AM335X_CM_PER_SPI1_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_L4LS_CLKCTRL);
    kprintf("AM335X_CM_PER_L4LS_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_L4FW_CLKCTRL);
    kprintf("AM335X_CM_PER_L4FW_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_MCASP1_CLKCTRL);
    kprintf("AM335X_CM_PER_MCASP1_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_UART1_CLKCTRL);
    kprintf("AM335X_CM_PER_UART1_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_UART2_CLKCTRL);
    kprintf("AM335X_CM_PER_UART2_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_UART3_CLKCTRL);
    kprintf("AM335X_CM_PER_UART3_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_UART4_CLKCTRL);
    kprintf("AM335X_CM_PER_UART4_CLKCTRL %x\n", r);
    
    r = in32(AM335X_CM_PER_TIMER7_CLKCTRL);
    kprintf("AM335X_CM_PER_TIMER7_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_TIMER2_CLKCTRL);
    kprintf("AM335X_CM_PER_TIMER2_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_TIMER3_CLKCTRL);
    kprintf("AM335X_CM_PER_TIMER3_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_TIMER4_CLKCTRL);
    kprintf("AM335X_CM_PER_TIMER4_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_GPIO1_CLKCTRL);
    kprintf("AM335X_CM_PER_GPIO1_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_GPIO2_CLKCTRL);
    kprintf("AM335X_CM_PER_GPIO2_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_GPIO3_CLKCTRL);
    kprintf("AM335X_CM_PER_GPIO3_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_TPCC_CLKCTRL);
    kprintf("AM335X_CM_PER_TPCC_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_DCAN0_CLKCTRL);
    kprintf("AM335X_CM_PER_DCAN0_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_DCAN1_CLKCTRL);
    kprintf("AM335X_CM_PER_DCAN1_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_EPWMSS1_CLKCTRL);
    kprintf("AM335X_CM_PER_EPWMSS1_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_L3_INSTR_CLKCTRL);
    kprintf("AM335X_CM_PER_L3_INSTR_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_L3_CLKCTRL);
    kprintf("AM335X_CM_PER_L3_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_IEEE5000_CLKCTRL);
    kprintf("AM335X_CM_PER_IEEE5000_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_ICSS_CLKCTRL);
    kprintf("AM335X_CM_PER_ICSS_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_TIMER5_CLKCTRL);
    kprintf("AM335X_CM_PER_TIMER5_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_TIMER6_CLKCTRL);
    kprintf("AM335X_CM_PER_TIMER6_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_MMC1_CLKCTRL);
    kprintf("AM335X_CM_PER_MMC1_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_MMC2_CLKCTRL);
    kprintf("AM335X_CM_PER_MMC2_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_TPTC1_CLKCTRL);
    kprintf("AM335X_CM_PER_TPTC1_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_TPTC2_CLKCTRL);
    kprintf("AM335X_CM_PER_TPTC2_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_SPINLOCK_CLKCTRL);
    kprintf("AM335X_CM_PER_SPINLOCK_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_MAILBOX0_CLKCTRL);
    kprintf("AM335X_CM_PER_MAILBOX0_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_L4HS_CLKSTCTRL);
    kprintf("AM335X_CM_PER_L4HS_CLKSTCTRL %x\n", r);
    r = in32(AM335X_CM_PER_L4HS_CLKCTRL);
    kprintf("AM335X_CM_PER_L4HS_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_OCPWP_L3_CLKSTCT);
    kprintf("AM335X_CM_PER_OCPWP_L3_CLKSTCT %x\n", r);
    r = in32(AM335X_CM_PER_OCPWP_CLKCTRL);
    kprintf("AM335X_CM_PER_OCPWP_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_ICSS_CLKSTCTRL);
    kprintf("AM335X_CM_PER_ICSS_CLKSTCTRL %x\n", r);
    r = in32(AM335X_CM_PER_CPSW_CLKSTCTRL);
    kprintf("AM335X_CM_PER_CPSW_CLKSTCTRL %x\n", r);
    r = in32(AM335X_CM_PER_LCDC_CLKSTCTRL);
    kprintf("AM335X_CM_PER_LCDC_CLKSTCTRL %x\n", r);
    r = in32(AM335X_CM_PER_CLKDIV32K_CLKCTRL);
    kprintf("AM335X_CM_PER_CLKDIV32K_CLKCTRL %x\n", r);
    r = in32(AM335X_CM_PER_CLK_24MHZ_CLKSTCT);
    kprintf("AM335X_CM_PER_CLK_24MHZ_CLKSTCT %x\n", r);
    
    r = in32(AM335X_CLKSEL_TIMER7_CLK);
    kprintf("AM335X_CLKSEL_TIMER7_CLK %x\n", r);
    r = in32(AM335X_CLKSEL_TIMER2_CLK);
    kprintf("AM335X_CLKSEL_TIMER2_CLK %x\n", r);
    r = in32(AM335X_CLKSEL_TIMER3_CLK);
    kprintf("AM335X_CLKSEL_TIMER3_CLK %x\n", r);
    r = in32(AM335X_CLKSEL_TIMER4_CLK);
    kprintf("AM335X_CLKSEL_TIMER4_CLK %x\n", r);
    r = in32(AM335X_CM_MAC_CLKSEL);
    kprintf("AM335X_CM_MAC_CLKSEL %x\n", r);
    r = in32(AM335X_CLKSEL_TIMER5_CLK);
    kprintf("AM335X_CLKSEL_TIMER5_CLK %x\n", r);
    r = in32(AM335X_CLKSEL_TIMER6_CLK);
    kprintf("AM335X_CLKSEL_TIMER6_CLK %x\n", r);
    r = in32(AM335X_CM_CPTS_RFT_CLKSEL);
    kprintf("AM335X_CM_CPTS_RFT_CLKSEL %x\n", r);
    r = in32(AM335X_CLKSEL_TIMER1MS_CLK);
    kprintf("AM335X_CLKSEL_TIMER1MS_CLK %x\n", r);
    r = in32(AM335X_CLKSEL_GFX_FCLK);
    kprintf("AM335X_CLKSEL_GFX_FCLK %x\n", r);
    r = in32(AM335X_CLKSEL_ICSS_OCP_CLK);
    kprintf("AM335X_CLKSEL_ICSS_OCP_CLK %x\n", r);
    r = in32(AM335X_CLKSEL_LCDC_PIXEL_CLK);
    kprintf("AM335X_CLKSEL_LCDC_PIXEL_CLK %x\n", r);
    r = in32(AM335X_CLKSEL_WDT1_CLK);
    kprintf("AM335X_CLKSEL_WDT1_CLK %x\n", r);
    r = in32(AM335X_CLKSEL_GPIO0_DBCLK);
    kprintf("AM335X_CLKSEL_GPIO0_DBCLK %x\n", r);

    r = in32(AM335X_CM_AUTOIDLE_DPLL_DISP);
    kprintf("AM335X_CM_AUTOIDLE_DPLL_DISP %x\n", r);
    r = in32(AM335X_CM_IDLEST_DPLL_DISP);
    kprintf("AM335X_CM_IDLEST_DPLL_DISP %x\n", r);
    r = in32(AM335X_CM_SSC_DELTAMSTEP_DPLL_DISP);
    kprintf("AM335X_CM_SSC_DELTAMSTEP_DPLL_DISP %x\n", r);
    r = in32(AM335X_CM_SSC_MODFREQDIV_DPLL_DISP);
    kprintf("AM335X_CM_SSC_MODFREQDIV_DPLL_DISP %x\n", r);
    r = in32(AM335X_CM_CLKSEL_DPLL_DISP);
    kprintf("AM335X_CM_CLKSEL_DPLL_DISP %x\n", r);
    r = in32(AM335X_CM_CLKMODE_DPLL_DISP);
    kprintf("AM335X_CM_CLKMODE_DPLL_DISP %x\n", r);
    r = in32(AM335X_CM_DIV_M2_DPLL_DISP);
    kprintf("AM335X_CM_DIV_M2_DPLL_DISP %x\n", r);

    r = in32(AM335X_CM_AUTOIDLE_DPLL_DDR);
    kprintf("AM335X_CM_AUTOIDLE_DPLL_DDR %x\n", r);
    r = in32(AM335X_CM_IDLEST_DPLL_DDR);
    kprintf("AM335X_CM_IDLEST_DPLL_DDR %x\n", r);
    r = in32(AM335X_CM_SSC_DELTAMSTEP_DPLL_DDR);
    kprintf("AM335X_CM_SSC_DELTAMSTEP_DPLL_DDR %x\n", r);
    r = in32(AM335X_CM_SSC_MODFREQDIV_DPLL_DDR);
    kprintf("AM335X_CM_SSC_MODFREQDIV_DPLL_DDR %x\n", r);
    r = in32(AM335X_CM_CLKSEL_DPLL_DDR);
    kprintf("AM335X_CM_CLKSEL_DPLL_DDR %x\n", r);
    r = in32(AM335X_CM_CLKMODE_DPLL_DDR);
    kprintf("AM335X_CM_CLKMODE_DPLL_DDR %x\n", r);
    r = in32(AM335X_CM_DIV_M2_DPLL_DDR);
    kprintf("AM335X_CM_DIV_M2_DPLL_DDR %x\n", r);

    r = in32(AM335X_CM_AUTOIDLE_DPLL_MPU);
    kprintf("AM335X_CM_AUTOIDLE_DPLL_MPU %x\n", r);
    r = in32(AM335X_CM_IDLEST_DPLL_MPU);
    kprintf("AM335X_CM_IDLEST_DPLL_MPU %x\n", r);
    r = in32(AM335X_CM_SSC_DELTAMSTEP_DPLL_MPU);
    kprintf("AM335X_CM_SSC_DELTAMSTEP_DPLL_MPU %x\n", r);
    r = in32(AM335X_CM_SSC_MODFREQDIV_DPLL_MPU);
    kprintf("AM335X_CM_SSC_MODFREQDIV_DPLL_MPU %x\n", r);
    r = in32(AM335X_CM_CLKSEL_DPLL_MPU);
    kprintf("AM335X_CM_CLKSEL_DPLL_MPU %x\n", r);
    r = in32(AM335X_CM_CLKMODE_DPLL_MPU);
    kprintf("AM335X_CM_CLKMODE_DPLL_MPU %x\n", r);
    r = in32(AM335X_CM_DIV_M2_DPLL_MPU);
    kprintf("AM335X_CM_DIV_M2_DPLL_MPU %x\n", r);
    r = in32(AM335X_CM_WKUP_ADC_TSC_CLKCTRL);
    kprintf("CM_WKUP_ADC_TSC_CLKCTRL %x\n", r);

    r = in32(AM335X_GPIO0_BASE+GPIO_CTRL);
    kprintf("AM335X_GPIO0_BASE %x\n", r);
    r = in32(AM335X_GPIO1_BASE+GPIO_CTRL);
    kprintf("AM335X_GPIO1_BASE %x\n", r);
    r = in32(AM335X_GPIO2_BASE+GPIO_CTRL);
    kprintf("AM335X_GPIO2_BASE %x\n", r);
    r = in32(AM335X_GPIO3_BASE+GPIO_CTRL);
    kprintf("AM335X_GPIO3_BASE %x\n", r);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/ti-am335x/init_clocks.c $ $Rev: 761375 $")
#endif

