/* ------------------------------------------
 * Copyright (c) 2015, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \version 2016.05
 * \date 2016-02-03
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */
#include "secureshield_config.h"
#include "secureshield_trap_exports.h"

.globl __secureshield_config
.globl secureshield_container_ctx
.globl __secureshield_stack
.weak  __secureshield_mode

.section .secureshield.main, "a"
__secureshield_config:
	/* secureshield expects its configuration section right
	   after itself in flash. If the configuration magic is not found,
	   secureshield will intentionally freeze to avoid accidentally
	   unprotected systems */
	.long  SECURESHIELD_MAGIC

	/* secureshield mode
	   0: disabled
	   1: enabled
	 */
	.long __secureshield_mode

	/* start and end address of protected bss */
	.long __secureshield_bss_start
	.long __secureshield_bss_end

	/* start and end address of secureshield main bss */
	.long __secureshield_bss_main_start
	.long __secureshield_bss_main_end

	/* start and end address of secureshield container bss */
	.long __secureshield_bss_containers_start
	.long __secureshield_bss_containers_end

	/* start and end address of secureshield code and data */
	.long __secureshield_main_start
	.long __secureshield_main_end

	/* start and end address of protected flash region */
	/* note: remaining flash size is available for configuration storage */
	.long __secureshield_secure_start
	.long __secureshield_secure_end

	 /* start and end address of boxes configuration tables */
	.long __secureshield_cfgtbl_start
	.long __secureshield_cfgtbl_end

	/* start and end address of list of pointers to boxes configuration tables */
	.long __secureshield_cfgtbl_ptr_start
	.long __secureshield_cfgtbl_ptr_end

	/* pointer to __secureshield_shield_context */
	.long __container_context

__secureshield_mode:
	/* secureshield default mode - user can override weak reference */
	.long 0

.section .bss
__container_context:
secureshield_container_ctx:
	.space 4


.section .keep.secureshield.bss.main, "aw", @nobits
	.space SECURESHIELD_STACK_SIZE
__secureshield_stack:

/* the following are return entries of monitor call */
	.text
	.align 4
	.global secureshield_monitor_call_int_out
secureshield_monitor_call_int_out:
	trap_s SECURESHIELD_MONITOR_CALL_INT_OUT

	.align 4
	.global secureshield_monitor_call_container_out
secureshield_monitor_call_container_out:
	trap_s SECURESHIELD_MONITOR_CALL_CONTAINER_OUT
