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

#ifndef __TIMER_WRAPPER_H__
#define __TIMER_WRAPPER_H__
#include <signal.h>
#include <time.h>
#include <stdint.h>

#define MAX_USEC_PER_SECOND 1000000

enum {
	PERIODIC_TIMER = 0,
	ONESHOT_TIMER
};


typedef void (*timer_cb) (void *data);

struct timer_wrapper {
	timer_t timerid;
	struct sigevent sev;
	struct itimerspec its;
	int created;
};


/*
 * timer_start - init a timer
 *
 * */
int timer_init(struct timer_wrapper *t, timer_cb func, void *data);
/*
 * timer_start - start a timer
 *
 * */
int timer_start(struct timer_wrapper *t, uint64_t usec, uint32_t oneshot);
/*
 * timer_stop - stop a timer
 *
 * */
int timer_stop(struct timer_wrapper *t);
/*
 * timer_remove - stop and delete a timer
 *
 * */
void timer_remove(struct timer_wrapper *t);

#endif //__TIMER_WRAPPER_H__

#ifdef __cplusplus
}
#endif
