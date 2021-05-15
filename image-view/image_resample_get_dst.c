#include "image_resample.h"

static inline uint32_t image_resample_get_src(register image_resample_s *restrict r, float x, float y)
{
	if (x >= 0 && x < r->s_width && y >= 0 && y < r->s_height)
	{
		register int32_t ix, iy;
		register const float *restrict p;
		register float B, G, R, A, s, k;
		ix = (int32_t) x;
		iy = (int32_t) y;
		#define gop(_op, _k)  \
			B _op p[0] * (_k);\
			G _op p[1] * (_k);\
			R _op p[2] * (_k);\
			A _op p[3] * (_k);\
			s _op (_k)
		p = r->src + ((iy * r->s_width + ix) << 2);
		k = (2 + ix + iy) - (x + y);
		gop(=, k);
		if (x + 1 < r->s_width)
		{
			p += 1 << 2;
			k = (1 + iy - ix) + (x - y);
			gop(+=, k);
			if (y + 1 < r->s_height)
			{
				p += r->s_width << 2;
				k = (x + y) - (ix + iy);
				gop(+=, k);
				p -= 1 << 2;
				label_only_y_plus:
				k = (1 + ix - iy) + (y - x);
				gop(+=, k);
			}
		}
		else if (y + 1 < r->s_height)
		{
			p += r->s_width << 2;
			goto label_only_y_plus;
		}
		if (!s) goto label_bgcolor;
		k = 1.0f / s;
		#undef gop
		return ((uint32_t) (uint8_t) (B * k + 0.5f)) |
			((uint32_t) (uint8_t) (G * k + 0.5f) << 8) |
			((uint32_t) (uint8_t) (R * k + 0.5f) << 16) |
			((uint32_t) (uint8_t) (A * k + 0.5f) << 24);
	}
	label_bgcolor:
	return r->bgcolor;
}

image_resample_s* image_resample_get_dst_subblock(image_resample_s *restrict r, uint32_t x, uint32_t y, uintptr_t n)
{
	register uint32_t *restrict p;
	register float *restrict m;
	register float z, a, b, c;
	if ((p = r->dst) && r->src && n)
	{
		p += y * r->d_width + x;
		m = r->matrix;
		goto label_entry;
		do {
			do {
				z = 1.0f / (m[6] * x + c);
				*p++ = image_resample_get_src(r,
					(m[0] * x + a) * z,
					(m[3] * x + b) * z);
				if (!--n)
					goto label_ok;
			} while(++x < r->d_width);
			x = 0;
			++y;
			label_entry:
			a = m[1] * y + m[2];
			b = m[4] * y + m[5];
			c = m[7] * y + m[8];
		} while (y < r->d_height);
		label_ok:
		return r;
	}
	return NULL;
}
