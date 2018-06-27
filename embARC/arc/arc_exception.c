/* ------------------------------------------
 * Copyright (c) 2016, Synopsys, Inc. All rights reserved.

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
 * \date 2014-07-15
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */

/**
 * \file
 * \ingroup ARC_HAL_EXCEPTION_CPU ARC_HAL_EXCEPTION_INTERRUPT
 * \brief C Implementation of exception and interrupt management
 */
#undef LIB_SECURESHIELD_OVERRIDES
#include "arc_exception.h"
#include "arc_cache.h"

#define	 DBG_LESS
#include "embARC_debug.h"

/**
 * \addtogroup ARC_HAL_EXCEPTION_CPU
 * @{
 * \var exc_entry_table
 * \brief exception entry table
 *
 * install exception entry table to ARC_AUX_INT_VECT_BASE in startup.
 * According to ARCv2 ISA, vectors are fetched in instruction space and thus
 * may be present in ICCM, Instruction Cache, or
 * main memory accessed by instruction fetch logic.
 * So it is put into a specific section .vector.
 *
 * Please note that the exc_entry_table maybe cached in ARC. Some functions is
 * defined in .s files.
 *
 */

/**
 * \ingroup ARC_HAL_EXCEPTION_CPU
 * \brief  default cpu exception handler
 * \param p_excinf pointer to the exception frame
 */
static void exc_handler_default(void *p_excinf)
{
	uint32_t excpt_cause_reg = 0;
	uint32_t excpt_ret_reg = 0;
	uint32_t exc_no = 0;

	excpt_cause_reg = _arc_aux_read(AUX_ECR);
	excpt_ret_reg = _arc_aux_read(AUX_ERRET);
	exc_no = (excpt_cause_reg >> 16) & 0xff;

	dbg_printf(DBG_LESS_INFO, "default cpu exception handler\r\n");
	dbg_printf(DBG_LESS_INFO, "excpt:%s, last sp:0x%08x, ecr:0x%08x, eret:0x%08x\r\n",
		exc_no2string(exc_no), (uint32_t)p_excinf, excpt_cause_reg, excpt_ret_reg);

	Asm("kflag 1");
}


/**
 * \ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * \brief  default interrupt handler
 * \param[in] p_excinf	information for interrupt handler
 */
static void int_handler_default(void *p_excinf)
{
	uint32_t int_cause_reg = 0;
	uint32_t excpt_ret_reg = 0;

	int_cause_reg = _arc_aux_read(AUX_IRQ_CAUSE);
	excpt_ret_reg = _arc_aux_read(AUX_ERRET);
	dbg_printf(DBG_LESS_INFO, "default interrupt handler\n\r");
	dbg_printf(DBG_LESS_INFO, "last sp:0x%08x, icause:0x%08x\n\r", (uint32_t)p_excinf, int_cause_reg);

	Asm("kflag 1");
}

__attribute__ ((aligned(1024), section(".vector")))
EXC_ENTRY exc_entry_table[NUM_EXC_ALL] = {
 	[0] = _arc_reset,
 	[1 ... NUM_EXC_CPU-1] = exc_entry_cpu,
	[NUM_EXC_CPU ... NUM_EXC_ALL-1] = exc_entry_int
 };
/**
 * \var exc_int_handler_table
 * \brief the cpu exception and interrupt exception handler table
 * called in exc_entry_default and exc_entry_int
 */
EXC_HANDLER exc_int_handler_table[NUM_EXC_ALL] = {
	 [0 ... NUM_EXC_CPU-1] = exc_handler_default,
	 [NUM_EXC_CPU ... NUM_EXC_ALL-1] = int_handler_default
};

/**
 * \var exc_nest_count
 * \brief the counter for exc/int processing, =0 no int/exc
 * >1 in int/exc processing
 * @}
 */
uint32_t exc_nest_count;


/**
 * \ingroup ARC_HAL_EXCEPTION_CPU
 * \brief  install a CPU exception entry
 * \param[in] excno exception number
 * \param[in] entry the entry of exception to install
 */
int32_t exc_entry_install(const uint32_t excno, EXC_ENTRY entry)
{
	uint32_t status;

	EXC_ENTRY *table = (EXC_ENTRY *)_arc_aux_read(AUX_INT_VECT_BASE);

	if (excno < NUM_EXC_ALL && entry != NULL
		&& table[excno] != entry) {
		status = cpu_lock_save();
		/* directly write to mem, as arc gets exception handler from mem not from cache */
		/* FIXME, here maybe icache is dirty, need to be invalidated */
		table[excno] = entry;

		if (_arc_aux_read(AUX_BCR_D_CACHE) > 0x2) {
		/* dcache is available */
			dcache_flush_line((uint32_t)&table[excno]);
		}

		if (_arc_aux_read(AUX_BCR_D_CACHE) > 0x2) {
		/* icache is available */
			icache_invalidate_line((uint32_t)&table[excno]);
		}
		cpu_unlock_restore(status);
		return 0;
	}
	return -1;
}

/**
 * \ingroup ARC_HAL_EXCEPTION_CPU
 * \brief  get the installed CPU exception entry
 * \param[in] excno exception number
 * \return the installed CPU exception entry
 */
EXC_ENTRY exc_entry_get(const uint32_t excno)
{
	if (excno < NUM_EXC_ALL) {
		return exc_entry_table[excno];
	}
	return NULL;
}

/**
 * \ingroup ARC_HAL_EXCEPTION_CPU
 * \brief  install an exception handler
 * \param[in] excno	exception number
 * \param[in] handler the handler of exception to install
 */
int32_t exc_handler_install(const uint32_t excno, EXC_HANDLER handler)
{
	if (excno < NUM_EXC_CPU && handler != NULL) {
		exc_int_handler_table[excno] = handler;
		return 0;
	}

	return -1;
}

/**
 * \ingroup ARC_HAL_EXCEPTION_CPU
 * \brief  get the installed exception handler
 * \param[in] excno	exception number
 * \return the installed exception handler or NULL
 */
EXC_HANDLER exc_handler_get(const uint32_t excno)
{
	if (excno < NUM_EXC_CPU) {
		return exc_int_handler_table[excno];
	}

	return NULL;
}

/**
 * \ingroup ARC_HAL_EXCEPTION_CPU ARC_HAL_EXCEPTION_INTERRUPT
 * \brief  intialize the exception and interrupt handling
 */
void exc_int_init(void)
{
	uint32_t i;
	uint32_t status;
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
	ictrl.save_u_to_u = 0;		/* user ctxt saved on kernel stack */
	ictrl.save_idx_regs = 1;	/* JLI, LDI, EI */

	status = cpu_lock_save();
	for (i = NUM_EXC_CPU; i < NUM_EXC_ALL; i++) {
		/* interrupt level triggered, disabled, priority is the lowest */
		_arc_aux_write(AUX_IRQ_SELECT, i);
		_arc_aux_write(AUX_IRQ_ENABLE, 0);
		_arc_aux_write(AUX_IRQ_TRIGGER, 0);
		_arc_aux_write(AUX_IRQ_PRIORITY, INT_PRI_MAX - INT_PRI_MIN);
	}
	_arc_aux_write(AUX_IRQ_CTRL, *(uint32_t *)&ictrl);

	cpu_unlock_restore(status);

	/** ipm should be set after cpu unlock restore to avoid reset of the status32 value */
	int_ipm_set(INT_PRI_MAX);
}

/**
 * \ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * \brief  install an interrupt handler
 * \param[in] intno	interrupt number
 * \param[in] handler interrupt handler to install
 */
int32_t int_handler_install(const uint32_t intno, INT_HANDLER handler)
{
	/*!< \todo parameter check ? */
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL && handler != NULL) {
		exc_int_handler_table[intno] = handler;
		return 0;
	}

	return -1;
}

/**
 * \ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * \brief  get the installed an interrupt handler
 * \param[in] intno interrupt number
 * \return the installed interrupt handler or NULL
 */
INT_HANDLER int_handler_get(const uint32_t intno)
{
	if (intno >= NUM_EXC_CPU && intno < NUM_EXC_ALL) {
		return exc_int_handler_table[intno];
	}

	return NULL;
}

/**
 * \brief set interrupt priority
 *
 * \param[in] intno interrupt number
 * \param[in] intpri interrupt priority
 */
void int_pri_set(const uint32_t intno, int32_t intpri)
{
	uint32_t status;

	status = cpu_lock_save();
	intpri = intpri - INT_PRI_MIN;
	_arc_aux_write(AUX_IRQ_SELECT, intno);
	_arc_aux_write(AUX_IRQ_PRIORITY, (uint32_t)intpri);

	cpu_unlock_restore(status);
}


/**
 * \brief	convert exception number to name
 * \param	exc_no exception number
 * \return	exception name description
 */
const char* exc_no2string(uint32_t exc_no)
{
	static const char* exception_name_string[NUM_EXC_CPU] = {
		"reset",
		"memory error",
		"instruction error",
		"machine check",
		"instruction tlb miss",
		"data tlb miss",
		"protection violation",
		"privilege violation",
		"swi",
		"trap",
		"extension",
		"divide zero",
		"data cache error"
		"memory misaligned",
		"exception 14",
		"exception 15"
	};

	if (exc_no >= NUM_EXC_CPU) {
		return NULL;
	}

	return exception_name_string[exc_no];
}
