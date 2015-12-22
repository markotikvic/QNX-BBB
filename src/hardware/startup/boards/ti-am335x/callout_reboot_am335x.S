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
 * TI AM335x system reboot
 */

#include "callout.ah"
#include <arm/am335x.h>

/*
 * -----------------------------------------------------------------------
 * Routine to patch callout code
 *
 * On entry:
 *  r0 - physical address of syspage
 *  r1 - virtual  address of syspage
 *  r2 - offset from start of syspage to start of the callout routine
 *  r3 - offset from start of syspage to read/write data used by callout
 * -----------------------------------------------------------------------
 */

patch_reboot:
    stmdb    sp!,{r4,lr}
    add      r4, r0, r2            /* address of callout routine */

    /*
     * Map reset control registers
     */
    mov      r0, #CM_PRCM_SIZE    /* size of reset control registers */
    ldr      r1, Lpaddr
    bl       callout_io_map

    /*
     * Patch the callout routine
     */
    CALLOUT_PATCH    r4, r0, r1, r2, ip
    ldmia    sp!,{r4,pc}

Lpaddr:    .word    PRCM_BASE

#define ARM_CPSR_T        (1 << 5)
#define ARM_CPSR_F        (1 << 6)
#define ARM_CPSR_I        (1 << 7)

CALLOUT_START(reboot_am335x, 0, patch_reboot)
    /*
     * Get the base address of Clock, Reset, and Power Mode Control registers (patched)
     */
    mov        ip,     #0x000000FF
    orr        ip, ip, #0x0000FF00
    orr        ip, ip, #0x00FF0000
    orr        ip, ip, #0xFF000000

    mov        r1, #RST_GLOBAL_COLD_SW        /* Asserts a global cold software reset. */
    str        r1, [ip, #PRM_DEVICE]

0:    b        0b

CALLOUT_END(reboot_am335x)

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/ti-am335x/callout_reboot_am335x.S $ $Rev: 735852 $";
.previous
#endif
#endif 
