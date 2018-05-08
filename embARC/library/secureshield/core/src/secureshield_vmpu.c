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
//#define DEBUG
//#define DEBUG_HOSTLINK 	/* use hostlink to output debug message */
#include "secureshield.h"
#include "secureshield_vmpu.h"
#include "secureshield_trap.h"

uint32_t g_vmpu_container_count;
uint8_t g_active_container;

static int32_t vmpu_sanity_checks(void)
{
	/* verify secureshield config structure */
	if (__secureshield_config.magic != SECURESHIELD_MAGIC) {
		EMBARC_HALT("config magic mismatch: &0x%08X = 0x%08X - expected 0x%08X"
			,&__secureshield_config, __secureshield_config.magic,
			SECURESHIELD_MAGIC);
		return -1;
	}

	/* verify basic assumptions about vmpu_bits/__builtin_clz */
	EMBARC_ASSERT(__builtin_clz(0) == 32);
	EMBARC_ASSERT(__builtin_clz(1UL << 31) == 0);
	EMBARC_ASSERT(vmpu_bits(0) == 0);
	EMBARC_ASSERT(vmpu_bits(1UL << 31) == 32);
	EMBARC_ASSERT(vmpu_bits(0x8000UL) == 16);
	EMBARC_ASSERT(vmpu_bits(0x8001UL) == 16);
	EMBARC_ASSERT(vmpu_bits(1) == 1);

	/* verify that __secureshield_config is within valid ROM for secureshield */
	EMBARC_ASSERT(((uint32_t)&__secureshield_config) >= SECURESHIELD_ROM_ORIGIN);

	EMBARC_ASSERT(((((uint32_t)&__secureshield_config) + sizeof(__secureshield_config))
			 <= (SECURESHIELD_ROM_ORIGIN + SECURESHIELD_ROM_LENGTH)));

	/* verify RAM relocation */
	SECURESHIELD_DBG("secureshield_ram : @0x%08X (%d bytes)\r\n",
		(uint32_t)__secureshield_config.bss_main_start,
	 	VMPU_REGION_SIZE(__secureshield_config.bss_main_start,
	 	__secureshield_config.bss_main_end));

	EMBARC_ASSERT(__secureshield_config.bss_main_end > __secureshield_config.bss_main_start);
	// EMBARC_ASSERT(VMPU_REGION_SIZE(__secureshield_config.bss_main_start,
	// 		__secureshield_config.bss_main_end) == SECURESHIELD_RAM_SIZE);


	/* verify that secure rom area is accessible and after public code */
	EMBARC_ASSERT(__secureshield_config.secure_start <= __secureshield_config.secure_end );
	EMBARC_ASSERT((uint32_t)__secureshield_config.secure_end <=
	 		(uint32_t)(SECURESHIELD_ROM_ORIGIN + SECURESHIELD_ROM_LENGTH));
	EMBARC_ASSERT((uint32_t)__secureshield_config.secure_start >=
	 	(uint32_t)&vmpu_sanity_checks );

	/* verify configuration table */
	EMBARC_ASSERT(__secureshield_config.cfgtbl_ptr_start <=
 		__secureshield_config.cfgtbl_ptr_end);
	EMBARC_ASSERT(__secureshield_config.cfgtbl_ptr_start >=
 		__secureshield_config.secure_start);
	EMBARC_ASSERT((uint32_t)__secureshield_config.cfgtbl_ptr_end <=
	 	(uint32_t)(SECURESHIELD_ROM_ORIGIN + SECURESHIELD_ROM_LENGTH));

	/* return error if secureshield is enabled in umonitor */
	if (!__secureshield_config.mode || (*__secureshield_config.mode == SECURESHIELD_ENABLED)) {
		return -1;
	}
	else {
		return 0;
	}
}

static void vmpu_setup_containers(void)
{
	uint32_t i, count;
	const CONTAINER_AC_ITEM *region;
	const CONTAINER_CONFIG **container_cfgtbl;
	uint8_t container_id;

	/* enumerate and initialize containers */
	g_vmpu_container_count = 0;
	for (container_cfgtbl = (const CONTAINER_CONFIG**) __secureshield_config.cfgtbl_ptr_start;
		container_cfgtbl < (const CONTAINER_CONFIG**) __secureshield_config.cfgtbl_ptr_end;
		container_cfgtbl++) {
		/* ensure that configuration resides in secureshield rom */
		if (!(VMPU_SECURESHIELD_ROM_ADDR(*container_cfgtbl) &&
			VMPU_SECURESHIELD_ROM_ADDR(
				((uint8_t*)(*container_cfgtbl)) + (sizeof(**container_cfgtbl)-1)
			))) {
			EMBARC_HALT("invalid address - *container_cfgtbl must point to rom (0x%08X)"
				, *container_cfgtbl);
			return;
		}

		/* check for magic value in container configuration */
		if (((*container_cfgtbl)->magic) != SECURESHIELD_CONTAINER_MAGIC) {
			EMBARC_HALT("container[%d] @0x%08X - invalid magic",
				g_vmpu_container_count,
				(uint32_t)(*container_cfgtbl));
			return;
		}

		/* increment container counter */
		if ((container_id = g_vmpu_container_count++) >= SECURESHIELD_MAX_CONTAINERS) {
			EMBARC_HALT("container number overflow");
			return;
		}

		/* load shield ACLs in table */
		SECURESHIELD_DBG("container[%d] AC table:\r\n", container_id);

		/* add AC for all container stacks, the actual start addresses and
		 * sizes are resolved later in vmpu_ac_stack */
		/* to add container's segments or sections ?*/
		vmpu_ac_stack(container_id, (*container_cfgtbl)->context_size, (*container_cfgtbl)->stack_size,
			(*container_cfgtbl)->type);

		/* enumerate container ACs */
		if ((region = (*container_cfgtbl)->ac_table)!=NULL) {
			count = (*container_cfgtbl)->ac_count;
			for(i = 0; i < count; i++) {
				/* \todo  check the region ? */
				/* \todo  support for system container */
				/* add AC, and force entry as user-provided */
				/* \todo: add more AC type support */
				if (region->ac & SECURESHIELD_AC_IRQ) {
					vmpu_ac_irq(container_id, region->param1, region->param2);
				} else if (region->ac & SECURESHIELD_AC_INTERFACE) {
					vmpu_ac_interface(container_id, region->param1, region->param2);
				} else if (region->ac & SECURESHIELD_AC_AUX) {
					vmpu_ac_aux(container_id, (uint32_t)region->param1, region->param2);
				} else { /* memory space based ACs */
					vmpu_ac_mem(container_id, region->param1,region->param2,
						region->ac);
				}
				/* proceed to next AC */
				region++;
			}
		}
	}

	SECURESHIELD_DBG("containers setup\r\n");
}

int32_t vmpu_init_pre(void)
{
	return vmpu_sanity_checks();
}

void vmpu_init_post(void)
{
	/* init memory protection */
	vmpu_arch_init();

	/* setup containers */
	vmpu_setup_containers();

	/* load background container 0 */
	vmpu_load_container(0);
}
