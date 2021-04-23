#include "inner.type.h"

static const char* utf82unicode(const char *restrict s, uint32_t *restrict unicode)
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
		if (n < 2) goto label_reseach;
		u = c & (0xff >> (n + 1));
		while (--n)
		{
			if (((c = (uint8_t) *s) & 0xc0) != 0x80)
				break;
			u = (u << 6) | (c & 0x3f);
			++s;
		}
	}
	*unicode = u;
	return s;
}

mpeg4_box_type_t mpeg4$define(inner, type, parse)(const char *restrict s)
{
	uintptr_t i;
	mpeg4_box_type_t r;
	uint32_t c;
	r.v = 0;
	for (i = 0; i < 4 && *s; ++i)
	{
		s = utf82unicode(s, &c);
		r.c[i] = (uint8_t) c;
	}
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
