/*
 * $QNXLicenseC:
 * Copyright 2007, 2008, QNX Software Systems.
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

#include "rtc.h"
#include <time.h>
#include <fcntl.h>
#include <hw/i2c.h>

/*
 * Epson RTC8564 Serial RTC
 */
#define RTC8564_CTRL1        0
#define RTC8564_CTRL2        1
#define RTC8564_SEC          2   /* 00-59 */
#define RTC8564_MIN          3   /* 00-59 */
#define RTC8564_HOUR         4   /* 0-1/00-23 */
#define RTC8564_MDAY         5   /* 01-31 */
#define RTC8564_WDAY         6   /* 01-07 */
#define RTC8564_MON          7   /* 01-12 */
#define RTC8564_YEAR         8   /* 00-99 */
#define RTC8564_MIN_ALARM    9
#define RTC8564_HOUR_ALARM   0xa
#define RTC8564_MDAY_ALARM   0xb
#define RTC8564_WDAY_ALARM   0xc
#define RTC8564_CLKOUT_FREQ  0xd
#define RTC8564_TIMER_CTRL   0xe
#define RTC8564_TIMER        0xf

#define RTC8564_I2C_ADDRESS   0x51
#define RTC8564_I2C_DEVNAME   "/dev/i2c1"
#define I2C_MUX_ADDR       0x74
#define I2C_RTC_CHANNEL     0x10

static int fd = -1;

static int
i2c_send_data(int file, _Uint8t addr, _Uint8t data)
{
    int error;

    struct {
        i2c_send_t hdr;
        _Uint8t reg_data;
    } wd_data;

    wd_data.hdr.slave.addr = addr;
    wd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    wd_data.hdr.len = 1;
    wd_data.hdr.stop = 1;
    wd_data.reg_data = data;

    if ((error = devctl(fd, DCMD_I2C_SEND, &wd_data, sizeof(wd_data), NULL))) {
        fprintf(stderr, "devctl failed\n");
        return -1;
    }

    return 0;
}

static int
rtc8564_i2c_read(unsigned char reg, unsigned char val[], unsigned char num)
{
    iov_t           siov[2], riov[2];
    i2c_sendrecv_t  hdr;

    hdr.slave.addr = RTC8564_I2C_ADDRESS;
    hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    hdr.send_len = 1;
    hdr.recv_len = num;
    hdr.stop = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg, sizeof(reg));

    SETIOV(&riov[0], &hdr, sizeof(hdr));
    SETIOV(&riov[1], val, num);

    return devctlv(fd, DCMD_I2C_SENDRECV, 2, 2, siov, riov, NULL);
}

static int
rtc8564_i2c_write(unsigned char reg, unsigned char val[], unsigned char num)
{
    iov_t           siov[3];
    i2c_send_t      hdr;

    hdr.slave.addr = RTC8564_I2C_ADDRESS;
    hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    hdr.len = num + 1;
    hdr.stop = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg, sizeof(reg));
    SETIOV(&siov[2], val, num);

    return devctlv(fd, DCMD_I2C_SEND, 3, 0, siov, NULL, NULL);
}

int
RTCFUNC(init,rtc8564)(struct chip_loc *chip, char *argv[])
{
    /* Open i2c1 device */
    fd = open((argv && argv[0] && argv[0][0])? argv[0]: RTC8564_I2C_DEVNAME, O_RDWR);

    if (fd < 0){
        fprintf(stderr, "Unable to open I2C device\n");
        return -1;
    } else {
        /* Select channel for the RTC */
        return i2c_send_data(fd, I2C_MUX_ADDR, I2C_RTC_CHANNEL);
    }
}

int
RTCFUNC(get,rtc8564)(struct tm *tm, int cent_reg)
{
    unsigned char date[7];

    rtc8564_i2c_read(RTC8564_SEC, date, 7);

    tm->tm_sec  = BCD2BIN(date[RTC8564_SEC - 2] & 0x7F);
    tm->tm_min  = BCD2BIN(date[RTC8564_MIN - 2] & 0x7F);
    tm->tm_hour = BCD2BIN(date[RTC8564_HOUR - 2] & 0x3F);
    tm->tm_mday = BCD2BIN(date[RTC8564_MDAY - 2] & 0x3F);
    tm->tm_wday = date[RTC8564_WDAY - 2] & 0x7;
    tm->tm_mon  = (date[RTC8564_MON - 2] & 0x1F) - 1;
    tm->tm_year = BCD2BIN(date[RTC8564_YEAR - 2]);

    if(tm->tm_year < 70)
        tm->tm_year += 100;

    return(0);
}

int
RTCFUNC(set,rtc8564)(struct tm *tm, int cent_reg)
{
    unsigned char   date[7];

    date[RTC8564_SEC - 2]  = BIN2BCD(tm->tm_sec);
    date[RTC8564_MIN - 2]  = BIN2BCD(tm->tm_min);
    date[RTC8564_HOUR - 2] = BIN2BCD(tm->tm_hour);
    date[RTC8564_MDAY - 2] = BIN2BCD(tm->tm_mday);
    date[RTC8564_MON - 2]  = BIN2BCD(tm->tm_mon + 1);
    date[RTC8564_YEAR - 2] = BIN2BCD(tm->tm_year % 100);
    date[RTC8564_WDAY - 2] = BIN2BCD(tm->tm_wday & 0x7);

    rtc8564_i2c_write(RTC8564_SEC, date, 7);

    return(0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/utils/r/rtc/nto/arm/clk_rtc8564.c $ $Rev: 759875 $")
#endif
