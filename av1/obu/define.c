#include "define.h"
#include <stddef.h>

int32_t av1_get_relative_dist(uint32_t a, uint32_t b, uint32_t bits)
{
	register uint32_t diff;
	diff = a - b;
	if (diff & (1u << (bits - 1)))
		diff |= (~0u << bits);
	return (int32_t) diff;
}

#define av1_string_return(_ds)          ({register const char *r = _ds; if (t < (sizeof(s) / sizeof(s[0])) && s[t]) r = s[t]; r;})
#define av1_string_return_reserved()    av1_string_return(s_reserved)
#define av1_string_return_unexpected()  av1_string_return(s_unexpected)

static const char *s_reserved = "(reserved)";
static const char *s_unexpected = "(unexpected)";

const char* av1_string_obu_type(av1_obu_type_t t)
{
	static const char *s[av1_obu_type$max] = {
		[av1_obu_type_sequence_header]        = "sequence_header",
		[av1_obu_type_temporal_delimiter]     = "temporal_delimiter",
		[av1_obu_type_frame_header]           = "frame_header",
		[av1_obu_type_tile_group]             = "tile_group",
		[av1_obu_type_metadata]               = "metadata",
		[av1_obu_type_frame]                  = "frame",
		[av1_obu_type_redundant_frame_header] = "redundant_frame_header",
		[av1_obu_type_tile_list]              = "tile_list",
		[av1_obu_type_padding]                = "padding"
	};
	return av1_string_return_reserved();
}

const char* av1_string_color_primaries(av1_color_primaries_t t)
{
	static const char *s[av1_color_primaries$max] = {
		[av1_color_primaries_BT_709]       = "BT.709",
		[av1_color_primaries_UNSPECIFIED]  = "Unspecified",
		[av1_color_primaries_BT_470_M]     = "BT.470 System M (historical)",
		[av1_color_primaries_BT_470_B_G]   = "BT.470 System B, G (historical)",
		[av1_color_primaries_BT_601]       = "BT.601",
		[av1_color_primaries_SMPTE_240]    = "SMPTE 240",
		[av1_color_primaries_GENERIC_FILM] = "Generic film (color filters using illuminant C)",
		[av1_color_primaries_BT_2020]      = "BT.2020, BT.2100",
		[av1_color_primaries_XYZ]          = "SMPTE 428 (CIE 1921 XYZ)",
		[av1_color_primaries_SMPTE_431]    = "SMPTE RP 431-2",
		[av1_color_primaries_SMPTE_432]    = "SMPTE EG 432-1",
		[av1_color_primaries_EBU_3213]     = "EBU Tech. 3213-E"
	};
	return av1_string_return_reserved();
}

const char* av1_string_transfer_characteristics(av1_transfer_characteristics_t t)
{
	static const char *s[av1_transfer_characteristics$max] = {
		[av1_transfer_characteristics_BT_709]         = "BT.709",
		[av1_transfer_characteristics_UNSPECIFIED]    = "Unspecified",
		[av1_transfer_characteristics_BT_470_M]       = "BT.470 System M (historical)",
		[av1_transfer_characteristics_BT_470_B_G]     = "BT.470 System B, G (historical)",
		[av1_transfer_characteristics_BT_601]         = "BT.601",
		[av1_transfer_characteristics_SMPTE_240]      = "SMPTE 240 M",
		[av1_transfer_characteristics_LINEAR]         = "Linear",
		[av1_transfer_characteristics_LOG_100]        = "Logarithmic (100 : 1 range)",
		[av1_transfer_characteristics_LOG_100_SQRT10] = "Logarithmic (100 * Sqrt(10) : 1 range)",
		[av1_transfer_characteristics_IEC_61966]      = "IEC 61966-2-4",
		[av1_transfer_characteristics_BT_1361]        = "BT.1361",
		[av1_transfer_characteristics_SRGB]           = "sRGB or sYCC",
		[av1_transfer_characteristics_BT_2020_10_BIT] = "BT.2020 10-bit systems",
		[av1_transfer_characteristics_BT_2020_12_BIT] = "BT.2020 12-bit systems",
		[av1_transfer_characteristics_SMPTE_2084]     = "SMPTE ST 2084, ITU BT.2100 PQ",
		[av1_transfer_characteristics_SMPTE_428]      = "SMPTE ST 428",
		[av1_transfer_characteristics_HLG]            = "BT.2100 HLG, ARIB STD-B67"
	};
	return av1_string_return_reserved();
}

const char* av1_string_matrix_coefficients(av1_matrix_coefficients_t t)
{
	static const char *s[av1_matrix_coefficients$max] = {
		[av1_matrix_coefficients_IDENTITY]    = "Identity matrix",
		[av1_matrix_coefficients_BT_709]      = "BT.709",
		[av1_matrix_coefficients_UNSPECIFIED] = "Unspecified",
		[av1_matrix_coefficients_FCC]         = "US FCC 73.628",
		[av1_matrix_coefficients_BT_470_B_G]  = "BT.470 System B, G (historical)",
		[av1_matrix_coefficients_BT_601]      = "BT.601",
		[av1_matrix_coefficients_SMPTE_240]   = "SMPTE 240 M",
		[av1_matrix_coefficients_SMPTE_YCGCO] = "YCgCo",
		[av1_matrix_coefficients_BT_2020_NCL] = "BT.2020 non-constant luminance, BT.2100 YCbCr",
		[av1_matrix_coefficients_BT_2020_CL]  = "BT.2020 constant luminance",
		[av1_matrix_coefficients_SMPTE_2085]  = "SMPTE ST 2085 YDzDx",
		[av1_matrix_coefficients_CHROMAT_NCL] = "Chromaticity-derived non-constant luminance",
		[av1_matrix_coefficients_CHROMAT_CL]  = "Chromaticity-derived constant luminance",
		[av1_matrix_coefficients_ICTCP]       = "BT.2100 ICtCp"
	};
	return av1_string_return_reserved();
}

const char* av1_string_chroma_sample_position(av1_chroma_sample_position_t t)
{
	static const char *s[av1_chroma_sample_position$max] = {
		[av1_chroma_sample_position_UNKNOWN]   = "unknow",
		[av1_chroma_sample_position_VERTICAL]  = "vertical",
		[av1_chroma_sample_position_COLOCATED] = "colocated",
	};
	return av1_string_return_unexpected();
}

const char* av1_string_frame_type(av1_frame_type_t t)
{
	static const char *s[av1_frame_type$max] = {
		[av1_frame_type_key]        = "key",
		[av1_frame_type_inter]      = "inter",
		[av1_frame_type_intra_only] = "intra_only",
		[av1_frame_type_switch]     = "switch"
	};
	return av1_string_return_unexpected();
}

const char* av1_string_interpolation_filter(av1_interpolation_filter_t t)
{
	static const char *s[av1_interpolation$max] = {
		[av1_interpolation_filter_eighttap]        = "eighttap",
		[av1_interpolation_filter_eighttap_smooth] = "eighttap_smooth",
		[av1_interpolation_filter_eighttap_sharp]  = "eighttap_sharp",
		[av1_interpolation_filter_bilinear]        = "bilinear",
		[av1_interpolation_filter_switchable]      = "switchable"
	};
	return av1_string_return_unexpected();
}
