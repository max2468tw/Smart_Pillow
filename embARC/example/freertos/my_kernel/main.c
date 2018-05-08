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
 * \date 2016-04-20
 * \author Qiang Gu(Qiang.Gu@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	EMBARC_APP_FREERTOS_KERNEL	embARC FreeRTOS Kernel Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_FREERTOS
 * \brief	embARC Example for testing FreeRTOS task switch and interrupt/exception handling
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *
 * ### Design Concept
 *     This example is designed to show the functionality of FreeRTOS.
 *
 * ### Usage Manual
 *     Test case for show how FreeRTOS is working by task switching and interrupt/exception processing.
 *     ![ScreenShot of freertos-demo under freertos](pic/images/example/emsk/emsk_freertos_demo.jpg)
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_FREERTOS_KERNEL
 * \brief	main source file of the freertos demo
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_KERNEL
 * @{
 */

#include "embARC.h"
#include "embARC_debug.h"

static void task1(void * par);
static void task2(void * par);
static void start_task(void * par);

/**
 * \var		task1_handle
 * \brief	handle of task1
 * \details	If task handle is not used, set Null.
 */
static TaskHandle_t task1_handle = NULL;
/**
 * \var		task2_handle
 * \brief	handle of task2
 * \details	If task handle is not used, set Null.
 */
static TaskHandle_t task2_handle = NULL;
/**
 * \var		task2_handle
 * \brief	handle of start_task
 * \details	If task handle is not used, set Null.
 */
static TaskHandle_t start_task_handle = NULL;

static DEV_GPIO *gpio;
static DEV_IIC *iic;
static uint32_t iic_slvaddr = 0x28;

#define EMSK_IIC_CHECK_EXP_NORTN(EXPR)		CHECK_EXP_NOERCD(EXPR, error_exit)
#define EMSK_GPIO_CHECK_EXP_NORTN(EXPR)		CHECK_EXP_NOERCD(EXPR, error_exit)

void my_emsk_gpio_init(void);
int32_t my_emsk_iic_init(uint32_t slv_addr);
/**
 * \brief  call FreeRTOS API, create and start tasks
 */
int main(void)
{	
	my_emsk_gpio_init();
	my_emsk_iic_init(iic_slvaddr);
	
	uint8_t config[2];
	config[0] = 0x08; // configuration of the I2C communication in HIGH SPEED Mode
	config[1] = 0x70; // configuration of Pmod AD2 (read of V1 to V3)
	iic->iic_write(config,2);
	
	if (xTaskCreate(start_task, "start_task", 128, (void *)NULL, 1, &start_task_handle)
		!= pdPASS) {	/*!< FreeRTOS xTaskCreate() API function */
		EMBARC_PRINTF("create start_task error\r\n");
		return -1;
	}
	
	vTaskStartScheduler();
error_exit:
	return 0;
}

/**
 * \brief  start_task in FreeRTOS
 * \details 
 * \param[in] *par
 */
static void start_task(void * par)
{
	if (xTaskCreate(task1, "task1", 128, (void *)NULL, 2, &task1_handle)
		!= pdPASS) {	/*!< FreeRTOS xTaskCreate() API function */
		EMBARC_PRINTF("create task1 error\r\n");
		return ;
	}
	if (xTaskCreate(task2, "task2", 128, (void *)NULL, 3, &task2_handle)
		!= pdPASS) {	/*!< FreeRTOS xTaskCreate() API function */
		EMBARC_PRINTF("create task2 error\r\n");
		return ;
	}
	vTaskDelete(start_task_handle);
}

/**
 * \brief  task1 in FreeRTOS
 * \details 
 * \param[in] *par
 */
static void task1(void * par)
{
	while(1)
	{	
		int val;
		uint8_t data[1];
		iic->iic_read(data,1);
		val = data[0] << 8;
		iic->iic_read(data,1);
		val = val + data[0];
		if (((val & 0x3000)>> 12) == 0)
			EMBARC_PRINTF("This is V1 : %d\n", val);
		else if(((val & 0x3000)>> 12) == 1)
			EMBARC_PRINTF("This is V2 : %d\n", val);
		else if(((val & 0x3000)>> 12) == 2)
			EMBARC_PRINTF("This is V3 : %d\n", val);
		else if(((val & 0x3000)>> 12) == 3)
			EMBARC_PRINTF("This is V4 : %d\n", val);
		else
			EMBARC_PRINTF("Wrong : %d\n", val);
		vTaskDelay(10);
	}
}

/**
 * \brief  task2 in FreeRTOS
 * \details 
 * \param[in] *par
 */
static void task2(void * par)
{
	while(1) {
		gpio->gpio_write(GPIO_BITS_MASK_ALL,GPIO_BITS_MASK_ALL);
		vTaskDelay(500);
		gpio->gpio_write(GPIO_BITS_MASK_NONE,GPIO_BITS_MASK_ALL);
		vTaskDelay(500);
	}
}

/** emsk on-board gpio init, gpio default off */
void my_emsk_gpio_init(void)
{
	gpio = gpio_get_dev(DW_GPIO_PORT_B);

	EMSK_GPIO_CHECK_EXP_NORTN(gpio != NULL);

	if (gpio->gpio_open(GPIO_BITS_MASK_ALL) == E_OPNED) {
		gpio->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, (void *)(GPIO_BITS_MASK_ALL));
		gpio->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)(GPIO_BITS_MASK_ALL));
	}

	gpio->gpio_write(GPIO_BITS_MASK_NONE, GPIO_BITS_MASK_ALL);

error_exit:
	return;
}

/** emsk on-board iic init */
/**
 * \brief	iic init
 * \param[in]	slv_addr	iic slave address
 * \retval	E_OK	init success
 * \retval	!E_OK	init failed
 */
int32_t my_emsk_iic_init(uint32_t slv_addr)
{
	int32_t ercd = E_OK;

	iic = iic_get_dev(DW_IIC_1_ID);

	EMSK_IIC_CHECK_EXP_NORTN(iic!=NULL);

	ercd = iic->iic_open(DEV_MASTER_MODE, IIC_SPEED_HIGH);
	if ((ercd == E_OK) || (ercd == E_OPNED)) {
		ercd = iic->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(slv_addr));
		iic_slvaddr = slv_addr;
	}

error_exit:
	return ercd;
}
/** @} */