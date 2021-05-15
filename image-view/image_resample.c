#include "image_resample.h"
#include <multicalc.h>
#include <stdlib.h>
#include <memory.h>
#include <alloca.h>
#include <math.h>

static void image_resample_free_func(image_resample_s *restrict r)
{
	if (r->multicalc)
		multicalc_free(r->multicalc);
	if (r->src)
		free((void *) r->src);
	if (r->dst)
		free(r->dst);
}

typedef struct image_resample_dst_multicalc_t image_resample_dst_multicalc_t;
static void image_resample_get_dst_multicalc_func(image_resample_dst_multicalc_t *restrict r);

image_resample_s* image_resample_alloc(uint32_t n_multicalc, uint32_t bgcolor)
{
	image_resample_s *restrict r;
	if ((r = (image_resample_s *) refer_alloz(sizeof(image_resample_s))))
	{
		refer_set_free(r, (refer_free_f) image_resample_free_func);
		if (!n_multicalc || (r->multicalc = multicalc_alloc(n_multicalc, 0)))
		{
			if (r->multicalc)
				multicalc_set_all_func(r->multicalc, (multicalc_do_f) image_resample_get_dst_multicalc_func);
			r->matrix[0] = 1;
			r->matrix[1] = 0;
			r->matrix[2] = 0;
			r->matrix[3] = 0;
			r->matrix[4] = 1;
			r->matrix[5] = 0;
			r->matrix[6] = 0;
			r->matrix[7] = 0;
			r->matrix[8] = 1;
			r->bgcolor = bgcolor;
			r->n_mc = n_multicalc;
			r->mc_have_min_pixels = 65536;
		}
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

void image_resample_m_reset(image_resample_s *restrict r)
{
	static const float E[9] = {
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	};
	memcpy(r->matrix, E, sizeof(E));
	image_resample_m_tran(r, (float) ((int32_t) (r->s_width >> 1) - (int32_t) (r->d_width >> 1)),
		(float) ((int32_t) (r->s_height >> 1) - (int32_t) (r->d_height >> 1)));
	if (r->s_width && r->s_height && r->d_width && r->d_height)
	{
		float kw, kh;
		kw = (float) r->s_width / r->d_width;
		kh = (float) r->s_height / r->d_height;
		image_resample_m_scale(r, (float) (r->d_width >> 1), (float) (r->d_height >> 1), kw > kh?kw:kh);
	}
}

struct image_resample_dst_multicalc_t {
	image_resample_s *resample;
	uintptr_t n;
	uint32_t x;
	uint32_t y;
};

image_resample_s* image_resample_get_dst_subblock(image_resample_s *restrict r, uint32_t x, uint32_t y, uintptr_t n);

static void image_resample_get_dst_multicalc_func(image_resample_dst_multicalc_t *restrict r)
{
	r->resample = image_resample_get_dst_subblock(r->resample, r->x, r->y, r->n);
}

image_resample_s* image_resample_get_dst(image_resample_s *restrict r)
{
	uintptr_t pn = r->d_width * r->d_height;
	if (!r->multicalc || pn <= r->mc_have_min_pixels)
	{
		label_mini:
		return image_resample_get_dst_subblock(r, 0, 0, pn);
	}
	else
	{
		image_resample_dst_multicalc_t *restrict mcd;
		uintptr_t L, p;
		uint32_t i, n;
		n = r->n_mc + 1;
		L = pn / n;
		if (L < r->mc_have_min_pixels)
			L = r->mc_have_min_pixels;
		n = (pn + (L - 1)) / L;
		if (n < 2) goto label_mini;
		mcd = (image_resample_dst_multicalc_t *) alloca(sizeof(image_resample_dst_multicalc_t) * n);
		if (!mcd) goto label_mini;
		p = 0;
		i = 0;
		--n;
		do {
			if (L > pn) L = pn;
			mcd[i].resample = r;
			mcd[i].n = L;
			mcd[i].x = p % r->d_width;
			mcd[i].y = p / r->d_width;
			pn -= L;
			p += L;
			if (i < n)
			{
				multicalc_set_data(r->multicalc, i, mcd + i);
				multicalc_set_status(r->multicalc, i, 1);
			}
		} while (++i <= n);
		multicalc_wake(r->multicalc);
		image_resample_get_dst_multicalc_func(mcd + n);
		multicalc_wait(r->multicalc);
		i = 0;
		do {
			if (!mcd->resample)
				r = NULL;
		} while (++i <= n);
		return r;
	}
}
