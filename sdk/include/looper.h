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

#ifndef __LOOPER_H__
#define __LOOPER_H__

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "list_node.h"

/*
* Callbacks which are needed to construct a looper.
*/
typedef void (*msg_handler)(void *data);
typedef void (*msg_free)(void *data);

/*
* msg_entity - message entity structure in message list
*/
struct msg_entity {
	struct list_node node;
	uint32_t msg_id;
	void *data;
};

/*
* looper - core structure for looper
*
* Use this structure to call start/stop/dispatch operations.
*
*/
struct looper {
	char name[128];
	int (*start)(struct looper *looper);
	int (*stop)(struct looper *looper);
	void (*dispatch)(struct looper *looper, void  *data);
	struct list_node head;
	uint32_t msg_id;
	bool running;
	pthread_mutex_t lock;
	pthread_cond_t condition;
	pthread_t tid;
	msg_handler loop_cb;
	msg_free free_cb;
};

/*
* looper_create - create looper structure
*
* Must create a looper structure to use looper operations.
*
*/
struct looper *looper_create(msg_handler loop_cb, msg_free free_cb, const char *name);

/*
* looper_destory - destory looper structure
*
* Destory looper structure when not used.
*
*/
void looper_destory(struct looper *looper);

#endif/*__LOOPER_H__*/

#ifdef __cplusplus
}
#endif
