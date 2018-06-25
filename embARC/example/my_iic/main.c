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
 * \version
 * \date
 * \author
--------------------------------------------- */

/**
 * \defgroup	EMBARC_APP_TMPL		embARC Template Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_BOARD_EMSK
 * \ingroup	EMBARC_APPS_BAREMETAL
 * \brief	embARC Example for template
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *
 * ### Design Concept
 *
 * ### Usage Manual
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_TMPL
 * \brief	main source of template example
 */

/**
 * \addtogroup	EMBARC_APP_TMPL
 * @{
 */
/* embARC HAL */
#include "embARC.h"
#include "embARC_debug.h"

static DEV_IIC *iic;
static uint32_t iic_slvaddr = 0x28;

#define EMSK_IIC_CHECK_EXP_NORTN(EXPR)		CHECK_EXP_NOERCD(EXPR, error_exit)

int32_t my_emsk_iic_init(uint32_t slv_addr);

/** main entry */
int main(void)
{
	my_emsk_iic_init(iic_slvaddr);
	uint8_t config[2];
	config[0] = 0x08; // configuration of the I2C communication in HIGH SPEED Mode
	config[1] = 0x70; // configuration of Pmod AD2 (read of V1 to V3)
	iic->iic_write(config,2);
	int val;
	uint8_t data[1];
	while(1)
	{
		iic->iic_read(data,1);
		val = data[0] << 8;
		iic->iic_read(data,1);
		val = val + data[0];
		if (((val & 0x3000)>> 12) == 0)
			EMBARC_PRINTF("This is V1 : \t%d\n", val);
		else if(((val & 0x3000)>> 12) == 1)
			EMBARC_PRINTF("This is V2 : \t\t%d\n", val);
		else if(((val & 0x3000)>> 12) == 2)
			EMBARC_PRINTF("This is V3 : \t\t\t%d\n", val);
		else if(((val & 0x3000)>> 12) == 3)
			EMBARC_PRINTF("This is V4 : %d\n", val);
		else
			EMBARC_PRINTF("Wrong : %d\n", val);
	}
	return E_SYS;
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