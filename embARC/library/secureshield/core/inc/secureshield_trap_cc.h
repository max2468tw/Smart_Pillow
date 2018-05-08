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
 * \date 2016-02-23
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */
#ifndef _SECURESHIELD_TRAP_CC_H_
#define _SECURESHIELD_TRAP_CC_H_

/* container call related definitions */

#include "secureshield.h"
#include "secureshield_vmpu.h"
#include "secureshield_trap_exports.h"

#define SECURESHIELD_TRAP_CONTAINER_CALL_OPCODE \
 	(uint16_t) (0x781E + (SECURESHIELD_MONITOR_CALL_CONTAINER_IN << 5))

typedef struct {
	uint16_t opcode;
	uint32_t branch;
	uint32_t magic;
	uint32_t dst_fn;
	uint32_t *cfg_ptr;
} EMBARC_PACKED CONTAINER_CALL;

static inline void trap_cc_check_magic(CONTAINER_CALL *trap_pc)
{
	/* need to do more checks ? */

	if (trap_pc->opcode != SECURESHIELD_TRAP_CONTAINER_CALL_OPCODE) {
		EMBARC_HALT("container call opcode invalid (0x%08X)", &trap_pc->opcode);
	}

	if (trap_pc->magic != SECURESHIELD_CONTAINER_CALL_MAGIC) {
		EMBARC_HALT("container call magic invalid (0x%08X)", &trap_pc->magic);
	}
}

static inline uint32_t trap_cc_get_dst_fn(CONTAINER_CALL *trap_pc)
{
	return trap_pc->dst_fn;
}

static inline uint8_t trap_cc_get_dst_id(CONTAINER_CALL *trap_pc)
{
	uint32_t container = trap_pc->cfg_ptr - __secureshield_config.cfgtbl_ptr_start;

	if (container <= 0 || container >= g_vmpu_container_count) {
		EMBARC_HALT("container out of range (%d)", container);
	}

	return (uint8_t) (container & 0xFF);
}

#endif/* _SECURESHIELD_TRAP_CC_H_ */
