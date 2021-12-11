#ifndef _otfont_type_ttcf_h_
#define _otfont_type_ttcf_h_

#include "define.h"

struct otfont_type_ttcf_t {
	otfont_type_tag_t tag;  // 'ttcf'
	uint16_t major_version; // must 1 (otfont_type_ttcf_t), 2 (otfont_type_ttcf_t otfont_type_ttcf_extend_2_t)
	uint16_t minor_version; // must 0
	uint32_t font_number;
	uint32_t font_offset_by_file[];
};

struct otfont_type_ttcf_extend_2_t {
	otfont_type_tag_t tag_dsig;  // 'DSIG' or 0
	uint32_t length;
	uint32_t offset_by_file;
};

#endif
