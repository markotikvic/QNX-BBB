#
# Copyright 2011, QNX Software Systems. 
# 
# Licensed under the Apache License, Version 2.0 (the "License"). You 
# may not reproduce, modify or distribute this software except in 
# compliance with the License. You may obtain a copy of the License 
# at: http://www.apache.org/licenses/LICENSE-2.0 
# 
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" basis, 
# WITHOUT WARRANTIES OF ANY KIND, either express or implied.
#
# This file may contain contributions from others, either as 
# contributors under the License or as licensors under other terms.  
# Please review this entire file for other proprietary rights or license 
# notices, as well as the QNX Development Suite License Guide at 
# http://licensing.qnx.com/license-guide/ for other information.
#
/*
 * dummy serial I/O
 */

#include "callout.ah"


/*
 * -----------------------------------------------------------------------
 * void	display_char_null(struct sypage_entry *, char)
 * -----------------------------------------------------------------------
 */
CALLOUT_START(display_char_null, 0, 0)
	mov		pc, lr
CALLOUT_END(display_char_null)


/*
 * -----------------------------------------------------------------------
 * char	poll_key_null(struct sypage_entry *)
 * -----------------------------------------------------------------------
 */
CALLOUT_START(poll_key_null, 0, 0)
	/*
	 * Always return -1
	 */
	mov	    r0, #-1
	mov		pc, lr
CALLOUT_END(poll_key_null)


/*
 * -----------------------------------------------------------------------
 * int break_detect(struct syspage_entry *)
 *
 * Return 1 if break detected, 0 otherwise
 * -----------------------------------------------------------------------
 */
CALLOUT_START(break_detect_null, 0, 0)
	mov		r0, #0
	mov		pc, lr
CALLOUT_END(break_detect_null)

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/arm/callout_debug_null.S $ $Rev: 701498 $";
.previous
#endif
#endif 
