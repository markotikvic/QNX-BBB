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


#include <sys/slog.h>
#include <sys/slogcodes.h>
#include "proto.h"

#define NANOSECONDS 1000000000

#ifdef VARIANT_omap4
    #include "clock_toggle.h"
    #define OPTION_STR          "a:c:ei:p:P:s:vh:l:f"
#else
    #define OPTION_STR          "a:i:p:P:s:vh:l:f"
#endif

int
omap_options(omap_dev_t *dev, int argc, char *argv[])
{
    int     c;
    int     prev_optind;
    int     done = 0;
    int	    temp;

    /* defaults */
    dev->intr = OMAP3530_I2C_1_IRQ;
    dev->intr_priority = OMAP_I2C_DEF_INTR_PRIORITY;
    dev->iid = -1;
    dev->physbase = OMAP35XX_I2C_BASE1;
    dev->reglen = OMAP_I2C_SIZE;
    dev->own_addr = OMAP_I2C_ADDRESS;
    dev->slave_addr = TWL4030_AUDIO_SLAVE_ADDRESS; /* audio codec */
    dev->options = 0;
	dev->re_start = 0;
    dev->high_adjust_fast = 0;
    dev->high_adjust_slow = 0;
    dev->low_adjust_fast = 0;
    dev->low_adjust_slow = 0;
#ifdef VARIANT_omap4
	dev->no_powmgm = 0;
	dev->soc_version = OMAP4_SOC;
#endif
    while (!done) {
        prev_optind = optind;
        c = getopt(argc, argv, OPTION_STR);
        switch (c) {

		case 'a':
            dev->own_addr = strtoul(optarg, &optarg, NULL);
            break;
#ifdef VARIANT_omap4
        case 'c':
            dev->soc_version = strtol(optarg, &optarg, NULL);
            
            if (dev->soc_version > MAX_SOC_SUPPORTED)
            {
                fprintf(stderr, "in %s: unsupported SoC version %d.\n", __FUNCTION__, dev->soc_version);
                return -1;
            }
            break;
#endif
        case 'i':
            dev->intr = strtol(optarg, &optarg, NULL);
            break;
#ifdef VARIANT_omap4
		case 'e':
			// No power management support
			dev->no_powmgm = 1;
            break;
#endif
        case 'p':
            dev->physbase = strtoul(optarg, &optarg, NULL);
            break;

        case 'P':
            temp = strtol(optarg, &optarg, 0);
            if (temp < sched_get_priority_min(SCHED_RR) || temp > sched_get_priority_max(SCHED_RR)) {
                fprintf(stderr, "priority %d is out of range\n", temp);
                slogf(_SLOGC_I2C, _SLOG_ERROR, "i2c driver: priority %d is out of range", temp);
                exit(1);
            }

            dev->intr_priority = temp;
            break;

        case 's':
            dev->slave_addr = strtoul(optarg, &optarg, NULL);
            break;

        case 'v':
            dev->options |= OMAP_OPT_VERBOSE;
            break;

        case 'h':
            temp = strtol(optarg, &optarg, NULL);

            dev->high_adjust_fast = (temp / (int)(NANOSECONDS / OMAP_I2C_ICLK_9600K));
            dev->high_adjust_slow = (temp / (int)(NANOSECONDS / OMAP_I2C_ICLK));

            break;

        case 'l':
            temp = strtol(optarg, &optarg, NULL);

            dev->low_adjust_fast = (temp / (int)(NANOSECONDS / OMAP_I2C_ICLK_9600K));
            dev->low_adjust_slow = (temp / (int)(NANOSECONDS / OMAP_I2C_ICLK));

            break;

        case 'f':
            dev->options |= OMAP_OPT_ROVR_XUDF_OK;
            break;

        case '?':
            if (optopt == '-') {
                ++optind;
                break;
            }
            return -1;

        case -1:
            if (prev_optind < optind) /* -- */
                return -1;

            if (argv[optind] == NULL) {
                done = 1;
                break;
            } 
            if (*argv[optind] != '-') {
                ++optind;
                break;
            }
            return -1;

        case ':':
        default:
            return -1;
        }
    }

    return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/i2c/omap35xx/options.c $ $Rev: 698026 $")
#endif
