#include "image_resample.h"
#include <stdlib.h>
#include <memory.h>
#include <math.h>

static void image_resample_free_func(image_resample_s *restrict r)
{
	if (r->src)
		free((void *) r->src);
	if (r->dst)
		free(r->dst);
}

image_resample_s* image_resample_alloc(void)
{
	image_resample_s *restrict r;
	if ((r = (image_resample_s *) refer_alloz(sizeof(image_resample_s))))
	{
		refer_set_free(r, (refer_free_f) image_resample_free_func);
		r->matrix[0] = 1;
		r->matrix[1] = 0;
		r->matrix[2] = 0;
		r->matrix[3] = 0;
		r->matrix[4] = 1;
		r->matrix[5] = 0;
		r->matrix[6] = 0;
		r->matrix[7] = 0;
		r->matrix[8] = 1;
		r->bgcolor = 0xff7f7f7f;
	}
	return r;
}

image_resample_s* image_resample_set_src(image_resample_s *restrict r, const uint32_t *restrict data, uint32_t width, uint32_t height)
{
	float *restrict p;
	if (data && width && height)
	{
		const uint8_t *restrict s;
		uintptr_t n;
		n = width * height * 4;
		s = (const uint8_t *) data;
		if (n && (p = (float *) malloc(n * sizeof(float))))
		{
			do {
				--n;
				p[n] = (float) s[n];
			} while (n);
			label_ok:
			if (r->src)
				free((void *) r->src);
			r->src = p;
			r->s_width = width;
			r->s_height = height;
			return r;
		}
	}
	else if (!data && !width && !height)
	{
		p = NULL;
		goto label_ok;
	}
	return NULL;
}

static image_resample_s* image_resample_need_dst(image_resample_s *restrict r, register uint64_t n)
{
	uint32_t *restrict p;
	uint64_t size;
	if (n <= r->d_size)
	{
		label_ok:
		return r;
	}
	--n;
	n |= 0xff;
	n |= n >> 32;
	n |= n >> 16;
	n |= n >> 8;
	n |= n >> 4;
	n |= n >> 2;
	n |= n >> 1;
	++n;
	if ((size = n * sizeof(uint32_t)) && (p = malloc(size)))
	{
		if (r->dst)
			free(r->dst);
		r->dst = p;
		r->d_size = n;
		goto label_ok;
	}
	return NULL;
}

image_resample_s* image_resample_set_dst(image_resample_s *restrict r, uint32_t width, uint32_t height)
{
	if (width && height)
	{
		if (image_resample_need_dst(r, width * height))
		{
			r->d_width = width;
			r->d_height = height;
			return r;
		}
	}
	else if (!width && !height)
	{
		if (r->dst)
		{
			free(r->dst);
			r->dst = NULL;
		}
		r->d_size = 0;
		r->d_width = 0;
		r->d_height = 0;
		return r;
	}
	return NULL;
}

static float* image_resample_mat3_mul(float r[9], float a[9], float b[9])
{
	#define set(x, y)  r[y * 3 + x] =\
		a[y * 3 + 0] * b[0 * 3 + x] +\
		a[y * 3 + 1] * b[1 * 3 + x] +\
		a[y * 3 + 2] * b[2 * 3 + x]
	#define set_line(y) set(0, y); set(1, y); set(2, y)
	set_line(0);
	set_line(1);
	set_line(2);
	#undef set_line
	#undef set
	return r;
}

void image_resample_m_mul(image_resample_s *restrict r, float k[9])
{
	float m[9];
	memcpy(r->matrix, image_resample_mat3_mul(m, r->matrix, k), sizeof(m));
}

void image_resample_m_tran(image_resample_s *restrict r, float x, float y)
{
	image_resample_m_mul(r, (float [9]) {
		1, 0, x,
		0, 1, y,
		0, 0, 1
	});
}

void image_resample_m_rota(image_resample_s *restrict r, float x, float y, float rad)
{
	float t[9] = {
		1, 0, x,
		0, 1, y,
		0, 0, 1
	};
	float k1[9], k2[9], s, c;
	s = sinf(rad);
	c = cosf(rad);
	image_resample_mat3_mul(k1, t, (float [9]) {
		c, -s, 0,
		s, c, 0,
		0, 0, 1
	});
	t[2] = -x;
	t[5] = -y;
	image_resample_mat3_mul(k2, k1, t);
	image_resample_m_mul(r, k2);
}

void image_resample_m_scale(image_resample_s *restrict r, float x, float y, float k)
{
	float t[9] = {
		1, 0, x,
		0, 1, y,
		0, 0, 1
	};
	float k1[9], k2[9];
	image_resample_mat3_mul(k1, t, (float [9]) {
		k, 0, 0,
		0, k, 0,
		0, 0, 1
	});
	t[2] = -x;
	t[5] = -y;
	image_resample_mat3_mul(k2, k1, t);
	image_resample_m_mul(r, k2);
}

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

image_resample_s* image_resample_get_dst(image_resample_s *restrict r)
{
	uint32_t *restrict p, x, y;
	float *restrict m, z;
	if ((p = r->dst) && r->src)
	{
		m = r->matrix;
		for (y = 0; y < r->d_height; ++y)
		{
			for (x = 0; x < r->d_width; ++x)
			{
				z = 1.0f / (m[6] * x + m[7] * y + m[8]);
				*p++ = image_resample_get_src(r,
					(m[0] * x + m[1] * y + m[2]) * z,
					(m[3] * x + m[4] * y + m[5]) * z);
			}
		}
		return r;
	}
	return NULL;
}
