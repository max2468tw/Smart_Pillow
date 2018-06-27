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
 * \date 2016-05-13
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */
/**
 * \defgroup	EMBARC_APP_BAREMETAL_SECURESHIELD_TEST_CASE	embARC Secureshield Test Cases Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_BAREMETAL
 * \brief	embARC Secureshield Test Cases example
 *
 * \details
 * ### Extra Required Tools
 *     - Designware nSIM Tool
 *
 * ### Extra Required Peripherals
 *
 * ### Design Concept
 *     This example is designed to test the secureshield's API functionality.
 *
 * ### Usage Manual
 *     Run this example you will see the following output.
 *
 *     ![ScreenShot of secureshield example](pic/images/example/emsk/baremetal_secureshield.jpg)
 *
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_BAREMETAL_SECURESHIELD_TEST_CASE
 * \brief	secureshield test example source file
 */

/**
 * \addtogroup	EMBARC_APP_BAREMETAL_SECURESHIELD_TEST_CASE
 * @{
 */

#undef LIB_SECURESHIELD_OVERRIDES
/* embARC HAL */
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_assert.h"

#define SWI_INTNO1 18
#define SWI_INTNO2 19

typedef struct {
	uint8_t secret[128];
	uint8_t initialized;
} TEST_CONTEXT;

extern int tst_func0(void);
extern int tst_func1(int arg1);
extern int tst_func2(int arg1, int arg2);
extern int tst_func3(int arg1, int arg2, int arg3);
extern int tst_func4(int arg1, int arg2, int arg3, int arg4);
extern int tst_func5(int arg1, int arg2, int arg3, int arg4, int arg5);
extern int tst_func6(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);


static void soft_interrupt1(void *p_exinf)
{
	EMBARC_PRINTF("soft interrupt 1 interrupt handler\r\n");
}

static void soft_interrupt2(void *p_exinf)
{
/* if secure container is not really in secure state, _arc_lr_reg will raise an exception */
	_arc_lr_reg(AUX_TIMER0_CNT);
	EMBARC_PRINTF("soft interrupt 2 interrupt handler\r\n");
}


static void default_interrupt_handler(void *p_exinf)
{
	EMBARC_PRINTF("default interrupt handler");
}

static const CONTAINER_AC_ITEM g_container1_act[] = {
	{tst_func0, 0, SECURESHIELD_AC_INTERFACE},
	{tst_func1, 1, SECURESHIELD_AC_INTERFACE},
	{tst_func2, 2, SECURESHIELD_AC_INTERFACE},
 };

static const CONTAINER_AC_ITEM g_container2_act[] = {
	{tst_func3, 3, SECURESHIELD_AC_INTERFACE},
	{tst_func5, 5, SECURESHIELD_AC_INTERFACE},
	{soft_interrupt2, SWI_INTNO2, SECURESHIELD_AC_IRQ}
};

static const CONTAINER_AC_ITEM g_container3_act[] = {
	{tst_func4, 4, SECURESHIELD_AC_INTERFACE},
	{tst_func6, 6, SECURESHIELD_AC_INTERFACE}
};

static const CONTAINER_AC_ITEM g_main_container_act[] = {
#ifdef BOARD_EMSK
#define PERIPHERAL_ADDR_BASE 0xf0000000
	/* By default, all peripherals are accessible to background container */
	{(void *)PERIPHERAL_ADDR_BASE, 0x10000, SECURESHIELD_ACDEF_UPERIPH},
	{default_interrupt_handler, INTNO_GPIO, SECURESHIELD_AC_IRQ},
	{default_interrupt_handler, INTNO_UART1, SECURESHIELD_AC_IRQ},
	{default_interrupt_handler, INTNO_I2C0, SECURESHIELD_AC_IRQ},
	{default_interrupt_handler, INTNO_SPI_MASTER, SECURESHIELD_AC_IRQ},
#endif
	/* By default, the auxiliary register space is accessible to background container */
	{(void *)0, 0x500, SECURESHIELD_AC_AUX},
	{soft_interrupt1, SWI_INTNO1, SECURESHIELD_AC_IRQ},
	{default_interrupt_handler, INTNO_TIMER0, SECURESHIELD_AC_IRQ},
	{default_interrupt_handler, INTNO_TIMER1, SECURESHIELD_AC_IRQ},
};


SECURESHIELD_SET_MODE_AC(SECURESHIELD_ENABLED, g_main_container_act);

/* configure containers */
SECURESHIELD_CONTAINER_CONFIG(container1, g_container1_act, 1024, TEST_CONTEXT);
SECURESHIELD_CONTAINER_CONFIG(container2, g_container2_act, 1024, 0, SECURESHIELD_CONTAINER_SECURE);
SECURESHIELD_CONTAINER_CONFIG(container3, g_container3_act, 1024, TEST_CONTEXT);

TEST_CONTEXT * challenge1;
TEST_CONTEXT * challenge2;


int tst_func0(void)
{
	EMBARC_PRINTF("tst_func0 in container 1, no arguments\r\n");
	EMBARC_PRINTF("my container id is:%d\r\n",secureshield_container_id_self());
	EMBARC_PRINTF("the id of caller is:%d\r\n", secureshield_container_id_caller());
	return 0;
}

int tst_func1(int arg1)
{
	EMBARC_PRINTF("tst_func1 in container 1, arguments are:%d\r\n", arg1);
	((TEST_CONTEXT *)secureshield_container_ctx)->initialized = 1;
	challenge1 = (TEST_CONTEXT *)secureshield_container_ctx;
	EMBARC_PRINTF("container call from 1 to 2\r\n");
	EMBARC_PRINTF("ret is:%x\r\n", container_call(container2, tst_func3, 1, 2, 3));
	return 1;
}

int tst_func2(int arg1, int arg2)
{
	EMBARC_PRINTF("container call in cycles:%d\r\n", secureshield_perf_end());
	EMBARC_PRINTF("tst_func2 in container 1, arguments are:%d,%d\r\n", arg1, arg2);
	secureshield_perf_start();
	return 2;
}

int tst_func3(int arg1, int arg2, int arg3)
{
	EMBARC_PRINTF("tst_func3 in container 2(secure container), arguments are:%d,%d,%d\r\n",
		arg1, arg2, arg3);
	EMBARC_PRINTF("container call from 2 to 3\r\n");
	EMBARC_PRINTF("ret is:%x\r\n", container_call(container3, tst_func4, 1, 2, 3, 4));
	/* secure container can directly access the system resources */
	_arc_lr_reg(AUX_TIMER0_CNT);
	int_enable(SWI_INTNO2);
	int_sw_trigger(SWI_INTNO2);

	return 3;
}

int tst_func4(int arg1, int arg2, int arg3, int arg4)
{
	EMBARC_PRINTF("tst_func4 in container 3, arguments are:%d,%d,%d,%d\r\n",
		arg1, arg2, arg3, arg4);
	/* container 3 initializes its own context */
	((TEST_CONTEXT *)secureshield_container_ctx)->initialized = 1;
	/* challenge2 is in background container */
	challenge2 = (TEST_CONTEXT *)secureshield_container_ctx;
	EMBARC_PRINTF("container call from 3 to 1\r\n");
	EMBARC_PRINTF("ret is:%x\r\n", container_call(container1, tst_func0));
	return 4;
}

int tst_func5(int arg1, int arg2, int arg3, int arg4, int arg5)
{
	EMBARC_PRINTF("tst_func5 in container 2(secure container), arguments are:%d,%d,%d,%d,%d\r\n",
		arg1, arg2, arg3, arg4, arg5);

	/* container 2 is secure, it can directly access all system resources, no exception raised */
	/* the area pointed by challenge1 belongs to container1 */
	challenge1->initialized = 0;
	return 5;
}

int tst_func6(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6)
{
	EMBARC_PRINTF("tst_func6 in container 3, arguments are:%d,%d,%d,%d,%d,%d\r\n",
		arg1, arg2, arg3, arg4, arg5, arg6);
	/* here timer interrupt belonging to background container will come out, test the case of
	interrupt handling with container switch */
	board_delay_ms(1000, 0);
	return 6;
}

static void tst_func7(int arg)
{
	EMBARC_PRINTF("test function 7 in background container:%d\r\n", arg);
	secureshield_cpu_lock();
	secureshield_int_sw_trigger(SWI_INTNO1);
	secureshield_cpu_unlock();
}


int main(void)
{
	unsigned int cycles;

#if ARC_FEATURE_RGF_BANKED_REGS < 16	/*\todo removed in the future */
	secureshield_int_pri_set(INTNO_TIMER0, INT_PRI_MIN);
#endif
	EMBARC_PRINTF("timer0 interrupt prioirty is:%d\r\n", secureshield_int_pri_get(INTNO_TIMER0));

	secureshield_perf_config();

	secureshield_perf_start();
	secureshield_int_enable(SWI_INTNO1);
	cycles = secureshield_perf_end();

	EMBARC_PRINTF("int_enable cycles:%d\r\n", cycles);

	EMBARC_PRINTF("container call from background to 1 \r\n");
	EMBARC_PRINTF("ret is:%x\r\n", container_call(container1, tst_func1, 1));

	secureshield_perf_start();
	container_call(container1, tst_func2, 1, 2);
	cycles = secureshield_perf_end();
	EMBARC_PRINTF("container call out cycles:%d\r\n", cycles);

	EMBARC_PRINTF("container call from background to 2\r\n");
	EMBARC_PRINTF("ret is:%x\r\n", container_call(container2, tst_func5, 1, 2, 3, 4, 5));

	EMBARC_PRINTF("container call from background to 3\r\n");
	EMBARC_PRINTF("ret is:%x\r\n", container_call(container3, tst_func6, 1, 2, 3, 4, 5, 6));

	EMBARC_PRINTF("timer0 count is:%x\r\n", secureshield_arc_lr_reg(AUX_TIMER0_CNT));

	tst_func7(7);

	/* comment out the following line to test memory violation */
	challenge2->initialized = 0;


	return E_SYS;
}

/** @} */
