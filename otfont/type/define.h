#ifndef _otfont_type_define_h_
#define _otfont_type_define_h_

#include <stdint.h>

typedef union otfont_type_tag_t {
	uint8_t tag[4];
	uint32_t value;
} otfont_type_tag_t;

#endif
