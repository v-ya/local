#ifndef _av1_obu_uncompressed_header_h_
#define _av1_obu_uncompressed_header_h_

#include "obu.h"

typedef enum av1_frame_type_t {
	av1_frame_type_key = 0,
	av1_frame_type_inter = 1,
	av1_frame_type_intra_only = 2,
	av1_frame_type_switch = 3,
	av1_frame_type$max
} av1_frame_type_t;

typedef struct av1_uncompressed_header_t {
	uint8_t show_existing_frame;       // 0|1
	uint8_t show_frame;                // 0|1
	uint8_t showable_frame;            // 0|1
	uint8_t error_resilient_mode;      // 0|1
	uint8_t frame_to_show_map_idx;     // [0, 7]
	uint8_t refresh_frame_flags;       // [0, 0xff]
	av1_frame_type_t frame_type;       // [0, 3]
	uint32_t frame_presentation_time;  // [0, 2^frame_presentation_time_length)
	uint32_t display_frame_id;         // [0, 2^3 ~ 2^25)
} av1_uncompressed_header_t;

av1_uncompressed_header_t* av1_uncompressed_header_init(av1_uncompressed_header_t *restrict header);
uint32_t av1_uncompressed_header_FrameIsIntra(const av1_uncompressed_header_t *restrict header);
av1_uncompressed_header_t* av1_uncompressed_header_read(av1_uncompressed_header_t *restrict header, av1_bits_reader_t *restrict reader, av1_env_t *restrict env);
const av1_uncompressed_header_t* av1_uncompressed_header_write(const av1_uncompressed_header_t *restrict header, av1_bits_writer_t *restrict writer, av1_env_t *restrict env);
uint64_t av1_uncompressed_header_bits(const av1_uncompressed_header_t *restrict header, av1_env_t *restrict env);
void av1_uncompressed_header_dump(const av1_uncompressed_header_t *restrict header, mlog_s *restrict mlog, av1_env_t *restrict env);

#endif
