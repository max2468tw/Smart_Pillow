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
 * \defgroup	EMBARC_APP_BAREMETAL_SECURESHIELD_SECRET embARC Secureshield Secret Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_BAREMETAL
 * \brief	embARC Secureshield secret example
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     - Designware nSIM Tool or
 *     - EMSK 2.2 arcem7d configuration
 *
 * ### Design Concept
 *     This example is designed to show how data could be protected with SecureShield. The application demonstrates a secure keystore component.
 *     - The secret and password are protected in the context of container 1 which is a container with secure privileges.
 *     - You need to input the right password to get the secret from the non-trusted background container; container 2 is trusted and can access the secret without password.
 *     - The init value of secret and password are stored in the data section of secure container 1. Non-secure containers can not access them directly.
 *     - Secure container 2 can use the container 1 secret internally for implementing other services. The background application can request such a service, without knowing or obtaining the secret itself.
 *     - For demonstration purpose, the background container can obtain the secret as well (using the password). Since the background container stores the result in global, public memory, this is normally not secure and not recommended. Instead see Container 2 for how to securely use the secret.
 *
 * ### Usage Manual
 *     When this example start to run, the try the following ntshell commands:
 *     - Run ntshell command **main -h** to see how to run this example.
 *     - Run ntshell command **main -p embarc** or **main -t** to get the secret.

 *     ![ScreenShot of secureshield example](pic/images/example/emsk/baremetal_secureshield_secret.jpg)
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_BAREMETAL_SECURESHIELD_SECRET
 * \brief	secureshield secret example source file
 */

/**
 * \addtogroup	EMBARC_APP_BAREMETAL_SECURESHIELD_SECRET
 * @{
 */
/* embARC HAL */
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_assert.h"

#define SECRET_LEN	64
#define PWD_LEN		6

#define GET_SECRET	1
#define SET_PWD		2
#define SET_SECRET	3

typedef struct {
	uint8_t * secret;
	uint8_t * pwd;
	uint8_t initialized;
	uint8_t fails;
} SECRET_CONTEXT;

typedef struct {
	uint8_t secret[SECRET_LEN+1];
	uint8_t initialized;
} SECRET_CONTEXT2;

static SECRET_CONTEXT *challenge;
static uint8_t public_data[SECRET_LEN];

static void default_interrupt_handler(void *p_exinf)
{
	EMBARC_PRINTF("default interrupt handler\r\n");
}

/* Container 1 : the keystore component */

/* non-secure containers cannot access pwd and secret, as they are in secure container data section through the use of the SECURE_DATA */
SECURE_DATA uint8_t container1_pwd[PWD_LEN+1] = {'e','m','b','a','r','c'};
SECURE_DATA uint8_t container1_secret[SECRET_LEN+1] = {'i', ' ','l', 'o', 'v', 'e', ' ', 'e', 'm',
	'b', 'a', 'r', 'c'};

static int32_t init_secret()
{
	SECRET_CONTEXT *ctx;

	ctx = (SECRET_CONTEXT *)secureshield_container_ctx;

	if (ctx->initialized == 0) {
		ctx->initialized = 1;
		ctx->fails = 0;
		ctx->pwd = container1_pwd;
		ctx->secret = container1_secret;
	}

	// simulate the case of memory leakage; make available the pointer to the secret context through a global variable
	challenge = (SECRET_CONTEXT *)secureshield_container_ctx;

	return 0;
}

// Note that some parameters of this function are pointers. Care should be taken that Container 1 has access to the memory pointed at
static int32_t operate_secret(char* pwd, uint32_t cmd, char * data)
{
	SECRET_CONTEXT *ctx;
	uint32_t len;

	ctx = (SECRET_CONTEXT *)secureshield_container_ctx;

	if (ctx->initialized == 0) {
		return -1;
	}

	// protect against brute force password guessing; for better protection, number of fails should be stored persistently.
	if (ctx->fails++ > 3) {
		EMBARC_PRINTF("Too many failed attempts to obtain secret.\r\n");
		// optionally, the secret could be erased here
		return -1;
	}

	// Check if caller is authorized to access the secret
	if (secureshield_container_id_caller() == 0) {
		if (strcmp(pwd, (const char *)ctx->pwd) != 0) {
			return -1;
		}
		memset(pwd, 0, strlen(pwd));
	} else if (secureshield_container_id_caller() == 2) {
		EMBARC_PRINTF("container2 is trusted, no need to check password\r\n");
	} else {
		EMBARC_PRINTF("Unknown container tried to access the secret\r\n");
		return -1;
	}

	// Caller is trusted now, perform the requested operation
	ctx->fails = 0;
	if (cmd == GET_SECRET) {
		// note that data is copied with Container 1 privileges to the memory pointer to by the 'data' argument
		// this could be a security risk, but since the caller is trusted the data pointer is trusted as well
		memcpy(data, ctx->secret, SECRET_LEN);
	} else if (cmd == SET_SECRET) {
		memset(ctx->secret, 0, SECRET_LEN);
		len = strlen(data);
		if (len > SECRET_LEN) {
			len = SECRET_LEN;
		}
		memcpy(ctx->secret, data, len);
		memset(data, 0, len);
	} else if (cmd == SET_PWD) {
		memset(ctx->pwd, 0, PWD_LEN);
		len = strlen(data);
		if (len > PWD_LEN) {
			len = PWD_LEN;
		}
		memcpy(ctx->pwd, data, len);
		memset(data, 0, len);
	} else {
		return -1;
	}

	return 0;
}

/* Container 2 : the trusted keystore client */

static void trusted_ops(void)
{
	uint8_t data[SECRET_LEN];
	SECRET_CONTEXT2 *ctx;

	ctx = (SECRET_CONTEXT2 *)secureshield_container_ctx;

	EMBARC_PRINTF("container 2 is trusted\r\n");
	/* data is on container's  stack, normally other containers can not access it. As
	container1 is a secure container, it can access container 2's stack */
	if(container_call(container1, operate_secret, NULL, GET_SECRET, data)) {
		EMBARC_PRINTF("get secret failed - someone tried to hack the system?\r\n");
		return;
	}

	if (! ctx->initialized) {
		EMBARC_PRINTF("container 2: got the secret for the first time and stored it for private use\r\n");
		memcpy(ctx->secret, data, SECRET_LEN);
		ctx->initialized = 1;
	} else if (strcmp((const char *)data, (const char *)ctx->secret) != 0) {
		EMBARC_PRINTF("container 2: the secret changed, updated my private copy!\r\n");
		memcpy(ctx->secret, data, SECRET_LEN);
	} else {
		EMBARC_PRINTF("got the secret, but not telling you since its secret and for use within this container only...\r\n");
	}

}


/* General container set-up and main application */

static const CONTAINER_AC_ITEM g_container1_act[] = {
	{init_secret, 0, SECURESHIELD_AC_INTERFACE},
	{operate_secret, 3, SECURESHIELD_AC_INTERFACE}
 };

static const CONTAINER_AC_ITEM g_container2_act[] = {
	{trusted_ops, 0, SECURESHIELD_AC_INTERFACE}
};

static const CONTAINER_AC_ITEM g_main_container_act[] = {
#ifdef BOARD_EMSK
#define PERIPHERAL_ADDR_BASE 0xf0000000
	/* By default, all peripherals are accessible to background container */
	{(void *)PERIPHERAL_ADDR_BASE, 0x10000, SECURESHIELD_ACDEF_UPERIPH},	/* MUX */
	{default_interrupt_handler, INTNO_GPIO, SECURESHIELD_AC_IRQ},
	{default_interrupt_handler, INTNO_UART1, SECURESHIELD_AC_IRQ},
#endif
	/* By default, the auxiliary register space is accessible to background container */
	{(void *)0, 0x500, SECURESHIELD_AC_AUX},
	{default_interrupt_handler, INTNO_TIMER0, SECURESHIELD_AC_IRQ},
};

/* enable secureshield, set the access control table of background container */
SECURESHIELD_SET_MODE_AC(SECURESHIELD_ENABLED, g_main_container_act);

/* configure the secret container */
/* container1 is secure container, it has access to most of the system resources */
SECURESHIELD_CONTAINER_CONFIG(container1, g_container1_act, 1024, SECRET_CONTEXT, SECURESHIELD_CONTAINER_SECURE);
SECURESHIELD_CONTAINER_CONFIG(container2, g_container2_act, 1024, SECRET_CONTEXT2);

static void main_help(void)
{
	EMBARC_PRINTF("Usage: main -p password -s secret -n new_pwd\r\n"
		"Operate the secret\r\n"
		"  -h/H/?    Show the help information\r\n"
		"  -p <pwd>  password\r\n"
		"  -s <secret> set new secret\r\n"
		"  -n <new_pwd> set new password\r\n"
		"  -t ask trusted container to access the secret for you\r\n"
		"  challenge 0, 1, or 2 : attempt obtaining the secret illegally\r\n"
		"Examples: \r\n"
		"  main challenge 0,1, or 2 : try to steal the secret method 0,1,2\r\n"
		"  main -t\r\n"
		"  main -p <pwd>  get the secret\r\n"
		"  main -p <pwd> -s <secret>   set the new secret\r\n"
		"  main -p <pwd> -n <new_pwd>  set the new password\r\n");
}

int main(int argc, char **argv)
{
	int32_t opt;
	char *pwd = NULL;
	char *secret = NULL;
	char *new_pwd = NULL;

	if (container_call(container1, init_secret)) {
		EMBARC_PRINTF("secret key initialized error\r\n");
		return E_SYS;
	}

	if (!strcmp(argv[1], "challenge")) {
		EMBARC_PRINTF("try to steal the secret\r\n");
		switch (argv[2][0]) {
			case '0' :
				EMBARC_PRINTF("the secret from container 1 context is:%s\r\n", challenge->secret);
				break;
			case '1' :
				EMBARC_PRINTF("the secret from direct memory read is:%s\r\n", container1_secret);
				break;
			case '2' :
				EMBARC_PRINTF("the password from direct memory read is:%s\r\n", container1_pwd);
				break;
			default:
				EMBARC_PRINTF("Please use challenge 0, 1, or 2\r\n");
				return E_OK;
		}
		return E_OK;
	}

	opterr = 0;
	optind = 1;

	while ((opt=getopt(argc, argv, "tp:s:n:hH?")) != -1) {
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				main_help();
				goto error_exit;
				break;
			case 'p':
				pwd = optarg;
				break;
			case 's':
				secret = optarg;
				if (strlen(secret) > SECRET_LEN) {
					EMBARC_PRINTF("secret is too long\r\n");
					goto error_exit;
				}
				break;
			case 't':
				EMBARC_PRINTF("Requesting operation from trusted container\r\n");
				container_call(container2, trusted_ops);
				return E_OK;
				break;
			case 'n':
				new_pwd = optarg;
				if (strlen(new_pwd) > PWD_LEN) {
					EMBARC_PRINTF("max password length 6 bytes\r\n");
					goto error_exit;
				}
				break;
			default:
				main_help();
				goto error_exit;
				break;
		}
	}

	/* UART is in background container, so a hacker could hijack or access the UART and public_data to
	get the secret. Here it's just for demo. In a real applicaton, container 2's method should be used */
	if (pwd != NULL) {
		if (secret == NULL && new_pwd == NULL) {
			if(container_call(container1, operate_secret, pwd, GET_SECRET, public_data)) {
				EMBARC_PRINTF("get secret failed\r\n");
			} else {
				EMBARC_PRINTF("the secret is:%s\r\n", public_data);
				memset(public_data, 0, SECRET_LEN);
			}
		} else if (secret != NULL && new_pwd == NULL) {
			if(container_call(container1, operate_secret, pwd, SET_SECRET, secret)) {
				EMBARC_PRINTF("set new secret failed\r\n");
			}
		} else if (secret == NULL && new_pwd != NULL) {
			if(container_call(container1, operate_secret, pwd, SET_PWD, new_pwd)) {
				EMBARC_PRINTF("set new password failed\r\n");
			}
		} else {
			EMBARC_PRINTF("cannot set password and secret at the same time\r\n");
		}
	} else {
		EMBARC_PRINTF("no password input\r\n");
	}

error_exit:
	return E_OK;
}

/** @} */