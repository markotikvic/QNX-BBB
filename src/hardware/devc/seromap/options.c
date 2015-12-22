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
#ifdef __USAGE
%C - Serial driver for TI OMAP 

%C [options] [port[^shift][,irq][,k]] &
Options:
 -a           Use auto-rts when hardware flow control is enabled
 -b number    Define initial baud rate (default 115200)
 -c clk[/div] Set the input clock rate and divisor
 -C number    Size of canonical input buffer (default 256)
 -e           Set options to "edit" mode
 -E           Set options to "raw" mode (default)
 -I number    Size of raw input buffer (default 2048)
 -f           Enable hardware flow control (default)
 -F           Disable hardware flow control
 -M           Disable MSR interrupt
 -O number    Size of output buffer (default 2048)
 -s           Enable software flow control
 -S           Disable software flow control (default)
 -t number    Set receive FIFO trigger level (default 16)
 -T number    Set transmit FIFO trigger level (default 8)
 -u unit      Set serial unit number (default 1)
 -l (0|1)     Enable Loopback mode (1=on, 0=off)
 -v           Set verbosity level
 -U uid:gid   Set the user id and group id
 -o log=<tx|rx|all>[@/logging_dir]
              Enable data logging to specified directory
              (default directory = /dev/shmem)
                                       
  k - place this after the irq value to indicate that a Maxim RS-232
      transceiver is used on this port, which requires a null character
      to be sent to it to  wake it up after going into "Autoshutdown Plus" mode

#endif
*/
#include "externs.h"

#ifdef OMAP5910
#define DEFAULT_CLK 12000000
#else
#define DEFAULT_CLK 48000000
#endif

#define DEFAULT_DIV 16
#define DEFAULT_FIFO_TX     8
#define DEFAULT_FIFO_RX     16

unsigned
options(int argc, char *argv[]) {
	int opt, numports = 0;
	char *cp;
	unsigned unit;
	unsigned fifo_tx = DEFAULT_FIFO_TX, fifo_rx = DEFAULT_FIFO_RX;
	unsigned fifo_tx2 = 0, fifo_rx2 = 0;
	DEV_OMAP	*dev;
	user_parm = NULL;
	static TTYINIT_OMAP devinit = {
		{
			0, 0, 0,			// Port, port shift, interrupt
			115200,				// Baud
			2048, 2048, 256,		// isize, osize, csize
			0, 0, 0, 0, 0, 			// cflag, iflag, lflag, oflag, fifo
			DEFAULT_CLK, 			// clk
			DEFAULT_DIV,			// div
			"/dev/ser",			// Name
         		NULL,           	 	// power handle
         		0,      	          	// power flags
         		0, 	             		// verbosity
		 		0,				// Rx highwater level
				"",				// Logging directory
				0				// Logging enable flags
		},
		0,					// loopback disable
		0,					// auto_rts_enable
		0					// modem status interrupt disable
	};

	unsigned maxim_xcvr_kick = 0;

	// Initialize the devinit to raw mode
	ttc(TTC_INIT_RAW, &devinit, 0);

	unit = 1;
	while (optind < argc) {
		// Process dash options.
		while ((opt = getopt(argc, argv, IO_CHAR_SERIAL_OPTIONS "ac:t:T:U:u:l:M")) != -1) {	
			switch (ttc(TTC_SET_OPTION, &devinit, opt)) {
			case 'a':
				devinit.auto_rts_enable = 1;
				break;
			case 'c':
				devinit.tty.clk = strtoul(optarg, &optarg, 0);
				if((cp = strchr(optarg, '/')))
					devinit.tty.div = strtoul(cp + 1, NULL, 0);
				break;
			case 't':
				fifo_rx = strtoul(optarg, NULL, 0);
				if (fifo_rx < 4 || fifo_rx > 60)
				{
					fprintf(stderr,"Illegal rx fifo trigger. \n");
					fprintf(stderr,"Trigger number must be between 4-60. \n");
					fifo_rx = DEFAULT_FIFO_RX;
				}
				break;
			case 'T':
				fifo_tx = strtoul(optarg, NULL, 0);
				if (fifo_tx < 4 || fifo_tx > 60) 
				{
					fprintf(stderr,"Illegal tx fifo trigger. \n");
					fprintf(stderr,"Tx trigger must be between 4-60\n");
					fifo_tx = DEFAULT_FIFO_TX;
				}
                break;
			case 'U':
				user_parm = strdup(optarg);
				break;
			case 'u':
				unit = strtoul(optarg, NULL, 0);
				break;
			case 'l':
				 devinit.loopback = strtoul(optarg, NULL, 0);
				 break;
			case 'M':
				devinit.no_msr_int = 1;
				break;
			}
		}

        /*
		 * devinit.fifo is used to store both the tx and rx fifo sizes,
		 * but it is defined as an unsigned char, so only fifo sizes up
		 * to 16 bytes are supported without special encoding. So, we'll
		 * encode the fifo sizes here.
		 *
         */
        switch (fifo_rx) {
			case 8:
				fifo_rx2 = FIFO_TRIG_8;
				break;
			case 16:
				fifo_rx2 = FIFO_TRIG_16;
				break;
			case 32:
				fifo_rx2 = FIFO_TRIG_32;
				break;
			case 56:
				fifo_rx2 = FIFO_TRIG_56;
				break;
			case 60:
				fifo_rx2 = FIFO_TRIG_60;
				break;
			default:
				fifo_rx2 = 0;
				break;
		}

        switch (fifo_tx) {
			case 8:
				fifo_tx2 = FIFO_TRIG_8;
				break;
			case 16:
				fifo_tx2 = FIFO_TRIG_16;
				break;
			case 32:
				fifo_tx2 = FIFO_TRIG_32;
				break;
			case 56:
				fifo_tx2 = FIFO_TRIG_56;
				break;
			default:
				fifo_tx2 = 0;
				break;
		}

        // Store the rx and tx fifo's
		
        devinit.tty.fifo = (fifo_rx2<<4) | fifo_tx2;

		// Process ports and interrupts.
		while (optind < argc  &&  *(optarg = argv[optind]) != '-') {
			devinit.tty.port = strtoull(optarg, &optarg, 16);
			if (*optarg == '^')
				devinit.tty.port_shift = strtoul(optarg + 1, &optarg, 0);

			if (*optarg == ',')
				devinit.tty.intr = strtoul(optarg + 1, &optarg, 0);

			if (*optarg == ',') {
				if ((*(optarg + 1) == 'k') || (*(optarg + 1) =='K'))
					maxim_xcvr_kick = 1;
			}

			if ((dev = create_device(&devinit, unit++, maxim_xcvr_kick)) == NULL)
			{
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Initialization of /dev/ser%d (port 0x%llx) failed", unit - 1, devinit.tty.port);
		        fprintf(stderr, "io-char: Initialization of port 0x%llx failed\n", devinit.tty.port);
			}
			else
				++numports;
			maxim_xcvr_kick = 0;
			++optind;

		    if (dev != NULL && dev->tty.verbose)
			{
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Port ...................... %s (0x%x)", dev->tty.name, dev->port[0]);
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "IRQ ....................... 0x%x", dev->intr);
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Rx fifo trigger ........... %d", fifo_rx);
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Tx fifo trigger ........... %d", fifo_tx);
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Rx flow control highwater . %d", dev->tty.highwater);
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Input buffer size ......... %d", dev->tty.ibuf.size);
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Output buffer size ........ %d", dev->tty.obuf.size);
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "Canonical buffer size ..... %d\n", dev->tty.cbuf.size);
			}
		}
	}

#if 0	// There is no default device
	if (numports == 0) {
		void	*link = NULL;
		for ( ;; ) {
			link = query_default_device(&devinit, link);
			if (link == NULL) break;
			create_device(&devinit, unit++, maxim_xcvr_kick);
			++numports;
		}
	}
#endif
	return (numports);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap/options.c $ $Rev: 714766 $")
#endif
