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
//#define DEBUG_HOSTLINK
#include "secureshield.h"
#include "secureshield_vmpu.h"
#include "secureshield_trap.h"

/* MPU region count */
#ifndef MPU_REGION_COUNT
#define MPU_REGION_COUNT 32
#endif/*MPU_REGION_COUNT*/

/* set default MPU region count */
#ifndef ARC_FEATURE_MPU_REGIONS
#define ARC_FEATURE_MPU_REGIONS 8
#endif/*ARC_FEATURE_MPU_REGIONS*/

/* reserve 2 regions, one reigon to protect code and data in flash can not be written,
 * one region to protect secureshield's data, bss, stack, container's stack and context.
 */
#define ARC_MPU_RESERVED_REGIONS 2

/* set default minimum region address alignment */
#ifndef ARC_FEATURE_MPU_ALIGNMENT_BITS
#define ARC_FEATURE_MPU_ALIGNMENT_BITS 11
#endif/*ARC_FEATURE_MPU_ALIGNMENT_BITS*/

/* derived region alignment settings */
#define ARC_FEATURE_MPU_ALIGNMENT (1UL<<ARC_FEATURE_MPU_ALIGNMENT_BITS)
#define ARC_FEATURE_MPU_ALIGNMENT_MASK (ARC_FEATURE_MPU_ALIGNMENT-1)


#ifndef CONTAINER_INTERFACE_COUNT
#define CONTAINER_INTERFACE_COUNT	8
#endif

#define AUX_MPU_RDB_VALID_MASK 0x1
#define AUX_MPU_EN_ENABLE   0x40000000
#define AUX_MPU_EN_DISABLE  0x0

#define AUX_MPU_RDP_UE  0x008    /* allow user execution */
#define AUX_MPU_RDP_UW  0x010    /* allow user write */
#define AUX_MPU_RDP_UR  0x020    /* allow user read */
#define AUX_MPU_RDP_KE  0x040    /* only allow kernel execution */
#define AUX_MPU_RDP_KW  0x080    /* only allow kernel write */
#define AUX_MPU_RDP_KR  0x100    /* only allow kernel read */

#define AUX_MPU_RDP_REGION_2K       0x402
#define AUX_MPU_RDP_REGION_4K       0x403
#define AUX_MPU_RDP_REGION_8K       0x600
#define AUX_MPU_RDP_REGION_16K      0x601
#define AUX_MPU_RDP_REGION_32K      0x602
#define AUX_MPU_RDP_REGION_64K      0x603
#define AUX_MPU_RDP_REGION_128K     0x800
#define AUX_MPU_RDP_REGION_256K     0x801
#define AUX_MPU_RDP_REGION_512K     0x802
#define AUX_MPU_RDP_REGION_1M       0x803
#define AUX_MPU_RDP_REGION_2M       0xA00
#define AUX_MPU_RDP_REGION_4M       0xA01
#define AUX_MPU_RDP_REGION_8M       0xA02
#define AUX_MPU_RDP_REGION_16M      0xA03
#define AUX_MPU_RDP_REGION_32M      0xC00
#define AUX_MPU_RDP_REGION_64M      0xC01
#define AUX_MPU_RDP_REGION_128M     0xC02
#define AUX_MPU_RDP_REGION_256M     0xC03
#define AUX_MPU_RDP_REGION_512M     0xE00
#define AUX_MPU_RDP_REGION_1G       0xE01
#define AUX_MPU_RDP_REGION_2G       0xE02
#define AUX_MPU_RDP_REGION_4G       0xE03

/* enable MPU, default mode, user read/write/execute/ are  allowed */
#define MPU_DEFAULT_MODE  (AUX_MPU_EN_ENABLE | AUX_MPU_RDP_KE | AUX_MPU_RDP_KR | AUX_MPU_RDP_KW | AUX_MPU_RDP_UE)


#define SECURE_CONTAINER_DEFAULT_STATUS	(AUX_STATUS_IE_MASK | (((INT_PRI_MAX - INT_PRI_MIN) << 1) & 0x1e))
#define NSECURE_CONTAINER_DEFAULT_STATUS	(SECURE_CONTAINER_DEFAULT_STATUS | AUX_STATUS_U_MASK)


typedef struct {
	uint32_t base;
	uint32_t end;
	uint32_t rdp;
	uint32_t size;
	uint32_t ac;
} MPU_REGION;

typedef struct {
	MPU_REGION *region;
	uint32_t type;
	uint32_t count;
} MPU_CONTAINER;


typedef struct {
	void * func;
	uint32_t args_num;
} CONTAINER_INTERFACE;


typedef struct {
	CONTAINER_INTERFACE *interface;
	uint32_t count;
} INTERFACE_CONTAINER;

static uint32_t g_mpu_slot;
static uint32_t g_mpu_region_count, g_container_mem_pos;
static MPU_REGION g_mpu_list[MPU_REGION_COUNT];
static MPU_CONTAINER g_mpu_container[SECURESHIELD_MAX_CONTAINERS];

static uint32_t g_interface_count;
static CONTAINER_INTERFACE g_interface_list[CONTAINER_INTERFACE_COUNT];
static INTERFACE_CONTAINER g_interface_container[SECURESHIELD_MAX_CONTAINERS];


static uint32_t g_vmpu_aligment_mask;

static const MPU_REGION* vmpu_fault_find_container_region(uint32_t fault_addr, const MPU_CONTAINER *container)
{
	uint32_t count;
	const MPU_REGION *region;

	count = container->count;
	region = container->region;
	while (count-- > 0) {
		if ((fault_addr >= region->base) && (fault_addr < region->end)) {
			return region;
		}
		else {
			region++;
		}
	}

	return NULL;
}

static const MPU_REGION* vmpu_fault_find_region(uint32_t fault_addr)
{
	const MPU_REGION *region;

	/* check fault_addr whether in current container  */
	if ((g_active_container) && ((region = vmpu_fault_find_container_region(
		fault_addr, &g_mpu_container[g_active_container])) == NULL)) {
		return NULL;
	}

	/* check base container 0 */
	if ((region = vmpu_fault_find_container_region(fault_addr, &g_mpu_container[0])) == NULL) {
		return NULL;
	}

	return region;
}

static int32_t vmpu_region_bits(uint32_t size)
{
	int32_t bits;

	bits = vmpu_bits(size)-1;

	/* minimum region size is 2048 bytes */
	if (bits < ARC_FEATURE_MPU_ALIGNMENT_BITS) {
		bits = ARC_FEATURE_MPU_ALIGNMENT_BITS;
	}

	/* round up if needed */
	if ((1UL << bits) < size) {
		bits++;
	}

	return bits;
}

static uint32_t vmpu_map_ac(CONTAINER_AC ac)
{
	uint32_t flags;
	CONTAINER_AC ac_res;

	/* map generic ACL's to internal ACL's */
	if (ac & SECURESHIELD_AC_UWRITE) {
		ac_res = SECURESHIELD_AC_UREAD | SECURESHIELD_AC_UWRITE |
			  SECURESHIELD_AC_KREAD | SECURESHIELD_AC_KWRITE;
		flags = AUX_MPU_RDP_UR | AUX_MPU_RDP_UW |
			AUX_MPU_RDP_KR | AUX_MPU_RDP_KW;
	}
	else {
		if (ac & SECURESHIELD_AC_UREAD) {
			if (ac & SECURESHIELD_AC_KWRITE) {
				ac_res = SECURESHIELD_AC_UREAD |
					 SECURESHIELD_AC_KREAD | SECURESHIELD_AC_KWRITE;
				flags = AUX_MPU_RDP_UR |
					AUX_MPU_RDP_KR | AUX_MPU_RDP_KW;
			}
			else {
				ac_res = SECURESHIELD_AC_UREAD |
					 SECURESHIELD_AC_KREAD;
				flags = AUX_MPU_RDP_UR |
					AUX_MPU_RDP_KR;
			}
		}
		else {
			if (ac & SECURESHIELD_AC_KWRITE) {
				ac_res = SECURESHIELD_AC_KREAD | SECURESHIELD_AC_KWRITE;
				flags = AUX_MPU_RDP_KR | AUX_MPU_RDP_KW;
			}
			else {
				if (ac & SECURESHIELD_AC_KREAD) {
					ac_res = SECURESHIELD_AC_KREAD;
					flags = AUX_MPU_RDP_KR;
				}
				else {
					ac_res = 0;
					flags = 0;
				}
			}
		}
	}

	if (ac & SECURESHIELD_AC_UEXECUTE) {
		ac_res |= SECURESHIELD_AC_UEXECUTE;
		flags |= AUX_MPU_RDP_UE;
	}

	if (ac & SECURESHIELD_AC_KEXECUTE) {
		ac_res |= SECURESHIELD_AC_KEXECUTE;
		flags |= AUX_MPU_RDP_KE;
	}

	/* check if we meet the expected ACL's */
	if ((ac_res) != (ac & SECURESHIELD_AC_ACCESS)) {
		EMBARC_HALT("inferred ACL's (0x%04X) don't match expected ACL's (0x%04X)",
		ac_res, (ac & SECURESHIELD_AC_ACCESS));
	}

	return flags;
}

static void vmpu_ac_update_container_region(MPU_REGION *region, uint8_t container_id,
	void* base, uint32_t size, CONTAINER_AC ac)
{
	uint32_t flags, bits, mask, size_rounded;

	SECURESHIELD_DBG("\tcontainer[%d] ac[%d]={0x%x,size=%d,ac=0x%x,",
		container_id, g_mpu_region_count, (uint32_t)base, size, ac);

	/* verify region alignment */
	bits = vmpu_region_bits(size);
	size_rounded = 1UL << bits;
	if (size_rounded != size) {
		if((ac & (SECURESHIELD_AC_SIZE_ROUND_UP|SECURESHIELD_AC_SIZE_ROUND_DOWN))==0) {
			EMBARC_HALT(
				"container size (%d) not rounded, rounding disabled (rounded=%d)",
				size, size_rounded
			);
			return;
		}

		if(ac & SECURESHIELD_AC_SIZE_ROUND_DOWN) {
			bits--;
			if(bits<ARC_FEATURE_MPU_ALIGNMENT_BITS) {
				EMBARC_HALT("region size (%d) can't be rounded down",size);
				return;
			}
			size_rounded = 1UL << bits;
		}
	}

	/* check for correctly aligned base address */
	mask = size_rounded - 1;
	if (((uint32_t)base) & mask) {
		EMBARC_HALT("base address 0x%x and size (%d) are inconsistent", base, size);
		return;
	}

	/* map generic ACL's to internal ACL's */
	flags = vmpu_map_ac(ac);

	bits += 0xA - ARC_FEATURE_MPU_ALIGNMENT_BITS;
	/* enable region & add size */
	region->rdp = flags | ((bits & 0x3) | ((bits & 0x1C) << 7));
	region->base = (uint32_t) base;
	region->end = (uint32_t) base + size_rounded;
	region->size = size_rounded;
	region->ac = ac;

	SECURESHIELD_DBG("rounded=%d}\n\r", size_rounded);

}


uint32_t vmpu_fault_find_ac(uint32_t fault_addr, uint32_t size)
{
	return 0;
}

/* FIXME: added very simple MPU region switching - optimize! */
void vmpu_switch(uint8_t src_id, uint8_t dst_id)
{
	int32_t i, mpu_slot;
	const MPU_CONTAINER *container;
	const MPU_REGION *region;

	if (!g_mpu_region_count || dst_id >= SECURESHIELD_MAX_CONTAINERS) {
		EMBARC_HALT("dst container out of range (%d)", dst_id);
		return;
	}

	//SECURESHIELD_DBG("switching from %d to %d\n\r", src_id, dst_id);
	/* remember active container */
	g_active_container = dst_id;

	/* update target container first to make target stack available */
	mpu_slot = ARC_MPU_RESERVED_REGIONS;

	/* according to ARCv2 ISA, to update MPU register, disable MPU first */
	_arc_aux_write(AUX_MPU_EN, 0);

	if (dst_id) {
		/* handle target container next */
		container = &g_mpu_container[dst_id];
		region = container->region;

		for (i = 0; i < container->count; i++)
		{
			if (mpu_slot >= ARC_FEATURE_MPU_REGIONS)
				 break;
			 /* RDP must be set before RDB */
			_arc_aux_write(AUX_MPU_RDP0 + mpu_slot * 2, region->rdp);
			_arc_aux_write(AUX_MPU_RDB0 + mpu_slot * 2,
				region->base | AUX_MPU_RDB_VALID_MASK);

			region++;
			mpu_slot++;
		}
	}

	/* handle main container at last */
	container = g_mpu_container;
	region = container->region;

	for (i = 0; i < container->count; i++) {
		if (mpu_slot >= ARC_FEATURE_MPU_REGIONS)
			 break;

		_arc_aux_write(AUX_MPU_RDP0 + mpu_slot * 2, region->rdp);
		_arc_aux_write(AUX_MPU_RDB0 + mpu_slot *2,
			region->base | AUX_MPU_RDB_VALID_MASK);

		/* process next slot */
		region++;
		mpu_slot++;
	}

	/* clear remaining slots */
	while (mpu_slot < ARC_FEATURE_MPU_REGIONS) {
		_arc_aux_write(AUX_MPU_RDP0 + mpu_slot * 2, 0);
		_arc_aux_write(AUX_MPU_RDB0 + mpu_slot * 2, 0);
		mpu_slot++;
	}

	/* re-enable MPU */
	_arc_aux_write(AUX_MPU_EN, MPU_DEFAULT_MODE);
}

void vmpu_load_container(uint8_t container_id)
{
	if (container_id == 0) {
		vmpu_switch(0, 0);
	}
	else {
		EMBARC_HALT("currently only container 0 can be loaded");
	}
	/*  container 0 is the background container and shared to other containers, must be loaded */
}

uint32_t vmpu_ac_static_region(uint8_t region, void* base, uint32_t size, CONTAINER_AC ac)
{
	MPU_REGION res;

	/* apply access control */
	vmpu_ac_update_container_region(&res, 0, base, size, ac);

  	/* according to ARCv2 ISA, to update MPU register, disable MPU first */
	//_arc_aux_write(AUX_MPU_EN, 0);

	_arc_aux_write(AUX_MPU_RDP0+region*2, res.rdp);
	_arc_aux_write(AUX_MPU_RDB0+region*2, res.base | AUX_MPU_RDB_VALID_MASK);

	/* re-enable MPU */
	//_arc_aux_write(AUX_MPU_EN, MPU_DEFAULT_MODE);

	return res.size;
}

void vmpu_ac_mem(uint8_t container_id, void* addr, uint32_t size, CONTAINER_AC ac)
{
	MPU_CONTAINER *container;
	MPU_REGION *region;

	if (g_mpu_region_count >= MPU_REGION_COUNT) {
		EMBARC_HALT("vmpu_ac_mem ran out of regions");
		return ;
	}

	/* assign container region pointer */
	container = &g_mpu_container[container_id];
	if (!container->region)
		container->region = &g_mpu_list[g_mpu_region_count];

	/* allocate new MPU region */
	region = &container->region[container->count];
	if (region->rdp) {
		EMBARC_HALT("unordered region allocation");
		return;
	}

	/* calculate MPU RASR/BASR registers */
	vmpu_ac_update_container_region(region, container_id, addr, size, ac);

	/* take account for new region */
	container->count++;
	g_mpu_region_count++;
}

void vmpu_ac_stack(uint8_t container_id, uint32_t context_size, uint32_t stack_size, uint32_t type)
{
/* note: for ARCv2, minimum region size is 2K bytes, region address must be
aligned to 2K bytes */
	int32_t bits;
	uint32_t size;


	if (type == SECURESHIELD_CONTAINER_SECURE && container_id != 0) {
		g_container_context[container_id].cpu_status = SECURE_CONTAINER_DEFAULT_STATUS;
	} else if (type == SECURESHILED_CONTAINER_NSECURE) {
		g_container_context[container_id].cpu_status = NSECURE_CONTAINER_DEFAULT_STATUS;
	} else {
		EMBARC_HALT("unsupported container type");
		return;
	}

	/* handle main container */
	if (!container_id) {
		/* container 0 is main container or background container, it uses the default stack
		and has no context */

		EMBARC_ASSERT(context_size == 0);
		EMBARC_ASSERT(stack_size == 0);

		/* assign main container stack pointer to existing
		 * unprivileged stack pointer */
		/* FIXME: AUX_USER_SP may be not correct */
		g_container_context[container_id].cur_sp = (uint32_t *)_arc_aux_read(AUX_USER_SP);
		g_container_context[container_id].context_ptr = NULL;
		return;
	}

	/* ensure that container stack is at least SECURESHIELD_MIN_STACK_SIZE */
	stack_size = SECURESHIELD_MIN_STACK(stack_size);

	/* ensure that 2/8th are available for protecting stack from
	 * context - include rounding error margin */
	bits = vmpu_region_bits(((stack_size + context_size)*8)/6);

	size = 1UL << bits;

	SECURESHIELD_DBG("\tcontainer[%d] stack=%d context=%d rounded=%d\n\r" , container_id,
		stack_size, context_size, size);

	/* check for correct context address alignment:
	 * alignment needs to be a multiple of the size */
	if ((g_container_mem_pos & (size-1)) != 0 )
		g_container_mem_pos = (g_container_mem_pos & ~(size-1)) + size;

	/* check if umonitor has enough memory left for stack and context */
	if ((g_container_mem_pos + size) > ((uint32_t)__secureshield_config.bss_containers_end)) {
		EMBARC_HALT("memory overflow - increase secureshield memory allocation");
		return;
	}

	/* allocate context pointer */
	g_container_context[container_id].context_ptr = (uint32_t*)g_container_mem_pos;
	/* ensure stack band on top for stack underflow detection */
	g_container_context[container_id].cur_sp = (uint32_t *)(g_container_mem_pos + size - SECURESHIELD_STACK_BAND_SIZE);

	/* create stack protection region */
	vmpu_ac_mem(container_id, (void*)g_container_mem_pos, size, SECURESHIELD_ACDEF_USTACK);

	/* move on to the next memory block */
	g_container_mem_pos += size;
}

int32_t vmpu_fault_recovery_mpu(uint32_t pc, uint32_t *sp, uint32_t type)
{
	const MPU_REGION *region;

	uint32_t fault_addr = _arc_aux_read(AUX_EFA);

	/* MPU region walk like MMU region walk */
	if ((region = vmpu_fault_find_region(fault_addr)) == NULL) {
		SECURESHIELD_DBG("the region for 0x%08X is not found\r\n", fault_addr);
		return 0;
	}

	_arc_aux_write(AUX_MPU_RDP0 + g_mpu_slot * 2, region->rdp);
	_arc_aux_write(AUX_MPU_RDB0 + g_mpu_slot * 2,
			region->base | AUX_MPU_RDB_VALID_MASK);
	g_mpu_slot++;

	if (g_mpu_slot >= ARC_FEATURE_MPU_REGIONS) {
		g_mpu_slot = ARC_MPU_RESERVED_REGIONS;
	}

	return 1;
}

int32_t vmpu_ac_interface(uint8_t container_id, void *func, uint32_t args_num)
{
	INTERFACE_CONTAINER *container;
	CONTAINER_INTERFACE *interface;

	if (container_id == 0) {
		EMBARC_HALT("container 0 is background container, no need to allocate interface");
		return -1;
	}

	if (g_interface_count >= CONTAINER_INTERFACE_COUNT) {
		EMBARC_HALT("no more place for interface");
		return -1;
	}

	SECURESHIELD_DBG("container %d has interface @%x with %d arguments\r\n",
		container_id, (uint32_t)func, args_num);

	/* assign interface container pointer */
	container = &g_interface_container[container_id];

	if (!container->interface) {
		container->interface = &g_interface_list[g_interface_count];
	}

	/* allocate new MPU region */
	interface = &container->interface[container->count];

	interface->func = func;
	interface->args_num = args_num;

	/* take account for new interface */
	container->count++;
	g_interface_count++;

	return 0;
}

void * vmpu_find_interface(uint8_t container_id, void *func, uint32_t args_num)
{
	uint32_t i;
	INTERFACE_CONTAINER *container;
	CONTAINER_INTERFACE *interface;

	if (container_id == 0) {
		EMBARC_HALT("container 0 has no interfaces");
		return NULL;
	}

	container = &g_interface_container[container_id];

	if (!container->interface) {
		return NULL;
	}

	interface = &container->interface[0];

	for (i = 0; i < container->count; i++) {
		if (interface->func == func && interface->args_num == args_num)
			return func;
		interface++;
	}

	return NULL;
}

/* vmpu_arch_init is target-dependent */
void vmpu_arch_init(void)
{
	uint32_t mpu_cfg;
	g_vmpu_aligment_mask = (1UL << ARC_FEATURE_MPU_ALIGNMENT_BITS);

	g_mpu_slot = ARC_MPU_RESERVED_REGIONS;
	/* init protected container memory enumeration pointer */
	mpu_cfg = _arc_aux_read(AUX_BCR_MPU);
	SECURESHIELD_DBG("MPU verison:%x, regions:%d\n\r", mpu_cfg & 0xff, (mpu_cfg >> 8) & 0xff);
	SECURESHIELD_DBG("MPU ALIGNMENT=0x%08X\r\n", g_vmpu_aligment_mask);
	SECURESHIELD_DBG("MPU ALIGNMENT_BITS=%d\r\n", vmpu_bits(g_vmpu_aligment_mask));

	SECURESHIELD_DBG("container stack sections start=0x%08X end=0x%08X (length=%d)\n\r",
		(uint32_t)__secureshield_config.bss_containers_start, (uint32_t)__secureshield_config.bss_containers_end,
		((uint32_t)__secureshield_config.bss_containers_end)-((uint32_t)__secureshield_config.bss_containers_start));

	g_container_mem_pos = (uint32_t)__secureshield_config.bss_containers_start;

	/* background for secureshield and application */
	/* application cannot access secureshield except execution */
	/* for iccm, UREAD, KREAD, KWRITE, U&S EXECUTE are ok */
	vmpu_ac_static_region(0, (void *)ROM_ORIGIN, ROM_LENGTH, ROM_AC);

	/* for application iccm, all operations are allowed */
	vmpu_ac_static_region(1, (void *)RAM_ORIGIN, RAM_LENGTH, RAM_AC);

	// vmpu_ac_static_region(0, (void *)SECURESHIELD_ROM_ORIGIN ,
	// 	(uint32_t)(__secureshield_config.secure_end) - SECURESHIELD_ROM_ORIGIN,
	// 		SECURESHIELD_ACDEF_KTEXT | SECURESHIELD_AC_UEXECUTE);

	// vmpu_ac_static_region(1, (void *)SECURESHIELD_RAM_ORIGIN,
	// 	(uint32_t)(__secureshield_config.bss_end)  - SECURESHIELD_RAM_ORIGIN,
	// 		SECURESHIELD_ACDEF_KRAM);

}
