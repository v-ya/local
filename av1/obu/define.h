#ifndef _av1_obu_define_h_
#define _av1_obu_define_h_

#include <stdint.h>

#define MAX_TILE_AREA                (4096 * 2304)
#define MAX_TILE_COLS                64
#define MAX_TILE_ROWS                64
#define MAX_TILE_WIDTH               4096
#define NUM_REF_FRAMES               8
#define PRIMARY_REF_NONE             7
#define REFS_PER_FRAME               7
#define SELECT_SCREEN_CONTENT_TOOLS  2
#define SELECT_INTEGER_MV            2
#define SUPERRES_DENOM_BITS          3
#define SUPERRES_DENOM_MIN           9
#define SUPERRES_NUM                 8

// type of ref[NUM_REF_FRAMES]
// maybe timeline:
// GOLDEN -> LAST3 -> LAST2 -> LAST -> INTRA -> BWDREF -> ALTREF2 -> ALTREF
#define REF_NONE_FRAME               ~0
#define REF_INTRA_FRAME              0
#define REF_LAST_FRAME               1
#define REF_LAST2_FRAME              2
#define REF_LAST3_FRAME              3
#define REF_GOLDEN_FRAME             4
#define REF_BWDREF_FRAME             5
#define REF_ALTREF2_FRAME            6
#define REF_ALTREF_FRAME             7

typedef enum av1_obu_type_t {
	av1_obu_type_reserved = 0,
	av1_obu_type_sequence_header = 1,
	av1_obu_type_temporal_delimiter = 2,
	av1_obu_type_frame_header = 3,
	av1_obu_type_tile_group = 4,
	av1_obu_type_metadata = 5,
	av1_obu_type_frame = 6,
	av1_obu_type_redundant_frame_header = 7,
	av1_obu_type_tile_list = 8,
	av1_obu_type_padding = 15,
	av1_obu_type$max = 16
} av1_obu_type_t;

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

typedef enum av1_frame_type_t {
	av1_frame_type_key = 0,
	av1_frame_type_inter = 1,
	av1_frame_type_intra_only = 2,
	av1_frame_type_switch = 3,
	av1_frame_type$max
} av1_frame_type_t;

typedef enum av1_interpolation_filter_t {
	av1_interpolation_filter_eighttap = 0,
	av1_interpolation_filter_eighttap_smooth = 1,
	av1_interpolation_filter_eighttap_sharp = 2,
	av1_interpolation_filter_bilinear = 3,
	av1_interpolation_filter_switchable = 4,
	av1_interpolation$max
} av1_interpolation_filter_t;

int32_t av1_get_relative_dist(uint32_t a, uint32_t b, uint32_t bits);
uint32_t av1_tile_log2(uint32_t blk_size, uint32_t target);

const char* av1_string_obu_type(av1_obu_type_t t);
const char* av1_string_color_primaries(av1_color_primaries_t t);
const char* av1_string_transfer_characteristics(av1_transfer_characteristics_t t);
const char* av1_string_matrix_coefficients(av1_matrix_coefficients_t t);
const char* av1_string_chroma_sample_position(av1_chroma_sample_position_t t);
const char* av1_string_frame_type(av1_frame_type_t t);
const char* av1_string_interpolation_filter(av1_interpolation_filter_t t);

#endif
