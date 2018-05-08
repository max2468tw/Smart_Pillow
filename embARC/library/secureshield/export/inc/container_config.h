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

#ifndef _CONTAINER_CONFIG_H_
#define _CONTAINER_CONFIG_H_

#define SECURESHIELD_SET_MODE(mode) \
	SECURESHIELD_SET_MODE_AC_COUNT(mode, NULL, 0)

#define SECURESHIELD_SET_MODE_AC(mode, ac_table) \
	SECURESHIELD_SET_MODE_AC_COUNT(mode, ac_table, EMBARC_ARRAY_COUNT(ac_table))

#define SECURESHIELD_SET_MODE_AC_COUNT(mode, ac_table, ac_table_count) \
	uint8_t __attribute__((section(".keep.secureshield.bss.containers"), aligned(2048))) __reserved_stack[SECURESHIELD_STACK_BAND_SIZE]; \
	\
	uint32_t __secureshield_mode = (mode); \
	\
	static const __attribute__((section(".keep.secureshield.cfgtbl"), aligned(4))) CONTAINER_CONFIG main_cfg = { \
		SECURESHIELD_CONTAINER_MAGIC, \
		SECURESHILED_CONTAINER_NSECURE, \
		0, \
		0, \
		ac_table, \
		ac_table_count \
	}; \
	\
	const __attribute__((section(".keep.secureshield.cfgtbl_ptr_first"), aligned(4))) void * const main_cfg_ptr = &main_cfg;

/* this macro selects an overloaded macro (variable number of arguments) */
#define __SECURESHIELD_CONTAINER_MACRO(_1, _2, _3, _4, _5, NAME, ...) NAME

#define __SECURESHIELD_CONTAINER_CONFIG(container_name, ac_table, stack_size, context_size, container_type) \
	\
	uint8_t __attribute__((section(".keep.secureshield.bss.containers"), aligned(2048))) \
		container_name ## _reserved[SECURESHIELD_STACK_SIZE_ROUND(((SECURESHIELD_MIN_STACK(stack_size) + (context_size))*8)/6)]; \
	\
	static const __attribute__((section(".keep.secureshield.cfgtbl"), aligned(4))) CONTAINER_CONFIG container_name ## _cfg = { \
		SECURESHIELD_CONTAINER_MAGIC, \
		container_type, \
		SECURESHIELD_MIN_STACK(stack_size), \
		context_size, \
		ac_table, \
		EMBARC_ARRAY_COUNT(ac_table) \
	}; \
	\
	const __attribute__((section(".keep.secureshield.cfgtbl_ptr"), aligned(4))) void * const container_name ## _cfg_ptr = &container_name ## _cfg;


#define __SECURESHIELD_CONTAINER_CONFIG_NOCONTEXT(container_name, ac_table, stack_size) \
	__SECURESHIELD_CONTAINER_CONFIG(container_name, ac_table, stack_size, 0,	\
		SECURESHILED_CONTAINER_NSECURE)


#define __SECURESHIELD_CONTAINER_CONFIG_CONTEXT(container_name, ac_table, stack_size, context_type) \
	__SECURESHIELD_CONTAINER_CONFIG(container_name, ac_table, stack_size, sizeof(context_type), \
		SECURESHILED_CONTAINER_NSECURE)

#define __SECURESHIELD_CONTIANER_CONFIG_TYPE(container_name, ac_table, stack_size, context_type, container_type) \
	__SECURESHIELD_CONTAINER_CONFIG(container_name, ac_table, stack_size, sizeof(context_type), \
		container_type)


#define SECURESHIELD_CONTAINER_CONFIG(...) \
	__SECURESHIELD_CONTAINER_MACRO(__VA_ARGS__, __SECURESHIELD_CONTIANER_CONFIG_TYPE, \
		__SECURESHIELD_CONTAINER_CONFIG_CONTEXT, \
		__SECURESHIELD_CONTAINER_CONFIG_NOCONTEXT)(__VA_ARGS__)

#define SECURE_TEXT	__attribute__ ((section(".secureshield.secure_container.text")))
#define SECURE_RODATA	__attribute__ ((section(".secureshield.secure_container.rodata")))
#define SECURE_DATA 	__attribute__ ((section(".secureshield.secure_container.data")))
#define SECURE_BSS 	__attribute__ ((section(".secureshield.secure_container.bss")))

extern uint32_t __secureshield_mode;
extern void * const secureshield_container_ctx;

#endif /* __CONTAINER_CONFIG_H__ */
