#include "image_resample.h"
#include <yaw.h>
#include <stdlib.h>
#include <memory.h>
#include <alloca.h>
#include <math.h>

static void image_resample_free_func(image_resample_s *restrict r)
{
	if (r->mtask)
		refer_free(r->mtask);
	if (r->src)
		free((void *) r->src);
}

image_resample_s* image_resample_alloc(uint32_t n_multicalc, uint32_t bgcolor)
{
	image_resample_s *restrict r;
	mtask_param_inst_t param;
	mtask_param_pipe_t pipe;
	if ((r = (image_resample_s *) refer_alloz(sizeof(image_resample_s))))
	{
		refer_set_free(r, (refer_free_f) image_resample_free_func);
		if (!n_multicalc) n_multicalc = 1;
		param.task_cache_number = 4096;
		param.pipe_number = 1;
		param.pipe_param = &pipe;
		pipe.core_number = n_multicalc;
		pipe.queue_input_size = 4096;
		pipe.queue_interrupt_size = 64;
		pipe.friendly = 1;
		if ((r->mtask = mtask_inst_alloc(&param)))
		{
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

image_resample_s* image_resample_set_dst(image_resample_s *restrict r, uint32_t *restrict data, uint32_t width, uint32_t height)
{
	r->d_width = width;
	r->d_height = height;
	r->d_size = (uint64_t) width * height;
	r->dst = data;
	return r;
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

// resample to dst

typedef struct image_resample_dst_task_s {
	image_resample_s *resample_only_pointer;
	uintptr_t n;
	uint32_t x;
	uint32_t y;
} image_resample_dst_task_s;

image_resample_s* image_resample_get_dst_subblock(image_resample_s *restrict r, uint32_t x, uint32_t y, uintptr_t n);

static void image_resample_dst_task_func(image_resample_dst_task_s *restrict data, mtask_context_t *restrict c)
{
	image_resample_get_dst_subblock(data->resample_only_pointer, data->x, data->y, data->n);
}

static void image_resample_get_dst_push_data(image_resample_s *restrict r)
{
	image_resample_dst_task_s *restrict data;
	uintptr_t nl, np;
	uint32_t y, rh;
	if (r->dst && r->d_width && r->d_height)
	{
		nl = (r->mc_have_min_pixels + r->d_width - 1) / r->d_width;
		if (nl)
		{
			rh = r->d_height;
			y = 0;
			while (rh)
			{
				if (nl > rh) nl = rh;
				np = nl * r->d_width;
				if ((data = (image_resample_dst_task_s *) refer_alloc(sizeof(image_resample_dst_task_s))))
				{
					data->resample_only_pointer = r;
					data->n = np;
					data->x = 0;
					data->y = y;
					mtask_push_task_nonblock(r->mtask, 0, (mtask_deal_f) image_resample_dst_task_func, data);
					refer_free(data);
				}
				y += nl;
				rh -= nl;
			}
		}
	}
}

static void image_resample_dst_semaphore_func(yaw_signal_s *restrict data, mtask_context_t *restrict c)
{
	yaw_signal_set(data, 1);
	yaw_signal_wake(data, ~0);
}

static image_resample_s* image_resample_get_dst_wait_data(image_resample_s *restrict r)
{
	yaw_signal_s *signal;
	image_resample_s *rr;
	rr = NULL;
	if ((signal = yaw_signal_alloc()))
	{
		yaw_signal_set(signal, 0);
		if (mtask_push_semaphore_single(r->mtask, 0, 1, (mtask_deal_f) image_resample_dst_semaphore_func, signal))
		{
			mtask_active_pipe(r->mtask, 0);
			do {
				yaw_signal_wait(signal, 0);
			} while (yaw_signal_get(signal) == 0);
			rr = r;
		}
		refer_free(signal);
	}
	return rr;
}

image_resample_s* image_resample_get_dst(image_resample_s *restrict r)
{
	image_resample_get_dst_push_data(r);
	mtask_active_pipe(r->mtask, 0);
	return image_resample_get_dst_wait_data(r);
}
