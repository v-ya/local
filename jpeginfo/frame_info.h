#ifndef _frame_info_h_
#define _frame_info_h_

#include <refer.h>
#include <rbtree.h>

typedef struct frame_channel_t {
	uint32_t channel_id;
	uint32_t mcu_h_number;
	uint32_t mcu_v_number;
	uint32_t quantization_id;
} frame_channel_t;

typedef struct frame_info_s {
	rbtree_t *channel_finder;
	uint32_t width;
	uint32_t height;
	uint32_t sample_bits;
	uint32_t channel_number;
	frame_channel_t channel[];
} frame_info_s;

typedef struct frame_scan_ch_t {
	uint32_t channel_id;
	uint32_t huffman_dc_id;
	uint32_t huffman_ac_id;
} frame_scan_ch_t;

typedef struct frame_scan_s {
	uint32_t scan_number;
	frame_scan_ch_t scan[];
} frame_scan_s;

frame_info_s* frame_info_alloc(uint32_t width, uint32_t height, uint32_t sample_bits, uint32_t channel_number);
frame_info_s* frame_info_set_channel(frame_info_s *restrict info, uint32_t index, uint32_t channel_id, uint32_t mcu_h_number, uint32_t mcu_v_number, uint32_t quantization_id);
const frame_channel_t* frame_info_get_channel(const frame_info_s *restrict info, uint32_t channel_id);

frame_scan_s* frame_scan_alloc(uint32_t scan_number);
frame_scan_s* frame_scan_set_scan(frame_scan_s *restrict scan, uint32_t index, uint32_t channel_id, uint32_t huffman_dc_id, uint32_t huffman_ac_id);

#endif
