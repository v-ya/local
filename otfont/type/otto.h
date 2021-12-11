#ifndef _otfont_type_otto_h_
#define _otfont_type_otto_h_

#include "define.h"

struct otfont_type_otto_table_t {
	otfont_type_tag_t tag;
	uint32_t check_sum;
	uint32_t offset_by_file;
	uint32_t length;
};

struct otfont_type_otto_t {
	otfont_type_tag_t tag;    // 'otto' or 0x00010000
	uint16_t table_number;
	uint16_t search_range;    // <= (2 ** floor(log2(table_number))) * 16
	uint16_t entry_selector;  // <= floor(log2(table_number)
	uint16_t range_shift;     // == (table_number * 16) - search_range
	struct otfont_type_otto_table_t table[];
	// 必须升序排列 (table.tag)
};

#endif
