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

#define __ASSEMBLY__
#include "arc.h"
#include "arc_asm_common.h"
#include "secureshield_trap_exports.h"

	.file "secureshield_entry.s"

/* entry for secureshield cpu exception handling */
	.text
	.global secureshield_exc_entry_cpu
	.align 4
secureshield_exc_entry_cpu:
 /*  as the sp may be not correct, pop and push is not allowed
 use ilink as temp register, ilink is not used in secureshield */
	lr	ilink, [AUX_ERSTATUS]
	btst	ilink, AUX_STATUS_U_BIT
	bz	exc_called_from_priv
	/* recover user mode sp, secureshield sp is in user_sp */
	lr	sp, [AUX_USER_SP]

exc_called_from_priv:

	EXCEPTION_PROLOGUE

	mov	blink, sp
	mov	r3, sp		/* as exception handler's parameter(exc_frame) */

/* secureshield_exc_nest_count +1 */
	ld	r0, [secureshield_exc_nest_count]
	add	r0, r0, 1
	st	r0, [secureshield_exc_nest_count]

	cmp	r0, 1
	bne	exc_handler_1

/* switch to secureshield stack when exception is in user mode */
	mov	sp, __secureshield_stack;
exc_handler_1:
	PUSH	blink

/* find the exception cause */
	lr	r0, [AUX_ECR]
	lsr	r0, r0, 16
	bmsk	r0, r0, 7
	mov	r1, secureshield_exc_handler_table
	ld.as	r2, [r1, r0]

	mov	r0, r3
	mov	r1, sp
	jl	[r2]		/* jump to exception handler where interrupts are not allowed! */

/*
 * come back to last sp, secureshield stack will not be used
 * 1. to save more callee registers, do container switch
 * 2. no container switch, just return from exception
 */
	POP	sp

/* secureshield_exc_nest_count -1 */
	mov	r1, secureshield_exc_nest_count
	ld	r0, [r1]
	sub	r0, r0, 1
	cmp	r0, 0
	bne.d	ret_exc /* for nest privileged mode, return directly */
	st	r0, [r1]

	ld	r0, [secureshield_target_sp]
	cmp	r0, 0
	beq	ret_exc

/* there is a context switch request */
	SAVE_CALLEE_REGS	/* save callee save registers */
	mov	sp, r0
	mov	r0, 0
	st	r0, [secureshield_target_sp]
	RESTORE_CALLEE_REGS

ret_exc:	/* return from exception to unprivileged mode*/
	EXCEPTION_EPILOGUE
/*
 * come back to the original sp:
 * if exception comes from unprivileged mode , sp == aux_user_sp == user stack,
 *	aex will be called
 * if exception comes from privileged mode, sp == aux_user_sp == secureshield stack
 *	aex will not called, uvisro_exc_nest_count is not 0,
 * note: when codes run in user mode, aux_user_sp does not contain the privileged stack pointer
 */
	sr sp,  [AUX_USER_SP]
#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
/* when gp is not changed during execution and sdata is enabled, the following is meaningful */
/* The newlib c of ARC GNU is compiled with sdata enabled */
	mov	gp, _f_sdata	/* init small-data base register */
#endif
	rtie


	.global secureshield_exc_entry_int
	.align 4
secureshield_exc_entry_int:
	clri	/* disable interrupt */
/* \todo check where interrupt comes from: unprivileged or privileged */
/*  Utilize the fact that Z bit is set if interrupt taken in U mode*/
	bnz	int_from_priv
/* recover user sp to save more registers */
	lr	sp, [AUX_USER_SP]
/* interrupt from privileged mode */
int_from_priv:
#if ARC_FEATURE_FIRQ == 1
	PUSH	r0 /* free up a register */
	lr	r0, [AUX_IRQ_ACT]
	btst	r0, 0
	bz	.L_normal_irq
	lr	r0, [0xb]	/* status p0 */
	sr	r0, [AUX_ERSTATUS] /* return pc is already in eret */
#if ARC_FEATURE_RGF_NUM_BANKS > 1
/* sp is also banked when there are more than 16 banked regs.
 */
#if ARC_FEATURE_RGF_BANKED_REGS >= 16
	sr	sp, [AUX_USER_SP]
#endif
	kflag	0x1000	/* switch bank0, loop enable */
#if ARC_FEATURE_RGF_BANKED_REGS >= 16
	lr	sp, [AUX_USER_SP]
#endif
	add	sp,sp, 4 /* r0 is banked , so just adjust sp */
#else
	POP	r0
#endif
	EXCEPTION_PROLOGUE
	b .L_common_for_irq_and_firq
#endif
.L_normal_irq:
#if ARC_FEATURE_FIRQ == 1
	POP	r0
#endif
	INTERRUPT_PROLOGUE

.L_common_for_irq_and_firq:
	mov	blink, sp
	mov	r3, sp		/* as exception handler's parameter(exc_frame) */

/* secureshield_exc_nest_count+1 */
	ld	r0, [secureshield_exc_nest_count]
	add	r0, r0, 1
	st	r0, [secureshield_exc_nest_count]

	cmp	r0, 1
	bne	int_handler_1

/* switch to secureshield stack when user mode switches to kernel mode */
	mov 	sp, __secureshield_stack
int_handler_1:
	PUSH	blink

	mov	r0, r3
	mov	r1, sp
	bl	secureshield_interrupt_handle

/*
 * come back to container sp, secureshield stack will not be used
 */
	POP	sp
	kflag	0x20	/* take interrupt as exception, return as an exception */
/* secureshield_exc_nest_count-1 */
	mov	r1, secureshield_exc_nest_count
	ld	r0, [r1]
	sub	r0, r0, 1
	st	r0, [r1]

	ld	r0, [secureshield_target_sp]
	cmp	r0, 0
	beq	ret_int_unprivileged_no_cx

/* there is a context switch request */
	SAVE_CALLEE_REGS	/* save callee save registers */
	mov	sp, r0
	mov	r0, 0
	st	r0, [secureshield_target_sp]
	RESTORE_CALLEE_REGS

	EXCEPTION_EPILOGUE

/* set blink to interrupt return */
	mov	blink, secureshield_monitor_call_int_out
/* exception exit epilogue */
	sr	sp, [AUX_USER_SP]
#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
/* when gp is not changed during execution and sdata is enabled, the following is meaningful */
/* The newlib c of ARC GNU is compiled with sdata enabled */
	mov	gp, _f_sdata	/* init small-data base register */
#endif
	rtie	/* rtie will do an exception return */


/* interrupt  belongs to the same container, no container switch */
ret_int_unprivileged_no_cx:
/* AUX_ERET, AUX_ERSTATUS, AUX_ERBTA are set in secureshield_interrupt_handle */
/* set blink to interrupt return */
	mov	blink, secureshield_monitor_call_int_out
	sr	sp, [AUX_USER_SP]
	rtie	/* rtie will do an exception return */

/** @endcond */
