#include "inner.h"

uintptr_t udns_inner_unicode4utf8(uint32_t *restrict unicode, uintptr_t unicode_size, const char *restrict utf8, uintptr_t utf8_size)
{
	static const uint8_t uclength[64] = {
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
	uintptr_t i, p;
	uint32_t u;
	uint32_t c;
	uint32_t n;
	i = p = 0;
	while (i < utf8_size)
	{
		if (p >= unicode_size)
			goto label_fail;
		if (!((c = (uint8_t) utf8[i++]) & 0x80))
			unicode[p++] = c;
		else
		{
			n = (uint32_t) uclength[(c >> 1) & 0x3f];
			if (n < 2) goto label_fail;
			u = c & (0xff >> (n + 1));
			while (--n)
			{
				if (i >= utf8_size)
					goto label_fail;
				if (((c = (uint8_t) utf8[i++]) & 0xc0) != 0x80)
					goto label_fail;
				u = (u << 6) | (c & 0x3f);
			}
			unicode[p++] = u;
		}
	}
	return p;
	label_fail:
	return 0;
}

uintptr_t udns_inner_unicode2utf8(const uint32_t *restrict unicode, uintptr_t unicode_size, char *restrict utf8, uintptr_t utf8_size)
{
	uintptr_t i, p;
	uint32_t u, n, m;
	i = p = 0;
	while (i < unicode_size)
	{
		if (p >= utf8_size)
			goto label_fail;
		u = unicode[i++];
		if (u < (1u << 7))
			utf8[p++] = (uint8_t) u;
		else
		{
			if (u < (1u << 11))
				n = 2;
			else if (u < (1u << 16))
				n = 3;
			else if (u < (1u << 21))
				n = 4;
			else if (u < (1u << 26))
				n = 5;
			else if (u < (1u << 31))
				n = 6;
			else goto label_fail;
			if (p + n > utf8_size)
				goto label_fail;
			m = n;
			while (--m)
			{
				utf8[p + m] = (uint8_t) ((u & 0x3f) | 0x80);
				u >>= 6;
			}
			utf8[p] = (uint8_t) ((((uint32_t) 0x007f >> n) & u) | ((uint32_t) 0xff00 >> n));
			p += n;
		}
	}
	return p;
	label_fail:
	return 0;
}
