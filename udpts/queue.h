/*
 * queue.h
 *
 *  Created on: Aug 6, 2011
 *      Author: root
 */

#ifndef QUEUE_H_
#define QUEUE_H_
#include <pthread.h>
#include <stdint.h>

typedef struct _queue {
	pthread_mutex_t locker;
	pthread_cond_t cond;
	long memberSize;
	int queSize;
	uint8_t *queMem;
	int firstIndex;
	int lastIndex;
	int writableIndex;
} Queue;

void initQueue(Queue *que, long memberSize, int queSize);

void freeQueue(Queue *que);

void printQueueHeader(Queue *que);

void addQueue(Queue *que, void *mem, long size);

void addQueue2(Queue *que, void *mem1, long size1, void *mem2, long size2);

int getQueue(Queue *que, void *header, long headersize, void *mem, long *sizeptr);

typedef struct _VideoData {
	void *data;
	long size;
	uint64_t pts;
} VideoData;

typedef struct _AudioData {
	void *data;
	long size;
	uint64_t pts;
} AudioData;

#endif /* QUEUE_H_ */
