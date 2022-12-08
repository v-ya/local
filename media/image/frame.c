#include "frame.h"

struct media_frame_id_s* media_frame_create_image_bgra32(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_bgra32, 1, 2)))
	{
		if (media_frame_id_initial(r, 0, 2, (const struct media_cell_info_t [2]) {
			d_media_ci__ibase(4),
			d_media_ci__inext(),
		})) return r;
		refer_free(r);
	}
	return NULL;
}

struct media_frame_id_s* media_frame_create_image_bgr24(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_bgr24, 1, 2)))
	{
		if (media_frame_id_initial(r, 0, 2, (const struct media_cell_info_t [2]) {
			d_media_ci__ibase(3),
			d_media_ci__inext(),
		})) return r;
		refer_free(r);
	}
	return NULL;
}

static void media_frame_touch_initial__image_bgr24_p4(struct media_channel_s *restrict channel, uintptr_t channel_index)
{
	uint8_t *restrict p;
	uintptr_t linesize_0, linesize_1, n, m;
	if (channel_index == 0)
	{
		linesize_0 = channel->cell[0].cell_size * channel->cell[0].cell_number;
		linesize_1 = channel->cell[1].cell_size;
		n = channel->cell[1].cell_number;
		if (linesize_1 > linesize_0 && n)
		{
			p = (uint8_t *) channel->data;
			do {
				p += linesize_0;
				m = linesize_1 - linesize_0;
				do {
					*p++ = 0;
				} while (--m);
			} while (--n);
		}
	}
}

struct media_frame_id_s* media_frame_create_image_bgr24_p4(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_bgr24_p4, 1, 2)))
	{
		r->touch_initial = media_frame_touch_initial__image_bgr24_p4;
		if (media_frame_id_initial(r, 0, 2, (const struct media_cell_info_t [2]) {
			d_media_ci__ibase(3),
			d_media_ci__inext_with_align(4),
		})) return r;
		refer_free(r);
	}
	return NULL;
}

struct media_frame_id_s* media_frame_create_image_yuv_8_411(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_yuv_8_411, 3, 2)))
	{
		static const struct media_cell_info_t ci_y[2] = {
			d_media_ci__ibase(1),
			d_media_ci__inext(),
		};
		static const struct media_cell_info_t ci_uv[2] = {
			d_media_ci__ibase_with_sink(1, 2),
			d_media_ci__inext_with_sink(2),
		};
		if (media_frame_id_initial(r, 0, 2, ci_y) &&
			media_frame_id_initial(r, 1, 2, ci_uv) &&
			media_frame_id_initial(r, 2, 2, ci_uv)
		) return r;
		refer_free(r);
	}
	return NULL;
}

struct media_frame_id_s* media_frame_create_image_yuv_8_111(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_yuv_8_111, 3, 2)))
	{
		static const struct media_cell_info_t ci_yuv[2] = {
			d_media_ci__ibase(1),
			d_media_ci__inext(),
		};
		if (media_frame_id_initial(r, 0, 2, ci_yuv) &&
			media_frame_id_initial(r, 1, 2, ci_yuv) &&
			media_frame_id_initial(r, 2, 2, ci_yuv)
		) return r;
		refer_free(r);
	}
	return NULL;
}
