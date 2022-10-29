#include "container.bmp.h"

static inline struct media_container_s* media_container_bmp_check_stream(struct media_container_s *restrict c, struct media_stream_s *restrict s)
{
	if (!vattr_get_vslot_number(c->stream, s->spec->stream_type))
		return c;
	return NULL;
}

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
	if (media_container_bmp_check_stream(c, s))
		return media_container_bmp_set_bits_per_pixels(c, 24);
	return NULL;
}

static d_media_stream__append_pre(bmp_bgra32)
{
	if (media_container_bmp_check_stream(c, s))
		return media_container_bmp_set_bits_per_pixels(c, 32);
	return NULL;
}

static d_media_stream__read_frame(bmp_oz)
{
	const struct media_container_pri_bmp_s *restrict pri;
	const struct media_stack_s *restrict stack;
	const struct media_stack__oz_t *restrict sp;
	struct media_io_s *restrict io;
	pri = (const struct media_container_pri_bmp_s *) s->inner->pri_data;
	stack = s->stack;
	if (media_container_inner_iframe_touch_data(frame, (uintptr_t) pri->width, (uintptr_t) pri->height))
	{
		if ((sp = (const struct media_stack__oz_t *) media_stack_get(stack, index)) &&
			frame->channel_chip[0]->size == sp->size)
		{
			io = s->inner->io;
			if (media_io_offset(io, &sp->offset) == sp->offset &&
				media_io_read(io, frame->channel_chip[0]->data, sp->size))
				return frame;
		}
	}
	return NULL;
}

static d_media_stream__write_frame(bmp_oz)
{
	volatile struct media_container_pri_bmp_s *pri;
	const struct media_channel_s *restrict ch;
	struct media_container_inner_s *restrict ci;
	struct media_stack__oz_t oz;
	uintptr_t w, h;
	ci = s->inner;
	pri = (struct media_container_pri_bmp_s *) ci->pri_data;
	ch = frame->channel_chip[0];
	if (!s->stack->stack_number && (w = frame->dv[0]) && (h = frame->dv[1]))
	{
		if (!pri->width) media_attr_set_int(ci->attr, media_nai_width, (int64_t) w);
		if (!pri->height) media_attr_set_int(ci->attr, media_nai_height, (int64_t) h);
		if ((uintptr_t) pri->width == w && (uintptr_t) pri->height == h &&
			media_io_inner_padding_align(ci->io, 0, (uintptr_t) pri->pixels_align, 0))
		{
			oz.offset = media_io_offset(ci->io, NULL);
			oz.size = ch->size;
			if (media_io_write(ci->io, ch->data, oz.size) == oz.size &&
				media_stack_push(s->stack, &oz))
			{
				pri->pixel_offset = (uint32_t) oz.offset;
				pri->image_size = (uint32_t) oz.size;
				return s;
			}
		}
	}
	return NULL;
}

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
			.write_frame = media_stream_symbol(write_frame, bmp_oz),
		},
		{
			.stream_type = media_st_image,
			.stack_layout = media_sl_oz,
			.frame_id_name = media_nf_bgra32,
			.initial_judge = NULL,
			.create_pri = NULL,
			.append_pre = media_stream_symbol(append_pre, bmp_bgra32),
			.read_frame = media_stream_symbol(read_frame, bmp_oz),
			.write_frame = media_stream_symbol(write_frame, bmp_oz),
		}
	};
	return media_stream_spec_append(stream_spec, media, spec, sizeof(spec) / sizeof(*spec));
}
