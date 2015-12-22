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

#include "externs.h"
#include <sys/mman.h>

#ifdef PWR_MAN
#define OMAP_CONTROL_ID_CODE 0x4A002204
#define OMAP5430_CONTROL_ID_CODE 0x0B94202F
#define OMAP5432_CONTROL_ID_CODE 0x0B99802F

int get_chip_type()
{ 
    uintptr_t port = mmap_device_io( 4, OMAP_CONTROL_ID_CODE); 
    if (port == MAP_DEVICE_FAILED) {
        return 0;
    }
    uint32_t chip_type = in32(port);
    if (chip_type == OMAP5430_CONTROL_ID_CODE || chip_type == OMAP5432_CONTROL_ID_CODE){
        rm_l4per_uart1_context = RM_L4PER_UART1_CONTEXT_OMAP5;
        rm_l4per_uart2_context = RM_L4PER_UART2_CONTEXT_OMAP5;
        rm_l4per_uart3_context = RM_L4PER_UART3_CONTEXT_OMAP5;
        rm_l4per_uart4_context = RM_L4PER_UART4_CONTEXT_OMAP5;
    }
    munmap_device_io(port, 4);
    return 1;
}

#endif

void
set_port(unsigned port, unsigned mask, unsigned data) 
{
	unsigned char c;

	c = read_omap(port);
	write_omap(port, (c & ~mask) | (data & mask));
}

static void
clear_device(const uintptr_t *port) 
{
	write_omap(port[OMAP_UART_IER], 0);					// Disable all interrupts
	/* Clear FIFOs */
	set_port(port[OMAP_UART_FCR], OMAP_FCR_RXCLR | OMAP_FCR_TXCLR, OMAP_FCR_RXCLR | OMAP_FCR_TXCLR); 
	read_omap(port[OMAP_UART_LSR]);						// Clear Line Status Interrupt
	read_omap(port[OMAP_UART_MSR]);						// Clear Modem Interrupt
}

//
// Clean up the device then add it to the interrupt list and enable it.
//
void
ser_attach_intr(DEV_OMAP *dev) {
	uintptr_t	*port = dev->port;
	// interrupt sources except transmit and modem status interrupt
	unsigned	ier = OMAP_IER_RHR|OMAP_IER_LS;
	
	// According to the National bug sheet you must wait for the transmit
	// holding register to be empty.
	do {
	} while((read_omap(port[OMAP_UART_LSR]) & OMAP_LSR_TXRDY) == 0);

	clear_device(port);

	dev->iid = InterruptAttach(dev->intr, ser_intr, dev, 0, 0);

	// Enable modem status interrupt (default)
	if (!dev->no_msr_int) {
		ier |= OMAP_IER_MS;
	}
	// Enable interrupt sources.
	write_omap(port[OMAP_UART_IER], ier);
}

DEV_OMAP *
create_device(TTYINIT_OMAP *dip, unsigned unit, unsigned maxim_xcvr_kick) {
	DEV_OMAP 			*dev;
	unsigned			i;
	uintptr_t			port;
	unsigned char		msr;
	unsigned char		tlr = 0, tcr = 0;
#ifdef PWR_MAN
    clk_enable_t        clk_cfg = clk_enable_none;
#endif

	// Get a device entry and the input/output buffers for it.
	if ((dev = malloc(sizeof(*dev))) == NULL)
	{
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Allocation of device entry failed (%d)", errno);
		return (dev);
	}	
	memset(dev, 0, sizeof(*dev));

	// Get buffers.
	dev->tty.ibuf.head = dev->tty.ibuf.tail = dev->tty.ibuf.buff = malloc(dev->tty.ibuf.size = dip->tty.isize);
	if (dev->tty.ibuf.buff == NULL)
	{
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Allocation of input buffer failed (%d)", errno);
		free(dev);
		return (NULL);
	}
						   
	dev->tty.obuf.head = dev->tty.obuf.tail = dev->tty.obuf.buff = malloc(dev->tty.obuf.size = dip->tty.osize);
	if (dev->tty.obuf.buff == NULL)
	{
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Allocation of output buffer failed (%d)", errno);
		free(dev->tty.ibuf.buff);
		free(dev);
		return (NULL);
	}

	dev->tty.cbuf.head = dev->tty.cbuf.tail = dev->tty.cbuf.buff = malloc(dev->tty.cbuf.size = dip->tty.csize);
	if (dev->tty.cbuf.buff == NULL)
	{
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Allocation of canonical buffer failed (%d)", errno);
		free(dev->tty.ibuf.buff);
		free(dev->tty.obuf.buff);
		free(dev);
		return (NULL);
	}

	if (dip->tty.highwater)
		dev->tty.highwater = dip->tty.highwater;
	else
		dev->tty.highwater = dev->tty.ibuf.size - (FIFO_SIZE * 2);

	strcpy(dev->tty.name, dip->tty.name);

	dev->tty.baud = dip->tty.baud;
	dev->tty.fifo = dip->tty.fifo;
	dev->tty.verbose = dip->tty.verbose;

	port = mmap_device_io(OMAP_UART_SIZE, dip->tty.port);
	for (i = 0; i < OMAP_UART_SIZE; i += 4)
		dev->port[i] = port + i;

	dev->intr = dip->tty.intr;
	dev->clk = dip->tty.clk;
	dev->div = dip->tty.div;

	dev->tty.flags = EDIT_INSERT | LOSES_TX_INTR;
	dev->tty.c_cflag = dip->tty.c_cflag;
	dev->tty.c_iflag = dip->tty.c_iflag;
	dev->tty.c_lflag = dip->tty.c_lflag;
	dev->tty.c_oflag = dip->tty.c_oflag;
	dev->tty.lflags = dip->tty.lflags;
	if (dip->tty.logging_path[0] != NULL)
		dev->tty.logging_path = strdup(dip->tty.logging_path);

#ifdef PWR_MAN
	dev->physbase = dip->tty.port;
	
	if (omap_clock_toggle_init(dev) != EOK)
	{
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Fail to initialize clocks for PM!");
		free(dev->tty.ibuf.buff);
		free(dev->tty.obuf.buff);
		free(dev);
		return (NULL);
	}
#ifdef WINBT
    clk_cfg = clk_enable_smart_wkup;
    
	if (omap_force_rts_init(dev) != EOK)
	{
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Fail to initialize force_rts for PM!");
		free(dev->tty.ibuf.buff);
		free(dev->tty.obuf.buff);
		free(dev);
		return (NULL);
	}
#endif

	omap_clock_enable(dev, clk_cfg);

#ifdef WINBT	
	bt_ctrl_init();
#endif	
#endif

	/* Set auto_rts mode */
	dev->auto_rts_enable = dip->auto_rts_enable;
	
	// Do not enable MSR interrupt                                                                                                        
	dev->no_msr_int  = dip->no_msr_int;   

	// Initialize termios cc codes to an ANSI terminal.
	ttc(TTC_INIT_CC, &dev->tty, 0);

	// Initialize the device's name.
	// Assume that the basename is set in device name.  This will attach
	// to the path assigned by the unit number/minor number combination
	unit = SET_NAME_NUMBER(unit) | NUMBER_DEV_FROM_USER;
	ttc(TTC_INIT_TTYNAME, &dev->tty, unit);

	// see if we have a maxim rs-232 transceiver that needs to be
	// kicked after it goes to sleep
	dev->kick_maxim = maxim_xcvr_kick;

	// Only setup IRQ handler for non-pcmcia devices.
	// Pcmcia devices will have this done later when card is inserted.
	if (dip->tty.port != 0 && dip->tty.intr != _NTO_INTR_SPARE) {
#ifdef OMAP5910
		/*
		 * Don't change default mode set in the very distant past. Even though
		 * MODE_SELECT should be DISABLE before changing DLH, DLL.
		 */
                // enable the UART
                write_omap(dev->port[OMAP_UART_MDR1], OMAP_MDR1_MODE_16X);
#else
		/*
		 * TRM states: Before initializing or modifying clock parameter controls
		 * (DLH, DLL), MODE_SELECT must be set to 0x7 (DISABLE). Failure to observe
		 * this rule can result in unpredictable module behavior.
		 */
		write_omap(dev->port[OMAP_UART_MDR1], OMAP_MDR1_MODE_DISABLE);
#endif
		/* Work around for silicon errata i202 states: */
		/* Need a delay = 5 L4 clock cycles + 5 UART functional clock cycle (@48MHz = ~0.2uS) */
		nanospin_ns(200);
		/* Clear FIFOs */
		set_port(dev->port[OMAP_UART_FCR], OMAP_FCR_RXCLR | OMAP_FCR_TXCLR, OMAP_FCR_RXCLR | OMAP_FCR_TXCLR);
		/* Wait for FIFO to empty: when empty, RX_FIFO_E bit is 0 and TX_FIFO_E bit is 1 */
		i = 5;
		while ((read_omap(dev->port[OMAP_UART_LSR]) & (OMAP_UART_LSR_THRE | OMAP_UART_LSR_DR)) != OMAP_UART_LSR_THRE)
		{
		   nanospin_ns(200);
		   if (--i == 0)
		   {
		      break;  /* No need to do anything drastic if FIFO is still not empty */
		   }
		}
		// enable access to divisor registers
		write_omap(dev->port[OMAP_UART_LCR], OMAP_LCR_DLAB);
		write_omap(dev->port[OMAP_UART_DLL], 0);
		write_omap(dev->port[OMAP_UART_DLH], 0);

		/* Switch to config mode B to get access to EFR Register */
		write_omap(dev->port[OMAP_UART_LCR], 0xBF);
		/* Enable access to TLR register */
		set_port(dev->port[OMAP_UART_EFR], OMAP_EFR_ENHANCED, OMAP_EFR_ENHANCED);
		/* Switch to operational mode to get acces to MCR register */
		write_omap(dev->port[OMAP_UART_LCR], 0x00);
		/* set MCR bit 6 to enable access to TCR and TLR registers */
	    set_port(dev->port[OMAP_UART_MCR], OMAP_MCR_TCRTLR, OMAP_MCR_TCRTLR);
		write_omap(dev->port[OMAP_UART_FCR], OMAP_FCR_ENABLE|OMAP_FCR_RXCLR|OMAP_FCR_TXCLR);

		tcr = 0x0e; 	/* set auto-rts assert at 56 bytes, restore at 0 bytes */
    	if (dev->tty.fifo) 
		{
        	/* Set RX fifo trigger level */
	        switch (dev->tty.fifo >> 4) {
    	        case FIFO_TRIG_8:
				default:   
					tlr = 0x20; 
					break;
        	    case FIFO_TRIG_16:  tlr = 0x40; break;
	            case FIFO_TRIG_32:  tlr = 0x80; break;
    	        case FIFO_TRIG_56:  tlr = 0xe0; break;
        	    case FIFO_TRIG_60:  
					tlr = 0xf0; 
					tcr = 0x0f; /* Ensure auto-rts trigger is not less the RX trigger */
					break;
	        }

    	    /* Set TX fifo trigger level */
        	switch (dev->tty.fifo & 0x0f) {
	            case FIFO_TRIG_8:   
				default:
					tlr |= 0x02;
					break;
    	        case FIFO_TRIG_16:  tlr |= 0x04; break;
        	    case FIFO_TRIG_32:  tlr |= 0x08; break;
            	case FIFO_TRIG_56:  tlr |= 0x0e; break;
	            case FIFO_TRIG_60:  tlr |= 0x0f; break;
    	    }
	    }

	    write_omap(dev->port[OMAP_UART_TCR], tcr);
	    write_omap(dev->port[OMAP_UART_TLR], tlr);
#ifdef PWR_MAN
	    write_omap(dev->port[OMAP_UART_SCR], OMAP_SCR_WAKEUPEN);
#else
		write_omap(dev->port[OMAP_UART_SCR], 0x00);
#endif
		/* Switch back to Config mode B to gain access to EFR again */
		write_omap(dev->port[OMAP_UART_LCR], 0xBF);
		/* remove access to TLR register */
		set_port(dev->port[OMAP_UART_EFR], OMAP_EFR_ENHANCED, 0);
		/* Switch to operational mode to get acces to MCR register */
		write_omap(dev->port[OMAP_UART_LCR], 0x00);
		/* clr MCR bit 6 to remove access to TCR and TLR registers */
	    set_port(dev->port[OMAP_UART_MCR], OMAP_MCR_TCRTLR, 0);

		ser_stty(dev);
		ser_attach_intr(dev);
	}

	/* Set modem control lines to a ready state */
	if (dev->auto_rts_enable)
	   	set_port(dev->port[OMAP_UART_MCR], OMAP_MCR_DTR, OMAP_MCR_DTR);
	else
	   	set_port(dev->port[OMAP_UART_MCR], OMAP_MCR_DTR|OMAP_MCR_RTS, OMAP_MCR_DTR|OMAP_MCR_RTS);

	if(dip->loopback){   
		set_port(dev->port[OMAP_UART_MCR], OMAP_MCR_LOOPBACK, OMAP_MCR_LOOPBACK);
	} else {
		set_port(dev->port[OMAP_UART_MCR], OMAP_MCR_LOOPBACK, 0);
	}

	// get current MSR stat
	msr = read_omap(dev->port[OMAP_UART_MSR]);

	if(msr & OMAP_MSR_DDCD)
		tti(&dev->tty, (msr & OMAP_MSR_DCD) ? TTI_CARRIER : TTI_HANGUP);
					
	if((msr & OMAP_MSR_DCTS)  &&  (dev->tty.c_cflag & OHFLOW))
		tti(&dev->tty, (msr & OMAP_MSR_CTS) ? TTI_OHW_CONT : TTI_OHW_STOP);

	// Attach the resource manager
	ttc(TTC_INIT_ATTACH, &dev->tty, 0);

#ifdef PWR_MAN
	dev->tty.io_devctlext = extra_devctl;
#ifdef WINBT	
	omap_force_rts(dev, 1);
#endif	
	omap_clock_disable(dev);
#endif

	return (dev);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap/init.c $ $Rev: 714766 $")
#endif
