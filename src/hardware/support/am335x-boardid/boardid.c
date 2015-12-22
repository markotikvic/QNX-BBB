/*
 * $QNXLicenseC:
 * Copyright 2009, QNX Software Systems.
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

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/mman.h>
#include <hw/i2c.h>
#include <hw/inout.h>
#include "boardid.h"
/*
 * define THREAD_POOL_PARAM_T such that we can avoid a compiler
 * warning when we use the dispatch_*() functions below
 */
#define THREAD_POOL_PARAM_T dispatch_context_t

#include <sys/iofunc.h>
#include <sys/dispatch.h>


#define STRLEN		64

typedef struct
{
	int		fd;
	int		i2c_address;
	char	devname[STRLEN];

} AM335X_BOARDID, *AM335X_PBOARDID;

static int bdid_read(AM335X_BOARDID *bdid, BDIDENT *bdident, int showerr)
{
    struct {
        i2c_send_t      hdr;
        uint8_t	        reg[2];
    } msgreg;

    struct {
        i2c_recv_t      hdr;
        uint8_t 		val[256];
    } msgval;
    int rbytes;
    int r;

    msgreg.hdr.slave.addr = bdid->i2c_address;
    msgreg.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
    msgreg.hdr.len        = 2;
    msgreg.hdr.stop       = 0;
    msgreg.reg[0]         = 0;
    msgreg.reg[1]         = 0;

    r = devctl(bdid->fd, DCMD_I2C_SEND, &msgreg, sizeof(msgreg), NULL);
    if (r)
    {
    	if (showerr) printf("I2C write failed (DCMD_I2C_SEND), error %d\n", r);
    	return (-1);
    }

    msgval.hdr.slave.addr = bdid->i2c_address;
    msgval.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
    msgval.hdr.len        = 256;
    msgval.hdr.stop       = 1;

    r = devctl(bdid->fd, DCMD_I2C_RECV, &msgval, sizeof(msgval), &rbytes);
    if (r)
    {
    	if (showerr) printf("I2C read failed (DCMD_I2C_RECV), error %d\n", r);
        return (-1);
    }

    memcpy(&bdident->header , &msgval.val[0]                         , AM335X_BDID_HEADER_LEN);
	memcpy(&bdident->bdname , &msgval.val[AM335X_BDID_BDNAME_OFFSET] , AM335X_BDID_BDNAME_LEN);
	bdident->bdname[AM335X_BDID_BDNAME_LEN] = 0;
	memcpy(&bdident->version, &msgval.val[AM335X_BDID_VERSION_OFFSET], AM335X_BDID_VERSION_LEN);
	bdident->version[AM335X_BDID_VERSION_LEN] = 0;
	memcpy(&bdident->serial , &msgval.val[AM335X_BDID_SERIAL_OFFSET] , AM335X_BDID_SERIAL_LEN);
	bdident->serial[AM335X_BDID_SERIAL_LEN] = 0;
	memcpy(&bdident->config , &msgval.val[AM335X_BDID_CONFIG_OFFSET] , AM335X_BDID_CONFIG_LEN);
	bdident->config[AM335X_BDID_SKU_LEN] = 0;
	memcpy(&bdident->macaddr, &msgval.val[AM335X_BDID_MAC1_OFFSET]   , AM335X_BDID_MAC_LEN*AM335X_MACS);

    return (rbytes);
}

int read_boardid(int address, BDIDENT *bdident, int showerr)
{
	AM335X_BOARDID	bdid;
	int				r;

	sprintf(bdid.devname, BOARDID_I2C_DEVICE);
	bdid.fd = open(bdid.devname, O_RDWR);
	if (bdid.fd == -1)
	{
		printf("Unable to open %s (errno=%d)\n", bdid.devname, errno);
		return -1;
	}

	r=devctl(bdid.fd, DCMD_I2C_LOCK, NULL, 0, NULL);
	if (r)
	{
		printf("Unable to lock %s (errno=%d)\n", bdid.devname, errno);
		close(bdid.fd);
		return -1;
	}

	bdid.i2c_address = address;
	r = bdid_read(&bdid, bdident, showerr);
	if (r<0)
	{
		if (showerr) printf("Unable to read %s (errno=%d)\n", bdid.devname, errno);
		close(bdid.fd);
		return -1;
	}

	r=devctl(bdid.fd, DCMD_I2C_UNLOCK, NULL, 0, NULL);
	if (r)
	{
		printf("Unable to unlock %s (errno=%d)\n", bdid.devname, errno);
		close(bdid.fd);
		return -1;
	}

	close(bdid.fd);

	return 0;
}

void dump(BDIDENT *bdident)
{
	int   sz=0;
	char *descr = "";

	if ((bdident->bdname[0] == 'A') &&
		(bdident->bdname[1] == '3') &&
		(bdident->bdname[2] == '3') &&
		(bdident->bdname[3] == '5') &&
		(bdident->bdname[4] == '1')    )
	{
		if (bdident->bdname[5] == '5') sz = 15;
		else
		if (bdident->bdname[5] == '3') sz = 13;
	}
	if (strcmp(bdident->config, "SKU#00")==0) descr = "base board for low cost evm";
	else
	if (strcmp(bdident->config, "SKU#01")==0) descr = "base board for gen purpose evm";
	else
	if (strcmp(bdident->config, "SKU#02")==0) descr = "base board for industrial motor control evm";
	else
	if (strcmp(bdident->config, "SKU#03")==0) descr = "base board for ip phone evm";

    printf("header:  %x\n", bdident->header);
    printf("name:    %s\n", bdident->bdname);
    printf("version: %s\n", bdident->version);
    printf("serial:  %s\n", bdident->serial);
    printf("config:  %s"  , bdident->config);
    if (sz)
    {
    	printf(" %s\n", descr);
    	printf("mac1:    %x.%x.%x.%x.%x.%x\n", bdident->macaddr[0][0], bdident->macaddr[0][1], bdident->macaddr[0][2], bdident->macaddr[0][3], bdident->macaddr[0][4], bdident->macaddr[0][5]);
    	printf("mac2:    %x.%x.%x.%x.%x.%x\n", bdident->macaddr[1][0], bdident->macaddr[1][1], bdident->macaddr[1][2], bdident->macaddr[1][3], bdident->macaddr[1][4], bdident->macaddr[1][5]);
    	if (sz==15)
    	{
    		printf("mac3:    %x.%x.%x.%x.%x.%x\n", bdident->macaddr[2][0], bdident->macaddr[2][1], bdident->macaddr[2][2], bdident->macaddr[2][3], bdident->macaddr[2][4], bdident->macaddr[2][5]);
    	}
    }
    else
    {
    	printf("\n");
    }
}

static resmgr_connect_funcs_t    connect_funcs;
static resmgr_io_funcs_t         io_funcs;
static iofunc_attr_t             attr;

int io_read  (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb);

#define BOARDID_STRLEN		256

static char boardid_buffer[BOARDID_STRLEN] = "unknown";

int create_driver(int argc, char *argv[])
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

    /* The attribute structure contains information about our particular device associated with
     * the name /dev/leds. It contains at least the following information: permissions and type
     * of device owner and group ID. Effectively, this is a per-name data structure. Later on,
     * we'll see how you could extend the structure to include your own per-device information.
     */
    iofunc_attr_init(&attr, S_IFNAM | 0666, 0, 0);
    attr.nbytes = strlen (boardid_buffer)+1;

    /* Before a resource manager can receive messages from other programs, it needs to inform
     * the other programs (via the process manager) that it's the one responsible for a particular
     * pathname prefix. This is done via pathname registration. When registered, other processes
     * can find and connect to this process using the registered name.
     */
    id = resmgr_attach(
            dpp,            /* dispatch handle        */
            &resmgr_attr,   /* resource manager attrs */
            "/dev/bdid",    /* device name            */
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
        SETIOV (ctp->iov, boardid_buffer + ocb->offset, nbytes);

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

int main(int argc, char *argv[])
{
	int			r, i;
	uint32_t	l;
	BDIDENT		bd;


	printf("Board ID\n");

	r = read_boardid(0x50, &bd, 1);
	if (!r) dump(&bd);

	/* Copy version to ID string */
	l = min(BOARDID_STRLEN, AM335X_BDID_VERSION_LEN);
	strncpy(boardid_buffer, bd.version, l);
	boardid_buffer[l] = 0;
	strcat (boardid_buffer, ",");
	/* Copy name to ID string */
	l = min(BOARDID_STRLEN-AM335X_BDID_VERSION_LEN-1, AM335X_BDID_BDNAME_LEN);
	strncat(boardid_buffer, bd.bdname , l);
	l = min(BOARDID_STRLEN, AM335X_BDID_BDNAME_LEN+1+AM335X_BDID_VERSION_LEN);
	boardid_buffer[l] = 0;
	strcat (boardid_buffer, ",");
	/* Copy serial to ID string */
	l = min(BOARDID_STRLEN-AM335X_BDID_VERSION_LEN-AM335X_BDID_BDNAME_LEN-2, AM335X_BDID_SERIAL_LEN);
	strncat(boardid_buffer, bd.serial , l);
	l = min(BOARDID_STRLEN, AM335X_BDID_SERIAL_LEN+1+AM335X_BDID_BDNAME_LEN+1+AM335X_BDID_VERSION_LEN);
	boardid_buffer[l] = 0;
	strcat (boardid_buffer, "\n");

	r = read_boardid(0x51, &bd, 0);
	if (!r) dump(&bd);

	r = read_boardid(0x52, &bd, 0);
	if (!r) dump(&bd);

	for (i=0; i<AM335X_I2C0_MAXCAPES; i++)
	{
		// Read optional cape ID. Don't print errors, likely caused by cape absence.
		r = read_boardid(AM335X_I2C0_CAPE0+i, &bd, 0 /* don't print errors */);
		if (!r) dump(&bd);
	}

	return create_driver(argc, argv);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/support/am335x-boardid/boardid.c $ $Rev: 725446 $")
#endif
