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

#include "startup.h"
#include <stdlib.h>
#include <arm/omap.h>
#include <arm/am335x.h>
#include "board.h"
#include "beaglebone.h"
#include <hw/omap_i2c.h>

int bdid_read(omap_i2c_dev_t *dev, int address, BDIDENT *bdident, int showerr)
{
    uint8_t        val[32]= { 0, 0 };
    int            rr;

    memset(val, 0, 32);

    /* Read reply from Board ID EEPROM */
    rr = omap_i2c_read(dev, 0, 2, &val[0], 32); 

    if(!rr)
    {
        memcpy(&bdident->header , &val[0]                         , AM335X_BDID_HEADER_LEN);
        memcpy(&bdident->bdname , &val[AM335X_BDID_BDNAME_OFFSET] , AM335X_BDID_BDNAME_LEN);
        bdident->bdname[AM335X_BDID_BDNAME_LEN] = 0;
        return (0);
    }
    return -1;
}

void iddump(BDIDENT *bdident)
{
    kprintf("header:  %x\n", bdident->header);
    kprintf("name:    %s\n", bdident->bdname);
}

void dump(unsigned int address, BDIDENT *bdident)
{
    kprintf("\n__________Board ID__________\n");

    if (bdident->header == 0xee3355aa)
    {
        iddump(bdident);
    }    
    else
    {        
        kprintf("Invalid header [%x], expected %x\n", bdident->header, 0xee3355aa);
    }    
    kprintf("____________________________\n", address);
}
    
void get_boardid_i2c(BEAGLEBONE_ID *boneid)
{    
    int             r;    
    int             i;    
    BDIDENT         bd;    

    if (boneid==NULL)    
    {        
        kprintf("Parameter NULL: can't return board id information\n");        
        return;    
    }    
    
    memset(boneid, 0, sizeof(BEAGLEBONE_ID));    
    boneid->basebd_type     = bb_not_detected;    
    
    for (i=0; i<AM335X_I2C0_MAXCAPES; i++)    
    {        
        boneid->cape_type[i] = ct_not_detected;    
    }    
    
   	omap_i2c_dev_t beaglebone_dev = {
		.base = AM335X_I2C0_BASE,
		.clock = AM335X_I2C0_EEPROM_SPEED,
		.slave = AM335X_I2C0_BBID
	};

    omap_i2c_init(&beaglebone_dev);    
    
    r = bdid_read(&beaglebone_dev, 0x50 , &bd, 1); 
    
    if (!r)    
    {        
        dump(0x50, &bd);        
        if (strcmp(bd.bdname, "A335BNLT")==0)
        {
            boneid->basebd_type = bb_BeagleBoneBlack;
        }
        else if (strcmp(bd.bdname, "A335BONE")==0)
        {
            boneid->basebd_type = bb_BeagleBone;
        }
        else
        {                     
            boneid->basebd_type = bb_unknown;        
            kprintf("boardid=%s\n",bd.bdname);        
        }    
    }    
        /* this section is to detect Cape */
#if 0    
    for (i=0; i<AM335X_I2C0_MAXCAPES; i++)    
    {        
        r = bdid_read(AM335X_I2C0_CAPE0+i, &bd, 0);
        if (!r)        
        {            
            dump(0x51, &bd);
            boneid->cape_type[i] = ct_unknown;    
        }    
    }
#endif

    if (boneid->basebd_type == bb_BeagleBone)
    {
        kprintf("    BeagleBone (original) detected    \n\n");    
    }
    else if (boneid->basebd_type == bb_BeagleBoneBlack)
    {
        kprintf("    BeagleBone Black detected    \n\n");
    }
    else
        kprintf("    Not a BeagleBone??    \n\n");        

    /* this section is to detect Cape */
#if 0    
    for (i=0; i<AM335X_I2C0_MAXCAPES; i++)    
    {        
        if (boneid->cape_type[i] == ct_unknown)    
            kprintf("Cape #%d detected\n", i);    
    }
#endif

}

unsigned long detect_frequency_using_power()
{    
    uint8_t        val[40]= { 0, 0 };    
    int            rr;
    unsigned long retval = AM335X_MPUFREQ_500M; // Default value assumes USB Powered    

   	omap_i2c_dev_t beaglebone_dev = {
		.base = AM335X_I2C0_BASE,
		.clock = AM335X_I2C0_PMIC_SPEED,
		.slave = AM335X_I2C0_PMIC
	};

    omap_i2c_init(&beaglebone_dev);    
    
    /* Write request to Power Chip */    
    rr   = omap_i2c_read(&beaglebone_dev, 0 , 2, &val[0], 32);        

    if(!rr)
    {
        /* By experiment, this bit is set when BeagleBoard powered by 5V,
        * and clear when powered by USB 
        */
        if (val[9] & 0x8)     
        {        
            retval = AM335X_MPUFREQ_720M;  
        }      
        return retval;
    }
    return -1;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/ti-am335x/beaglebone/detectid-bone.c $ $Rev: 722974 $")
#endif
