#include "inner.type.h"

static const char* utf82unicode(const char *restrict s, uint32_t *restrict unicode, uint32_t *restrict error)
{
	const uint8_t uclength[64] = {
		// 1 0xxxxx -
		[0x00 ... 0x1f] = 1,
		// 1 10xxxx -
		[0x20 ... 0x2f] = 2,
		// 1 110xxx -
		[0x30 ... 0x37] = 3,
		// 1 1110xx -
		[0x38 ... 0x3b] = 4,
		// 1 11110x -
		[0x3c] = 5,
		[0x3d] = 5,
		// 1 111110 -
		[0x3e] = 6,
		[0x3f] = 0
	};
	uint32_t u;
	uint32_t c;
	uint32_t n;
	u = 0;
	label_reseach:
	if (!((c = (uint8_t) *s++) & 0x80))
	{
		if (!(u = c)) --s;
	}
	else
	{
		// check this char length
		n = (uint32_t) uclength[(c >> 1) & 0x3f];
		if (n < 2) goto label_fail_reseach;
		u = c & (0xff >> (n + 1));
		while (--n)
		{
			if (((c = (uint8_t) *s) & 0xc0) != 0x80)
				goto label_fail_return;
			u = (u << 6) | (c & 0x3f);
			++s;
		}
	}
	label_return:
	*unicode = u;
	return s;
	label_fail_reseach:
	if (error) *error = 1;
	goto label_reseach;
	label_fail_return:
	if (error) *error = 2;
	goto label_return;
}

mpeg4_box_type_t mpeg4$define(inner, type, parse)(const char *restrict s)
{
	mpeg4_box_type_t r;
	uint32_t c;
	r.v = 0;
	#define d_code(_n)  \
		if (!*s) goto label_return;\
		s = utf82unicode(s, &c, NULL);\
		r.c[_n] = (uint8_t) c;
	d_code(0)
	d_code(1)
	d_code(2)
	d_code(3)
	#undef d_code
	label_return:
	return r;
}

mpeg4_box_type_t mpeg4$define(inner, type, check)(const char *restrict s)
{
	mpeg4_box_type_t r;
	uint32_t c, error;
	error = 0;
	if (!s) goto label_fail;
	#define d_code(_n)  \
		if (!*s) goto label_fail;\
		s = utf82unicode(s, &c, &error);\
		if (error) goto label_fail;\
		if (c > 0xff) goto label_fail;\
		r.c[_n] = (uint8_t) c;
	d_code(0)
	d_code(1)
	d_code(2)
	d_code(3)
	#undef d_code
	return r;
	label_fail:
	r.v = 0;
	return r;
}

const char* mpeg4$define(inner, type, string)(char *restrict buffer, mpeg4_box_type_t type)
{
	register char *restrict p;
	register uintptr_t i;
	uint8_t c;
	p = buffer;
	for (i = 0; i < 4; ++i)
	{
		if (!((c = type.c[i]) & 0x80))
			*p++ = (char) c;
		else
		{
			*p++ = (char) ((c >> 6) | 0xc0);
			*p++ = (char) ((c & 0x3f) | 0x80);
		}
	}
	*p = 0;
	return buffer;
}
