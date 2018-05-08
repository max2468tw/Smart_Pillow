/*------------------------------------------
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
#ifndef _SECURESHIELD_TRAP_EXPORTS_H_
#define _SECURESHIELD_TRAP_EXPORTS_H_

/* maximum depth of nested context switches
 * this includes both interrupts and container call, as they use the same container stack
 * for their context switches */
#define SECURESHIELD_CONTAINER_STACK_MAX_DEPTH 0x10

#define SECURESHIELD_MONITOR_CALL_SYS			(0)
#define SECURESHIELD_MONITOR_CALL_CONTAINER_IN		(1)
#define SECURESHIELD_MONITOR_CALL_CONTAINER_OUT		(2)
#define SECURESHIELD_MONITOR_CALL_INT_OUT		(3)
#define SECURESHIELD_MONITOR_CALL_AUX			(4)
#define SECURESHIELD_MONITOR_CALL_INT_EXC		(5)


/* interrupt related operation codes */
#define SECURESHIELD_INT_EXC_GET		(0)
#define SECURESHIELD_INT_EXC_INSTALL		(1)
#define SECURESHIELD_INT_EXC_ENABLE		(2)
#define SECURESHIELD_INT_EXC_ENABLED		(3)
#define SECURESHIELD_INT_EXC_DISABLE		(4)
#define SECURESHIELD_INT_EXC_PRI_GET		(5)
#define SECURESHIELD_INT_EXC_PRI_SET		(6)
#define SECURESHIELD_INT_EXC_PROBE		(7)
#define SECURESHIELD_INT_EXC_LEVEL_CONFIG	(8)
#define SECURESHIELD_INT_EXC_SW_TRIG		(9)
#define SECURESHIELD_INT_EXC_CPU_LOCK		(10)
#define SECURESHIELD_INT_EXC_CPU_UNLOCK		(11)


/* system-level operations codes */
#define SECURESHIELD_SYS_LR			(0)
#define SECURESHIELD_SYS_SR			(1)
#define SECURESHIELD_SYS_PERF_CFG		(2)
#define SECURESHIELD_SYS_PERF_START		(3)
#define SECURESHIELD_SYS_PERF_END		(4)
#define SECURESHIELD_SYS_CONTAINER_ID_SELF	(5)
#define SECURESHIELD_SYS_CONTAINER_ID_CALLER	(6)

#define SECURESHIELD_CONTAINER_CALL_MAGIC  0x12345678

/* get the immediate from  trap instruction */
#define SECURESHIELD_TRAP_IMMEDIATE(trap_instn) ((trap_instn >> 5) & 0x3F)

#define SECURESHIELD_TRAP_ID_MASK		(0x3F)

/* macro to execute an TRAP call; additional metadata can be provided, which will
 * be appended right after the trap instruction */
/* note: the macro is implicitly overloaded to allow 0 to 7 32bits arguments */

#define SECURESHIELD_MONITOR_CALL(id, metadata, ...) \
	({ \
		SECURESHIELD_MACRO_REGS_ARGS(uint32_t, ##__VA_ARGS__) \
		SECURESHIELD_MACRO_REGS_RETVAL(uint32_t, res) \
		Asm( \
			"trap_s %[trap_id]\n" \
			metadata \
			: SECURESHIELD_MACRO_ASM_OUTPUT(res) \
			: SECURESHIELD_MACRO_ASM_INPUT(__VA_ARGS__), \
			[trap_id] "i" (id) \
		); \
		res; \
	})


#endif/*_SECURESHIELD_TRAP_EXPORTS_H_*/
