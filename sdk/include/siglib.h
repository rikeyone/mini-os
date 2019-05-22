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

#ifndef __SIGLIB_H__
#define __SIGLIB_H__
#include <signal.h>

typedef void (*sigfunc)(int signo);

/*
 * set_signal_thread - create a thread to handle signal
 * @func: signal handler set by caller.
 *
 * NOTE:
 * This function must be called before any new thread
 * is created.
 *
 * This function can handle all signals except:
 * SIGKILL,SIGSTOP,SIGFPE,SIGILL,SIGSEGV,SIGBUS
 * these signals above will doing the default handler.
 * */
void set_signal_thread(sigfunc func);

/*
 * set_signal - set process signal handler by sigaction
 * @func: signal handler set by caller.
 */
int set_signal(int signo, sigfunc func);

#endif //__MINIOS_SIGNAL_H__

#ifdef __cplusplus
}
#endif
