#include "frame_info.h"

static void frame_info_free_func(frame_info_s *restrict r)
{
	rbtree_clear(&r->channel_finder);
}

frame_info_s* frame_info_alloc(uint32_t width, uint32_t height, uint32_t sample_bits, uint32_t channel_number)
{
	frame_info_s *restrict r;
	if ((r = (frame_info_s *) refer_alloz(sizeof(frame_info_s) + sizeof(frame_channel_t) * channel_number)))
	{
		refer_set_free(r, (refer_free_f) frame_info_free_func);
		r->width = width;
		r->height = height;
		r->sample_bits = sample_bits;
		r->channel_number = channel_number;
		return r;
	}
	return NULL;
}

frame_info_s* frame_info_set_channel(frame_info_s *restrict info, uint32_t index, uint32_t channel_id, uint32_t mcu_h_number, uint32_t mcu_v_number, uint32_t quantization_id)
{
	frame_channel_t *restrict c;
	if (index < info->channel_number)
	{
		c = info->channel + index;
		c->channel_id = channel_id;
		c->mcu_h_number = mcu_h_number;
		c->mcu_v_number = mcu_v_number;
		c->quantization_id = quantization_id;
		if (rbtree_insert(&info->channel_finder, NULL, (uint64_t) channel_id, c, NULL))
			return info;
	}
	return NULL;
}

const frame_channel_t* frame_info_get_channel(const frame_info_s *restrict info, uint32_t channel_id)
{
	rbtree_t *restrict c;
	if ((c = rbtree_find(&info->channel_finder, NULL, (uint64_t) channel_id)))
		return (const frame_channel_t *) c->value;
	return NULL;
}

frame_scan_s* frame_scan_alloc(uint32_t scan_number)
{
	frame_scan_s *restrict r;
	if ((r = (frame_scan_s *) refer_alloz(sizeof(frame_scan_s) + sizeof(frame_scan_ch_t) * scan_number)))
	{
		r->scan_number = scan_number;
		return r;
	}
	return NULL;
}

frame_scan_s* frame_scan_set_scan(frame_scan_s *restrict scan, uint32_t index, uint32_t channel_id, uint32_t huffman_dc_id, uint32_t huffman_ac_id)
{
	frame_scan_ch_t *restrict s;
	if (index < scan->scan_number)
	{
		s = scan->scan + index;
		s->channel_id = channel_id;
		s->huffman_dc_id = huffman_dc_id;
		s->huffman_ac_id = huffman_ac_id;
		return scan;
	}
	return NULL;
}
