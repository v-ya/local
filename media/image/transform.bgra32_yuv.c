#include "transform.h"

// Y = |  0.299   0.587   0.114 |   R
// U = | -0.169  -0.331   0.500 | * G
// V = |  0.500  -0.419  -0.081 |   B
// 
// R = | 1           1.402 |   Y
// G = | 1  -0.344  -0.714 | * U
// B = | 1   1.772         |   V

#define bsh_bits       8
#define bsh_number     (1 << bsh_bits)
#define bsh_mask       (1 << (bsh_bits - 1))
#define bsh_value(_v)  (int32_t) (_v * bsh_number + 0.5)

#define bsh_rv  bsh_value(1.402)
#define bsh_gu  bsh_value(0.344)
#define bsh_gv  bsh_value(0.714)
#define bsh_bu  bsh_value(1.772)

static inline void media_transform_inner_yuv2rgb(uint8_t *restrict bgr, int32_t y, int32_t u, int32_t v)
{
	int32_t r, g, b;
	y <<= bsh_bits;
	u -= 128;
	v -= 128;
	r = y + v * bsh_rv;
	g = y - u * bsh_gu - v * bsh_gv;
	b = y + u * bsh_bu;
	r = (r + (r & bsh_mask)) >> bsh_bits;
	g = (g + (g & bsh_mask)) >> bsh_bits;
	b = (b + (b & bsh_mask)) >> bsh_bits;
	#define clip(_v, _min, _max)  if (_v < _min) _v = _min; if (_v > _max) _v = _max
	clip(r, 0, 255);
	clip(g, 0, 255);
	clip(b, 0, 255);
	#undef clip
	bgr[0] = b;
	bgr[1] = g;
	bgr[2] = r;
}

// yuv_8_411 => bgra32

static d_media_runtime__deal(yuv_8_411_bgra32, struct media_transform_param__d2c31_t, refer_t)
{
	const uint8_t *restrict sy, *restrict su, *restrict sv;
	uint8_t *restrict d, *restrict dd;
	uintptr_t w, n, x, y, xe;
	sy = param->src1;
	su = param->src2;
	sv = param->src3;
	d = param->dst;
	w = param->w;
	n = param->n;
	x = param->x;
	y = param->y;
	sy += param->src1_linesize * y;
	su += param->src2_linesize * (y >> 1);
	sv += param->src3_linesize * (y >> 1);
	d += param->dst_linesize * y;
	while (n)
	{
		xe = (w <= x + n)?w:(x + n);
		n -= xe - x;
		while (x < xe)
		{
			media_transform_inner_yuv2rgb(
				dd = &d[x * 4],
				(int32_t) sy[x],
				(int32_t) su[x >> 1],
				(int32_t) sv[x >> 1]);
			dd[3] = 0;
			x += 1;
		}
		sy += param->src1_linesize;
		if (y & 1)
		{
			su += param->src2_linesize;
			sv += param->src3_linesize;
		}
		d += param->dst_linesize;
		y += 1;
		x = 0;
	}
	return param;
}

static d_media_runtime__emit(yuv_8_411_bgra32)
{
	return media_transform_inner_image_emit__d2c31(task,
		(const struct media_frame_s *) pri_data[0],
		(struct media_frame_s *) pri_data[1], rt, uc,
		(media_runtime_deal_f) media_runtime_symbol(deal, yuv_8_411_bgra32), NULL);
}

static d_media_transform__task_append(yuv_8_411_bgra32)
{
	struct media_runtime_task_step_t steps[] = {
		{
			.emit = media_runtime_symbol(emit, yuv_8_411_bgra32),
			.pri_number = 2,
			.pri_data = (const refer_t []) {sf, df}
		}
	};
	return media_runtime_task_list_append(list, steps, sizeof(steps) / sizeof(*steps));
}

struct media_transform_id_s* media_transform_create_image__yuv_8_411_bgra32(const struct media_s *restrict media)
{
	struct media_transform_id_func_t func = {
		.initial_judge = NULL,
		.create_pri = NULL,
		.open_codec = NULL,
		.dst_frame = NULL,
		.task_append = media_transform_symbol(task_append, yuv_8_411_bgra32),
	};
	return media_transform_id_alloc(sizeof(struct media_transform_id_s), media, media_nf_yuv_8_411, media_nf_bgra32, &func);
}

// yuv_8_111 => bgra32

static d_media_runtime__deal(yuv_8_111_bgra32, struct media_transform_param__d2c31_t, refer_t)
{
	const uint8_t *restrict sy, *restrict su, *restrict sv;
	uint8_t *restrict d, *restrict dd;
	uintptr_t w, n, x, y, xe;
	sy = param->src1;
	su = param->src2;
	sv = param->src3;
	d = param->dst;
	w = param->w;
	n = param->n;
	x = param->x;
	y = param->y;
	sy += param->src1_linesize * y;
	su += param->src2_linesize * y;
	sv += param->src3_linesize * y;
	d += param->dst_linesize * y;
	while (n)
	{
		xe = (w <= x + n)?w:(x + n);
		n -= xe - x;
		while (x < xe)
		{
			media_transform_inner_yuv2rgb(
				dd = &d[x * 4],
				(int32_t) sy[x],
				(int32_t) su[x],
				(int32_t) sv[x]);
			dd[3] = 0;
			x += 1;
		}
		sy += param->src1_linesize;
		su += param->src2_linesize;
		sv += param->src3_linesize;
		d += param->dst_linesize;
		y += 1;
		x = 0;
	}
	return param;
}

static d_media_runtime__emit(yuv_8_111_bgra32)
{
	return media_transform_inner_image_emit__d2c31(task,
		(const struct media_frame_s *) pri_data[0],
		(struct media_frame_s *) pri_data[1], rt, uc,
		(media_runtime_deal_f) media_runtime_symbol(deal, yuv_8_111_bgra32), NULL);
}

static d_media_transform__task_append(yuv_8_111_bgra32)
{
	struct media_runtime_task_step_t steps[] = {
		{
			.emit = media_runtime_symbol(emit, yuv_8_111_bgra32),
			.pri_number = 2,
			.pri_data = (const refer_t []) {sf, df}
		}
	};
	return media_runtime_task_list_append(list, steps, sizeof(steps) / sizeof(*steps));
}

struct media_transform_id_s* media_transform_create_image__yuv_8_111_bgra32(const struct media_s *restrict media)
{
	struct media_transform_id_func_t func = {
		.initial_judge = NULL,
		.create_pri = NULL,
		.open_codec = NULL,
		.dst_frame = NULL,
		.task_append = media_transform_symbol(task_append, yuv_8_111_bgra32),
	};
	return media_transform_id_alloc(sizeof(struct media_transform_id_s), media, media_nf_yuv_8_111, media_nf_bgra32, &func);
}
