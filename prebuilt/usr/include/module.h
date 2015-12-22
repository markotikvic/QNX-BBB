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

#ifndef _MODULE_H_INCLUDED
#define _MODULE_H_INCLUDED



/*
 * args				Process args for this module (only called every
 *					time the module name appears on the command line.
 * attach			The "config driver" attach is called first, to initialize
 *					the xpt subsystem. It then calles all the MODULE_SIM
 *					attach entries to initalize the hardware. Lastly, the
 *					MODULE_DRIVER attach are called to initialize the
 *					connection to the OS.
 * detach			Used to tell all modules to cleanup as the driver will
 *					be terminating.
 */

struct _cam_entry;

typedef struct module_entry {
	char			*name;
	int				(*args)(char *options);
	int				(*attach)(struct _cam_entry *data);
	int				(*detach)(void);
} MODULE_ENTRY;

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devb/include/module.h $ $Rev: 680332 $")
#endif
