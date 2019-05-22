 /*
 * Mini work queue/ msg queue implementation
 *
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

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <pthread.h>
#include "work_queue.h"


/*
 * insert job to a queue head
*/
int job_add_head(struct work_queue *queue, struct job *job)
{
	pthread_mutex_lock(&queue->lock);
	job->next = queue->head;
	job->prev = NULL;

	if (queue->head != NULL)
		/*add job to a no empty queue*/
		queue->head->prev = job;
	else
		/*add job to a empty queue*/
		queue->tail = job;
	queue->head = job;
	queue->member++;
	pthread_mutex_unlock(&queue->lock);
	pthread_cond_signal(&queue->cond);
	return 0;
}

/*
 * add job to a queue tail
*/
int job_add_tail(struct work_queue *queue, struct job *job)
{

	pthread_mutex_lock(&queue->lock);

	job->next = NULL;
	job->prev = queue->tail;

	if (queue->tail != NULL)
		queue->tail->next = job;
	else
		queue->head = job;
	queue->tail = job;
	queue->member++;
	pthread_mutex_unlock(&queue->lock);
	pthread_cond_signal(&queue->cond);
	return 0;
}

/*
* find a job from queue head, if queue is empty, block until find one job
*/
struct job *job_find_head_wait(struct work_queue *queue)
{
	struct job *job = NULL;
	pthread_mutex_lock(&queue->lock);
	while (queue->head == NULL)
		pthread_cond_wait(&queue->cond, &queue->lock);
	job = queue->head;
	pthread_mutex_unlock(&queue->lock);
	return job;
}

/*
* find a job from queue head, if queue is empty, return null
*/
struct job *job_find_head_nowait(struct work_queue *queue)
{
	struct job *job = NULL;
	pthread_mutex_lock(&queue->lock);
	job = queue->head;
	pthread_mutex_unlock(&queue->lock);
	return job;
}

/*
* remove the given job from a queue
*/

void job_remove(struct work_queue *queue, struct job *job)
{
	pthread_mutex_lock(&queue->lock);
	if (job == queue->head){
		queue->head = job->next;
		if (queue->tail == job)
			queue->tail = NULL;
		else
			job->next->prev = job->prev;
	} else if (job == queue->tail) {
		queue->tail = job->prev;
		job->prev->next = job->next;
	} else {
		job->prev->next = job->next;
		job->next->prev = job->prev;
	}
	queue->member--;
	pthread_mutex_unlock(&queue->lock);

}

struct job *job_alloc(int len)
{
	struct job *job;
	job = (struct job *)malloc(sizeof(struct job));
	if (job == NULL)
		return NULL;
	job->data = (char *)malloc(len);
	if (job->data == NULL) {
		free(job);
		return NULL;
	}
	job->length = len;
	return job;
}

void job_free(struct job *job)
{
	if (!job) {
		free(job->data);
		free(job);
	}
}

/*
 * initialize a queue
*/
int queue_init(struct work_queue *queue)
{
	int err;
	queue->head = NULL;
	queue->tail = NULL;
	queue->member = 0;
	err = pthread_mutex_init(&queue->lock, NULL);
	if (err != 0)
		return -1;
	err = pthread_cond_init(&queue->cond, NULL);
	if (err != 0)
		return -1;
	return 0;
}

void queue_deinit(struct work_queue *queue)
{
	struct job *job = NULL;
	pthread_mutex_lock(&queue->lock);
	while(queue->head) {
		job = queue->head;
		queue->head = job->next;
		if (queue->tail == job)
			queue->tail = NULL;
		else
			job->next->prev = job->prev;
		job_free(job);
	}
	queue->member = 0;
	pthread_mutex_unlock(&queue->lock);
}

bool queue_is_empty(struct work_queue *queue)
{
	bool rc;
	pthread_mutex_lock(&queue->lock);
	if (queue->head == NULL && queue->tail == NULL)
		rc = true;
	else
		rc = false;
	pthread_mutex_unlock(&queue->lock);
	return rc;
}
