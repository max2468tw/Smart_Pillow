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
 * \date 2015-07-10
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */
/**
 * \file
 * \ingroup	BOARD_NSIM_DRV_UART_OBJ
 * \brief	header file of uart object instantiation in nsim
 */

/**
 * \addtogroup	BOARD_NSIM_DRV_UART_OBJ
 * @{
 */
#ifndef _NSIM_UART_OBJ_H_
#define _NSIM_UART_OBJ_H_

#include "dev_uart.h"

/**
 * \name	NSIM UART Object Number
 * @{
 */
#define DW_UART_NUM	(1)	/*!< NSIM UART vaild number */
/** @} end of name */

/**
 * \name    NSIM UART Object ID Macros
 * @{
 */
#define NSIM_UART_0_ID	0	/*!< uart 0 id macro */
/** @} end of name */

/**
 * \name	NSIM UART Object Control Macros
 * @{
 */
#define USE_NSIM_UART_0	1     	/*!< enable use nsim uart 0 (stdio) */
/** @} end of name */

extern void nsim_uart_all_install(void);

#endif /* _NSIM_UART_OBJ_H_ */
