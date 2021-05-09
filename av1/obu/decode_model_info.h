#ifndef _av1_obu_decode_model_info_h_
#define _av1_obu_decode_model_info_h_

#include "obu.h"

typedef struct av1_decode_model_info_t {
	uint32_t buffer_delay_length;             // [1, 32]
	uint32_t num_units_in_decoding_tick;      // [0, 2^32)
	uint32_t buffer_removal_time_length;      // [1, 32]
	uint32_t frame_presentation_time_length;  // [1, 32]
} av1_decode_model_info_t;

av1_decode_model_info_t* av1_decode_model_info_init(av1_decode_model_info_t *restrict dm_info);
av1_decode_model_info_t* av1_decode_model_info_read(av1_decode_model_info_t *restrict dm_info, av1_bits_reader_t *restrict reader);
const av1_decode_model_info_t* av1_decode_model_info_write(const av1_decode_model_info_t *restrict dm_info, av1_bits_writer_t *restrict writer);
uint64_t av1_decode_model_info_bits(const av1_decode_model_info_t *restrict dm_info);
void av1_decode_model_info_dump(const av1_decode_model_info_t *restrict dm_info, mlog_s *restrict mlog);

#endif
