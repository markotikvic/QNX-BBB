/*
 * $QNXLicenseC:
 * Copyright 2008, QNX Software Systems. 
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

/*
 * Generic ARMv7 CPU initialisation
 *
 */

#define ID_ISAR0_DIV_BIT     (24)
#define ID_ISAR0_DIV_MASK    (0xF  << ID_ISAR0_DIV_BIT)

void
armv_setup_v7(struct cpuinfo_entry *cpu, unsigned cpuid, unsigned cpunum)
{
	unsigned	tmp;

	/*
	 * Check for presence of VFP (cp10/11) and enable access if present
	 */
	tmp = arm_v6_cp15_cpacr_get();
	tmp |= ARM_V6_CPACR_VFP;
	arm_v6_cp15_cpacr_set(tmp);	

	__asm__ __volatile__("isb");

	/*
	 * Access bits will not be written if coprocessor is not present
	 */
	tmp = arm_v6_cp15_cpacr_get();
	if ((tmp & ARM_V6_CPACR_VFP) == ARM_V6_CPACR_VFP) {
		unsigned	mvfr0;
		unsigned	mvfr1;

		/*
		 * Indicate we have an FPU unit
		 */
		cpu->flags |= CPU_FLAG_FPU;

		/*
		 * Check if we have Neon or VFP only
		 */
		__asm__ __volatile__("mrc	p10, 7, %0, c7, c0, 0" : "=r"(mvfr0));
		__asm__ __volatile__("mrc	p10, 7, %0, c6, c0, 0" : "=r"(mvfr1));
		if ((mvfr1 & 0x00ffff00) != 0) {
			cpu->flags |= ARM_CPU_FLAG_NEON;
		}

		if (debug_flag) {
			__asm__ __volatile__("mrc	p10, 7, %0, c0, c0, 0" : "=r" (tmp));
			kprintf("CPU%d: VFP-d%d FPSID=%x\n", cpunum, (mvfr0 & 0xf) * 16, tmp);
			if (cpu->flags & ARM_CPU_FLAG_NEON) {
				kprintf("CPU%d: NEON MVFR0=%x MVFR1=%x\n", cpunum, mvfr0, mvfr1);
			}
		}
	}

	/**
	 * ID_ISAR0
	 * [27:24] Divide_instrs
	 * Indicates the level of support for divide instructions (SDIV, UDIV):
	 * 0 - Divide instructions not supported
	 * 1 - SDIV and UDIV in the Thumb instruction set
	 * 2 - DIV and UDIV in the ARM instruction set
	 * Reset value for Cortex A15 & QCT Krait = 0x2 (both ARM and Thumb supported)
	 */
	register unsigned	id_isar0;
	__asm__ __volatile__("mrc	p15, 0, %0, c0, c2, 0" : "=r"(id_isar0)); /* read ID_ISAR0 */
	if ((id_isar0 & ID_ISAR0_DIV_MASK) >> ID_ISAR0_DIV_BIT) {
		cpu->flags |= ARM_CPU_FLAG_IDIV;
	}

	/*
	 * Indicate we are ARMv7 architecture using ARMv6 extended page tables
	 */
	cpu->flags |= ARM_CPU_FLAG_V7 | ARM_CPU_FLAG_V6 | ARM_CPU_FLAG_V6_ASID;
	
#if (_PADDR_BITS-0) == 64
	if (paddr_bits != 32) {
		// We want to set a few more things if LPAE is enabled
		if (debug_flag) {
			kprintf("%s: cpu %d setting LPAE overrides\n", __func__, cpunum);
		}

		// Procnto expects to see ARM_CPU_FLAG_V6 | ARM_CPU_FLAG_V6_ASID set
		cpu->flags |=  ARM_CPU_FLAG_LPAE;

		// LPAE requires the MAIR0/1 registers to be set
		// These are the same registers as PRRR/NMRR
		// The default values do not make sense for LPAE, so we have to init them

		// V6/V7 settings had normal memory as wb-cachable, write allocate.
		// LPAE adds a read-allocate hint, so we use that
		
		// For Omap5/A15, TI has indicated that there isn't any outer-sharable memory;
		// all RAM is inner-shareable and all outer-sharable are devices.
		//
		// These values have to be consistent with the contents of TTBCR for things to work.
		//
		// attr0 = 0x4f = outer non-cacheable, inner wb+rw allocate (works)
		//       = 0xff = outer/inner cacheable wb+rw allocate (also works)
		// attr1 = 0x44 = inner/outer non-cachable
		// attr2 = 0x04 = device
		// attr3 = 0x00 = strongly-ordered
		// attr4 = 0x4b = outer non-cacheable; inner cacheable wt+rw allocate (for shm_ctl_special)
		// attr5 = 0x4c = outer non-cacheable; inner cacheable wb-rw allocate (for shm_ctl_special)
		// attr6 = 0x48 = outer non-cacheable; inner cacheable wt-rw allocate (for shm_ctl_special)
		// attr7 = 0x00 = unused
		
		arm_lpae_mair_set(0, 0x0004444f);
		arm_lpae_mair_set(1, 0x00484c4b);

		// ttbcr, ttbr0, ttbr1 are currently set in the vstart routine
		// There isn't any reason they couldn't be set here.

		// Use vstart_lpae to enable MMU to start procnto
		vstart = vstart_lpae;

		if (debug_flag) {
			kprintf("SCO %s: vstart_lpae:%x\n", __func__, vstart_lpae);
		}
	} else 
#endif
	{
		// No LPAE or not enabled; use vstart_v7 to enable MMU to start kernel
		vstart = vstart_v7;
	}
	
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/armv_setup_v7.c $ $Rev: 759590 $")
#endif
