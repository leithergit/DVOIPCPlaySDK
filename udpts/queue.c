/*
 * queue.c
 *
 *  Created on: Aug 6, 2011
 *      Author: root
 */
#include "queue.h"
#include <libavutil/avutil.h>
#include <sys/time.h>

void initQueue(Queue *que, long memberSize, int queSize) {
	pthread_mutex_init(&que->locker, NULL);
//	pthread_mutex_init(&que->cond, NULL);
	pthread_cond_init(&que->cond, NULL);
	que->firstIndex = -1;
	que->lastIndex = -1;
	que->writableIndex = 0;
	que->memberSize = memberSize;
	que->queSize = queSize;
	que->queMem = av_malloc(memberSize * queSize);
}

void freeQueue(Queue *que)  {
	pthread_mutex_destroy(&que->locker);
	pthread_cond_destroy(&que->cond);
	av_free(que->queMem);
}

void printQueueHeader(Queue *que) {
	VideoData *data;
	for (int i = 0; i<que->queSize; i++) {
		data = (VideoData *)&que->queMem[i * que->memberSize];
		printf("queue[%d] = %ld\n", i, data->size);
	}
}

void addQueue(Queue *que, void *mem, long size) {
	long writePtr;
	pthread_mutex_lock(&que->locker);
	writePtr = que->writableIndex * que->memberSize;
	memcpy((void *)&que->queMem[writePtr], mem, size);
	if (que->firstIndex == -1) {
		que->firstIndex = que->lastIndex = que->writableIndex;
	} else {
		que->lastIndex = que->writableIndex;
		if (que->lastIndex == que->firstIndex) {
			que->firstIndex = (que->firstIndex + 1) % que->queSize;
		}
	}
	que->writableIndex = (que->writableIndex + 1) % que->queSize;
	pthread_cond_signal(&que->cond);
	pthread_mutex_unlock(&que->locker);
}

void addQueue2(Queue *que, void *mem1, long size1, void *mem2, long size2) {
	long writePtr;
	pthread_mutex_lock(&que->locker);
	writePtr = que->writableIndex * que->memberSize;
	memcpy((void *)&que->queMem[writePtr], mem1, size1);
	memcpy((void *)&que->queMem[writePtr + size1], mem2, size2);
	if (que->firstIndex == -1) {
		que->firstIndex = que->lastIndex = que->writableIndex;
	} else {
		que->lastIndex = que->writableIndex;
		if (que->lastIndex == que->firstIndex) {
			que->firstIndex = (que->firstIndex + 1) % que->queSize;
		}
	}
	que->writableIndex = (que->writableIndex + 1) % que->queSize;
	pthread_cond_signal(&que->cond);
	pthread_mutex_unlock(&que->locker);
}

int getQueue(Queue *que, void *header, long headersize, void *mem, long *sizeptr) {
	uint64_t startPtr;
//	struct timespec ts;
//	ts.tv_sec = 0;
//	ts.tv_nsec = 1000000L;
	pthread_mutex_lock(&que->locker);
	if (que->firstIndex == -1) {
		struct timespec timeout;
		timeout.tv_sec=time(0)+1;
		timeout.tv_nsec=0;
//		nanosleep(&ts, NULL);
		pthread_cond_timedwait(&que->cond, &que->locker, &timeout);
		if (que->firstIndex == -1) {
			pthread_mutex_unlock(&que->locker);
			return 1;
		}
	}
	startPtr = que->firstIndex * que->memberSize;
	memcpy(header, &que->queMem[startPtr], headersize);
	memcpy(mem, &que->queMem[startPtr + headersize], *sizeptr);
	if (que->firstIndex == que->lastIndex) {
		que->firstIndex = -1;
		que->lastIndex = -1;
	} else {
		que->firstIndex = (que->firstIndex + 1) % que->queSize;
	}
	pthread_mutex_unlock(&que->locker);
	return 0;
}
