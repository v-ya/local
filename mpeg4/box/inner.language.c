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

uint16_t mpeg4$define(inner, language, build)(const inner_language_t *restrict r)
{
	register uint32_t v;
	v = (r->s[0] - 0x60) & 0x1f;
	v <<= 5;
	v |= (r->s[1] - 0x60) & 0x1f;
	v <<= 5;
	v |= (r->s[2] - 0x60) & 0x1f;
	return (uint16_t) v;
}
