#include "zigzag.h"

static void media_zigzag_initial(uintptr_t *restrict i2s, uintptr_t *restrict s2i, uintptr_t w, uintptr_t h)
{
	uintptr_t n, xy, seq, y, x, index;
	n = w + h;
	xy = seq = 0;
	while (xy <= n)
	{
		for (x = 0; x <= xy; ++x)
		{
			y = xy - x;
			if (x < w && y < h)
			{
				index = y * w + x;
				i2s[index] = seq;
				s2i[seq] = index;
				++seq;
			}
		}
		++xy;
		if (xy > n) break;
		for (y = 0; y <= xy; ++y)
		{
			x = xy - y;
			if (x < w && y < h)
			{
				index = y * w + x;
				i2s[index] = seq;
				s2i[seq] = index;
				++seq;
			}
		}
		++xy;
	}
}

struct media_zigzag_s* media_zigzag_alloc(uintptr_t w, uintptr_t h)
{
	struct media_zigzag_s *restrict r;
	uintptr_t *restrict i2s, *restrict s2i;
	uintptr_t n, size;
	size = sizeof(struct media_zigzag_s) + sizeof(uintptr_t) * 2 * (n = w * h);
	if (w && h && (r = (struct media_zigzag_s *) refer_alloc(size)))
	{
		r->w = w;
		r->h = h;
		r->i2s = i2s = (uintptr_t *) (r + 1);
		r->s2i = s2i = i2s + n;
		media_zigzag_initial(i2s, s2i, w, h);
		return r;
	}
	return NULL;
}
