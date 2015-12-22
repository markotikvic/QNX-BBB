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

/*
 * Process data in a line status register
 */
static int
process_lsr(DEV_OMAP *dev, unsigned char lsr) {
	unsigned key = 0, eventflag = 0;

	// Return immediately if no errors.
	if((lsr & (OMAP_LSR_BI|OMAP_LSR_OE|OMAP_LSR_FE|OMAP_LSR_PE)) == 0)
		return(0);

	// Save the error as out-of-band data which can be retrieved via devctl().
	dev->tty.oband_data |= (lsr >> 1) & 0x0f;
	atomic_set(&dev->tty.flags, OBAND_DATA);
// Uncomment for post 1.0 since there was no time to test sufficiently
//	if(dev->tty.notify[2].cnt) {
//		dev->tty.notify[2].cnt = 0;	// Disarm
//		dev->tty.notify[2].event.sigev_value.sival_int |= _NOTIFY_COND_OBAND;
//		atomic_set(&dev->tty.flags, EVENT_NOTIFY_OBAND);
//		eventflag = 1;
//		}

	// Read whatever input data happens to be in the buffer to "eat" the
	// spurious data associated with break, parity error, etc.
	key = read_omap(dev->port[OMAP_UART_RHR]);

	if(lsr & OMAP_LSR_BI)
		key |= TTI_BREAK;
	else if(lsr & OMAP_LSR_OE)
		key |= TTI_OVERRUN;
	else if(lsr & OMAP_LSR_FE)
		key |= TTI_FRAME;
	else if(lsr & OMAP_LSR_PE)
		key |= TTI_PARITY;

	return(tti(&dev->tty, key) | eventflag);
	}

/*
 * Serial interrupt handler
 */
const struct sigevent *
ser_intr(void *area, int id) {
	int				status, cnt;
	unsigned char	msr, lsr;
	DEV_OMAP		*dev = area;
	struct sigevent *event = NULL;
	unsigned		iir;
	uintptr_t		*port = dev->port;

#ifdef PWR_MAN
	/* Our idle state can be changed by a devctl so we must use a spinlock */
	InterruptLock(&dev->idle_spinlock);
#endif

	while (1) {
		status = 0;

#ifdef PWR_MAN
        if (dev->idle) {

            omap_clock_enable_isr(dev);
#ifdef WINBT            
            omap_force_rts(dev, 0);

            // once we are in idle mode the only interrupt that can wake us up is from am CTS line change
            tti(&dev->tty, TTI_OHW_STOP);

            // start a spare timer for debouncing, if this timer actually
            // expires, then this was a CTS glitch, if the timer
            // is cleared by having data on the RX line, it will send up the
            // oband notification to wake up the host.
            dev->signal_oband_notification = 1;

            if( dev->tty.un.s.spare_tmr == 0 ){
                atomic_set (&dev->tty.eflags, EVENT_TIMER_QUEUE);
                dev->tty.un.s.spare_tmr = 4;

                // queue the event here because the switch statement can just exit
                // without actually queuing the event with setting the status flag
                if((dev->tty.flags & EVENT_QUEUED) == 0) {
                    event = &ttyctrl.event;
                    dev_lock(&ttyctrl);
                    ttyctrl.event_queue[ttyctrl.num_events++] = &dev->tty;
                    atomic_set(&dev->tty.flags, EVENT_QUEUED);
                    dev_unlock(&ttyctrl);
                    continue;
                }
            }
#endif // End of #ifdef WINBT
        }

		unsigned ssr = read_omap(port[OMAP_UART_SSR]);
		if (ssr & OMAP_SSR_WAKEUP_STS) {
			/* Clear the wake up interrupt */
			set_port(port[OMAP_UART_SCR], OMAP_SCR_WAKEUPEN, 0);
		}
#endif


		iir = read_omap(port[OMAP_UART_IIR]) & OMAP_II_MASK;

		switch(iir) {
			case OMAP_II_RX:		// Receive data
			case OMAP_II_RXTO:		// Receive data timeout
			case OMAP_II_LS:		// Line status change
				cnt = 0;
				lsr = read_omap(port[OMAP_UART_LSR]);
				do {
					if( lsr & (OMAP_LSR_BI|OMAP_LSR_OE|OMAP_LSR_FE|OMAP_LSR_PE) ) {
						// Error character
						status |= process_lsr(dev, lsr);
					}
					else {
						// Good character
						status |= tti(&dev->tty, (read_omap(port[OMAP_UART_RHR])) & 0xff);
						cnt++;
					}
					lsr = read_omap(port[OMAP_UART_LSR]);
				} while(lsr & OMAP_LSR_RXRDY && cnt < FIFO_SIZE);
#ifdef WINBT
				if( cnt && dev->signal_oband_notification ){

				    // received data after a CTS wake up
				    // notify the host that it's a valid CTS wakeup.
				    dev->signal_oband_notification = 0;
		            dev->tty.oband_data |= _OBAND_SER_MS;
		            atomic_set(&dev->tty.flags, OBAND_DATA);
		            atomic_set(&dev->tty.flags, EVENT_NOTIFY_OBAND);
		            status |= 1;
				}

				if (cnt && dev->tty.un.s.spare_tmr) {
				    // received data, clear spare timer
	                dev->tty.un.s.spare_tmr = 0;
	            }
#endif
				break;

			case OMAP_II_TX:		// Transmit buffer empty

				// disable thr interrupt
				set_port(dev->port[OMAP_UART_IER], OMAP_IER_THR, 0);

				dev->tty.un.s.tx_tmr = 0;
				/* Send event to io-char, tto() will be processed at thread time */
				atomic_set(&dev->tty.flags, EVENT_TTO);
				status |= 1;
				break;

			case OMAP_II_MS:		// Modem change
				msr = read_omap(port[OMAP_UART_MSR]);

				if(msr & OMAP_MSR_DDCD) {
					status |= tti(&dev->tty, (msr & OMAP_MSR_DCD) ? TTI_CARRIER : TTI_HANGUP);
				}

				if((msr & OMAP_MSR_DCTS)  &&  (dev->tty.c_cflag & OHFLOW)) {
					status |= tti(&dev->tty, (msr & OMAP_MSR_CTS) ? TTI_OHW_CONT : TTI_OHW_STOP);
				}

				/* OBAND notification of Modem status change */
				dev->tty.oband_data |= _OBAND_SER_MS;
				atomic_set(&dev->tty.flags, OBAND_DATA);
				atomic_set(&dev->tty.flags, EVENT_NOTIFY_OBAND);
				status |= 1;

				break;

			case OMAP_II_NOINTR:	// No interrupt
				if (read_omap(port[OMAP_UART_SSR]) & OMAP_SSR_WAKEUP_STS) {	// Wake up interrupt
					set_port(port[OMAP_UART_SCR], OMAP_SCR_WAKEUPEN, 0);		// clear wakeup interrupt
					set_port(port[OMAP_UART_SCR], OMAP_SCR_WAKEUPEN, OMAP_SCR_WAKEUPEN);	// re-enable wakeup interrupt
				}
			default:
				goto done;
		}

		if (status) {
			if((dev->tty.flags & EVENT_QUEUED) == 0) {
				event = &ttyctrl.event;
				dev_lock(&ttyctrl);
				ttyctrl.event_queue[ttyctrl.num_events++] = &dev->tty;
				atomic_set(&dev->tty.flags, EVENT_QUEUED);
				dev_unlock(&ttyctrl);
			}
		}
	}

done:
#ifdef PWR_MAN
	InterruptUnlock(&dev->idle_spinlock);
#endif

	return (event);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap/intr.c $ $Rev: 680332 $")
#endif
