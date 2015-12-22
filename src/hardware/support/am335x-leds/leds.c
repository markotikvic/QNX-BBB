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

/*
 * LED USR 0 = GPIO1_21
 * LED USR 1 = GPIO1_22
 * LED USR 2 = GPIO1_23
 * LED USR 3 = GPIO1_24
 */
#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <hw/inout.h>

#include <arm/am335x.h>

/*
 * define THREAD_POOL_PARAM_T such that we can avoid a compiler
 * warning when we use the dispatch_*() functions below
 */
#define THREAD_POOL_PARAM_T dispatch_context_t

#include <sys/iofunc.h>
#include <sys/dispatch.h>

// LEDx is USRx (x=0/1/2/3) in the beaglebone schematics. AM335X multiplexed pins are:
// USR0: GPMC_A5/GMII2_TXD0 /RGMII2_TD0 /RMII2_TXD0/GPMC_A21/PR1_MII1_RXD3/EQEP1B_IN   /GPIO1_21
// USR1: GPMC_A6/GMII2_TXCLK/RGMII2_TCLK/MMC2_DAT4 /GPMC_A22/PR1_MII1_RXD2/EQEP1_INDEX /GPIO1_22
// USR2: GPMC_A7/GMII2_RXCLK/RGMII2_RCLK/MMC2_DAT5 /GPMC_A23/PR1_MII1_RXD1/EQEP1_STROBE/GPIO1_23
// USR3: GPMC_A8/GMII2_RXD3 /RGMII2_RD3 /MMC2_DAT6 /GPMC_A24/PR1_MII1_RXD0/MCASP0_ACLKX/GPIO1_24
// We access the leds via GPIO1.
#define LED0		(1<<21)
#define LED1		(1<<22)
#define LED2		(1<<23)
#define LED3		(1<<24)
#define LEDS(n)		((n&0xf)<<21)

static resmgr_connect_funcs_t    connect_funcs;
static resmgr_io_funcs_t         io_funcs;
static iofunc_attr_t             attr;

int io_read  (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb);
int io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);

static char *buffer = "Leds driver\n";

int main(int argc, char **argv)
{
    /* declare variables we'll be using */
    thread_pool_attr_t   pool_attr;
    resmgr_attr_t        resmgr_attr;
    dispatch_t           *dpp;
    thread_pool_t        *tpp;
    int                  id;

    /* We need to set up a mechanism so that clients can send messages to the resource manager.
     * This is done via the dispatch_create() function which creates and returns the dispatch
     * structure. This structure contains the channel ID. Note that the channel ID isn't actually
     * created until you attach something, as in resmgr_attach(), message_attach(), and
     * pulse_attach().
     * Note: The dispatch structure (of type dispatch_t) is opaque; you can't access its contents
     * directly. Use message_connect() to create a connection using this hidden channel ID.
     */
    if ((dpp = dispatch_create()) == NULL)
    {
        fprintf(stderr,
                "%s: Unable to allocate dispatch handle.\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    /* The resource manager attribute structure is used to configure:
     * - how many IOV structures are available for server replies (nparts_max)
     * - the minimum receive buffer size (msg_max_size)
     */
    memset(&resmgr_attr, 0, sizeof resmgr_attr);
    resmgr_attr.nparts_max   = 1;
    resmgr_attr.msg_max_size = 2048;

    /* Here we supply two tables that specify which function to call when a particular message
     * arrives: the connect functions table and the I/O functions table.
     * Instead of filling in these tables manually, we call iofunc_func_init() to place the
     * iofunc_*_default() handler functions into the appropriate spots.
     */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                     _RESMGR_IO_NFUNCS, &io_funcs);
    io_funcs.read  = io_read;
    io_funcs.write = io_write;

    /* The attribute structure contains information about our particular device associated with
     * the name /dev/leds. It contains at least the following information: permissions and type
     * of device owner and group ID. Effectively, this is a per-name data structure. Later on,
     * we'll see how you could extend the structure to include your own per-device information.
     */
    iofunc_attr_init(&attr, S_IFNAM | 0666, 0, 0);
    attr.nbytes = strlen (buffer)+1;

    /* Before a resource manager can receive messages from other programs, it needs to inform
     * the other programs (via the process manager) that it's the one responsible for a particular
     * pathname prefix. This is done via pathname registration. When registered, other processes
     * can find and connect to this process using the registered name.
     */
    id = resmgr_attach(
            dpp,            /* dispatch handle        */
            &resmgr_attr,   /* resource manager attrs */
            "/dev/leds",    /* device name            */
            _FTYPE_ANY,     /* open type              */
            0,              /* flags                  */
            &connect_funcs, /* connect routines       */
            &io_funcs,      /* I/O routines           */
            &attr);         /* handle                 */
    if(id == -1) {
        fprintf(stderr, "%s: Unable to attach name.\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* initialize thread pool attributes */
    memset(&pool_attr, 0, sizeof pool_attr);
    pool_attr.handle = dpp;
    pool_attr.context_alloc = dispatch_context_alloc;
    pool_attr.block_func = dispatch_block;
    pool_attr.unblock_func = dispatch_unblock;
    pool_attr.handler_func = dispatch_handler;
    pool_attr.context_free = dispatch_context_free;
    pool_attr.lo_water = 2;
    pool_attr.hi_water = 4;
    pool_attr.increment = 1;
    pool_attr.maximum = 50;

    /* allocate a thread pool handle */
    if((tpp = thread_pool_create(&pool_attr,
                                 POOL_FLAG_EXIT_SELF)) == NULL) {
        fprintf(stderr, "%s: Unable to initialize thread pool.\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    /* start the threads, will not return */
    thread_pool_start(tpp);

    return 0;
}

int io_read (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
    int         nleft;
    int         nbytes;
    int         nparts;
    int         status;

    if ((status = iofunc_read_verify (ctp, msg, ocb, NULL)) != EOK)
        return (status);

    if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
        return (ENOSYS);

    /*
     *  On all reads (first and subsequent), calculate
     *  how many bytes we can return to the client,
     *  based upon the number of bytes available (nleft)
     *  and the client's buffer size
     */

    nleft = ocb->attr->nbytes - ocb->offset;
    nbytes = min (msg->i.nbytes, nleft);

    if (nbytes > 0) {
        /* set up the return data IOV */
        SETIOV (ctp->iov, buffer + ocb->offset, nbytes);

        /* set up the number of bytes (returned by client's read()) */
        _IO_SET_READ_NBYTES (ctp, nbytes);

        /*
         * advance the offset by the number of bytes
         * returned to the client.
         */

        ocb->offset += nbytes;

        nparts = 1;
    } else {
        /*
         * they've asked for zero bytes or they've already previously
         * read everything
         */

        _IO_SET_READ_NBYTES (ctp, 0);

        nparts = 0;
    }

    /* mark the access time as invalid (we just accessed it) */

    if (msg->i.nbytes > 0)
        ocb->attr->flags |= IOFUNC_ATTR_ATIME;

    return (_RESMGR_NPARTS (nparts));
}

int doleds(unsigned char ch)
{
	uintptr_t	gpio_base;
	uint32_t	val;

    // Map device registers
    gpio_base = mmap_device_io(AM335X_GPIO_SIZE, AM335X_GPIO1_BASE);
    if(gpio_base == MAP_DEVICE_FAILED)
    {
        perror("Can't map device I/O");
        return 0;
    }

    // Read GPIO output enable register
    //  0 The corresponding GPIO port is configured as an output.
    //  1 The corresponding GPIO port is configured as an input.
    val  = in32(gpio_base + GPIO_OE);
    val &= ~(LED0|LED1|LED2|LED3);
    out32(gpio_base + GPIO_OE, val);

    // Write GPIO data output register
    val  = in32(gpio_base + GPIO_DATAOUT);
    val &= ~(LED0|LED1|LED2|LED3);
    val |= (LEDS(ch));
    out32(gpio_base + GPIO_DATAOUT, val);

    munmap_device_io(gpio_base, AM335X_GPIO_SIZE);

    return 1;
}

int io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
    int 		    status;
    char		    *buf;
    unsigned int	leds=0;

    if ((status = iofunc_write_verify(ctp, msg, ocb, NULL)) != EOK)
        return (status);

    if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
        return(ENOSYS);

    /* set up the number of bytes (returned by client's write()) */

    _IO_SET_WRITE_NBYTES (ctp, msg->i.nbytes);

    buf = (char *) malloc(msg->i.nbytes + 1);
    if (buf == NULL)
        return(ENOMEM);

    /*
     *  Reread the data from the sender's message buffer.
     *  We're not assuming that all of the data fit into the
     *  resource manager library's receive buffer.
     */

    resmgr_msgread(ctp, buf, msg->i.nbytes, sizeof(msg->i));
    buf [msg->i.nbytes] = '\0'; /* just in case the text is not NULL terminated */

    leds = strtol(buf, 0,0) & 0xf;
    
    doleds(leds);

    free(buf);

    if (msg->i.nbytes > 0)
        ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

    return (_RESMGR_NPARTS (0));
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/support/am335x-leds/leds.c $ $Rev: 719661 $")
#endif
