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


#include "rtc.h"
#include <time.h>
#include <fcntl.h>
#include <hw/i2c.h>


/* 
 * MAX8925 RTC Serial Access Timekeeper
 */
#define MAX8925_SEC           0         /* 00-59 */
#define MAX8925_MIN           1         /* 00-59 */
#define MAX8925_HOUR          2         /* 00-11/00-23 */
#define MAX8925_DAY           3         /* 01-07 */
#define MAX8925_DATE          4         /* 01-31 */
#define MAX8925_MONTH         5         /* 01-12 */
#define MAX8925_YEAR1         6         /* 00-99/years */
#define MAX8925_YEAR2         7         /* 00-99/centuries */

#define MAX8925_HOUR_12_N24   0x80      /* hour select mode bit */
#define MAX8925_HOUR_AM_PM    0x20      /* AM/PM bit */

#define MAX8925_SEC_MASK      0x7F      /* 00-59 */
#define MAX8925_MIN_MASK      0x7F      /* 00-59 */
#define MAX8925_HOUR_MASK     0xBF      /* 00-11/00-23 */
#define MAX8925_DAY_MASK      0x07      /* 01-07 */
#define MAX8925_DATE_MASK     0x3F      /* 01-31 */
#define MAX8925_MONTH_MASK    0x1F      /* 01-12 */

#define MAX8925_I2C_ADDRESS   (0xD0 >> 1)
#define MAX8925_I2C_DEVNAME    "/dev/i2c0"


static int fd = -1;


static int
max8925_i2c_read(unsigned char reg, unsigned char val[], unsigned char num)
{
    iov_t           siov[2], riov[2];
    i2c_sendrecv_t  hdr;

    hdr.slave.addr  = MAX8925_I2C_ADDRESS;
    hdr.slave.fmt   = I2C_ADDRFMT_7BIT;
    hdr.send_len    = 1;
    hdr.recv_len    = num;
    hdr.stop        = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg, sizeof(reg));

    SETIOV(&riov[0], &hdr, sizeof(hdr));
    SETIOV(&riov[1], val, num);

    return devctlv(fd, DCMD_I2C_SENDRECV, 2, 2, siov, riov, NULL);
}

static int
max8925_i2c_write(unsigned char reg, unsigned char val[], unsigned char num)
{
    iov_t           siov[3];
    i2c_send_t      hdr;

    hdr.slave.addr  = MAX8925_I2C_ADDRESS;
    hdr.slave.fmt   = I2C_ADDRFMT_7BIT;
    hdr.len         = num + 1;
    hdr.stop        = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg, sizeof(reg));
    SETIOV(&siov[2], val, num);

    return devctlv(fd, DCMD_I2C_SEND, 3, 0, siov, NULL, NULL);
}

int
RTCFUNC(init,max8925)(struct chip_loc *chip, char *argv[])
{
    fd = open((argv && argv[0] && argv[0][0])? argv[0]: MAX8925_I2C_DEVNAME, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Unable to open I2C device\n");
        return -1;
    }

    return 0;
}

int
RTCFUNC(get,max8925)(struct tm *tm, int cent_reg)
{
    unsigned char   date[8];

    if (EOK != max8925_i2c_read(MAX8925_SEC, date, 8)) {
        fprintf(stderr, "RTC: max8925_i2c_read() failed\n");
        return (-1);
    }

    tm->tm_sec  = BCD2BIN(date[MAX8925_SEC]   & MAX8925_SEC_MASK);
    tm->tm_min  = BCD2BIN(date[MAX8925_MIN]   & MAX8925_MIN_MASK);
    tm->tm_hour = BCD2BIN(date[MAX8925_HOUR]  & MAX8925_HOUR_MASK);
    tm->tm_wday = BCD2BIN(date[MAX8925_DAY]   & MAX8925_DAY_MASK) - 1;
    tm->tm_mday = BCD2BIN(date[MAX8925_DATE]  & MAX8925_DATE_MASK);
    tm->tm_mon  = BCD2BIN(date[MAX8925_MONTH] & MAX8925_MONTH_MASK) - 1;
    tm->tm_year = BCD2BIN(date[MAX8925_YEAR1]) + 100 * BCD2BIN(date[MAX8925_YEAR2]) - 1900;

    return(0);
}

int
RTCFUNC(set,max8925)(struct tm *tm, int cent_reg)
{
    unsigned char   date[8];

    date[MAX8925_SEC]   = BIN2BCD(tm->tm_sec);
    date[MAX8925_MIN]   = BIN2BCD(tm->tm_min);
    date[MAX8925_HOUR]  = BIN2BCD(tm->tm_hour);
    date[MAX8925_DAY]   = BIN2BCD(tm->tm_wday + 1);
    date[MAX8925_DATE]  = BIN2BCD(tm->tm_mday);
    date[MAX8925_MONTH] = BIN2BCD(tm->tm_mon + 1);
    date[MAX8925_YEAR1] = BIN2BCD((tm->tm_year + 1900) % 100);
    date[MAX8925_YEAR2] = BIN2BCD((tm->tm_year + 1900) / 100);

    if (EOK !=  max8925_i2c_write(MAX8925_SEC, date, 8)) {
        fprintf(stderr, "RTC: max8925_i2c_write() failed\n");
        return (-1);
    }

    return(0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/utils/r/rtc/clk_max8925.c $ $Rev: 680331 $")
#endif
