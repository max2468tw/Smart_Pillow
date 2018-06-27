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
#include <string.h>
#include "secureshield.h"
#include "secureshield_vmpu.h"
#include "secureshield_trap.h"
#include "secureshield_int.h"

extern uint32_t *__secureshield_stack;


/* switch the secureshield stack and user stack */
#define unprivilege_to_secureshield() \
({\
	_arc_aux_write(AUX_USER_SP, (uint32_t)&__secureshield_stack); \
	Asm("aex %%sp, [%[user_sp]]" :: [user_sp] "i" (AUX_USER_SP));\
})

#define secureshield_to_unprivilege() \
({\
	 Asm(									\
	 "sr .Lgo_to_unprivilege, [%[eret]]\n"	/* set eret */ 			\
	 "seti\n"			/* unlock cpu */			\
	 "lr %%r0, [%[status]]\n"	/* create a fake exception return */	\
	 "or %%r0, %%r0, 0x80\n"	/* U bit is set */			\
	 "sr %%r0, [%[erstatus]]\n"						\
	 "lr %%r0, [%[bta]]\n"		/* keep the right bta */		\
	 "sr %%r0, [%[erbta]]\n"						\
	 "rtie\n"			/* fake exception return */		\
	 ".Lgo_to_unprivilege:\n"						\
	:: [eret] "i" (AUX_ERRET),						\
	[status] "i" (AUX_STATUS32), [erstatus] "i" (AUX_ERSTATUS),		\
	[bta] "i" (AUX_BTA),[erbta] "i" (AUX_ERBTA)				\
	);									\
})

static void secureshield_init_pre(void)
{
	/* todo: init secureshield's data and memory */
	memset(__secureshield_config.bss_start, 0,
		VMPU_REGION_SIZE(__secureshield_config.bss_start, __secureshield_config.bss_end));
}

static void secureshield_init_post(void)
{
	/* init MPU */
	vmpu_init_post();
}

void secureshield_init(void)
{
	secureshield_init_pre();
	*(__secureshield_config.mode) = SECURESHIELD_DISABLED;
	/* setup xprintf here, need to output log message */
	xprintf_setup();
	/* run basic sanity checks */
	if(vmpu_init_pre() == 0) {
		unprivilege_to_secureshield();
		/* secureshield interrupt init */
		secureshield_int_init();
		/* finish initialization */
		secureshield_init_post();
		*(__secureshield_config.mode) = SECURESHIELD_ENABLED;
		secureshield_to_unprivilege();
    	}
}