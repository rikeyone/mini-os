/*
 * Copyright (C) 2019 xiehaocheng <xiehaocheng127@163.com>
 *
 * All Rights Reserved
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifdef __cplusplus
export "C" {
#endif

#ifndef __WATCHDOG_TIMER_H__
#define __WATCHDOG_TIMER_H__
#include <signal.h>
#include <time.h>

struct watchdog_timer {
	timer_t timerid;
	struct sigevent sev;
	struct itimerspec its;
	int interval;
	int created;
};

/*
 * software_watchdog_init - init and create a watchog timer
 * @t: watchdog timer structure
 *
 * This function just init the watchdog structure but don't
 * start the watchdog.
 *
 * When watchdog expired, it means something bad happend,
 * so raise an SIGABRT to kill the process.
 *
 * This watchdog library is based on SIGABRT, so shouldn't
 * block this signal.
 * */
int software_watchdog_init(struct watchdog_timer *t);

/*
 * software_watchdog_reset - reset watchdog timeout time
 * @t: watchdog timer structure
 * @second: update watchdog timeout to seconds later
 * */
int software_watchdog_reset(struct watchdog_timer *t, int second);

/*
 * software_watchdog_start - start a watchdog
 * @t: watchdog timer structure
 * @second: set watchdog timeout to seconds later
 *
 * This function will start the watchdog to count time.
 *
 * */
int software_watchdog_start(struct watchdog_timer *t, int second);

/*
 * software_watchdog_feed - feed a watchdog
 * @t: watchdog timer structure
 *
 * This function will reset watchdog to seconds later.
 * This timeout value is the value set by init / reset.
 *
 * */
int software_watchdog_feed(struct watchdog_timer *t);

/*
 * software_watchdog_remove - remove a watchdog timer
 * @t: watchdog timer structure
 *
 * Remove the watchdog descriptor when not used.
 * */
void software_watchdog_remove(struct watchdog_timer *t);

#endif //__WATCHDOG_TIMER_H__

#ifdef __cplusplus
}
#endif
