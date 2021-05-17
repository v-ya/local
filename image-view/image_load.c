#include "image_load.h"
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

static const uint32_t* image_load(const char *restrict path, uint32_t *restrict width, uint32_t *restrict height)
{
	const uint32_t *restrict r;
	uint32_t *restrict data;
	AVFormatContext *format;
	AVCodec *codec;
	AVCodecContext *codecer;
	AVPacket *packet;
	AVFrame *frame;
	struct SwsContext *sws;
	uintptr_t linesize, pad;
	int stream_index, rdecode;
	r = data = NULL;
	format = NULL;
	codec = NULL;
	codecer = NULL;
	packet = NULL;
	frame = NULL;
	sws = NULL;
	av_log_set_level(AV_LOG_ERROR);
	if (avformat_open_input(&format, path, NULL, NULL))
		goto label_fail;
	if (!format)
		goto label_fail;
	stream_index = av_find_best_stream(format, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
	if (stream_index < 0)
		goto label_fail;
	if (!codec)
		goto label_fail;
	if (!(codecer = avcodec_alloc_context3(codec)))
		goto label_fail;
	if (avcodec_open2(codecer, codec, NULL))
		goto label_fail;
	if (!(packet = av_packet_alloc()))
		goto label_fail;
	if (!(frame = av_frame_alloc()))
		goto label_fail;
	rdecode = AVERROR(EAGAIN);
	do {
		if (av_read_frame(format, packet))
			goto label_fail;
		if (packet->stream_index == stream_index)
		{
			if (avcodec_send_packet(codecer, packet))
				goto label_fail;
			rdecode = avcodec_receive_frame(codecer, frame);
			if (rdecode && rdecode != AVERROR(EAGAIN))
				goto label_fail;
		}
		av_packet_unref(packet);
	} while (rdecode);
	if (frame->width <= 0 || frame->height <= 0)
		goto label_fail;
	linesize = (*width = (uint32_t) frame->width) * sizeof(uint32_t);
	*height = (uint32_t) frame->height;
	pad = ((linesize + 63) & ~63) - linesize;
	if (!(data = malloc(linesize * (size_t) frame->height + pad)))
		goto label_fail;
	if (!(sws = sws_getContext(
		frame->width, frame->height, frame->format,
		frame->width, frame->height, AV_PIX_FMT_BGRA,
		SWS_BICUBIC, NULL, NULL, NULL)))
		goto label_fail;
	rdecode = frame->width * sizeof(uint32_t);
	if (sws_scale(sws, (const uint8_t **) frame->data, frame->linesize,
		0, frame->height, (uint8_t **) &data, &rdecode) != frame->height)
		goto label_fail;
	av_frame_unref(frame);
	r = data;
	data = NULL;
	label_fail:
	if (data) free(data);
	if (sws) sws_freeContext(sws);
	if (frame) av_frame_free(&frame);
	if (packet) av_packet_free(&packet);
	if (codecer) avcodec_free_context(&codecer);
	if (format) avformat_close_input(&format);
	return r;
}

static void image_bgra_free_func(image_bgra_s *restrict r)
{
	if (r->data)
		free((void *) r->data);
}

image_bgra_s* image_bgra_alloc(const char *restrict path)
{
	image_bgra_s *restrict r;
	r = (image_bgra_s *) refer_alloz(sizeof(image_bgra_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) image_bgra_free_func);
		r->data = image_load(path, &r->width, &r->height);
		r->pixels = r->width * r->height;
		if (r->data)
			return r;
		refer_free(r);
	}
	return NULL;
}
