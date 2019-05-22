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

#ifndef __DEBUG__
#define __DEBUG__

#include <stdio.h>
#include <stdlib.h>

#define pr_debug(fmt,...)   do{ printf("[%ld] DEBUG: "fmt,(long)getpid(),##__VA_ARGS__); fflush(stdout); }while(0)
#define pr_info(fmt,...)    do{ printf("[%ld] INFO:  "fmt,(long)getpid(),##__VA_ARGS__); fflush(stdout); }while(0)
#define pr_err(fmt,...)   	do{ printf("[%ld] ERROR: "fmt,(long)getpid(),##__VA_ARGS__); fflush(stdout); }while(0)
#define err_exit(fmt,...)   do{ printf("[%ld] ERROR: "fmt,(long)getpid(),##__VA_ARGS__); exit(1); }while(0)

#endif //__DEBUG__

#ifdef __cplusplus
}
#endif
