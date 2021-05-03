#ifndef _mpeg4_inner_fullbox_h_
#define _mpeg4_inner_fullbox_h_

#include "box.include.h"

typedef struct inner_fullbox_t {
	uint32_t version;
	uint32_t flags;
} inner_fullbox_t;

inner_fullbox_t* mpeg4$define(inner, fullbox, get)(inner_fullbox_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);
uint8_t* mpeg4$define(inner, fullbox, set)(uint8_t *restrict data, const inner_fullbox_t *restrict r);

// buffer[*]
const char* mpeg4$define(inner, flags, string)(char *restrict buffer, uint32_t flags, const char *flag_name[], uintptr_t flag_number);

void mpeg4$define(inner, fullbox, dump)(mlog_s *restrict mlog, const inner_fullbox_t *restrict r, const char *restrict flags, uint32_t level);

#define inner_method_set_fullbox(_n, _t, _p, _vm) \
	const mpeg4_stuff_t* mpeg4$define(stuff, _n, set$version_and_flags)(_t *restrict r, uint32_t version, uint32_t flags)\
	{\
		if (version <= _vm)\
		{\
			r->_p.version = version;\
			r->_p.flags = flags;\
			return &r->stuff;\
		}\
		return NULL;\
	}

#define inner_method_get_fullbox(_n, _t, _p) \
	const mpeg4_stuff_t* mpeg4$define(stuff, _n, get$version_and_flags)(_t *restrict r, uint32_t *restrict version, uint32_t *restrict flags)\
	{\
		if (version) *version = r->_p.version;\
		if (flags) *flags = r->_p.flags;\
		return &r->stuff;\
	}

#endif
