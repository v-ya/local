#ifndef _av1_obu_frame_render_size_h_
#define _av1_obu_frame_render_size_h_

#include "obu.h"

typedef struct av1_frame_render_size_t {
	uint32_t frame_width;     // [1, 65536]
	uint32_t frame_height;    // [1, 65536]
	uint32_t render_width;    // [1, 65536]
	uint32_t render_height;   // [1, 65536]
	uint32_t superres_denom;  // [8, 16]
	// calc
	uint32_t UpscaledWidth;
	uint32_t FrameWidth;
	uint32_t FrameHeight;
	uint32_t RenderWidth;
	uint32_t RenderHeight;
	uint32_t MiCols;
	uint32_t MiRows;
} av1_frame_render_size_t;

av1_frame_render_size_t* av1_frame_render_size_init(av1_frame_render_size_t *restrict frsize);
av1_frame_render_size_t* av1_frame_render_size_read_superres_params(av1_frame_render_size_t *restrict frsize, av1_bits_reader_t *restrict reader, const struct av1_obu_sequence_header_t *restrict header);
av1_frame_render_size_t* av1_frame_render_size_read(av1_frame_render_size_t *restrict frsize, av1_bits_reader_t *restrict reader, const struct av1_obu_sequence_header_t *restrict header, uint8_t frame_size_override_flag);
const av1_frame_render_size_t* av1_frame_render_size_write_superres_params(const av1_frame_render_size_t *restrict frsize, av1_bits_writer_t *restrict writer, const struct av1_obu_sequence_header_t *restrict header);
const av1_frame_render_size_t* av1_frame_render_size_write(const av1_frame_render_size_t *restrict frsize, av1_bits_writer_t *restrict writer, const struct av1_obu_sequence_header_t *restrict header, uint8_t frame_size_override_flag);
uint64_t av1_frame_render_size_bits_superres_params(const av1_frame_render_size_t *restrict frsize, const struct av1_obu_sequence_header_t *restrict header);
uint64_t av1_frame_render_size_bits(const av1_frame_render_size_t *restrict frsize, const struct av1_obu_sequence_header_t *restrict header, uint8_t frame_size_override_flag);
void av1_frame_render_size_dump(const av1_frame_render_size_t *restrict frsize, mlog_s *restrict mlog);

#endif
