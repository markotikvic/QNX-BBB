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




#include "startup.h"
#include <arm/mpcore.h>

/*
 * Additional Cortex A9 MPCore specific CPU initialisation
 */

#define	CPUID_REVISION(x)	((x) & 0x00f0000f)
#define	CPUID_R2P0			0x00200000
#define CPUID_R2P2			0x00200002
#define CPUID_R3P0			0x00300000

void
armv_setup_a9mp(struct cpuinfo_entry *cpu, unsigned cpuid)
{
	unsigned	aux;
	unsigned	cpunum;
	unsigned	scu_diag;

	cpunum = arm_v6_cpunum();
	if (cpunum == 0) {
		if (mpcore_scu_base == ~0) {
			crash("mpcore_scu_base is not set");
		}

		/*
		 * SCU invalidate all and enable SCU
		 */
		out32(mpcore_scu_base + MPCORE_SCU_INVALIDATE, -1);
		out32(mpcore_scu_base + MPCORE_SCU_CTRL, MPCORE_SCU_CTRL_EN);
		scu_diag = in32(mpcore_scu_base + MPCORE_SCU_DIAGNOSTIC);        
		scu_diag |= 0x1;              //disable migratory
		out32(mpcore_scu_base + MPCORE_SCU_DIAGNOSTIC, scu_diag);
	}
	else {
		/*
		 * Initialise the GIC cpu interface for this cpu.
		 * For CPU0, this should be initialised in init_intrinfo()
		 */
		arm_gic_cpu_init();	
	}

	/*
	 * Configure CP15 auxiliary control register:
	 * - clear EXCL (bit 7) to set inclusive caches
	 * - set SMP (bit 6)
	 * - set FW  (bit 0) enable cache/TLB maintainence broadcast
	 */
	aux = arm_v6_cp15_auxcr_get();	
	aux &= ~(1 << 7);
	aux |= (1 << 6) | (1 << 0);
	if (CPUID_REVISION(cpuid) < CPUID_R2P0) {
		aux &= ~((1 << 2) | (1 << 1));	// Errata #719332 - disable prefetch
	}
	arm_v6_cp15_auxcr_set(aux);	

	/*
	 * Processor implements ARMv7 Multiprocessor Extensions
	 */
	cpu->flags |= ARM_CPU_FLAG_V7_MP;

	if (CPUID_REVISION(cpuid) < CPUID_R2P0) {
		/*
		 * Inner-shareable TLB ops don't work correctly so make sure
		 * procnto-smp applies the necessary workaround using TLBIALLIS
		 */
		cpu->flags &= ~ARM_CPU_FLAG_V7_MP;
		cpu->flags |= ARM_CPU_FLAG_V7_MP_ERRATA;	// Errata #720789

		mmu_cr_set |= ARM_MMU_CR_RR;		// Errata #716044
	}

	/*
	 * Fix for ARM Errata 742230 (applicable to Cortex-A9 MPCore versions <= R2P2) and
	 * ARM Errata 794072 (applicable to all versions of the Cortex-A9 MPCore)
	 */
	__asm__ __volatile__("mrc	p15, 0, %0, c15, c0, 1" : "=r"(aux));
	aux |= (1 << 4);
	__asm__ __volatile__("mcr	p15, 0, %0, c15, c0, 1" : : "r"(aux));

	if (CPUID_REVISION(cpuid) < CPUID_R3P0) {
		/* 
		 * Apply fix for ARM_ERRATA_751472 for all r0, r1, r2 revisions:
		 * "An interrupted ICIALLUIS operation may prevent the completion of a
		 * following broadcasted operation" 
		 */
		__asm__ __volatile__("mrc p15, 0, %0, c15, c0, 1" : "=r"(aux));	// read diagnostic register
		aux |= (1<<11);								// set bit #11 
		__asm__ __volatile__("mcr p15, 0, %0, c15, c0, 1" : : "r"(aux));	// write diagnostic register

		if (CPUID_REVISION(cpuid) >= CPUID_R2P0) {
			/*
		 	* Apply fix for ARM_ERRATA_743622 for all r2 revisions:
		 	* "Faulty logic in the Store Buffer may lead to data corruption"
		 	*/
			__asm__ __volatile__("mrc p15, 0, %0, c15, c0, 1" : "=r"(aux));	// read diagnostic register
			aux |= (1<<6);								// set bit #6
			__asm__ __volatile__("mcr p15, 0, %0, c15, c0, 1" : : "r"(aux));	// write diagnostic register
		}
	}

	/*
	 * Perform generic ARMv7 CPU initialisation
	 */
	armv_setup_v7(cpu, cpuid, cpunum);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/armv_setup_a9mp.c $ $Rev: 754192 $")
#endif
