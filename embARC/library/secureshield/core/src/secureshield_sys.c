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
#include "secureshield_sys.h"


#ifndef CONTAINER_AUX_COUNT
#define CONTAINER_AUX_COUNT		8
#endif

typedef struct {
	uint32_t start;
	uint32_t end;
	uint32_t size;
} CONTAINER_AUX;

typedef struct {
	CONTAINER_AUX *aux;
	uint32_t count;
} AUX_CONTAINER;


static uint32_t g_aux_count;
static CONTAINER_AUX g_aux_list[CONTAINER_AUX_COUNT];
static AUX_CONTAINER g_aux_container[SECURESHIELD_MAX_CONTAINERS];


static void _secureshield_aux_check(uint8_t container_id, uint32_t addr)
{
	uint32_t i;
	AUX_CONTAINER *container;
	CONTAINER_AUX *aux;

	if(container_id >= SECURESHIELD_MAX_CONTAINERS) {
		EMBARC_HALT("container_id out of range");
		return;
	}

	container = &g_aux_container[container_id];

	aux = &container->aux[0];

	for (i = 0; i < container->count; i++) {
		if (addr >= aux->start && addr < aux->end)
			return;
		aux++;
	}

	/* if aux is not found, try container 0 which is background container */
	container = &g_aux_container[0];
	aux = &container->aux[0];

	if (!container->aux) {
		EMBARC_HALT("no aux region is allocated for container 0 and %d", container_id);
		return;
	}

	for (i = 0; i < container->count; i++) {
		if (addr >= aux->start && addr < aux->end)
			return;
		aux++;
	}

	EMBARC_HALT("the aux regs:0x%x is not allocated to container 0 and %d", addr, container_id);
	return ;
}

uint32_t _secureshield_arc_lr_reg(uint32_t addr)
{
	_secureshield_aux_check(g_active_container, addr);
	return _arc_lr_reg(addr);

}

void _secureshield_arc_sr_reg(uint32_t addr, uint32_t val)
{
	_secureshield_aux_check(g_active_container, addr);
	_arc_sr_reg(addr, val);
}

int32_t vmpu_ac_aux(uint8_t container_id, uint32_t start, uint32_t size)
{
	AUX_CONTAINER *container;
	CONTAINER_AUX *aux;

	if(g_aux_count >= CONTAINER_AUX_COUNT) {
		EMBARC_HALT("no more place for interface");
		return -1;
	}

	if(container_id >= SECURESHIELD_MAX_CONTAINERS) {
		EMBARC_HALT("container_id out of range");
		return -1;
	}

	SECURESHIELD_DBG("container %d has aux regs area from @0x%x with size %d\r\n",
		container_id, start, size);

	/* assign aux container pointer */
	container = &g_aux_container[container_id];

	if(!container->aux)
		container->aux = &g_aux_list[g_aux_count];

	/* allocate new aux region */
	aux = &container->aux[container->count];

	aux->start = start;
	aux->size = size;
	aux->end = start + size;

	/* take account for new aux region */
	container->count++;
	g_aux_count++;

	return 0;
}

static uint32_t g_perf_overhead;

void _secureshield_perf_config(uint32_t overhead)
{
	if (overhead == 0) {
		// enable timer1 to measure performance
		_arc_sr_reg(AUX_TIMER1_CTRL, 0);
		_arc_sr_reg(AUX_TIMER1_LIMIT, 0xFFFFFFFF);
		_arc_sr_reg(AUX_TIMER1_CTRL, 0x2);
	} else {
		g_perf_overhead = overhead;
	}
}

void _secureshield_perf_start(void)
{
	_arc_sr_reg(AUX_TIMER1_CNT, 0);
}


uint32_t _secureshield_perf_end(void)
{
	return _arc_lr_reg(AUX_TIMER1_CNT) - g_perf_overhead;
}

int32_t _secureshield_container_id_self(void)
{
	return g_active_container;
}

int32_t _secureshield_container_id_caller(void)
{
	if (g_container_stack_ptr < 1) {
		/* no container call in stack */
		return -1;
	}

	/* how about the case of container switch caused by IRQ */

	return g_container_stack[g_container_stack_ptr - 1].src_id;
}

void secureshield_sys_ops(INT_EXC_FRAME *frame)
{
	/* FIXME: remove switch case structure, use jump table */
	/* r0 is used as operation id */
	switch (frame->r0) {
		case SECURESHIELD_SYS_LR:
			SECURESHIELD_DBG("read aux reg 0x%x\r\n", frame->r1);
			frame->r0 = _secureshield_arc_lr_reg(frame->r1);
			break;
		case SECURESHIELD_SYS_SR:
			SECURESHIELD_DBG("write aux reg 0x%x\r\n", frame->r1);
			_secureshield_arc_sr_reg(frame->r1, frame->r2);
			break;
		case SECURESHIELD_SYS_PERF_CFG:
			_secureshield_perf_config(frame->r1);
			break;
		case SECURESHIELD_SYS_PERF_START:
			_secureshield_perf_start();
			break;
		case SECURESHIELD_SYS_PERF_END:
			frame->r0 = _secureshield_perf_end();
			break;
		case SECURESHIELD_SYS_CONTAINER_ID_SELF:
			frame->r0 = _secureshield_container_id_self();
			break;
		case SECURESHIELD_SYS_CONTAINER_ID_CALLER:
			frame->r0 = _secureshield_container_id_caller();
			break;
		default:
			SECURESHIELD_DBG("Unsupported sys level operation:%d\r\n", frame->r0);
			break;
	}
}

