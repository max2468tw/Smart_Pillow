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
//#define DEBUG
//#define DEBUG_HOSTLINK	/* use hostlink to output debug message */
#include "secureshield.h"
#include "secureshield_trap.h"
#include "secureshield_vmpu.h"
#include "secureshield_int.h"
#include "secureshield_sys.h"

extern void secureshield_monitor_call_container_out(void);
extern void secureshield_exc_entry_cpu(void);

/* container call stack variables */
CONTAINER_STACK_ITEM g_container_stack[SECURESHIELD_CONTAINER_STACK_MAX_DEPTH];
uint32_t g_container_stack_ptr;

CONTAINER_CONTEXT g_container_context[SECURESHIELD_MAX_CONTAINERS];
uint8_t g_container_stack_curr_id;
uint32_t *secureshield_target_sp;


static void trap_container_call_in(INT_EXC_FRAME *src_frame)
{
	uint32_t trap_pc;
	uint8_t src_id, dst_id;
	INT_EXC_FRAME *dst_frame;
	uint32_t dst_fn;
	/* number of arguments to pass to the target function */
	uint8_t args;
	uint32_t *src, *dst;

	/* get it from stack or AUX register */
	trap_pc = src_frame->ret - 2;

	trap_cc_check_magic((CONTAINER_CALL *)trap_pc);
	dst_fn = trap_cc_get_dst_fn((CONTAINER_CALL *)trap_pc);
	dst_id = trap_cc_get_dst_id((CONTAINER_CALL *)trap_pc);

	src_id = g_container_stack_curr_id;
	/* check src and dst IDs */
	if (src_id == dst_id) {
		EMBARC_HALT("container call is not allowed in the same container", src_id);
	}

	args = src_frame->r0; /* r0 is argument number */

	/* get the right interface from interface table */
	if (vmpu_find_interface(dst_id, (void *)dst_fn, args) == NULL) {
		EMBARC_HALT("interface @%x not found in container %d", dst_fn, dst_id);
	}

	/* push the calling container and set the callee container */
	/* the left registers of src container will be saved later, reserve space here */
	container_stack_push(src_id, ((uint32_t *)src_frame) - ARC_CALLEE_FRAME_SIZE,
		src_frame->status32, dst_id);

	/* create the cpu frame and exception frame for the destination container */
	dst_frame = (INT_EXC_FRAME *)(g_container_context[dst_id].cur_sp - ARC_EXC_FRAME_SIZE);
	/* need to check whether dst_sp is overflow ? */
	secureshield_target_sp = ((uint32_t *)dst_frame) - ARC_CALLEE_FRAME_SIZE;

	dst_frame->erbta = 0; /* erbta, is 0 the correct value? */
	dst_frame->ret = dst_fn; /* eret */
	dst_frame->status32 = g_container_context[dst_id].cpu_status;

	/* set the dst_fn as the return address of destination container  */
	dst_frame->blink = (uint32_t)secureshield_monitor_call_container_out;

	/* copy parameters */
	src = (uint32_t *)&(src_frame->r1);
	dst = (uint32_t *)&(dst_frame->r0);
	/* r1->r0, r2->r1, ... r6->r5 */
	while(args--) {
		*dst = *src;
		dst++;
		src++;
	}

	if (g_container_context[dst_id].cpu_status & AUX_STATUS_U_MASK) {
		*(__secureshield_config.mode) = SECURESHIELD_ENABLED;
	} else {
		*(__secureshield_config.mode) = SECURESHIELD_DISABLED;
	}
	/* set the context stack pointer for the dst box */
	*(__secureshield_config.container_context) = g_container_context[dst_id].context_ptr;

	/* switch access control tables */
	vmpu_switch(src_id, dst_id);
}

static void trap_container_call_out(INT_EXC_FRAME *dst_frame)
{
	uint32_t src_id, dst_id;
	PROCESSOR_FRAME *src;

	/* discard the created cpu frame, recover the original sp of destination container */
	dst_id = g_container_stack_curr_id;

	container_stack_pop(dst_id, (uint32_t *)dst_frame + ARC_EXC_FRAME_SIZE,
		dst_frame->status32);

	src_id = g_container_stack[g_container_stack_ptr].src_id;

	/* set the target stack pointer */
	secureshield_target_sp = g_container_stack[g_container_stack_ptr].src_sp;

	src = (PROCESSOR_FRAME *)g_container_stack[g_container_stack_ptr].src_sp;

	/* copy return value */
	src->exc_frame.r0 = dst_frame->r0;

	if (g_container_context[src_id].cpu_status & AUX_STATUS_U_MASK) {
		*(__secureshield_config.mode) = SECURESHIELD_ENABLED;
	} else {
		*(__secureshield_config.mode) = SECURESHIELD_DISABLED;
	}
	/* set the context stack pointer back to the one of the src container */
	*(__secureshield_config.container_context) = g_container_context[src_id].context_ptr;

	/* switch access control tables */
	vmpu_switch(dst_id, src_id);
}


void secureshield_trap_handler(void *exc_frame, uint32_t *umonitor_sp)
{
	INT_EXC_FRAME *trap_frame = (INT_EXC_FRAME *)exc_frame;
	uint8_t trap_id;

	trap_id = _arc_lr_reg(AUX_ECR) & SECURESHIELD_TRAP_ID_MASK;

	switch (trap_id) {
		case SECURESHIELD_MONITOR_CALL_SYS:
			SECURESHIELD_DBG("system operations\r\n");
			secureshield_sys_ops(trap_frame);
			break;
		case SECURESHIELD_MONITOR_CALL_CONTAINER_IN:
			SECURESHIELD_DBG("container call in\r\n");
			trap_container_call_in(trap_frame);
			break;
		case SECURESHIELD_MONITOR_CALL_CONTAINER_OUT:
			SECURESHIELD_DBG("container call out\r\n");
			trap_container_call_out(trap_frame);
			break;
		case SECURESHIELD_MONITOR_CALL_INT_OUT:
			SECURESHIELD_DBG("interrupt return\r\n");
			secureshield_int_return(trap_frame, umonitor_sp);
			break;
		case SECURESHIELD_MONITOR_CALL_INT_EXC:
			SECURESHIELD_DBG("interrupt operations\r\n");
			secureshield_int_ops(trap_frame);
			break;
		default:
			SECURESHIELD_DBG("unknown monitor call id%d\r\n", trap_id);
			break;
	}
}
