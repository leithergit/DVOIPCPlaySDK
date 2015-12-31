/*
 * utils.c
 *
 *  Created on: 2011-9-18
 *      Author: wudegang
 */

#include "utils.h"
#include "libavutil/avutil.h"

void init_queue(NewQueue *que, int size) {
	pthread_mutex_init(&que->locker, NULL);
	pthread_cond_init(&que->cond, NULL);
	que->buf = (uint8_t*)av_mallocz(sizeof(uint8_t)*size);
	que->read_ptr = que->write_ptr = 0;
	que->bufsize = size;
//	fprintf(stdout, "que->bufsize=%d\n", size);
}

void free_queue(NewQueue* que) {
	pthread_mutex_destroy(&que->locker);
	pthread_cond_destroy(&que->cond);
	av_free(que->buf);
}

void put_queue(NewQueue*que, uint8_t* buf, int size) {
	uint8_t* dst = que->buf + que->write_ptr;

	pthread_mutex_lock(&que->locker);

	if ((que->write_ptr + size) > que->bufsize) {
		memcpy(dst, buf, (que->bufsize - que->write_ptr));
		memcpy(que->buf, buf+(que->bufsize - que->write_ptr), size-(que->bufsize - que->write_ptr));
	} else {
		memcpy(dst, buf, size*sizeof(uint8_t));
	}
	que->write_ptr = (que->write_ptr + size) % que->bufsize;

	pthread_cond_signal(&que->cond);
	pthread_mutex_unlock(&que->locker);
}

int get_queue(NewQueue*que, uint8_t* buf, int size) {
	uint8_t* src = que->buf + que->read_ptr;
	int wrap = 0;

	pthread_mutex_lock(&que->locker);

	int pos = que->write_ptr;

	if (pos < que->read_ptr) {
		pos += que->bufsize;
		wrap = 1;
	}

	if ( (que->read_ptr + size) > pos) {
		pthread_mutex_unlock(&que->locker);
		return 1;
//		struct timespec timeout;
//		timeout.tv_sec=time(0)+1;
//		timeout.tv_nsec=0;
//		pthread_cond_timedwait(&que->cond, &que->locker, &timeout);
//		if ( (que->read_ptr + size) > pos ) {
//			pthread_mutex_unlock(&que->locker);
//			return 1;
//		}
	}

	if (wrap) {
		fprintf(stdout, "wrap...\n");
		memcpy(buf, src, (que->bufsize - que->read_ptr));
		memcpy(buf+(que->bufsize - que->read_ptr), src+(que->bufsize - que->read_ptr), size-(que->bufsize - que->read_ptr));
	} else {
		memcpy(buf, src, sizeof(uint8_t)*size);
	}
	que->read_ptr = (que->read_ptr + size) % que->bufsize;
	pthread_mutex_unlock(&que->locker);

	return 0;
}

void* udp_ts_recv(/*int argc, char** argv*/void *param) {
	struct sockaddr_in s_addr;
	struct sockaddr_in c_addr;
	int sock;
	socklen_t addr_len;
	int len;
	uint8_t buf[4096];
//	FILE *fp;

	char** argv = ((UdpParam*)param)->argv;
	NewQueue *queue = ((UdpParam*)param)->queue;

	UdpParam udphead;
	udphead.argv = argv;
	udphead.queue = queue;


	printf("argv[1]:%s\n", argv[1]);


//	fp = fopen("out.ts", "w");

	if ( (sock = socket(AF_INET, SOCK_DGRAM, 0))  == -1) {
		perror("socket");
		exit(errno);
	} else
		printf("create socket.\n\r");

	memset(&s_addr, 0, sizeof(struct sockaddr_in));
	s_addr.sin_family = AF_INET;
	if (argv[1])
		s_addr.sin_port = htons(atoi(argv[1]));
	else
		s_addr.sin_port = htons(7838);


	s_addr.sin_addr.s_addr = INADDR_ANY;

	if ( (bind(sock, (struct sockaddr*)&s_addr, sizeof(s_addr))) == -1 ) {
		perror("bind");
		exit(errno);
	}else
		printf("bind address to socket.\n\r");

	addr_len = sizeof(struct sockaddr);
	while(1) {
		len = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr*)&c_addr, &addr_len);
		if (len < 0) {
			perror("recvfrom");
			exit(errno);
		}
//		fwrite(buf, sizeof(char), len, fp);
		udphead.size = len;
//		printf("recv from %s:%d,msg len:%d\n\r", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port), len);
		put_queue(queue, buf, len);
	}
//	fclose(fp);
	return NULL;
}


