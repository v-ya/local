#ifndef _mpeg4_inner_fullbox_h_
#define _mpeg4_inner_fullbox_h_

#include "box.include.h"

typedef struct inner_fullbox_t {
	uint32_t version;
	uint32_t flags;
} inner_fullbox_t;

inner_fullbox_t* mpeg4$define(inner, fullbox, get)(inner_fullbox_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);

// buffer[*]
const char* mpeg4$define(inner, flags, string)(char *restrict buffer, uint32_t flags, const char *flag_name[], uintptr_t flag_number);

void mpeg4$define(inner, fullbox, dump)(mlog_s *restrict mlog, const inner_fullbox_t *restrict r, const char *restrict flags, uint32_t level);

#endif
