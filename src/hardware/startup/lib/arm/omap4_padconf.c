/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
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


#include <startup.h>
/* Indicate we want to get access to the configuration table from the header file */
#define OMAP4430_PADCONF_CFG_TABLE
#include "arm/omap4_padconf.h"


static int
omap4_padconf_init
(
  unsigned char        my_version,
  uint32_t             base,
  uint32_t             first_offset,
  uint32_t             last_offset,
  padconf_func_map_t * func_tbl,
  // Pointer to a NULL terminated configuration table!
  pin_cfg_t *          cfg_tbl,
  unsigned             table_size
)
{
    uint32_t        reg_out;
    uint32_t        reg_in;
    unsigned        idx;
    
    /* Validate config table */
    
    /* Process each element in the table */
    while(table_size-- > 0) {
        
        unsigned    reg_idx;

        ASSERT((cfg_tbl->reg>=first_offset) && (cfg_tbl->reg<=last_offset));

        // check to see if entry is for a specific board revision and skip
        // if necessary.
        if (cfg_tbl->options & PADCONF_VERSION_SPECIFIC_FLAG)
        {
          cfg_tbl->options &= ~PADCONF_VERSION_SPECIFIC_FLAG;
          if ((my_version < cfg_tbl->version_start) || 
              (my_version > cfg_tbl->version_end))
          {
            cfg_tbl++;
            continue;
          }
        }

        /* Extract register index from its address and pad. */
        /* There are two pads per register, with pad0 first and pad1 second */
        reg_idx = (cfg_tbl->reg  - first_offset) >> 1;
        if (cfg_tbl->pad == PADCONF_PAD1)
          reg_idx++;
        
        ASSERT(func_tbl[reg_idx].id == cfg_tbl->reg);
        ASSERT(func_tbl[reg_idx].pad == cfg_tbl->pad);
                
        /* Load PADCONF option and mask function as we'll validate it first.*/
        reg_out = cfg_tbl->options & ~(PADCONF_MUXMODE_MASK);
        
        /* Walk through all supported option for this register and determine if
           the specify function is supported!.
         */
            
        for (idx=0; idx<8; idx++) {
            
            if (cfg_tbl->func == func_tbl[reg_idx].func[idx]) {
                
                /* We've found a valid function for this pin */

                break;
            }
        }
        
        /* Validate we found a valid function associated for this port */
        if (idx >= 8) {
            crash("%s: PADCONF offset 0x%x, idx %d specified unsuported function %d!\n"
                 , __func__, cfg_tbl->reg, reg_idx, cfg_tbl->func);
        }
        
        reg_out |= PADCONF_MUXMODE_FUNC(idx);
        reg_in = in32(base + cfg_tbl->reg);

        if (cfg_tbl->pad == PADCONF_PAD1)
        {
          reg_out <<= 16;
          reg_in &= 0x0000ffff;
        }
        else
          reg_in &= 0xffff0000;

        out32(base + cfg_tbl->reg, reg_in | reg_out);
        cfg_tbl++;
    }
    
    return 0;
}

int
omap4_padconf_core_init
(
   unsigned char my_version,
   pin_cfg_t*    cfg_tbl,            // Pointer to a NULL terminated configuration table!
   unsigned      table_size
)
{
  return omap4_padconf_init(my_version,
                               OMAP44XX_CONTROL_PADCONF_BASE,
                               PADCONF_CORE_FIRST_OFFSET,
                               PADCONF_CORE_LAST_OFFSET,
                               padconf_func_table,
                               cfg_tbl, table_size);
}

int
omap4_padconf_wkup_init
(
   unsigned char my_version,
   pin_cfg_t*    cfg_tbl,            // Pointer to a NULL terminated configuration table!
   unsigned      table_size
)
{
  return omap4_padconf_init(my_version,
                               OMAP44XX_CONTROL_PADCONF_WKUP_BASE,
                               PADCONF_WKUP_FIRST_OFFSET,
                               PADCONF_WKUP_LAST_OFFSET,
                               padconf_wkup_func_table,
                               cfg_tbl, table_size);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/omap4_padconf.c $ $Rev: 736706 $")
#endif
