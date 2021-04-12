#include "inner.language.h"

inner_language_t* mpeg4$define(inner, language, parse)(inner_language_t *restrict r, uint16_t language)
{
	register uint32_t v;
	r->s[3] = 0;
	v = language;
	r->s[2] = (v & 0x1f) + 0x60;
	v >>= 5;
	r->s[1] = (v & 0x1f) + 0x60;
	v >>= 5;
	r->s[0] = (v & 0x1f) + 0x60;
	return r;
}
