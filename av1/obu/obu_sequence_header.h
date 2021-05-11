#ifndef _av1_obu_sequence_header_h_
#define _av1_obu_sequence_header_h_

#include "obu.h"
#include "timing_info.h"
#include "decode_model_info.h"

typedef struct av1_obu_sequence_header_extra_t {
	av1_timing_info_t timing_info;
	av1_decode_model_info_t decode_model_info;
} av1_obu_sequence_header_extra_t;

typedef struct av1_obu_sequence_header_t {
	uint8_t seq_profile;                                    // [0, 7]
	uint8_t still_picture;                                  // 0|1
	uint8_t reduced_still_picture_header;                   // 0|1
	uint8_t timing_info_present_flag;                       // 0|1
	uint8_t decoder_model_info_present_flag;                // 0|1
	uint8_t initial_display_delay_present_flag;             // 0|1
	uint8_t operating_points_cnt;                           // [1, 32]
	uint8_t frame_width_bits;                               // [1, 16]
	uint8_t frame_height_bits;                              // [1, 16]
	uint8_t frame_id_numbers_present_flag;                  // 0|1
	uint8_t delta_frame_id_length;                          // [2, 17]
	uint8_t additional_frame_id_length;                     // [1, 8]
	uint8_t use_128x128_superblock;                         // 0|1
	uint8_t enable_filter_intra;                            // 0|1
	uint8_t enable_intra_edge_filter;                       // 0|1
	uint8_t enable_interintra_compound;                     // 0|1
	uint8_t enable_masked_compound;                         // 0|1
	uint8_t enable_warped_motion;                           // 0|1
	uint8_t enable_dual_filter;                             // 0|1
	uint8_t enable_order_hint;                              // 0|1
	uint8_t enable_jnt_comp;                                // 0|1
	uint8_t enable_ref_frame_mvs;                           // 0|1
	uint8_t seq_force_screen_content_tools;                 // [0, 2]
	uint8_t seq_force_integer_mv;                           // [0, 2]
	uint8_t order_hint_bits;                                // [1, 8]
	uint8_t enable_superres;                                // 0|1
	uint8_t enable_cdef;                                    // 0|1
	uint8_t enable_restoration;                             // 0|1
	uint8_t color_config_high_bitdepth;                     // 0|1
	uint8_t color_config_twelve_bit;                        // 0|1
	uint8_t color_config_mono_chrome;                       // 0|1
	uint8_t color_config_color_description_present_flag;    // 0|1
	uint8_t color_config_color_range;                       // 0|1
	uint8_t color_config_subsampling_x;                     // 0|1
	uint8_t color_config_subsampling_y;                     // 0|1
	uint8_t color_config_separate_uv_delta_q;               // 0|1
	uint8_t film_grain_params_present;                      // 0|1
	uint32_t max_frame_width;                               // [1, 65536]
	uint32_t max_frame_height;                              // [1, 65536]
	av1_color_primaries_t color_config_color_primaries;
	av1_transfer_characteristics_t color_config_transfer_characteristics;
	av1_matrix_coefficients_t color_config_matrix_coefficients;
	av1_chroma_sample_position_t color_config_chroma_sample_position;
	uint8_t seq_level_idx[32];                              // [0, 32)
	uint8_t seq_tier[32];                                   // 0|1
	uint8_t decoder_model_present_for_this_op[32];          // 0|1
	uint8_t low_delay_mode_flag[32];                        // 0|1
	uint8_t initial_display_delay[32];                      // [1, 16]
	uint16_t operating_point_idc[32];                       // [0, 2^12)
	uint32_t decoder_buffer_delay[32];                      // [0, 2^decode_model_info.buffer_delay_length)
	uint32_t encoder_buffer_delay[32];                      // [0, 2^decode_model_info.buffer_delay_length)
	av1_obu_sequence_header_extra_t extra;
} av1_obu_sequence_header_t;

av1_obu_sequence_header_t* av1_obu_sequence_header_init(av1_obu_sequence_header_t *restrict header);
uint32_t av1_obu_sequence_header_OrderHintBits(const av1_obu_sequence_header_t *restrict header);
uint32_t av1_obu_sequence_header_BitDepth(const av1_obu_sequence_header_t *restrict header);
uint32_t av1_obu_sequence_header_NumPlanes(const av1_obu_sequence_header_t *restrict header);
av1_obu_sequence_header_t* av1_obu_sequence_header_read(av1_obu_sequence_header_t *restrict header, av1_bits_reader_t *restrict reader);
const av1_obu_sequence_header_t* av1_obu_sequence_header_write(const av1_obu_sequence_header_t *restrict header, av1_bits_writer_t *restrict writer);
uint64_t av1_obu_sequence_header_bits(const av1_obu_sequence_header_t *restrict header);
void av1_obu_sequence_header_dump(const av1_obu_sequence_header_t *restrict header, mlog_s *restrict mlog);

#endif
