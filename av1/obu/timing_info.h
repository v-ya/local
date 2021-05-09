#ifndef _av1_obu_timing_info_h_
#define _av1_obu_timing_info_h_

#include "obu.h"

typedef struct av1_timing_info_t {
	uint32_t num_units_in_display_tick;  // [0, 2*32)
	uint32_t time_scale;                 // [0, 2*32)
	uint32_t num_ticks_per_picture;      // [0, 2*32)
} av1_timing_info_t;

av1_timing_info_t* av1_timing_info_init(av1_timing_info_t *restrict timing_info);
av1_timing_info_t* av1_timing_info_read(av1_timing_info_t *restrict timing_info, av1_bits_reader_t *restrict reader);
const av1_timing_info_t* av1_timing_info_write(const av1_timing_info_t *restrict timing_info, av1_bits_writer_t *restrict writer);
uint64_t av1_timing_info_bits(const av1_timing_info_t *restrict timing_info);
void av1_timing_info_dump(const av1_timing_info_t *restrict timing_info, mlog_s *restrict mlog);

#endif
