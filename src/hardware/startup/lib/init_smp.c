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
#include <hw/inout.h>

volatile int		cpu_starting;
volatile int		syspage_available;
void				(*smp_spin_vaddr)(void);
output_callout_t	*smp_prev_output_rtn;

extern struct callout_rtn	smp_spin;
extern void 				smp_start(void);


static void
transfer_aps(void) {
	unsigned				i;
	volatile uint32_t		*pending;

	pending = &SYSPAGE_ENTRY(smp)->pending;

	for(i = 1; i < lsp.syspage.p->num_cpu; ++i) {
		//Get one AP into the syspage spin callout
		*pending = 1;
		mem_barrier();
#ifdef __MIPS__
		setcp0_count(0);
#endif
		syspage_available = i;
		do {
			mem_barrier();	 // give the bus a break.
		} while(*pending != 0);
	}
}


static void
start_aps(void) {
	int			i;
	unsigned	count;

	for(i = 1; i < lsp.syspage.p->num_cpu; ++i) {
		cpu_starting = i + 1;

		if(board_smp_start(i, smp_start)) {
			count = 0;
			do {
				if(++count == 0) ap_fail(i);
			} while(cpu_starting != 0);
		} else {
			ap_fail(i);
		}
	}

	smp_hook_rtn = transfer_aps;

#if defined(__X86__)
	// For backwards compatability, remove later
	lsp.cpu.x86_smpinfo.p->lapic_addr = TOPTR(lsp.smp.p->cpu);
#endif
}


static output_callout_t *
callout_output_smp(int sizing) {
	struct smp_entry *smp = lsp.smp.p;

	callout_output_one(sizing, &smp_spin_vaddr);
	if(smp->send_ipi != NULL) {
		callout_output_one(sizing, &smp->send_ipi);
	}
#if defined(__PPC__)
	// For backwards compatability, remove later
	{
		struct ppc_smpinfo_entry	*ppc_smp = lsp.cpu.ppc_smpinfo.p;

		if(ppc_smp->send_ipi != NULL) {
			callout_output_one(sizing, &ppc_smp->send_ipi);
		}
	}
#endif
	return(smp_prev_output_rtn);
}

//
// Initialize smp structure in the system page.
//
void
init_smp() {
	struct smp_entry		*smp;
	unsigned				num_cpus;

	if(max_cpus > 1) {
		num_cpus = board_smp_num_cpu();
		if(num_cpus > max_cpus) num_cpus = max_cpus;
		lsp.syspage.p->num_cpu = num_cpus;
	
		if(num_cpus > 1) {
			smp = set_syspage_section(&lsp.smp, sizeof(*lsp.smp.p));
	
#if defined(__PPC__)
			// For backwards compatability, remove later
			set_syspage_section(&lsp.cpu.ppc_smpinfo, sizeof(*lsp.cpu.ppc_smpinfo.p));
#endif
			smp_spin_vaddr = (void (*)(void))&smp_spin;

			board_smp_init(smp, num_cpus);
	
			smp_hook_rtn = start_aps;
	
			//Hook into the sizing/writing list
			smp_prev_output_rtn = callout_output_rtn;
			callout_output_rtn = (output_callout_t *)callout_output_smp;

#if defined(__X86__)
			// For backwards compatability, remove later
			{
#if LAPIC_ID_MAX != PROCESSORS_MAX
#error x86/syspage:LAPIC_ID_MAX != startup.h:PROCESSORS_MAX
#endif
				unsigned	i;
				struct x86_smpinfo_entry	*x86_smp;
		
				x86_smp = set_syspage_section(&lsp.cpu.x86_smpinfo, sizeof(*lsp.cpu.x86_smpinfo.p));
				memset(x86_smp->lapic_id, ~0, sizeof(x86_smp->lapic_id));
				for(i = 0; i < num_cpus; ++i) {
					extern uint8_t cpuid_remap[];
					/*
					 * the syspage has a field 'lapicid_to_index'. I'm not sure what its original purpose
					 * was but it no longer conveys any useful information and nothing seems to use it.
					 * I going to use this field to provide the LAPIC ID's of each of the processors in
					 * the system. This will be useful in routing interrupts. Now, the name of the field
					 * is actually the inverse of what I'm using it for. The contents of 'lapicid_to_index[]'
					 * will actually be the LAPIC ID for the CPU at the specified index (CPU's are numbered
					 * starting with 0 as the boot CPU (BSP) increasing contiguously for the number of
					 * processors in the system)
					 */
					x86_smp->lapic_id[i] = cpuid_remap[i];
					if (debug_flag > 1) kprintf("CPU%u has APIC ID 0x%x\n", i, x86_smp->lapic_id[i]);
				}
			}
#endif
		}
	}
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/init_smp.c $ $Rev: 690416 $")
#endif
