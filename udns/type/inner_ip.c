#include "inner.h"
#include <memory.h>

uint8_t* udns_inner_get_ipv4(const char *restrict s_ipv4, uint8_t *restrict a_ipv4)
{
	uint32_t a, i;
	for (i = 4; i; ++s_ipv4)
	{
		a = 0;
		for (;;)
		{
			if (*s_ipv4 >= '0' && *s_ipv4 <= '9')
				a = a * 10 + *s_ipv4 - '0';
			else if (!*s_ipv4 || *s_ipv4 == '.')
				break;
			else goto label_fail;
			if (a > 255)
				goto label_fail;
			++s_ipv4;
		}
		*a_ipv4++ = (uint8_t) a;
		--i;
		if (!*s_ipv4)
			break;
	}
	if (!i) return a_ipv4;
	label_fail:
	return NULL;
}

uint8_t* udns_inner_get_ipv6(const char *restrict s_ipv6, uint8_t *restrict a_ipv6)
{
	static const uint8_t check[256] = {
		['0' ... '9'] = 1,
		['A' ... 'F'] = 1,
		['a' ... 'f'] = 1
	};
	static const uint8_t mapping[256] = {
		['0'] =  0, ['1'] =  1, ['2'] =  2, ['3'] =  3, ['4'] =  4,
		['5'] =  5, ['6'] =  6, ['7'] =  7, ['8'] =  8, ['9'] =  9,
		['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13, ['E'] = 14, ['F'] = 15,
		['a'] = 10, ['b'] = 11, ['c'] = 12, ['d'] = 13, ['e'] = 14, ['f'] = 15,
	};
	const char *ipv6_seg_start;
	uintptr_t i, zi, n;
	zi = ~(uintptr_t) 0;
	if (*s_ipv6 != ':')
	{
		for (i = 0; i < 8; ++i)
		{
			if (*s_ipv6 != ':')
			{
				if (!*s_ipv6)
					break;
				// ::ffff:0.0.0.0
				if (*s_ipv6 == '.' && i == 2 && !zi && a_ipv6[0] == 0xff && a_ipv6[1] == 0xff)
					goto label_tail_ipv4;
				goto label_fail;
			}
			++s_ipv6;
			if (*s_ipv6 == ':')
			{
				if (~zi)
					goto label_fail;
				zi = i;
				++s_ipv6;
			}
			label_entry:
			ipv6_seg_start = s_ipv6;
			if (!check[(uint8_t) *s_ipv6])
			{
				if (!*s_ipv6)
					break;
				goto label_fail;
			}
			n = 0;
			while (check[(uint8_t) *s_ipv6])
			{
				n = (n << 4) | mapping[(uint8_t) *s_ipv6++];
				if (n >= 0x10000)
					goto label_fail;
			}
			a_ipv6[i << 1] = (uint8_t) (n >> 8);
			a_ipv6[(i << 1) | 1] = (uint8_t) n;
		}
		if (*s_ipv6)
			goto label_fail;
		if (i < 8)
		{
			if (!~zi)
				goto label_fail;
			if (i > zi)
				memmove(a_ipv6 + ((8 - i + zi) << 1), a_ipv6 + (zi << 1), (i - zi) << 1);
			memset(a_ipv6 + (zi << 1), 0, (8 - i) << 1);
		}
		label_okay:
		return a_ipv6;
	}
	else if (s_ipv6[1] == ':')
	{
		s_ipv6 += 2;
		zi = i = 0;
		goto label_entry;
	}
	label_fail:
	return NULL;
	label_tail_ipv4:
	memset(a_ipv6, 0, 10);
	a_ipv6[10] = 0xff;
	a_ipv6[11] = 0xff;
	if (udns_inner_get_ipv4(ipv6_seg_start, a_ipv6 + 12))
		goto label_okay;
	goto label_fail;
}
