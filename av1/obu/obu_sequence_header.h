#ifndef _av1_obu_sequence_header_h_
#define _av1_obu_sequence_header_h_

#include "obu.h"
#include "timing_info.h"
#include "decode_model_info.h"

typedef enum av1_color_primaries_t {
	av1_color_primaries_BT_709 = 1,        // BT.709
	av1_color_primaries_UNSPECIFIED = 2,   // Unspecified
	av1_color_primaries_BT_470_M = 4,      // BT.470 System M (historical)
	av1_color_primaries_BT_470_B_G = 5,    // BT.470 System B, G (historical)
	av1_color_primaries_BT_601 = 6,        // BT.601
	av1_color_primaries_SMPTE_240 = 7,     // SMPTE 240
	av1_color_primaries_GENERIC_FILM = 8,  // Generic film (color filters using illuminant C)
	av1_color_primaries_BT_2020 = 9,       // BT.2020, BT.2100
	av1_color_primaries_XYZ = 10,          // SMPTE 428 (CIE 1921 XYZ)
	av1_color_primaries_SMPTE_431 = 11,    // SMPTE RP 431-2
	av1_color_primaries_SMPTE_432 = 12,    // SMPTE EG 432-1
	av1_color_primaries_EBU_3213 = 22,     // EBU Tech. 3213-E
	av1_color_primaries$max
} av1_color_primaries_t;

typedef enum av1_transfer_characteristics_t {
	av1_transfer_characteristics_BT_709 = 1,           // BT.709
	av1_transfer_characteristics_UNSPECIFIED = 2,      // Unspecified
	av1_transfer_characteristics_BT_470_M = 4,         // BT.470 System M (historical)
	av1_transfer_characteristics_BT_470_B_G = 5,       // BT.470 System B, G (historical)
	av1_transfer_characteristics_BT_601 = 6,           // BT.601
	av1_transfer_characteristics_SMPTE_240 = 7,        // SMPTE 240 M
	av1_transfer_characteristics_LINEAR = 8,           // Linear
	av1_transfer_characteristics_LOG_100 = 9,          // Logarithmic (100 : 1 range)
	av1_transfer_characteristics_LOG_100_SQRT10 = 10,  // Logarithmic (100 * Sqrt(10) : 1 range)
	av1_transfer_characteristics_IEC_61966 = 11,       // IEC 61966-2-4
	av1_transfer_characteristics_BT_1361 = 12,         // BT.1361
	av1_transfer_characteristics_SRGB = 13,            // sRGB or sYCC
	av1_transfer_characteristics_BT_2020_10_BIT = 14,  // BT.2020 10-bit systems
	av1_transfer_characteristics_BT_2020_12_BIT = 15,  // BT.2020 12-bit systems
	av1_transfer_characteristics_SMPTE_2084 = 16,      // SMPTE ST 2084, ITU BT.2100 PQ
	av1_transfer_characteristics_SMPTE_428 = 17,       // SMPTE ST 428
	av1_transfer_characteristics_HLG = 18,             // BT.2100 HLG, ARIB STD-B67
	av1_transfer_characteristics$max
} av1_transfer_characteristics_t;

typedef enum av1_matrix_coefficients_t {
	av1_matrix_coefficients_IDENTITY = 0,      // Identity matrix
	av1_matrix_coefficients_BT_709 = 1,        // BT.709
	av1_matrix_coefficients_UNSPECIFIED = 2,   // Unspecified
	av1_matrix_coefficients_FCC = 4,           // US FCC 73.628
	av1_matrix_coefficients_BT_470_B_G = 5,    // BT.470 System B, G (historical)
	av1_matrix_coefficients_BT_601 = 6,        // BT.601
	av1_matrix_coefficients_SMPTE_240 = 7,     // SMPTE 240 M
	av1_matrix_coefficients_SMPTE_YCGCO = 8,   // YCgCo
	av1_matrix_coefficients_BT_2020_NCL = 9,   // BT.2020 non-constant luminance, BT.2100 YCbCr
	av1_matrix_coefficients_BT_2020_CL = 10,   // BT.2020 constant luminance
	av1_matrix_coefficients_SMPTE_2085 = 11,   // SMPTE ST 2085 YDzDx
	av1_matrix_coefficients_CHROMAT_NCL = 12,  // Chromaticity-derived non-constant luminance
	av1_matrix_coefficients_CHROMAT_CL = 13,   // Chromaticity-derived constant luminance
	av1_matrix_coefficients_ICTCP = 14,        // BT.2100 ICtCp
	av1_matrix_coefficients$max
} av1_matrix_coefficients_t;

typedef enum av1_chroma_sample_position_t {
	// Unknown (in this case the source video transfer function must be signaled outside the AV1 bitstream)
	av1_chroma_sample_position_UNKNOWN = 0,
	// Horizontally co-located with (0, 0) luma sample, vertical position in the middle between two luma samples
	av1_chroma_sample_position_VERTICAL = 1,
	// co-located with (0, 0) luma sample
	av1_chroma_sample_position_COLOCATED = 2,
	av1_chroma_sample_position$max = 4
} av1_chroma_sample_position_t;

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
	uint8_t seq_screen_content_tools;                       // [0, 2]
	uint8_t seq_integer_mv;                                 // [0, 2]
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
	uint32_t max_frame_width;                               // [0, 65536]
	uint32_t max_frame_height;                              // [0, 65536]
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
