#ifndef _mpeg4_inner_language_h_
#define _mpeg4_inner_language_h_

#include "box.include.h"

typedef union inner_language_t {
	char s[4];
	uint32_t v;
} inner_language_t;

inner_language_t* mpeg4$define(inner, language, parse)(inner_language_t *restrict r, uint16_t language);

#endif
