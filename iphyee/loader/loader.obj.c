#include "loader.h"
#include <vattr.h>
#include <mlog.h>
#include <fsys.h>
#include <memory.h>

// parse

typedef struct iphyee_loader_parse_obj_t iphyee_loader_parse_obj_t;

struct iphyee_loader_parse_obj_t {
	const layer_model_s *m;
	layer_model_item_s *model;
	layer_model_item_s *meshes;
	layer_model_item_s *mesh;
	layer_file_s *vdata;
	layer_file_s *ndata;
	layer_file_s *tdata;
	layer_file_s *fmdata;
	layer_file_s *fidata;
};

static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_initial(iphyee_loader_parse_obj_t *restrict c, const layer_model_s *restrict m)
{
	memset(c, 0, sizeof(*c));
	c->m = m;
	if ((c->model = layer_model_create_item(c->m, iphyee_loader_stype__o_model)))
	{
		if ((c->meshes = layer_model_set_o_create(c->model, iphyee_loader_stype__o_meshes, iphyee_loader_stype__o_meshes)))
			return c;
		refer_free(c->model);
	}
	return NULL;
}

static void iphyee_loader_parse_obj_finally(iphyee_loader_parse_obj_t *restrict c)
{
	if (c->model) refer_free(c->model);
	if (c->vdata) refer_free(c->vdata);
	if (c->ndata) refer_free(c->ndata);
	if (c->tdata) refer_free(c->tdata);
	if (c->fmdata) refer_free(c->fmdata);
	if (c->fidata) refer_free(c->fidata);
}

static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_new_mesh(iphyee_loader_parse_obj_t *restrict c, const char *restrict name, uintptr_t length)
{
	refer_string_t key;
	c->mesh = NULL;
	if ((key = refer_dump_string_with_length(name, length)))
	{
		c->mesh = layer_model_set_o_create(c->meshes, key, iphyee_loader_stype__o_mesh);
		refer_free(key);
		if (c->mesh) return c;
	}
	return NULL;
}

static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_save_mesh(iphyee_loader_parse_obj_t *restrict c)
{
	layer_model_item_s *o, *v;
	if (!c->vdata && !c->ndata && !c->tdata && !c->fmdata && !c->fidata)
		goto label_okay;
	if (c->mesh || iphyee_loader_parse_obj_new_mesh(c, NULL, 0))
	{
		if (c->vdata)
		{
			if (!(o = layer_model_set_o_create(c->mesh, iphyee_loader_sname__o_vertex, iphyee_loader_stype__o_vertex)))
				goto label_fail;
			if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__e_fbits, iphyee_loader_stype__e_fbits)))
				goto label_fail;
			if (!layer_model_set_e_value(v, 32))
				goto label_fail;
			if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__d_vertex, layer_model_stype__data)))
				goto label_fail;
			if (!layer_model_set_d(v, c->vdata, 0, layer_file_size(c->vdata)))
				goto label_fail;
			if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__u_vertex_count, layer_model_stype__uint)))
				goto label_fail;
			if (!layer_model_set_u(v, layer_file_size(c->vdata) / (sizeof(float) * 3)))
				goto label_fail;
			refer_free(c->vdata);
			c->vdata = NULL;
		}
		if (c->ndata)
		{
			if (!(o = layer_model_set_o_create(c->mesh, iphyee_loader_sname__o_normal, iphyee_loader_stype__o_normal)))
				goto label_fail;
			if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__e_fbits, iphyee_loader_stype__e_fbits)))
				goto label_fail;
			if (!layer_model_set_e_value(v, 32))
				goto label_fail;
			if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__d_normal, layer_model_stype__data)))
				goto label_fail;
			if (!layer_model_set_d(v, c->ndata, 0, layer_file_size(c->ndata)))
				goto label_fail;
			if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__u_normal_count, layer_model_stype__uint)))
				goto label_fail;
			if (!layer_model_set_u(v, layer_file_size(c->ndata) / (sizeof(float) * 3)))
				goto label_fail;
			refer_free(c->ndata);
			c->ndata = NULL;
		}
		if (c->tdata)
		{
			if (!(o = layer_model_set_o_create(c->mesh, iphyee_loader_sname__o_texture, iphyee_loader_stype__o_texture)))
				goto label_fail;
			if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__e_fbits, iphyee_loader_stype__e_fbits)))
				goto label_fail;
			if (!layer_model_set_e_value(v, 32))
				goto label_fail;
			if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__d_texture, layer_model_stype__data)))
				goto label_fail;
			if (!layer_model_set_d(v, c->tdata, 0, layer_file_size(c->tdata)))
				goto label_fail;
			if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__u_texture_count, layer_model_stype__uint)))
				goto label_fail;
			if (!layer_model_set_u(v, layer_file_size(c->tdata) / (sizeof(float) * 2)))
				goto label_fail;
			refer_free(c->tdata);
			c->tdata = NULL;
		}
		if (c->fmdata || c->fidata)
		{
			if (!(o = layer_model_set_o_create(c->mesh, iphyee_loader_sname__o_fragment, iphyee_loader_stype__o_fragment)))
				goto label_fail;
			if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__e_ubits, iphyee_loader_stype__e_ubits)))
				goto label_fail;
			if (!layer_model_set_e_value(v, 32))
				goto label_fail;
			if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__b_fragment_vertex, layer_model_stype__boolean)))
				goto label_fail;
			if (!layer_model_set_b(v, 1))
				goto label_fail;
			if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__b_fragment_normal, layer_model_stype__boolean)))
				goto label_fail;
			if (!layer_model_set_b(v, 1))
				goto label_fail;
			if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__b_fragment_texture, layer_model_stype__boolean)))
				goto label_fail;
			if (!layer_model_set_b(v, 1))
				goto label_fail;
			if (c->fmdata)
			{
				if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__d_fragment, layer_model_stype__data)))
					goto label_fail;
				if (!layer_model_set_d(v, c->fmdata, 0, layer_file_size(c->fmdata)))
					goto label_fail;
				if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__u_fragment_count, layer_model_stype__uint)))
					goto label_fail;
				if (!layer_model_set_u(v, (c->fidata?layer_file_size(c->fidata):0) / (sizeof(uint32_t) * 2)))
					goto label_fail;
				refer_free(c->fmdata);
				c->fmdata = NULL;
			}
			if (c->fidata)
			{
				if (!(o = layer_model_set_o_create(o, iphyee_loader_sname__x_fraginfo, iphyee_loader_stype__o_fraginfo)))
					goto label_fail;
				if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__e_ubits, iphyee_loader_stype__e_ubits)))
					goto label_fail;
				if (!layer_model_set_e_value(v, 32))
					goto label_fail;
				if (!(v = layer_model_set_o_create(o, iphyee_loader_sname__d_fraginfo, layer_model_stype__data)))
					goto label_fail;
				if (!layer_model_set_d(v, c->fidata, 0, layer_file_size(c->fidata)))
					goto label_fail;
				refer_free(c->fidata);
				c->fidata = NULL;
			}
		}
		label_okay:
		c->mesh = NULL;
		return c;
	}
	label_fail:
	return NULL;
}

static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_vdata(iphyee_loader_parse_obj_t *restrict c, const float vec[3])
{
	layer_file_s *data;
	if ((data = c->vdata) || (c->vdata = data = layer_file_alloc_mbuf(NULL, 0)))
	{
		if (layer_file_write(data, vec, 1, sizeof(float) * 3))
			return c;
	}
	return NULL;
}
static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_ndata(iphyee_loader_parse_obj_t *restrict c, const float vec[3])
{
	layer_file_s *data;
	if ((data = c->ndata) || (c->ndata = data = layer_file_alloc_mbuf(NULL, 0)))
	{
		if (layer_file_write(data, vec, 1, sizeof(float) * 3))
			return c;
	}
	return NULL;
}
static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_tdata(iphyee_loader_parse_obj_t *restrict c, const float vec[2])
{
	layer_file_s *data;
	if ((data = c->tdata) || (c->tdata = data = layer_file_alloc_mbuf(NULL, 0)))
	{
		if (layer_file_write(data, vec, 1, sizeof(float) * 2))
			return c;
	}
	return NULL;
}
static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_fmdata(iphyee_loader_parse_obj_t *restrict c, const uint32_t vec[3])
{
	layer_file_s *data;
	if ((data = c->fmdata) || (c->fmdata = data = layer_file_alloc_mbuf(NULL, 0)))
	{
		if (layer_file_write(data, vec, 1, sizeof(uint32_t) * 3))
			return c;
	}
	return NULL;
}
static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_fidata(iphyee_loader_parse_obj_t *restrict c, const uint32_t pc[2])
{
	layer_file_s *data;
	if ((data = c->fidata) || (c->fidata = data = layer_file_alloc_mbuf(NULL, 0)))
	{
		if (layer_file_write(data, pc, 1, sizeof(uint32_t) * 2))
			return c;
	}
	return NULL;
}

#define d_test_space(_c, _e, _x)  (_c _e ' ' _x _c _e '\t' _x _c _e '\r' _x _c _e '\n')

static uintptr_t iphyee_loader_parse_obj_float(const char *restrict p, uintptr_t n, uintptr_t i, float *restrict v)
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
	*v = (float) (neg?-vv:vv);
	return i;
}
static uintptr_t iphyee_loader_parse_obj_uint(const char *restrict p, uintptr_t n, uintptr_t i, uint32_t *restrict v)
{
	uint32_t vv;
	vv = 0;
	while (i < n && p[i] >= '0' && p[i] <= '9')
	{
		vv = vv * 10 + ((uint32_t) p[i] - (uint32_t) '0');
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
	if (iphyee_loader_parse_obj_save_mesh(c))
		return iphyee_loader_parse_obj_new_mesh(c, p + in, i - in);
	return NULL;
}
static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_v(iphyee_loader_parse_obj_t *restrict c, const char *restrict p, uintptr_t n, uintptr_t i)
{
	float vec[3];
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
	if (iphyee_loader_parse_obj_vdata(c, vec))
		return c;
	label_fail:
	return NULL;
}
static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_vn(iphyee_loader_parse_obj_t *restrict c, const char *restrict p, uintptr_t n, uintptr_t i)
{
	float vec[3];
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
	if (iphyee_loader_parse_obj_ndata(c, vec))
		return c;
	label_fail:
	return NULL;
}
static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_vt(iphyee_loader_parse_obj_t *restrict c, const char *restrict p, uintptr_t n, uintptr_t i)
{
	float vec[2];
	while (i < n && d_test_space(p[i], ==, ||)) i += 1;
	i = iphyee_loader_parse_obj_float(p, n, i, vec);
	if (i >= n || d_test_space(p[i], !=, &&))
		goto label_fail;
	while (i < n && d_test_space(p[i], ==, ||)) i += 1;
	i = iphyee_loader_parse_obj_float(p, n, i, vec + 1);
	if (i < n && d_test_space(p[i], !=, &&))
		goto label_fail;
	if (iphyee_loader_parse_obj_tdata(c, vec))
		return c;
	label_fail:
	return NULL;
}
static iphyee_loader_parse_obj_t* iphyee_loader_parse_obj_f(iphyee_loader_parse_obj_t *restrict c, const char *restrict p, uintptr_t n, uintptr_t i)
{
	uint32_t vec[3];
	uint32_t pc[2];
	pc[0] = (c->fmdata?layer_file_size(c->fmdata):0) / (sizeof(uint32_t) * 3);
	pc[1] = 0;
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
		if (!iphyee_loader_parse_obj_fmdata(c, vec))
			goto label_fail;
		pc[1] += 1;
	}
	if (pc[1] >= 3 && iphyee_loader_parse_obj_fidata(c, pc))
		return c;
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

layer_model_item_s* iphyee_loader_parse_obj_by_memory(const iphyee_loader_s *restrict loader, const char *restrict data, uintptr_t size)
{
	iphyee_loader_parse_obj_t c;
	const char *restrict p;
	uintptr_t n;
	if (iphyee_loader_parse_obj_initial(&c, loader->model))
	{
		while (size)
		{
			p = memchr(data, '\n', size);
			n = p?((uintptr_t) p - (uintptr_t) data + 1):size;
			if (!iphyee_loader_parse_obj_line(&c, data, n))
				goto label_fail;
			data += n;
			size -= n;
		}
		if (iphyee_loader_parse_obj_save_mesh(&c))
			return c.model;
	}
	label_fail:
	iphyee_loader_parse_obj_finally(&c);
	return NULL;
}

layer_model_item_s* iphyee_loader_parse_obj_by_file(const iphyee_loader_s *restrict loader, layer_file_s *restrict file, uint64_t offset, uint64_t size)
{
	layer_model_item_s *restrict item;
	layer_file_memory_t m;
	item = NULL;
	if (file && size && (m = layer_file_mmap(file, offset, (uintptr_t) size, 0, layer_file_io_read)))
	{
		item = iphyee_loader_parse_obj_by_memory(loader, layer_file_mdata(file, m, NULL), (uintptr_t) size);
		layer_file_munmap(file, m);
	}
	return item;
}

layer_model_item_s* iphyee_loader_parse_obj_by_path(const iphyee_loader_s *restrict loader, const char *restrict path)
{
	layer_model_item_s *restrict item;
	layer_file_s *restrict file;
	item = NULL;
	if (path && (file = layer_file_alloc_mmap(path, layer_file_io_read)))
	{
		item = iphyee_loader_parse_obj_by_file(loader, file, 0, layer_file_size(file));
		refer_free(file);
	}
	return item;
}

// build

static void iphyee_loader_build_obj_v(const layer_model_item_s *restrict vertex, mlog_s *restrict ml)
{
	const layer_model_item_s *restrict v;
	layer_file_s *restrict file;
	layer_file_memory_t m;
	uint64_t offset, size;
	const float *restrict p;
	uintptr_t i, n;
	if (vertex && (v = layer_model_get_o_value(vertex, iphyee_loader_sname__e_fbits)) &&
		layer_model_get_e_value(v) == 32 &&
		(v = layer_model_get_o_value(vertex, iphyee_loader_sname__d_vertex)) &&
		layer_model_get_d(v, &file, &offset, &size) &&
		file && (m = layer_file_mmap(file, offset, size, 0, layer_file_io_read)))
	{
		p = layer_file_mdata(file, m, &n);
		n /= sizeof(float) * 3;
		for (i = 0; i < n; ++i)
		{
			mlog_printf(ml, "v %f %f %f\n", p[0], p[1], p[2]);
			p += 3;
		}
		layer_file_munmap(file, m);
	}
}
static void iphyee_loader_build_obj_vn(const layer_model_item_s *restrict normal, mlog_s *restrict ml)
{
	const layer_model_item_s *restrict v;
	layer_file_s *restrict file;
	layer_file_memory_t m;
	uint64_t offset, size;
	const float *restrict p;
	uintptr_t i, n;
	if (normal && (v = layer_model_get_o_value(normal, iphyee_loader_sname__e_fbits)) &&
		layer_model_get_e_value(v) == 32 &&
		(v = layer_model_get_o_value(normal, iphyee_loader_sname__d_normal)) &&
		layer_model_get_d(v, &file, &offset, &size) &&
		file && (m = layer_file_mmap(file, offset, size, 0, layer_file_io_read)))
	{
		p = layer_file_mdata(file, m, &n);
		n /= sizeof(float) * 3;
		for (i = 0; i < n; ++i)
		{
			mlog_printf(ml, "vn %f %f %f\n", p[0], p[1], p[2]);
			p += 3;
		}
		layer_file_munmap(file, m);
	}
}
static void iphyee_loader_build_obj_vt(const layer_model_item_s *restrict texture, mlog_s *restrict ml)
{
	const layer_model_item_s *restrict v;
	layer_file_s *restrict file;
	layer_file_memory_t m;
	uint64_t offset, size;
	const float *restrict p;
	uintptr_t i, n;
	if (texture && (v = layer_model_get_o_value(texture, iphyee_loader_sname__e_fbits)) &&
		layer_model_get_e_value(v) == 32 &&
		(v = layer_model_get_o_value(texture, iphyee_loader_sname__d_texture)) &&
		layer_model_get_d(v, &file, &offset, &size) &&
		file && (m = layer_file_mmap(file, offset, size, 0, layer_file_io_read)))
	{
		p = layer_file_mdata(file, m, &n);
		n /= sizeof(float) * 2;
		for (i = 0; i < n; ++i)
		{
			mlog_printf(ml, "vt %f %f\n", p[0], p[1]);
			p += 2;
		}
		layer_file_munmap(file, m);
	}
}
static void iphyee_loader_build_obj_f(const layer_model_item_s *restrict fragment, mlog_s *restrict ml)
{
	const layer_model_item_s *restrict v;
	const layer_model_item_s *restrict u;
	layer_file_s *restrict fm;
	layer_file_s *restrict fi;
	layer_file_memory_t mfm;
	layer_file_memory_t mfi;
	uint64_t offset, size;
	const uint32_t *restrict pfm;
	const uint32_t *restrict pfi;
	uintptr_t i, n, m, j, k;
	if (fragment && (v = layer_model_get_o_value(fragment, iphyee_loader_sname__e_ubits)) &&
		layer_model_get_e_value(v) == 32 &&
		(v = layer_model_get_o_value(fragment, iphyee_loader_sname__d_fragment)) &&
		layer_model_get_d(v, &fm, &offset, &size) &&
		fm && (mfm = layer_file_mmap(fm, offset, size, 0, layer_file_io_read)))
	{
		if ((u = layer_model_get_o_value(fragment, iphyee_loader_sname__x_fraginfo)) &&
			(v = layer_model_get_o_value(u, iphyee_loader_sname__e_ubits)) &&
			layer_model_get_e_value(v) == 32 &&
			(v = layer_model_get_o_value(u, iphyee_loader_sname__d_fraginfo)) &&
			layer_model_get_d(v, &fi, &offset, &size) &&
			fi && (mfi = layer_file_mmap(fi, offset, size, 0, layer_file_io_read)))
		{
			pfm = layer_file_mdata(fm, mfm, &m);
			pfi = layer_file_mdata(fi, mfi, &n);
			m /= sizeof(uint32_t) * 3;
			n /= sizeof(uint32_t) * 2;
			for (i = 0; i < n; ++i)
			{
				if ((k = (uintptr_t) pfi[0] + pfi[1]) <= m)
				{
					mlog_printf(ml, "f");
					for (j = (uintptr_t) pfi[0]; j < k; ++j)
						mlog_printf(ml, " %u/%u/%u", pfm[j * 3], pfm[j * 3 + 1], pfm[j * 3 + 2]);
					mlog_printf(ml, "\n");
				}
				pfi += 2;
			}
			layer_file_munmap(fi, mfi);
		}
		layer_file_munmap(fm, mfm);
	}
}
static void iphyee_loader_build_obj_mesh(const layer_model_item_s *restrict mesh, const char *restrict name, mlog_s *restrict ml)
{
	if (mesh)
	{
		mlog_printf(ml, "o %s\n", name);
		iphyee_loader_build_obj_v(layer_model_get_o_value(mesh, iphyee_loader_sname__o_vertex), ml);
		iphyee_loader_build_obj_vn(layer_model_get_o_value(mesh, iphyee_loader_sname__o_normal), ml);
		iphyee_loader_build_obj_vt(layer_model_get_o_value(mesh, iphyee_loader_sname__o_texture), ml);
		iphyee_loader_build_obj_f(layer_model_get_o_value(mesh, iphyee_loader_sname__o_fragment), ml);
	}
}

static int iphyee_loader_build_obj_mlog_report(const char *restrict msg, uintptr_t length, layer_file_s *restrict pri)
{
	if (length) layer_file_write(pri, msg, 1, length);
	return 1;
}

layer_file_s* iphyee_loader_build_obj_to_file(const layer_model_item_s *restrict model)
{
	const vattr_vlist_t *restrict vl;
	const vattr_s *restrict vattr;
	layer_file_s *restrict file;
	mlog_s *restrict ml;
	if (model && (file = layer_file_alloc_mbuf(NULL, 0)))
	{
		if ((ml = mlog_alloc(0)))
		{
			mlog_set_report(ml, (mlog_report_f) iphyee_loader_build_obj_mlog_report, file);
			if ((model = layer_model_get_o_value(model, iphyee_loader_sname__o_meshes)) &&
				(vattr = layer_model_get_o_vattr(model)))
			{
				for (vl = vattr->vattr; vl; vl = vl->vattr_next)
					iphyee_loader_build_obj_mesh(layer_model_get_o_value(model, vl->vslot->key), vl->vslot->key, ml);
			}
			refer_free(ml);
		}
		return file;
	}
	return NULL;
}

const char* iphyee_loader_build_obj_to_path(const layer_model_item_s *restrict model, const char *restrict path)
{
	layer_file_s *restrict file;
	fsys_file_s *restrict fp;
	layer_file_memory_t m;
	const char *result;
	const void *data;
	uintptr_t size;
	uintptr_t n;
	result = NULL;
	if (path && (file = iphyee_loader_build_obj_to_file(model)))
	{
		if ((m = layer_file_mmap(file, 0, layer_file_size(file), 0, layer_file_io_read)))
		{
			data = layer_file_mdata(file, m, &size);
			if ((fp = fsys_file_alloc(path, fsys_file_flag_write |
				fsys_file_flag_create | fsys_file_flag_truncate)))
			{
				if (fsys_file_write(fp, data, size, &n) && n == size)
					result = path;
				refer_free(fp);
			}
			layer_file_munmap(file, m);
		}
		refer_free(file);
	}
	return result;
}
