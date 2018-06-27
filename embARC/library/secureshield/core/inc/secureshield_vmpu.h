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
#ifndef _SECURESHIELD_VMPU_H_
#define _SECURESHIELD_VMPU_H_

#include "secureshield_vmpu_exports.h"

#define VMPU_ROM_ADDR_MASK	(~(((uint32_t)(ROM_LENGTH)) - 1))
#define VMPU_ROM_ADDR(addr)	((VMPU_ROM_ADDR_MASK & (uint32_t)(addr)) == \
				(ROM_ORIGIN & VMPU_ROM_ADDR_MASK))

#define VMPU_SECURESHIELD_ROM_ADDR_MASK		(~(((uint32_t)(SECURESHIELD_ROM_LENGTH)) - 1))
#define VMPU_SECURESHIELD_ROM_ADDR(addr)	((VMPU_SECURESHIELD_ROM_ADDR_MASK & (uint32_t)(addr)) == \
			(SECURESHIELD_ROM_ORIGIN & VMPU_SECURESHIELD_ROM_ADDR_MASK))


#define VMPU_REGION_SIZE(p1, p2)	((p1 >= p2) ? 0 : \
					((uint32_t) (p2) - (uint32_t) (p1)))

/* add a memory resource to container */
extern void vmpu_ac_mem(uint8_t container_id, void *addr, uint32_t size, CONTAINER_AC ac);
/* add an irq resource to container */
extern void vmpu_ac_irq(uint8_t container_id, void *function, uint32_t isr_id);
/* add a peripheral resource to container */
extern int32_t vmpu_ac_peripheral(CONTAINER_AC ac, uint16_t periperal_id);
/* add a interface to container */
extern int32_t vmpu_ac_interface(uint8_t container_id, void *function, uint32_t args_num);
/* find the interface in the container */
extern void * vmpu_find_interface(uint8_t container_id, void *function, uint32_t args_num);
/* add a aux resource to container */
extern int32_t vmpu_ac_aux(uint8_t container_id, uint32_t start, uint32_t size);
/* add stack and context to a container */
extern void vmpu_ac_stack(uint8_t container_id, uint32_t context_size, uint32_t stack_size, uint32_t container_type);
/* add a static region which does not belong to any container */
extern uint32_t vmpu_ac_static_region(uint8_t region, void* base, uint32_t size, CONTAINER_AC ac);
/* switch a container */
extern void vmpu_switch(uint8_t src_id, uint8_t dst_id);
/* load a container */
extern void vmpu_load_container(uint8_t container_id);
/* according to the fault_addr and size, find the correct access control */
extern uint32_t vmpu_fault_find_ac(uint32_t fault_addr, uint32_t size);
/* try to recover from an exception raiser by MPU, i.e. , MPU walking */
extern int32_t vmpu_fault_recovery_mpu(uint32_t pc, uint32_t *sp, uint32_t type);
/* hardware dependent init */
extern void vmpu_arch_init(void);
/* pre-init of vmpu */
extern int32_t vmpu_init_pre(void);
/* post-init of vmpu */
extern void vmpu_init_post(void);

extern uint32_t g_vmpu_container_count;
extern uint8_t g_active_container;

#endif/*_SECURESHIELD_VMPU_H_*/
