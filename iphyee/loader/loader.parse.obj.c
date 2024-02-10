#include "loader.model.h"
#include <memory.h>

typedef struct iphyee_loader_parse_obj_t iphyee_loader_parse_obj_t;

struct iphyee_loader_parse_obj_t {
	iphyee_loader_model_s *model;
	iphyee_loader_model_surface_s *surface;
};

#define d_test_space(_c, _e, _x)  (_c _e ' ' _x _c _e '\t' _x _c _e '\r' _x _c _e '\n')

static uintptr_t iphyee_loader_parse_obj_float(const char *restrict p, uintptr_t n, uintptr_t i, double *restrict v)
{
	double vv, uu;
	uint32_t neg;
	vv = 0;
	neg = 0;
	if (i < n && p[i] == '-')
	{
		neg = 1;
		i += 1;
	}
	while (i < n && p[i] >= '0' && p[i] <= '9')
	{
		vv = vv * 10 + ((int) p[i] - '0');
		i += 1;
	}
	if (i < n && p[i] == '.')
	{
		uu = (double) 1;
		i += 1;
		while (i < n && p[i] >= '0' && p[i] <= '9')
		{
			uu /= 10;
			vv += ((int) p[i] - '0') * uu;
			i += 1;
		}
	}
	*v = (neg?-vv:vv);
	return i;
}
static uintptr_t iphyee_loader_parse_obj_uint(const char *restrict p, uintptr_t n, uintptr_t i, uintptr_t *restrict v)
{
	uintptr_t vv;
	vv = 0;
	while (i < n && p[i] >= '0' && p[i] <= '9')
	{
		vv = vv * 10 + ((uintptr_t) p[i] - (uintptr_t) '0');
		i += 1;
	}
	*v = vv;
	return i;
}

static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_o(iphyee_loader_parse_obj_t *restrict c, const char *restrict p, uintptr_t n, uintptr_t i)
{
	uintptr_t in;
	while (i < n && d_test_space(p[i], ==, ||)) i += 1;
	in = i;
	while (i < n && d_test_space(p[i], !=, &&)) i += 1;
	if ((c->surface = iphyee_loader_model_append_fragment_surface(c->model, p + in, i - in)))
		return c;
	return NULL;
}
static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_v(iphyee_loader_parse_obj_t *restrict c, const char *restrict p, uintptr_t n, uintptr_t i)
{
	double vec[3];
	while (i < n && d_test_space(p[i], ==, ||)) i += 1;
	i = iphyee_loader_parse_obj_float(p, n, i, vec);
	if (i >= n || d_test_space(p[i], !=, &&))
		goto label_fail;
	while (i < n && d_test_space(p[i], ==, ||)) i += 1;
	i = iphyee_loader_parse_obj_float(p, n, i, vec + 1);
	if (i >= n || d_test_space(p[i], !=, &&))
		goto label_fail;
	while (i < n && d_test_space(p[i], ==, ||)) i += 1;
	i = iphyee_loader_parse_obj_float(p, n, i, vec + 2);
	if (i < n && d_test_space(p[i], !=, &&))
		goto label_fail;
	if (iphyee_loader_model_append_vertex(c->model, vec))
		return c;
	label_fail:
	return NULL;
}
static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_vn(iphyee_loader_parse_obj_t *restrict c, const char *restrict p, uintptr_t n, uintptr_t i)
{
	double vec[3];
	while (i < n && d_test_space(p[i], ==, ||)) i += 1;
	i = iphyee_loader_parse_obj_float(p, n, i, vec);
	if (i >= n || d_test_space(p[i], !=, &&))
		goto label_fail;
	while (i < n && d_test_space(p[i], ==, ||)) i += 1;
	i = iphyee_loader_parse_obj_float(p, n, i, vec + 1);
	if (i >= n || d_test_space(p[i], !=, &&))
		goto label_fail;
	while (i < n && d_test_space(p[i], ==, ||)) i += 1;
	i = iphyee_loader_parse_obj_float(p, n, i, vec + 2);
	if (i < n && d_test_space(p[i], !=, &&))
		goto label_fail;
	if (iphyee_loader_model_append_normal(c->model, vec))
		return c;
	label_fail:
	return NULL;
}
static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_vt(iphyee_loader_parse_obj_t *restrict c, const char *restrict p, uintptr_t n, uintptr_t i)
{
	double vec[2];
	while (i < n && d_test_space(p[i], ==, ||)) i += 1;
	i = iphyee_loader_parse_obj_float(p, n, i, vec);
	if (i >= n || d_test_space(p[i], !=, &&))
		goto label_fail;
	while (i < n && d_test_space(p[i], ==, ||)) i += 1;
	i = iphyee_loader_parse_obj_float(p, n, i, vec + 1);
	if (i < n && d_test_space(p[i], !=, &&))
		goto label_fail;
	if (iphyee_loader_model_append_texture(c->model, vec))
		return c;
	label_fail:
	return NULL;
}
static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_f(iphyee_loader_parse_obj_t *restrict c, const char *restrict p, uintptr_t n, uintptr_t i)
{
	uintptr_t fragment_offset;
	uintptr_t fragment_number;
	uintptr_t vec[3];
	fragment_offset = c->model->fragment->fragment->count;
	fragment_number = 0;
	while (i < n && d_test_space(p[i], ==, ||)) i += 1;
	while (i < n)
	{
		i = iphyee_loader_parse_obj_uint(p, n, i, vec);
		if (i < n && p[i] == '/')
		{
			i += 1;
			i = iphyee_loader_parse_obj_uint(p, n, i, vec + 1);
			if (i < n && p[i] == '/')
			{
				i += 1;
				i = iphyee_loader_parse_obj_uint(p, n, i, vec + 2);
			}
			else vec[2] = 0;
		}
		else vec[1] = 0, vec[2] = 0;
		if (i < n && d_test_space(p[i], !=, &&))
			goto label_fail;
		while (i < n && d_test_space(p[i], ==, ||)) i += 1;
		if (!iphyee_loader_model_append_fragment(c->model, vec, 3))
			goto label_fail;
		fragment_number += 1;
	}
	if (c->surface || (c->surface = iphyee_loader_model_append_fragment_surface(c->model, NULL, 0)))
	{
		if (iphyee_loader_model_append_surface(c->model, c->surface, fragment_offset, fragment_number))
			return c;
	}
	label_fail:
	return NULL;
}
static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_line(iphyee_loader_parse_obj_t *restrict c, const char *restrict p, uintptr_t n)
{
	uintptr_t i;
	for (i = 0; i < n && d_test_space(p[i], !=, &&); ++i) ;
	if (i == 1)
	{
		if (p[0] == 'v') return iphyee_loader_parse_obj_v(c, p, n, i);
		if (p[0] == 'f') return iphyee_loader_parse_obj_f(c, p, n, i);
		if (p[0] == 'o') return iphyee_loader_parse_obj_o(c, p, n, i);
	}
	else if (i == 2)
	{
		if (p[0] == 'v')
		{
			if (p[1] == 'n') return iphyee_loader_parse_obj_vn(c, p, n, i);
			if (p[1] == 't') return iphyee_loader_parse_obj_vt(c, p, n, i);
		}
	}
	return c;
}
static layer_model_item_s* iphyee_loader_parse_obj_func(const iphyee_loader_s *loader, const char *restrict data, uintptr_t size)
{
	iphyee_loader_parse_obj_t c;
	layer_model_item_s *model;
	const char *restrict p;
	uintptr_t n;
	model = NULL;
	if ((c.model = iphyee_loader_model_alloc(loader,
		iphyee_loader_model_eblock__vertex |
		iphyee_loader_model_eblock__normal |
		iphyee_loader_model_eblock__texture |
		iphyee_loader_model_eblock__nocheck,
		32, 32)))
	{
		c.surface = NULL;
		while (size)
		{
			p = memchr(data, '\n', size);
			n = p?((uintptr_t) p - (uintptr_t) data + 1):size;
			if (!iphyee_loader_parse_obj_line(&c, data, n))
				goto label_fail;
			data += n;
			size -= n;
		}
		if (iphyee_loader_model_update(c.model))
			model = (layer_model_item_s *) refer_save(c.model->model);
		label_fail:
		refer_free(c.model);
	}
	return model;
}

layer_model_item_s* iphyee_loader_parse_obj_by_memory(const iphyee_loader_s *restrict loader, const char *restrict data, uintptr_t size)
{
	return iphyee_loader_parse_obj_func(loader, data, size);
}

layer_model_item_s* iphyee_loader_parse_obj_by_file(const iphyee_loader_s *restrict loader, layer_file_s *restrict file, uint64_t offset, uint64_t size)
{
	return (layer_model_item_s *) iphyee_loader_data_file2memory(
		file, offset, size, layer_file_io_read,
		(iphyee_loader_data_memory_f) iphyee_loader_parse_obj_func, loader);
}

layer_model_item_s* iphyee_loader_parse_obj_by_path(const iphyee_loader_s *restrict loader, const char *restrict path)
{
	return (layer_model_item_s *) iphyee_loader_data_path2memory(path, layer_file_io_read,
		(iphyee_loader_data_memory_f) iphyee_loader_parse_obj_func, loader);
}
