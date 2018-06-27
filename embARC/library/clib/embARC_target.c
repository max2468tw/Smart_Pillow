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
 * \date 2016-03-02
 * \author Huaqi Fang(Huaqi.Fang@synopsys.com)
--------------------------------------------- */
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "embARC_debug.h"
#include "embARC_syscalls.h"

//////////////////////////////
// OTHER REQUIRED FUNCTIONS //
//////////////////////////////
#if defined(__MW__)
char *optarg;	/* Argument to the current option, if any */
int opterr = 1;	/* If non-zero, print getopt error messages */
int optind = 1;	/* Index to the next element in argv, start at 1 */
/* --------------------------------------------------------------------------*/
/**
 * \brief  Emulates getopt(3) in unix. We need a separate function because
 *		   DOS hosts may not know about the existence of getopt().
 *
 * \param argc
 * \param argv
 * \param optstring
 *
 * \returns   -1, error
 *	      other, option char
 */
/* ----------------------------------------------------------------------------*/
int getopt(int argc, char * const argv[], const char *optstring)
{
	static int optset_in_progress = 0;
	static char *curr = NULL;
	if (optind >= argc) { return -1; }

	if (!optset_in_progress) {
		curr = argv[optind++];
	}

	while (curr) {
		if (!optset_in_progress && curr[0] != '-') {
			/*
			* First non option argument
			*/
			optind--;
			return -1;
		}

		if (!optset_in_progress && curr[0] == '-' && curr[1] == '-') {
			/*
			* Option processing forcefully
			* terminated by "--"
			*/
			return -1;
		}

		static char *optset = NULL;
		if (!optset_in_progress) {
			optset = &curr[1];
		}

		optset_in_progress = 0;

		while (*optset) {
			/*
			* For all options -a -b -c grouped
			* together as -abc
			*/
			const char *optvalid = optstring;
			while (*optvalid) {
				/*
				* Compare *optset with each valid
				* option character
				*/
				if (optset[0] == optvalid[0]) {
					if (optvalid[1] == ':') {
						int i = 1;
						while (optset[i] && isspace(optset[i])) { i++; }
						/*
						* Option needs an argument
						*/
						if (optset[i]) {
							/*
							* The rest of the optset string
							* is the argument to the option
							*/
							optarg = &optset[i];
						} else {
							/*
							* The next element of argv is the
							* argument to the option
							*/
							optarg = argv[optind++];
							if (argc < optind || !optarg || !optarg[0] || optarg[0] == '-') {
								/*
								* Option's argument is empty
								*/
								if (opterr) {
									DBG("option '-%c' needs an argument\r\n", optset[0]);
								}
								return '?';
							}
						}
						return optset[0];
					} else {
						/*
						* Option does not need an argument
						*/
						int result = optset[0];
						optset++;
						if (optset[0]) {
							/*
							* We need to continue with this
							* optset when we return
							*/
							optset_in_progress = 1;
						}
						optarg = NULL;
						return result;
					}
				}
				optvalid++;
			}

			if (opterr) {
				DBG("invalid option '-%c'\r\n", optset[0]);
			}
			/*
			* Process the next option when we return
			*/
			optset++;
			if (optset[0]) {
				/*
				* We need to continue with this
				* optset when we return
				*/
				optset_in_progress = 1;
			}
			return '?';
		}
		curr = argv[optind++];
	}
	/*
	* If all command line arguments were option
	* strings, we would come here.
	*/
	return -1;
}
#endif

const char szEnglishMonth[12][4] = { \
	"Jan","Feb","Mar","Apr", \
	"May","Jun","Jul","Aug", \
	"Sep","Oct","Nov","Dec"};

time_t get_build_timedate(struct tm *build_tm)
{
	if (!build_tm) return 0;
	char szMonth[5];
	int tm_year, tm_mon = 0, tm_day;
	int tm_hour, tm_min, tm_sec;
	time_t build_time;

	/** Get Build Date */
	sscanf(__DATE__, "%s %d %d", szMonth, &tm_day, &tm_year) ;
	for (int i = 0; i < 12; i++) {
		if (strncmp(szMonth, szEnglishMonth[i], 3) == 0) {
			tm_mon = i;
			break;
		}
	}
	/** Get Build Time */
	sscanf(__TIME__, "%d:%d:%d", &tm_hour, &tm_min, &tm_sec);
	build_tm->tm_sec = tm_sec;
	build_tm->tm_min = tm_min;
	build_tm->tm_hour = tm_hour;
	build_tm->tm_mday = tm_day;
	build_tm->tm_mon = tm_mon;
	build_tm->tm_year = tm_year - 1900;
	build_tm->tm_isdst = 0;

	build_time = mktime(build_tm);
	return build_time;
}