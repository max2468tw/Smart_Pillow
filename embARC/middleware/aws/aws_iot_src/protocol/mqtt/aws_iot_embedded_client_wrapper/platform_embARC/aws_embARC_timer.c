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
 * \version 2015.10
 * \date 2014-07-03
 * \author Huaqi Fang(Huaqi.Fang@synopsys.com)
--------------------------------------------- */

#include "timer_interface.h"

#include <lwip/sys.h>

/** check if expired, if true, return 1, else 0 */
char expired(Timer* timer)
{
	long left = (long)(timer->end_time) - (long)sys_now();
	return (left < 0);
}

/** countdown in ms */
void countdown_ms(Timer* timer, unsigned int timeout)
{
	timer->end_time = sys_now() + timeout;
}

/** Count down in unit of second, this is for MQTT Keep Alive timer */
void countdown(Timer* timer, unsigned int timeout)
{
	countdown_ms(timer, timeout*1000);
}


int left_ms(Timer* timer)
{
	long left = (long)(timer->end_time) - (long)sys_now();
	return (left < 0) ? 0 : left;
}


void InitTimer(Timer* timer)
{
	timer->end_time = 0;
}
