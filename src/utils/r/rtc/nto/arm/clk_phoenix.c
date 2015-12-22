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
#include <string.h>
#include <fcntl.h>
#include <hw/i2c.h>


/* 
 * PHOENIX RTC Serial Access Timekeeper
 */
#define PHOENIX_SEC           0         /* 0x00-0x59 */
#define PHOENIX_MIN           1         /* 0x00-0x59 */
#define PHOENIX_HOUR          2         /* 0x00-0x23, MSB is AM(0)/PM(1) */
#define PHOENIX_DATE          3         /* 0x01-0x31 */
#define PHOENIX_MONTH         4         /* 0x01-0x12 */
#define PHOENIX_YEAR          5         /* 0x00-0x99/years */
#define PHOENIX_WEEK          6         /* 0x0-0x6 */
#define PHOENIX_CTRL_REG      16
#define PHOENIX_STATUS_REG    17

// CTRL reg bit
#define PHOENIX_HOUR_12_N24   0x08      /* hour select mode bit */
#define PHOENIX_STOP_RTC      0x01      /* set to 1 to enable RTC */

// AM/PM flag in hour reg
#define PHOENIX_HOUR_AM_PM    0x80      /* AM/PM bit */

#define PHOENIX_SEC_MASK      0x7F     
#define PHOENIX_MIN_MASK      0x7F    
#define PHOENIX_HOUR_MASK     0x3F   
#define PHOENIX_DATE_MASK     0x3F  
#define PHOENIX_MONTH_MASK    0x1F 
#define PHOENIX_YEAR_MASK     0xFF 
#define PHOENIX_WEEK_MASK     0x07 

#define PHOENIX_I2C_ADDRESS   (0x48)
#define PHOENIX_I2C_DEVNAME   "/dev/i2c0"

static int fd = -1;
static int slave = PHOENIX_I2C_ADDRESS;
static unsigned char ctrl_value = 0;

static int
phoenix_i2c_read(unsigned char reg, unsigned char val[], unsigned char num)
{
    iov_t           siov[2], riov[2];
    i2c_sendrecv_t  hdr;

    hdr.slave.addr  = slave;
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
phoenix_i2c_write(unsigned char reg, unsigned char val[], unsigned char num)
{
    iov_t           siov[3];
    i2c_send_t      hdr;

    hdr.slave.addr  = slave;
    hdr.slave.fmt   = I2C_ADDRFMT_7BIT;
    hdr.len         = num + 1;
    hdr.stop        = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg, sizeof(reg));
    SETIOV(&siov[2], val, num);

    return devctlv(fd, DCMD_I2C_SEND, 3, 0, siov, NULL, NULL);
}

int
RTCFUNC(init,phoenix)(struct chip_loc *chip, char *argv[])
{
#define MAX_NAME  20
	char 		i2c_dev[MAX_NAME] = PHOENIX_I2C_DEVNAME;
	int 		opt;
	char        *value;
	char		*options;
	static char  *opts[] = {
		"i2c",		/* I2C device name (default: /dev/i2c0) */
		"slave",	/* slave address (default: 0x48) */
		NULL };

	/* 
	 * One example of using argv[] is on Jacinto6, the slave device number is different from the default one,
	 * So we need to specify the i2c device info in command line:
	 *    rtc phoenix "slave=0x58"
	 *    rtc phoenix "i2c=/dev/i2c0,slave=0x58"
	 */
	 if (argv && argv[0]) {
		options = argv[0];
		while (options && *options != '\0') {
			if ((opt = getsubopt( &options, opts, &value)) == -1) {
		 		break;
		 	}
		 
			switch (opt) {
				case 0:
					strncpy(i2c_dev, value, MAX_NAME);
					break;
				case 1:
					slave = strtol(value, 0, 0);
					break;
				default:
					break;
			}
		}
	 }

    fd = open((const char *)i2c_dev, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Unable to open I2C device\n");
        return -1;
    }

    if (EOK != phoenix_i2c_read(PHOENIX_CTRL_REG, &ctrl_value, 1)) {
        fprintf(stderr, "RTC: phoenix_i2c_read() failed\n");
        return (-1);
    }
    if ((ctrl_value & PHOENIX_STOP_RTC) == 0)
    {
      fprintf(stderr, "RTC is stopped.  Enabling RTC.\n");
      ctrl_value |= PHOENIX_STOP_RTC;
      if (EOK !=  phoenix_i2c_write(PHOENIX_CTRL_REG, &ctrl_value, 1)) {
          fprintf(stderr, "RTC: phoenix_i2c_write() failed\n");
          return (-1);
      }
    }

    return 0;
}

int
RTCFUNC(get,phoenix)(struct tm *tm, int cent_reg)
{
    unsigned char   date[7];

    if (EOK != phoenix_i2c_read(PHOENIX_SEC, date, 7)) {
        fprintf(stderr, "RTC: phoenix_i2c_read() failed\n");
        return (-1);
    }

    tm->tm_sec  = BCD2BIN(date[PHOENIX_SEC]   & PHOENIX_SEC_MASK);
    tm->tm_min  = BCD2BIN(date[PHOENIX_MIN]   & PHOENIX_MIN_MASK);
    tm->tm_hour = BCD2BIN(date[PHOENIX_HOUR]  & PHOENIX_HOUR_MASK);
    if ((ctrl_value & PHOENIX_HOUR_12_N24) &&
        (date[PHOENIX_HOUR] & PHOENIX_HOUR_AM_PM))
    {
      tm->tm_hour += 12;
    }
    tm->tm_mday = BCD2BIN(date[PHOENIX_DATE]  & PHOENIX_DATE_MASK);
    tm->tm_mon  = BCD2BIN(date[PHOENIX_MONTH] & PHOENIX_MONTH_MASK) - 1;
    tm->tm_year = BCD2BIN(date[PHOENIX_YEAR] & PHOENIX_YEAR_MASK) + 2000 - 1900;
    tm->tm_wday = BCD2BIN(date[PHOENIX_WEEK] & PHOENIX_WEEK_MASK);

    return(0);
}

int
RTCFUNC(set,phoenix)(struct tm *tm, int cent_reg)
{
    unsigned char   date[7];

    date[PHOENIX_SEC]   = BIN2BCD(tm->tm_sec);
    date[PHOENIX_MIN]   = BIN2BCD(tm->tm_min);
    if ((ctrl_value & PHOENIX_HOUR_12_N24) &&
        (tm->tm_hour==12))
    {
      date[PHOENIX_HOUR]  = BIN2BCD(tm->tm_hour);
      date[PHOENIX_HOUR] |= PHOENIX_HOUR_AM_PM;
    }
    else if ((ctrl_value & PHOENIX_HOUR_12_N24) &&
             (tm->tm_hour>12))
    {
      date[PHOENIX_HOUR]  = BIN2BCD(tm->tm_hour-12);
      date[PHOENIX_HOUR] |= PHOENIX_HOUR_AM_PM;
    }
    else
      date[PHOENIX_HOUR]  = BIN2BCD(tm->tm_hour);
    
    date[PHOENIX_DATE]  = BIN2BCD(tm->tm_mday);
    date[PHOENIX_MONTH] = BIN2BCD(tm->tm_mon + 1);
    if (tm->tm_year<100)
    {
      fprintf(stderr, "RTC: year must be >=2000\n");
      return (-1);
    }
    date[PHOENIX_YEAR] = BIN2BCD((tm->tm_year) % 100);
    date[PHOENIX_WEEK] = BIN2BCD(tm->tm_wday);

    if (EOK !=  phoenix_i2c_write(PHOENIX_SEC, date, 7)) {
        fprintf(stderr, "RTC: phoenix_i2c_write() failed\n");
        return (-1);
    }

    return(0);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/utils/r/rtc/nto/arm/clk_phoenix.c $ $Rev: 725908 $")
#endif
