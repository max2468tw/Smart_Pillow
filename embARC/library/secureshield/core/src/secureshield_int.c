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


/* default interrupt exception entry */
extern void secureshield_exc_entry_int(void);
/* default cpu exception entry */
extern void secureshield_exc_entry_cpu(void);
/* trap exception handler */
extern void secureshield_trap_handler(void *frame);

static void secureshield_exc_handler_default(void *frame);
static void secureshield_exc_handler_privilege_v(void *frame);
static void secureshield_exc_handler_protect_v(void * frame);


uint32_t secureshield_exc_nest_count;

EMBARC_ALIGNED(1024)
const EXC_ENTRY secureshield_exc_entry_table[NUM_EXC_ALL] = {
	[0] = _arc_reset,
	[1 ... NUM_EXC_CPU-1] = secureshield_exc_entry_cpu,
//#if INT_PRI_MAX == INT_PRI_MIN && ARC_FEATURE_RGF_NUM_BANKS > 1 && ARC_FEATURE_FIRQ == 1
//	[NUM_EXC_CPU ... NUM_EXC_ALL-1] = secureshield_exc_entry_firq;
//#else
	[NUM_EXC_CPU ... NUM_EXC_ALL-1] = secureshield_exc_entry_int
//#endif
 };


EXC_HANDLER secureshield_exc_handler_table[NUM_EXC_CPU] = {
	secureshield_exc_handler_default,	/* Reset */
	secureshield_exc_handler_default,	/* Memory Error */
	secureshield_exc_handler_default,	/* Instruction Error */
	secureshield_exc_handler_default,	/* EV_MachineCheck */
	secureshield_exc_handler_default,	/* Reserved */
	secureshield_exc_handler_default,	/* Reserved */
	secureshield_exc_handler_protect_v,	/* EV_PortV */
	secureshield_exc_handler_privilege_v,	/* EV_PrivilegeV */
	secureshield_exc_handler_default,	/* EV_SWI	*/
	secureshield_trap_handler,		/* EV_Trap	*/
	secureshield_exc_handler_default,	/* EV_Extension	*/
	secureshield_exc_handler_default,	/* EV_DivZero	*/
	secureshield_exc_handler_default,	/* EV_DCError	*/
	secureshield_exc_handler_default,	/* EV_Maligned	*/
	secureshield_exc_handler_default,	/* Unused	*/
	secureshield_exc_handler_default	/* Unused	*/
};

SECURESHIELD_INT_HANDLER secureshield_int_handler_table[NUM_EXC_INT];

static uint32_t vector_base_prev;

static void secureshield_exc_handler_privilege_v(void *frame)
{
	uint32_t excpt_cause_reg;
	uint32_t excpt_ret_reg;
	uint8_t cause_code;
	uint8_t parameter;

	excpt_cause_reg = _arc_aux_read(AUX_ECR);
	excpt_ret_reg = _arc_aux_read(AUX_ERRET);

	parameter = excpt_ret_reg & 0xff;
	cause_code = (excpt_cause_reg >> 8) & 0xff;

	SECURESHIELD_DBG("privilege violation exception handler:\r\n");
	switch (cause_code) {
		case 0x0:
			EMBARC_HALT("privilege violation at 0x%8x"
				, excpt_ret_reg);
			break;
		case 0x1:
			EMBARC_HALT("disabled extension, parameter:%x"
				, parameter);
			break;
		case 0x02:
			EMBARC_HALT("action point hit, parameter:%x"
				, parameter);
			break;
		case 0x03:
			EMBARC_HALT("kernel only extension violation at 0x%8x"
				, excpt_ret_reg);
			break;
		default:
			EMBARC_HALT("unknown cause");
			break;
	}
	/* \todo, how to recover from privilege violation exception */
}


static void secureshield_exc_handler_protect_v(void * frame)
{
	uint32_t excpt_cause_reg;
	uint32_t excpt_ret_reg;
	uint8_t cause_code;
	uint8_t parameter;
	int32_t ret = 0;

	excpt_cause_reg = _arc_aux_read(AUX_ECR);
	excpt_ret_reg = _arc_aux_read(AUX_ERRET);

	parameter = excpt_ret_reg & 0xff;
	cause_code = (excpt_cause_reg >> 8) & 0xff;


	SECURESHIELD_DBG("protection violation exception handler:\r\n");
	switch (cause_code) {
		case 0x0:
			SECURESHIELD_DBG("instruction fetch violation at 0x%8x\r\n"
				, excpt_ret_reg);
			break;
		case 0x1:
			SECURESHIELD_DBG("memory read violation parameter:%x\r\n"
				, parameter);
			if (parameter & 0x04) {
				ret = vmpu_fault_recovery_mpu(excpt_ret_reg, frame, 0);
			}
			break;
		case 0x02:
			SECURESHIELD_DBG("memory write violation, parameter:%x\r\n"
				, parameter);
			if (parameter & 0x04) {
				ret = vmpu_fault_recovery_mpu(excpt_ret_reg, frame, 1);
			}
			break;
		case 0x03:
			SECURESHIELD_DBG("memory execution violation, parameter:%x\r\n"
				, parameter);
			if (parameter & 0x04) {
				ret = vmpu_fault_recovery_mpu(excpt_ret_reg, frame, 2);
			}
			break;
		default:
			SECURESHIELD_DBG("unknown cause:%x\r\n", cause_code);
			break;
	}

	if (ret == 0)
		EMBARC_HALT("halt caused by protection violation");

}

static void secureshield_exc_handler_default(void *frame) {
	uint32_t excpt_cause_reg;
	uint32_t excpt_ret_reg;
	uint32_t vector_no;

	excpt_cause_reg = _arc_aux_read(AUX_ECR);
	excpt_ret_reg = _arc_aux_read(AUX_ERRET);
	vector_no = (excpt_cause_reg >> 16) & 0xff;

	SECURESHIELD_DBG("default cpu exception handler\r\n");
	SECURESHIELD_DBG("excpt:%d, last sp:0x%08x, ecr:0x%08x, eret:0x%08x\r\n",
		vector_no, (uint32_t)frame, excpt_cause_reg, excpt_ret_reg);

	EMBARC_HALT("halt caused by raising the default cpu exception handler");

}


static void _secureshield_int_default_check(uint32_t intno)
{
	/* check the range */
	if (intno < NUM_EXC_CPU && intno >= NUM_EXC_ALL) {
		EMBARC_HALT("Not allowed: IRQ %d is out of range\n\r", intno);
	}

	/* check the entry whether is owned by secureshield */
	if (secureshield_exc_entry_table[intno] != secureshield_exc_entry_int) {
		EMBARC_HALT(
			"Permission denied: IRQ %d entry is not owned by secureshield"
			, intno);

	}

}

static void _secureshield_int_ac_check(uint32_t intno)
{
	SECURESHIELD_INT_HANDLER* exc;
	uint32_t is_installed;

	_secureshield_int_default_check(intno);

	/* reduce an offset */
	exc = &secureshield_int_handler_table[intno - NUM_EXC_CPU];

	is_installed = exc->handler ? 1 : 0;

	if (is_installed == 0) {
		EMBARC_HALT("IRQ %d is not installed");
	}

	/* if the intno does not belong to the active container */
	if (exc->id != g_active_container && exc->id) {
		EMBARC_HALT(
			"Permission denied: IRQ %d is owned by container %d"
			, intno, exc->id);
	}
}

/* add irq ac */
void vmpu_ac_irq(uint8_t container_id, void *function, uint32_t intno)
{
	SECURESHIELD_INT_HANDLER* exc;

	exc = &secureshield_int_handler_table[intno - NUM_EXC_CPU];

	_secureshield_int_default_check(intno);

	/* interrupt is already installed */
	/* other container can change background container's irq resources */
	if(exc->id) {
		EMBARC_HALT(
			"Permission denied: IRQ %d is owned by container %d\n\r"
			, intno, exc->id);
	}

	exc->id = container_id;
	exc->handler = function;
}

void _secureshield_int_disable(uint32_t intno)
{

	_secureshield_int_ac_check(intno);

	int_disable(intno);
	secureshield_int_handler_table[intno - NUM_EXC_CPU].enabled = 0;

}

void _secureshield_int_enable(uint32_t intno)
{
	_secureshield_int_ac_check(intno);

	int_enable(intno);
	secureshield_int_handler_table[intno - NUM_EXC_CPU].enabled = 1;
}

uint32_t _secureshield_int_enabled(uint32_t intno)
{
	_secureshield_int_ac_check(intno);

	return int_enabled(intno);
}

void _secureshield_int_pri_set(uint32_t intno, int intpri)
{

	_secureshield_int_ac_check(intno);

	int_pri_set(intno, intpri);
}

int32_t _secureshield_int_pri_get(uint32_t intno)
{
	_secureshield_int_ac_check(intno);

	return int_pri_get(intno);
}

int32_t _secureshield_int_handler_install(uint32_t intno, INT_HANDLER handler)
{

	SECURESHIELD_INT_HANDLER* exc;

	_secureshield_int_default_check(intno);

	/* reduce an offset */
	exc = &secureshield_int_handler_table[intno - NUM_EXC_CPU];

	/* if the intno does not belong to the active container */
	if (exc->id != g_active_container && exc->id) {
		EMBARC_HALT(
			"Permission denied: IRQ %d is owned by container %d"
			, intno, exc->id);
	}


	exc->handler = (EXC_HANDLER) handler;
	exc->id = handler ? g_active_container : 0;


	SECURESHIELD_DBG("IRQ %d %s container %d\n\r",
		intno,
		handler ? "registered to" : "released by",
		g_container_stack_curr_id);
	return 0;
}

INT_HANDLER _secureshield_int_handler_get(uint32_t intno)
{
	_secureshield_int_ac_check(intno);
	return secureshield_int_handler_table[intno - NUM_EXC_CPU].handler;
}

void _secureshield_int_sw_trigger(uint32_t intno)
{

	_secureshield_int_ac_check(intno);

	int_sw_trigger(intno);
}


uint32_t _secureshield_int_probe(const uint32_t intno)
{
	_secureshield_int_ac_check(intno);

	return int_probe(intno);
}

void _secureshield_int_level_config(const uint32_t intno, const uint32_t level)
{
	_secureshield_int_ac_check(intno);

	int_level_config(intno, level);
}


void _secureshield_cpu_lock(void)
{
	uint32_t i;
	SECURESHIELD_INT_HANDLER* exc;

	exc = secureshield_int_handler_table;
	for(i = 0; i < NUM_EXC_INT; i++) {
		if ((exc->id == g_active_container || exc->id == 0 ) && exc->handler != NULL) {
			int_disable(i + NUM_EXC_CPU);
		}
		exc++;
	}

}

void _secureshield_cpu_unlock(void)
{
	uint32_t i;
	SECURESHIELD_INT_HANDLER* exc;

	exc = secureshield_int_handler_table;
	for(i = 0; i < NUM_EXC_INT; i++) {
		if ((exc->id == g_active_container || exc->id == 0) && exc->handler != NULL
			 && exc->enabled) {
			int_enable(i + NUM_EXC_CPU);
		}
		exc++;
	}

}

void secureshield_int_init(void)
{
	uint32_t i;
	struct aux_irq_ctrl {
	/* note: little endian */
		uint32_t save_nr_gpr_pairs:5, res:4,
			save_blink:1, save_lp_regs:1, save_u_to_u:1,
			res2:1, save_idx_regs:1, res3:18;
	} ictrl;
#ifndef ARC_FEATURE_RF16
	ictrl.save_nr_gpr_pairs = 6;	/* r0 to r11 (r12 saved manually) */
#else
	ictrl.save_nr_gpr_pairs = 3;	/* r0 to r3, r10, r11  */
#endif
	ictrl.save_blink = 1;
	ictrl.save_lp_regs = 1;		/* LP_COUNT, LP_START, LP_END */
	ictrl.save_u_to_u = 1;		/* user context saved on user stack */
	ictrl.save_idx_regs = 1;	/* JLI, LDI, EI */

	/* remember previous exception table */
	vector_base_prev = _arc_aux_read(AUX_INT_VECT_BASE);
	_arc_aux_write(AUX_INT_VECT_BASE, (uint32_t)secureshield_exc_entry_table);
	for (i = NUM_EXC_CPU;i < NUM_EXC_ALL; i++)
	{

		/* interrupt level triggered, disabled, priority is the lowest */
		_arc_aux_write(AUX_IRQ_SELECT, i);
		_arc_aux_write(AUX_IRQ_ENABLE, 0);
		_arc_aux_write(AUX_IRQ_TRIGGER, 0);
		_arc_aux_write(AUX_IRQ_PRIORITY, INT_PRI_MAX - INT_PRI_MIN);
	}
	/* U bit is set, register is saved in user stack */
	_arc_aux_write(AUX_IRQ_CTRL, *(uint32_t *)&ictrl);
	int_ipm_set(INT_PRI_MAX);
}

extern uint32_t * secureshield_target_sp;

void secureshield_interrupt_handle(INT_EXC_FRAME *src_frame, uint32_t *umonitor_sp)
{
	uint32_t src_id, dst_id;
	INT_EXC_FRAME* dst_frame;
	uint32_t dst_fn;

	/* reuse src_id and dst_id */
	src_id = _arc_aux_read(AUX_IRQ_CAUSE);

	/* if triggered by software, clear it */
	dst_id = _arc_aux_read(AUX_IRQ_HINT);

	if (src_id == dst_id) {
		_arc_aux_write(AUX_IRQ_HINT, 0);
	}

	/* verify IRQ access privileges */
	_secureshield_int_default_check(src_id);

	/* get dst_id and interrupt handler */
	dst_id = secureshield_int_handler_table[src_id - NUM_EXC_CPU].id;
	dst_fn = (uint32_t)secureshield_int_handler_table[src_id - NUM_EXC_CPU].handler;


	/* check interrupt handler */
	if(!dst_fn) {
		EMBARC_HALT("Unprivileged handler for IRQ %d not found", dst_id);
	}

	/* gather information from current state */
	src_id = g_container_stack_curr_id;

	src_frame->erbta = 0;

	/* a proper context switch is only needed when container changed */
	if(src_id != dst_id) {
		/* save current state */
		container_stack_push(src_id, (uint32_t *)src_frame - ARC_CALLEE_FRAME_SIZE,
			src_frame->status32, dst_id);
		/* gather information from current state */
		/* create the cpu frame and exception frame for the destination container */
		dst_frame = (INT_EXC_FRAME *)(g_container_context[dst_id].cur_sp - ARC_EXC_FRAME_SIZE);
		secureshield_target_sp = ((uint32_t *)dst_frame) - ARC_CALLEE_FRAME_SIZE;

		dst_frame->erbta = 0;
		dst_frame->ret = dst_fn;
		dst_frame->status32 = g_container_context[dst_id].cpu_status;

		/* set the context stack pointer for the dst container */
		*(__secureshield_config.container_context) = g_container_context[dst_id].context_ptr;
		/* switch access control table */
		vmpu_switch(src_id, dst_id);
	} else {
		/* save current state */
		container_stack_push(src_id, (uint32_t *)src_frame, src_frame->status32, dst_id);
		_arc_sr_reg(AUX_ERRET, dst_fn);
		_arc_sr_reg(AUX_ERSTATUS, src_frame->status32);
		_arc_sr_reg(AUX_ERBTA, 0);
	}

	if (g_container_context[dst_id].cpu_status & AUX_STATUS_U_MASK) {
		*(__secureshield_config.mode) = SECURESHIELD_ENABLED;
	} else {
		*(__secureshield_config.mode) = SECURESHIELD_DISABLED;
	}
}

void secureshield_int_return(INT_EXC_FRAME *dst_frame, uint32_t *umonitor_sp)
{
	uint32_t src_id, dst_id;
	uint32_t *src_sp;


	/* discard the created cpu frame, recover the original sp of destination container */
	dst_id = g_container_stack_curr_id;

	container_stack_pop(dst_id, (uint32_t *)dst_frame + ARC_EXC_FRAME_SIZE, dst_frame->status32);

	src_id = g_container_stack[g_container_stack_ptr].src_id;
	src_sp = g_container_stack[g_container_stack_ptr].src_sp;

	if (src_id != dst_id) {
		secureshield_target_sp = src_sp;
		/* set the context stack pointer back to the one of the src container */
		*(__secureshield_config.container_context) = g_container_context[src_id].context_ptr;
		/* switch access control table */
		vmpu_switch(dst_id, src_id);
	} else {
		*umonitor_sp = (uint32_t)src_sp; /* directly return */
	}

	if (g_container_context[src_id].cpu_status & AUX_STATUS_U_MASK) {
		*(__secureshield_config.mode) = SECURESHIELD_ENABLED;
	} else {
		*(__secureshield_config.mode) = SECURESHIELD_DISABLED;
	}
	/* clear the first set bit in AUX_IRQ_ACT to simulate the quit of interrupt */
	//_arc_sr_reg(AUX_IRQ_ACT, _arc_lr_reg(AUX_IRQ_ACT) & ~(1 << _arc_lr_reg(AUX_IRQ_PRIORITY)));
	// Asm is more effective, it bitscan option is enabled */
	Asm(
	" lr %%r0, [%[irq_act]]\n"
	" ffs %%r1, %%r0\n"
	" bclr %%r0, %%r0, %%r1\n"
	" sr %%r0, [%[irq_act]]\n" :: [irq_act] "i" (AUX_IRQ_ACT));
}

void secureshield_int_ops(INT_EXC_FRAME *frame)
{
	/* FIXME: remove switch case structure, use jump table */
	/* r0 is used as operation id */
	switch (frame->r0) {
		case SECURESHIELD_INT_EXC_GET:
			frame->r0 = (uint32_t)_secureshield_int_handler_get(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_INSTALL:
			_secureshield_int_handler_install(frame->r1, (INT_HANDLER)frame->r2);
			break;
		case SECURESHIELD_INT_EXC_ENABLE:
			_secureshield_int_enable(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_ENABLED:
			frame->r0 = _secureshield_int_enabled(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_DISABLE:
			_secureshield_int_disable(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_PRI_SET:
			_secureshield_int_pri_set(frame->r1, frame->r2);
			break;
		case SECURESHIELD_INT_EXC_PRI_GET:
			frame->r0 = _secureshield_int_pri_get(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_PROBE:
			frame->r0 = _secureshield_int_probe(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_LEVEL_CONFIG:
			_secureshield_int_level_config(frame->r1, frame->r2);
			break;
		case SECURESHIELD_INT_EXC_SW_TRIG:
			_secureshield_int_sw_trigger(frame->r1);
			break;
		case SECURESHIELD_INT_EXC_CPU_LOCK:
			_secureshield_cpu_lock();
			break;
		case SECURESHIELD_INT_EXC_CPU_UNLOCK:
			_secureshield_cpu_unlock();
			break;
		default:
			SECURESHIELD_DBG("Unsupported interrupt operation:%d\r\n", frame->r0);
			break;
	}
}