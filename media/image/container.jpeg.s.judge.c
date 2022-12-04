#include "container.jpeg.h"

static d_media_attr_set(jpeg, width, struct media_stream_pri_jpeg_s)
{
	pri->width = (uint32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(jpeg, width, struct media_stream_pri_jpeg_s)
{
	pri->width = 0;
}
static d_media_attr_set(jpeg, height, struct media_stream_pri_jpeg_s)
{
	pri->height = (uint32_t) value->value.av_int;
	return attr;
}
static d_media_attr_unset(jpeg, height, struct media_stream_pri_jpeg_s)
{
	pri->height = 0;
}
static d_media_attr_set(jpeg, bpc, struct media_stream_pri_jpeg_s)
{
	uint32_t bpc;
	switch ((bpc = (uint32_t) value->value.av_int))
	{
		case 8:
		case 10:
		case 12:
			pri->bpc = bpc;
			return attr;
		default: return NULL;
	}
}
static d_media_attr_unset(jpeg, bpc, struct media_stream_pri_jpeg_s)
{
	pri->bpc = 8;
}
static d_media_attr_set(jpeg, channel, struct media_stream_pri_jpeg_s)
{
	uint32_t channel;
	switch ((channel = (uint32_t) value->value.av_int))
	{
		case 3:
			pri->channel = channel;
			return attr;
		default: return NULL;
	}
}
static d_media_attr_unset(jpeg, channel, struct media_stream_pri_jpeg_s)
{
	pri->channel = 3;
}

d_media_stream__initial_judge(jpeg)
{
	if (d_media_attr_judge_add(judge, jpeg, width, media_nasi_width, uint32) &&
		d_media_attr_judge_add(judge, jpeg, height, media_nasi_height, uint32) &&
		d_media_attr_judge_add(judge, jpeg, bpc, media_nasi_bpc, uint32) &&
		d_media_attr_judge_add(judge, jpeg, channel, media_nasi_jpeg_channel, uint32)
	) return judge;
	return NULL;
}
