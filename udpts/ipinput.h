/*
 * ipinput.h
 *
 *  Created on: 2011-9-18
 *      Author: wudegang
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "queue.h"

#ifndef IPINPUT_H_
#define IPINPUT_H_

typedef struct _udp_param {
	char** argv;
	Queue *queue;
} UdpParam;

void* udp_ts_recv(void* param);

#endif /* IPINPUT_H_ */
