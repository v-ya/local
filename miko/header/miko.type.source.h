#ifndef _miko_type_source_h_
#define _miko_type_source_h_

#include <refer.h>

typedef struct miko_source_s miko_source_s;
typedef struct miko_source_pos_t miko_source_pos_t;

struct miko_source_s {
	refer_string_t name;
	refer_nstring_t source;
};

struct miko_source_pos_t {
	const char *line_string;
	uintptr_t line_length;
	uintptr_t row_of_source;
	uintptr_t col_of_line;
	uintptr_t space_of_line;
};

#endif
