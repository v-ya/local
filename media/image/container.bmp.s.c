#include "container.bmp.h"

static struct media_container_s* media_container_bmp_set_bits_per_pixels(struct media_container_s *restrict c, int64_t value)
{
	struct media_container_inner_s *restrict ci;
	ci = c->inner;
	if (media_attr_set_int(ci->attr, media_nai_bpp, value) &&
		media_attr_set_int(ci->attr, media_nai_bmp_compression, 0))
		return c;
	return NULL;
}

static d_media_stream__append_pre(bmp_bgra24)
{
	return media_container_bmp_set_bits_per_pixels(c, 24);
}

static d_media_stream__append_pre(bmp_bgra32)
{
	return media_container_bmp_set_bits_per_pixels(c, 32);
}

static d_media_stream__read_frame(bmp_oz)
{
	const struct media_container_pri_bmp_s *restrict pri;
	const struct media_stack_s *restrict stack;
	const struct media_stack__oz_t *restrict sp;
	struct media_io_s *restrict io;
	uintptr_t image_size[2];
	pri = (const struct media_container_pri_bmp_s *) s->inner->pri_data;
	stack = s->stack;
	image_size[0] = (uintptr_t) pri->width;
	image_size[1] = (uintptr_t) pri->height;
	if (media_frame_test_dimension(frame, 2, image_size) || media_frame_set_dimension(frame, 2, image_size))
	{
		if ((sp = (const struct media_stack__oz_t *) media_stack_get(stack, index)) &&
			frame->channel_chip[0]->size == sp->size &&
			media_frame_touch_data(frame))
		{
			io = s->inner->io;
			if (media_io_offset(io, &sp->offset) == sp->offset &&
				media_io_read(io, frame->channel_chip[0]->data, sp->size))
				return frame;
		}
	}
	return NULL;
}

// static d_media_stream__write_frame(bmp_oz)
// {
// 	;
// }

d_media_stream__spec_append(bmp)
{
	static const struct media_stream_spec_param_t spec[] = {
		{
			.stream_type = media_st_image,
			.stack_layout = media_sl_oz,
			.frame_id_name = media_nf_bgr24_p4,
			.initial_judge = NULL,
			.create_pri = NULL,
			.append_pre = media_stream_symbol(append_pre, bmp_bgra24),
			.read_frame = media_stream_symbol(read_frame, bmp_oz),
			.write_frame = NULL,
		},
		{
			.stream_type = media_st_image,
			.stack_layout = media_sl_oz,
			.frame_id_name = media_nf_bgra32,
			.initial_judge = NULL,
			.create_pri = NULL,
			.append_pre = media_stream_symbol(append_pre, bmp_bgra32),
			.read_frame = media_stream_symbol(read_frame, bmp_oz),
			.write_frame = NULL,
		}
	};
	return media_stream_spec_append(stream_spec, media, spec, sizeof(spec) / sizeof(*spec));
}
