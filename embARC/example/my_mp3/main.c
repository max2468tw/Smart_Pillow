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
#include <stdio.h>

static DEV_UART *uart;

void my_emsk_uart_init(void);
void init();
void volume(uint8_t vol);
void play(uint8_t num);
void next();
void pause();
void start();
// Tx : Pmod1 Pin 3 (J1) // resistor
// Rx : Pmod1 Pin 4 (J1)

// Tx : Pmod5 Pin 8 (J1) // resistor
// Rx : Pmod5 Pin 9 (J1)

/** main entry */
int main(void)
{
	cpu_lock();
	board_init();
	cpu_unlock();
	my_emsk_uart_init();
	init();
	volume(30);
	play(1);
	board_delay_ms(10000, 1);
	pause();
	board_delay_ms(10000, 1);
	start();
	while(1)
	{
		board_delay_ms(10000, 1);
	}
	return E_SYS;
}
void init()
{
	uint8_t _sending[10];
	_sending[0] = 0x7E;
	_sending[1] = 0xFF;
	_sending[2] = 0x06;
	_sending[3] = 0x0C;
	_sending[4] = 0x01;
	_sending[5] = 0x00;
	_sending[6] = 0x00;
	_sending[7] = 0xFE;
	_sending[8] = 0xEE;
	_sending[9] = 0xEF;
	uart->uart_write(_sending, 10);
	board_delay_ms(1000, 1);
}
void volume(uint8_t vol) //Set volume value. From 0 to 30
{
	uint8_t _sending[10];
	_sending[0] = 0x7E;
	_sending[1] = 0xFF;
	_sending[2] = 0x06;
	_sending[3] = 0x06;
	_sending[4] = 0x01;
	_sending[5] = 0x00;
	_sending[6] = vol; // volume
	_sending[7] = 0xFE;
	_sending[8] = 0xF4 - vol;
	_sending[9] = 0xEF;
	uart->uart_write(_sending, 10);
	board_delay_ms(1000, 1);
}

void play(uint8_t num)
{
	uint8_t _sending[10];
	_sending[0] = 0x7E;
	_sending[1] = 0xFF;
	_sending[2] = 0x06;
	_sending[3] = 0x03;
	_sending[4] = 0x01;
	_sending[5] = 0x00;
	_sending[6] = num; // songs
	_sending[7] = 0xFE;
	_sending[8] = 0xF7-num;
	_sending[9] = 0xEF;
	uart->uart_write(_sending, 10);
	board_delay_ms(1000, 1);	
}

void next()
{
	uint8_t _sending[10];
	_sending[0] = 0x7E;
	_sending[1] = 0xFF;
	_sending[2] = 0x06;
	_sending[3] = 0x01;
	_sending[4] = 0x01;
	_sending[5] = 0x00;
	_sending[6] = 0x00;
	_sending[7] = 0xFE;
	_sending[8] = 0xF9;
	_sending[9] = 0xEF;
	uart->uart_write(_sending, 10);
	board_delay_ms(1000, 1);	
}

void pause()
{
	uint8_t _sending[10];
	_sending[0] = 0x7E;
	_sending[1] = 0xFF;
	_sending[2] = 0x06;
	_sending[3] = 0x0E;
	_sending[4] = 0x01;
	_sending[5] = 0x00;
	_sending[6] = 0x00;
	_sending[7] = 0xFE;
	_sending[8] = 0xEC;
	_sending[9] = 0xEF;
	uart->uart_write(_sending, 10);
	board_delay_ms(1000, 1);
}
void start()
{
	uint8_t _sending[10];
	_sending[0] = 0x7E;
	_sending[1] = 0xFF;
	_sending[2] = 0x06;
	_sending[3] = 0x0D;
	_sending[4] = 0x01;
	_sending[5] = 0x00;
	_sending[6] = 0x00;
	_sending[7] = 0xFE;
	_sending[8] = 0xED;
	_sending[9] = 0xEF;
	uart->uart_write(_sending, 10);
	board_delay_ms(1000, 1);
}
/** emsk on-board uart init */
void my_emsk_uart_init(void)
{
	uart = uart_get_dev(DW_UART_2_ID);
	//uart = uart_get_dev(DW_UART_0_ID);

	uart->uart_open(UART_BAUDRATE_9600);

error_exit:
	return;
}
/** @} */