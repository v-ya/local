#include "container.jpeg.h"

static struct media_stream_s* media_container_jpeg_set_bpc_and_channel(struct media_stream_s *restrict s, uint32_t bpc, uint32_t channel)
{
	if (media_attr_set_int(s->attr, media_nasi_bpc, bpc) &&
		media_attr_set_int(s->attr, media_nasi_jpeg_channel, channel))
		return s;
	return NULL;
}

static d_media_stream__append_pre(jpeg_yuv_8)
{
	if (media_container_jpeg_set_bpc_and_channel(s, 8, 3))
		return c;
	return NULL;
}

static d_media_stream__read_frame(jpeg_ozp)
{
	const struct media_stack_s *restrict stack;
	const struct media_stack__ozp_t *restrict sp;
	const struct mi_jpeg_frame_info_s *restrict fi;
	struct media_io_s *restrict io;
	uintptr_t dv[4];
	stack = s->stack;
	if ((sp = (const struct media_stack__ozp_t *) media_stack_get(stack, index)) &&
		(fi = (const struct mi_jpeg_frame_info_s *) sp->pri))
	{
		dv[0] = (uintptr_t) fi->frame->channel;
		dv[1] = fi->q_size;
		dv[2] = fi->h_size;
		dv[3] = sp->size;
		if (media_frame_set_dimension(frame, 4, dv) && frame->channel == 5 &&
			media_channel_set_data(frame->channel_chip[0], fi->frame, sizeof(*fi->frame), fi->frame) &&
			media_channel_set_data(frame->channel_chip[1], fi->ch, sizeof(*fi->ch) * fi->frame->channel, fi->ch) &&
			media_channel_set_data(frame->channel_chip[2], fi->q, fi->q_size, fi->q) &&
			media_channel_set_data(frame->channel_chip[3], fi->h, fi->h_size, fi->h) &&
			frame->channel_chip[4]->size == sp->size &&
			media_frame_touch_data(frame))
		{
			io = s->inner->io;
			if (media_io_offset(io, &sp->offset) == sp->offset &&
				media_io_read(io, frame->channel_chip[4]->data, sp->size))
				return frame;
		}
	}
	return NULL;
}

// static d_media_stream__write_frame(jpeg_ozp)
// {
// 	volatile struct media_container_pri_jpeg_s *pri;
// 	const struct media_channel_s *restrict ch;
// 	struct media_container_inner_s *restrict ci;
// 	struct media_stack__oz_t oz;
// 	uintptr_t w, h;
// 	ci = s->inner;
// 	pri = (struct media_container_pri_jpeg_s *) ci->pri_data;
// 	ch = frame->channel_chip[0];
// 	if (!s->stack->stack_number && (w = frame->dv[0]) && (h = frame->dv[1]))
// 	{
// 		if (!pri->width) media_attr_set_int(ci->attr, media_naci_width, (int64_t) w);
// 		if (!pri->height) media_attr_set_int(ci->attr, media_naci_height, (int64_t) h);
// 		if ((uintptr_t) pri->width == w && (uintptr_t) pri->height == h &&
// 			media_io_inner_padding_align(ci->io, 0, (uintptr_t) pri->pixels_align, 0))
// 		{
// 			oz.offset = media_io_offset(ci->io, NULL);
// 			oz.size = ch->size;
// 			if (media_io_write(ci->io, ch->data, oz.size) == oz.size &&
// 				media_stack_push(s->stack, &oz))
// 			{
// 				pri->pixel_offset = (uint32_t) oz.offset;
// 				pri->image_size = (uint32_t) oz.size;
// 				return s;
// 			}
// 		}
// 	}
// 	return NULL;
// }

static d_media_stream__write_frame(jpeg_ozp) { return NULL; }

d_media_stream__spec_append(jpeg)
{
	static const struct media_stream_spec_param_t spec[] = {
		{
			.stream_type = media_st_image,
			.stack_layout = media_sl_ozp,
			.frame_id_name = media_nf_jpeg_yuv_8_411,
			.initial_judge = media_stream_symbol(initial_judge, jpeg),
			.create_pri = media_stream_symbol(create_pri, jpeg),
			.append_pre = media_stream_symbol(append_pre, jpeg_yuv_8),
			.read_frame = media_stream_symbol(read_frame, jpeg_ozp),
			.write_frame = media_stream_symbol(write_frame, jpeg_ozp),
		},
		{
			.stream_type = media_st_image,
			.stack_layout = media_sl_ozp,
			.frame_id_name = media_nf_jpeg_yuv_8_111,
			.initial_judge = media_stream_symbol(initial_judge, jpeg),
			.create_pri = media_stream_symbol(create_pri, jpeg),
			.append_pre = media_stream_symbol(append_pre, jpeg_yuv_8),
			.read_frame = media_stream_symbol(read_frame, jpeg_ozp),
			.write_frame = media_stream_symbol(write_frame, jpeg_ozp),
		}
	};
	return media_stream_spec_append(stream_spec, media, spec, sizeof(spec) / sizeof(*spec));
}
