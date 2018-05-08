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
#ifndef _SECURESHIELD_H_
#define _SECURESHIELD_H_

#undef LIB_SECURESHIELD_OVERRIDES
#include "embARC_toolchain.h"
#include "arc.h"
#include "arc_builtin.h"
#include "arc_exception.h"
#include "console_io.h"

#include "embARC_debug.h"
#include "embARC_assert.h"

/* definitions that are made visible externally */
#include "secureshield_exports.h"
#include "secureshield_config.h"

typedef struct {
	uint32_t magic;
	uint32_t *mode;

	/* protected bss */
	uint32_t *bss_start, *bss_end;

	/* secureshield main bss */
	uint32_t *bss_main_start, *bss_main_end;

	/* secure shields's bss */
	uint32_t *bss_containers_start, *bss_containers_end;

	/* secureshield code and data */
	uint32_t *main_start, *main_end;

	/* protected flash memory region */
	uint32_t *secure_start, *secure_end;

	/* container configuration tables */
	uint32_t *cfgtbl_start, *cfgtbl_end;

	/* pointers to container configuration tables */
	uint32_t *cfgtbl_ptr_start, *cfgtbl_ptr_end;

	/* address of __secureshield_container_context */
	uint32_t **container_context;
} EMBARC_PACKED SECURESHIELD_CONFIG;

extern const SECURESHIELD_CONFIG __secureshield_config;

#define SECURESHIELD_DBG DBG


extern void secureshield_init(void);

#endif/*_SECURESHIELD_H_*/
