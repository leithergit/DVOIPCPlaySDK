/*
 * main.c
 *
 *  Created on: 2011-9-18
 *      Author: wudegang
 */

#include "utils.h"
//#include "queue.h"
#include <pthread.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>


NewQueue recvqueue;
UdpParam udpParam;

int read_data(void *opaque, uint8_t *buf, int buf_size) {
//	UdpParam udphead;
	int size = buf_size;
	int ret;
//	printf("read data %d\n", buf_size);
	do {
		ret = get_queue(&recvqueue, buf, buf_size);
	} while (ret);

//	printf("read data Ok %d\n", buf_size);
	return size;
}

#define BUF_SIZE	4096*500

int main(int argc, char** argv) {

	if (argc != 2) {
		printf("Usage:%s [port]\n", argv[0]);
		return -1;
	}
	
	init_queue(&recvqueue, 1024*1024*10);

	udpParam.argv = argv;
	udpParam.queue = &recvqueue;
	uint8_t *buf = av_mallocz(sizeof(uint8_t)*BUF_SIZE);


#if 0
	udp_ts_recv(&udpParam);
#else
	pthread_t udp_recv_thread;
	pthread_create(&udp_recv_thread, NULL, udp_ts_recv, &udpParam);
	pthread_detach(udp_recv_thread);
#endif


	av_register_all();

	AVCodec *pVideoCodec, *pAudioCodec;
	AVCodecContext *pVideoCodecCtx = NULL;
	AVCodecContext *pAudioCodecCtx = NULL;
	AVIOContext * pb = NULL;
	AVInputFormat *piFmt = NULL;
	AVFormatContext *pFmt = NULL;

	pb = avio_alloc_context(buf, BUF_SIZE, 0, NULL, read_data, NULL, NULL);
	if (!pb) {
		fprintf(stderr, "avio alloc failed!\n");
		return -1;
	}

	if (av_probe_input_buffer(pb, &piFmt, "", NULL, 0, 0) < 0) {
		fprintf(stderr, "probe failed!\n");
//			return -1;
	} else {
		fprintf(stdout, "probe success!\n");
		fprintf(stdout, "format: %s[%s]\n", piFmt->name, piFmt->long_name);
	}

	pFmt = avformat_alloc_context();
	pFmt->pb = pb;
	if (avformat_open_input(&pFmt, "", piFmt, NULL) < 0) {
		fprintf(stderr, "avformat open failed.\n");
		return -1;
	} else {
		fprintf(stdout, "open stream success!\n");
	}

	if (av_find_stream_info(pFmt) < 0) {
		fprintf(stderr, "could not fine stream.\n");
		return -1;
	}

	av_dump_format(pFmt, 0, "", 0);

	int videoindex = -1;
	int audioindex = -1;
	for (int i = 0; i < pFmt->nb_streams; i++) {
		if ( (pFmt->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) &&
				(videoindex < 0) ) {
			videoindex = i;
		}
		if ( (pFmt->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) &&
				(audioindex < 0) ) {
			audioindex = i;
		}
	}

	if (videoindex < 0 || audioindex < 0) {
		fprintf(stderr, "videoindex=%d, audioindex=%d\n", videoindex, audioindex);
		return -1;
	}

	AVStream *pVst,*pAst;
	pVst = pFmt->streams[videoindex];
	pAst = pFmt->streams[audioindex];

	pVideoCodecCtx = pVst->codec;
	pAudioCodecCtx = pAst->codec;

	pVideoCodec = avcodec_find_decoder(pVideoCodecCtx->codec_id);
	if (!pVideoCodec) {
		fprintf(stderr, "could not find video decoder!\n");
		return -1;
	}
	if (avcodec_open(pVideoCodecCtx, pVideoCodec) < 0) {
		fprintf(stderr, "could not open video codec!\n");
		return -1;
	}

	pAudioCodec = avcodec_find_decoder(pAudioCodecCtx->codec_id);
	if (!pAudioCodec) {
		fprintf(stderr, "could not find audio decoder!\n");
		return -1;
	}
	if (avcodec_open(pAudioCodecCtx, pAudioCodec) < 0) {
		fprintf(stderr, "could not open audio codec!\n");
		return -1;
	}

	int got_picture;
	uint8_t samples[AVCODEC_MAX_AUDIO_FRAME_SIZE*3/2];
	AVFrame *pframe = avcodec_alloc_frame();
	AVPacket pkt;
	av_init_packet(&pkt);

	while(1) {
		if (av_read_frame(pFmt, &pkt) >= 0) {

			if (pkt.stream_index == videoindex) {
				avcodec_decode_video2(pVideoCodecCtx, pframe, &got_picture, &pkt);
				if (got_picture) {
					fprintf(stdout, "decode one video frame!\r");
				}
			}else if (pkt.stream_index == audioindex) {
				int frame_size = AVCODEC_MAX_AUDIO_FRAME_SIZE*3/2;
				if (avcodec_decode_audio3(pAudioCodecCtx, (int16_t *)samples, &frame_size, &pkt) >= 0) {
					fprintf(stdout, "decode one audio frame!\r");
				}
			}
			av_free_packet(&pkt);
		}
	}

	av_free(buf);
	av_free(pframe);
	free_queue(&recvqueue);
	return 0;
}

