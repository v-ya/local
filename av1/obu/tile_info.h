#ifndef _av1_obu_tile_info_h_
#define _av1_obu_tile_info_h_

#include "obu.h"

struct av1_frame_render_size_t;

typedef struct av1_tile_info_t {
	// bits
	uint8_t uniform_tile_spacing_flag;       // 0|1
	uint8_t tile_size_bytes;                 // [1, 4]
	uint32_t increment_tile_cols_log2_bits;  // [0, ?]
	uint32_t increment_tile_rows_log2_bits;  // [0, ?]
	uint32_t context_update_tile_id;         // bits (TileRowsLog2+TileColsLog2)
	// calc
	uint32_t TileColsLog2;
	uint32_t TileRowsLog2;
	uint32_t TileCols;
	uint32_t TileRows;
	// bits
	uint32_t width_in_sbs[MAX_TILE_COLS];    // [1, ?]
	uint32_t height_in_sbs[MAX_TILE_ROWS];   // [1, ?]
	// calc
	uint32_t MiColStarts[MAX_TILE_COLS];
	uint32_t MiRowStarts[MAX_TILE_ROWS];
} av1_tile_info_t;

av1_tile_info_t* av1_tile_info_init(av1_tile_info_t *restrict tinfo);
av1_tile_info_t* av1_tile_info_read(av1_tile_info_t *restrict tinfo, av1_bits_reader_t *restrict reader, const struct av1_frame_render_size_t *restrict frsize, uint8_t use_128x128_superblock);
const av1_tile_info_t* av1_tile_info_write(const av1_tile_info_t *restrict tinfo, av1_bits_writer_t *restrict writer, const struct av1_frame_render_size_t *restrict frsize, uint8_t use_128x128_superblock);
uint64_t av1_tile_info_bits(const av1_tile_info_t *restrict tinfo, const struct av1_frame_render_size_t *restrict frsize, uint8_t use_128x128_superblock);
void av1_tile_info_dump(const av1_tile_info_t *restrict tinfo, mlog_s *restrict mlog);

#endif
