#include "inner.h"

static uintptr_t udns_inner_punycode_bias_adapt(uintptr_t delta, uintptr_t n_points, uintptr_t is_first)
{
	uintptr_t k;
	delta /= is_first?700:2;
	delta += delta / n_points;
	// 455 = ((36 - 1) * 26) / 2
	for (k = 0; delta > 455; k += 36)
		delta /= 35;
	return k + ((36 * delta) / (delta + 38));
}

static uintptr_t udns_inner_punycode_encode_unicode(uint8_t *restrict punycode, uintptr_t size, uintptr_t bias, uintptr_t delta)
{
	static const uint8_t mapping[36] = {
		'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l',
		'm', 'n', 'o', 'p', 'q', 'r',
		's', 't', 'u', 'v', 'w', 'x',
		'y', 'z', '0', '1', '2', '3',
		'4', '5', '6', '7', '8', '9'
	};
	uintptr_t i, k, t;
	i = 0;
	k = 36;
	for (;;)
	{
		if (k <= bias)
			t = 1;
		else if (k >= bias + 26)
			t = 26;
		else t = k - bias;
		if (delta < t)
			break;
		if (i >= size)
			goto label_fail;
		delta -= t;
		punycode[i++] = mapping[t + delta % (36 - t)];
		delta /= (36 - t);
		k += 36;
	}
	if (i >= size)
		goto label_fail;
	punycode[i++] = mapping[delta];
	return i;
	label_fail:
	return 0;
}

static uintptr_t udns_inner_punycode_decode_unicode(const uint8_t *restrict punycode, uintptr_t size, uintptr_t bias, uintptr_t *restrict delta)
{
	static const uint8_t check[256] = {
		['0' ... '9'] = 1,
		['A' ... 'Z'] = 1,
		['a' ... 'z'] = 1,
	};
	static const uint8_t mapping[256] = {
		// 0 - 9
		['0'] = 26, ['1'] = 27, ['2'] = 28, ['3'] = 29, ['4'] = 30,
		['5'] = 31, ['6'] = 32, ['7'] = 33, ['8'] = 34, ['9'] = 35,
		// A - Z
		['A'] =  0, ['B'] =  1, ['C'] =  2, ['D'] =  3, ['E'] =  4,
		['F'] =  5, ['G'] =  6, ['H'] =  7, ['I'] =  8, ['J'] =  9,
		['K'] = 10, ['L'] = 11, ['M'] = 12, ['N'] = 13, ['O'] = 14,
		['P'] = 15, ['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19,
		['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23, ['Y'] = 24,
		['Z'] = 25,
		// a - z
		['a'] =  0, ['b'] =  1, ['c'] =  2, ['d'] =  3, ['e'] =  4,
		['f'] =  5, ['g'] =  6, ['h'] =  7, ['i'] =  8, ['j'] =  9,
		['k'] = 10, ['l'] = 11, ['m'] = 12, ['n'] = 13, ['o'] = 14,
		['p'] = 15, ['q'] = 16, ['r'] = 17, ['s'] = 18, ['t'] = 19,
		['u'] = 20, ['v'] = 21, ['w'] = 22, ['x'] = 23, ['y'] = 24,
		['z'] = 25,
	};
	uintptr_t i, w, k, u, t;
	i = 0;
	w = 1;
	k = 36;
	for (;;)
	{
		if (i >= size)
			goto label_fail;
		if (!check[punycode[i]])
			goto label_fail;
		u = mapping[punycode[i]];
		++i;
		*delta += u * w;
		if (k <= bias)
			t = 1;
		else if (k >= bias + 26)
			t = 26;
		else t = k - bias;
		if (u < t)
			break;
		w *= 36 - t;
		k += 36;
	}
	return i;
	label_fail:
	return 0;
}

uintptr_t udns_inner_punycode4unicode(uint8_t *restrict punycode, uintptr_t punycode_size, const uint32_t *unicode, uintptr_t unicode_size, udns_inner_punycode_point_t *restrict unicode_point)
{
	uintptr_t index, point, i, u, p, p_last;
	point = ~(uintptr_t) 0;
	index = 0;
	for (i = 0; i < unicode_size; ++i)
	{
		if (index >= punycode_size)
			goto label_fail;
		if ((u = unicode[i]) < 128)
			punycode[index++] = u;
		else
		{
			// order littel => big to unicode_point[]
			p = point;
			p_last = ~(uintptr_t) 0;
			unicode_point[i].lmin = index;
			unicode_point[i].rmin = unicode_size - i - 1;
			while (~p && unicode[p] <= u)
			{
				if (unicode[p] < u)
					++unicode_point[i].lmin;
				--unicode_point[p].rmin;
				p = unicode_point[p_last = p].point;
			}
			unicode_point[i].point = p;
			if (~p_last)
				unicode_point[p_last].point = i;
			else point = i;
			while (~p)
			{
				if (p > i)
					++unicode_point[p].lmin;
				p = unicode_point[p].point;
			}
		}
	}
	if ((i = index))
	{
		if (index >= punycode_size)
			goto label_fail;
		punycode[index++] = '-';
	}
	if (~(p = point))
	{
		uintptr_t bias, delta, h, rn;
		h = i;
		bias = 72;
		delta = 0;
		u = 128;
		p_last = i = 0;
		do {
			if (unicode[p] != p_last)
				delta += (unicode[p] - u) * (h + 1) + unicode_point[p].lmin + !!p_last;
			else delta = unicode_point[p].lmin - i;
			u = (p_last = unicode[p]) + 1;
			// encode to punycode
			rn = udns_inner_punycode_encode_unicode(punycode + index, punycode_size - index, bias, delta);
			if (!rn) goto label_fail;
			index += rn;
			// update bias
			bias = udns_inner_punycode_bias_adapt(delta, ++h, p == point);
			i = unicode_point[p].lmin;
			delta = unicode_point[p].rmin;
		} while (~(p = unicode_point[p].point));
	}
	return index;
	label_fail:
	return 0;
}

uintptr_t udns_inner_punycode2unicode(const uint8_t *restrict punycode, uintptr_t punycode_size, uint32_t *unicode, uintptr_t unicode_size)
{
	uintptr_t i, n, u, rn, bias, delta, delta_last, u_last;
	if (punycode_size)
	{
		n = 0;
		for (i = punycode_size; i && punycode[i - 1] != '-'; --i) ;
		if (i)
		{
			n = i - 1;
			if (n > unicode_size)
				goto label_fail;
			for (rn = 0; rn < n; ++rn)
				unicode[rn] = (uint32_t) punycode[rn];
		}
		u = 128;
		bias = 72;
		delta = u_last = 0;
		while (i < punycode_size)
		{
			if (n >= unicode_size)
				goto label_fail;
			delta_last = delta;
			rn = udns_inner_punycode_decode_unicode(punycode + i, punycode_size - i, bias, &delta);
			if (!rn)
				goto label_fail;
			i += rn;
			++n;
			bias = udns_inner_punycode_bias_adapt(delta - delta_last, n, !delta_last);
			u += delta / n;
			delta = delta % n;
			for (rn = n - 1; rn > delta; --rn)
				unicode[rn] = unicode[rn - 1];
			unicode[delta++] = (uint32_t) u;
		}
		return n;
	}
	label_fail:
	return 0;
}
