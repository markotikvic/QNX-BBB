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

/*
 * clk_rocket.c: Contains rtc driver for rocket
 *
 * Created on June 6, 2011
 *
 */

#include <time.h>
#include <fcntl.h>
#include <hw/i2c.h>
#include "rtc.h"

/* Rocket Sub-Device ID */
#define DEVID_ROCKET             0

#define ROCKET_I2C_ADDRESS       0x4A
#define ROCKET_I2C_DEVNAME       "/dev/i2c0"

/** ROCKET_REG_UTCCTRL
 *  Bit  0   (rw) UTC Disable
 *  Bits 1-7      Reserved
 */
#define ROCKET_REG_UTCCTRL       0x10
#define ROCKET_UTC_UTCDIS        (1<<0)

/** ROCKET_REG_UTCCOUNT0-7
 *  Bits 0-7 (r): 64 bit value indicating UTC time in ms
 */
#define ROCKET_REG_UTCCOUNT0    0x11
#define ROCKET_REG_UTCCOUNT1    0x12
#define ROCKET_REG_UTCCOUNT2    0x13
#define ROCKET_REG_UTCCOUNT3    0x14
#define ROCKET_REG_UTCCOUNT4    0x15
#define ROCKET_REG_UTCCOUNT5    0x16
#define ROCKET_REG_UTCCOUNT6    0x17
#define ROCKET_REG_UTCCOUNT7    0x18


/* global variables */
static int fd = -1;
static unsigned char ctrl_value = 0;
static unsigned char devid_rocket = DEVID_ROCKET;

static int  reg_utc_count[8] = {
        ROCKET_REG_UTCCOUNT0,
        ROCKET_REG_UTCCOUNT1,
        ROCKET_REG_UTCCOUNT2,
        ROCKET_REG_UTCCOUNT3,
        ROCKET_REG_UTCCOUNT4,
        ROCKET_REG_UTCCOUNT5,
        ROCKET_REG_UTCCOUNT6,
        ROCKET_REG_UTCCOUNT7};

/**
 * rocket_i2c_read:
 * -Description: read pmic utc register
 * -Arguments:
 *  -reg: register to be read
 *  -val: data received
 *  -num: length of data received
 * -Returns:
 *  -EOK on success, status or errno on failure
 */
static int
rocket_i2c_read(unsigned char reg, unsigned char val[], unsigned num)
{
    iov_t           siov[3], riov[2];
    i2c_sendrecv_t  hdr;
    int status;

    hdr.slave.addr  = ROCKET_I2C_ADDRESS;
    hdr.slave.fmt   = I2C_ADDRFMT_7BIT;
    hdr.send_len    = 2;
    hdr.recv_len    = num;
    hdr.stop        = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &devid_rocket, sizeof(devid_rocket));
    SETIOV(&siov[2], &reg, sizeof(reg));

#if 0
    printf("Read reg:");
    printf(" %xh", devid_rocket);
    printf(" %xh", reg);
    printf("\n");
#endif

    SETIOV(&riov[0], &hdr, sizeof(hdr));
    SETIOV(&riov[1], val, num);

    status = devctlv(fd, DCMD_I2C_SENDRECV, 3, 2, siov, riov, NULL);
    if(status != EOK)
        return status;

#if 0
    int i;
    printf("Data recvd:");
    for (i = 0; i < num; ++i) {
        printf(" %xh", val[i]);
    }
    printf("\n");
#endif

    return EOK;
}

/**
 * rocket_i2c_write:
 * -Description: write pmic utc register
 * -Arguments:
 *  -reg: register to be written
 *  -val: data to be written
 *  -num: length of data to be written
 * -Returns:
 *  -EOK on success, status or errno on failure
 */
static int
rocket_i2c_write(unsigned char reg, unsigned char val[], unsigned num)
{
    iov_t           siov[4];
    i2c_send_t      hdr;

    hdr.slave.addr  = ROCKET_I2C_ADDRESS;
    hdr.slave.fmt   = I2C_ADDRFMT_7BIT;
    hdr.len         = num + 2;
    hdr.stop        = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &devid_rocket, sizeof(devid_rocket));
    SETIOV(&siov[2], &reg, sizeof(reg));
    SETIOV(&siov[3], val, num);

#if 0
    int i;
    printf("Write reg:");
    printf(" %xh", devid_rocket);
    printf(" %xh", reg);
    for (i = 0; i < num; ++i) {
        printf(" %xh", val[i]);
    }
    printf("\n");
#endif

    return devctlv(fd, DCMD_I2C_SEND, 4, 0, siov, NULL, NULL);
}

/**
 * init_rocket:
 * -Description: initialize rocket RTC
 * -Arguments:
 *  -chip:
 *  -argv:
 * -Returns:
 *  -EOK on success, -1 on failure
 */
int
RTCFUNC(init,rocket)(struct chip_loc *chip, char *argv[])
{
    int i;
    int status;
    unsigned char   data=0x00;

    fd = open((argv && argv[0] && argv[0][0])? argv[0]: ROCKET_I2C_DEVNAME, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Unable to open I2C device\n");
        return -1;
    }

    /* read control register */
    if ( (status = rocket_i2c_read( ROCKET_REG_UTCCTRL, &ctrl_value, 1 ))) {
        fprintf(stderr, "%s %s read failed: %s\n", __FILE__, __FUNCTION__, strerror(status));
        return (-1);
    }

    /* check with Rocket to see if the UTC is enabled */
    if( ctrl_value & ROCKET_UTC_UTCDIS ) {
        fprintf(stderr, "UTC is disabled. Enabling UTC.\n");

        /* writing UTC counter registers which also enables the counters */
        for( i = 0; i < 8; i++ ) {
            if ( (status = rocket_i2c_write( reg_utc_count[i], &data, 1 )) ) {
                fprintf(stderr, "%s %s failed: %s\n", __FILE__, __FUNCTION__, strerror(status));
                return (-1);
            }
        }
    }

    return EOK;
}

/**
 * get_rocket:
 * -Description: get rocket RTC
 * -Arguments:
 *  -tm: time structure(contains broken-down time: offset from 1900/01/01/00:00:00)
 *  -cent_reg:
 * -Returns:
 *  -EOK on success, -1 on failure
 */
int
RTCFUNC(get,rocket)(struct tm *tm, int cent_reg)
{
    int             i;
    int             status;
    unsigned char   data;
    uint64_t        now_time_ms = 0x0;
    time_t          now_time_s;
    struct tm       *p_tm;

    /* read control register */
    if ( (status = rocket_i2c_read( ROCKET_REG_UTCCTRL, &ctrl_value, 1 )) ) {
        fprintf(stderr, "%s %s failed: %s\n", __FILE__, __FUNCTION__, strerror(status));
        return (-1);
    }

    /* check with Rocket to see if the UTC is disabled */
    if( ctrl_value & ROCKET_UTC_UTCDIS ) {
        fprintf(stderr, "%s %s failed: UTC is disabled\n", __FILE__, __FUNCTION__);
        return (-1);
    }

    /* get the counter value (calendar time in milliseconds) */
    for( i = 0; i < 8; i++ ) {
        if ( (status = rocket_i2c_read( reg_utc_count[i], &data, 1 )) ) {
            fprintf(stderr, "%s %s failed: %s\n", __FILE__, __FUNCTION__, strerror(status));
            return (-1);
        }
        now_time_ms |= (uint64_t)data << i*8 ;
    }

    /* convert calendar time to seconds */
    now_time_s = now_time_ms/1000;

    /* convert calendar time(seconds offset from 1970/01/01/00:00:00) to a broken-down time */
    p_tm = gmtime(&now_time_s);

    /* assign broken-down time to tm */
    tm->tm_year = p_tm->tm_year;
    tm->tm_mon  = p_tm->tm_mon;
    tm->tm_mday = p_tm->tm_mday;
    tm->tm_hour = p_tm->tm_hour;
    tm->tm_min  = p_tm->tm_min;
    tm->tm_sec  = p_tm->tm_sec;
    tm->tm_wday = p_tm->tm_wday;

    return EOK;
}

/**
 * set_rocket:
 * -Description: set rocket RTC
 * -Arguments:
 *  -tm: time structure(contains broken-down time: seconds offset from 1900/01/01/00:00:00)
 *  -cent_reg:
 * -Returns:
 *  -EOK on success, -1 on failure
 */
int
RTCFUNC(set,rocket)(struct tm *tm, int cent_reg)
{
    int i;
    int status;
    unsigned char   data;
    time_t          new_time_s;
    uint64_t        new_time_ms;

    /* convert broken-down time to current calendar time in seconds */
    new_time_s = mktime(tm);

    /* convert current calendar time in milliseconds */
    new_time_ms = (uint64_t)new_time_s * 1000;

    /* disable the RTC module */
    ctrl_value = ROCKET_UTC_UTCDIS;
    if ( (status = rocket_i2c_write( ROCKET_REG_UTCCTRL, &ctrl_value, 1 )) ) {
        fprintf(stderr, "%s %s failed: %s\n", __FILE__, __FUNCTION__, strerror(status));
        return (-1);
    }

    /* write the new RTC value, which also enables the counters */
    for( i = 0; i < 8; i++ ) {
        data = ( new_time_ms >> i*8 ) & 0xff;
        if ( (status = rocket_i2c_write( reg_utc_count[i], &data, 1 )) ) {
            fprintf(stderr, "%s %s failed: %s\n", __FILE__, __FUNCTION__, strerror(status));
            return (-1);
        }
    }

    return EOK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/utils/r/rtc/nto/arm/clk_rocket.c $ $Rev: 680331 $")
#endif
