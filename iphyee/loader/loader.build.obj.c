#include "loader.model.view.h"
#include <exbuffer.h>
#include <mlog.h>
#include <memory.h>

typedef struct iphyee_loader_build_obj_t iphyee_loader_build_obj_t;
typedef struct iphyee_loader_build_obj_test_t iphyee_loader_build_obj_test_t;

typedef void (*iphyee_loader_build_obj_index_f)(const iphyee_loader_build_obj_test_t *restrict test);
typedef void (*iphyee_loader_build_obj_dump_f)(const iphyee_loader_build_obj_test_t *restrict test, uintptr_t index);
typedef void (*iphyee_loader_build_obj_fragment_f)(iphyee_loader_build_obj_t *restrict c, const iphyee_loader_build_obj_test_t *restrict test);

struct iphyee_loader_build_obj_t {
	exbuffer_t buffer;
	iphyee_loader_model_view_s *view;
	uintptr_t *vtn_index[3];
	uintptr_t vtn_count[3];
	uintptr_t vtn_ndump[3];
	uintptr_t fcount;
};

struct iphyee_loader_build_obj_test_t {
	mlog_s *restrict mlog;
	iphyee_loader_build_obj_index_f index;
	iphyee_loader_build_obj_dump_f dump;
	iphyee_loader_build_obj_fragment_f fragment;
	const char *restrict value_name;
	const void *restrict surface_data;
	const void *restrict fragment_data;
	const void *restrict value_data;
	uintptr_t *restrict value_index;
	uintptr_t *restrict value_ndump;
	uintptr_t surface_count;
	uintptr_t fragment_count;
	uintptr_t value_count;
	uintptr_t length_of_fragment;
	uintptr_t index_at_fragment;
	uintptr_t number_of_value;
};

#define d_dump(_name, _type)  \
	static void _name(const iphyee_loader_build_obj_test_t *restrict test, uintptr_t index)\
	{\
		const _type *p;\
		uintptr_t n;\
		p = (const _type *) test->value_data;\
		p += index * (n = test->number_of_value);\
		mlog_printf(test->mlog, test->value_name);\
		for (index = 0; index < n; ++index)\
			mlog_printf(test->mlog, " %f", p[index]);\
		mlog_printf(test->mlog, "\n");\
	}
#define d_index(_name, _type)  \
	static void _name(const iphyee_loader_build_obj_test_t *restrict test)\
	{\
		const struct { _type offset; _type number; } *restrict surface_data;\
		const _type *restrict fragment_data;\
		uintptr_t *restrict value_index;\
		uintptr_t *restrict value_ndump;\
		uintptr_t i, n, m, vn, p, t;\
		uintptr_t ii, nn, pp, idx;\
		if ((p = test->index_at_fragment) < (t = test->length_of_fragment))\
		{\
			surface_data = test->surface_data;\
			fragment_data = (const _type *) test->fragment_data;\
			value_index = test->value_index;\
			value_ndump = test->value_ndump;\
			n = test->surface_count;\
			m = test->fragment_count;\
			vn = test->value_count;\
			for (i = 0; i < n; ++i)\
			{\
				ii = (uintptr_t) surface_data[i].offset;\
				nn = (uintptr_t) surface_data[i].number;\
				if ((nn += ii) <= m)\
				{\
					while (ii < nn)\
					{\
						idx = fragment_data[pp = ii * t + p];\
						if (idx < vn && value_index[idx] >= *value_ndump)\
						{\
							test->dump(test, idx);\
							value_index[idx] = *value_ndump;\
							*value_ndump += 1;\
						}\
						ii += 1;\
					}\
				}\
			}\
		}\
	}
#define d_fragment(_name, _type)  \
	static void _name(iphyee_loader_build_obj_t *restrict c, const iphyee_loader_build_obj_test_t *restrict test)\
	{\
		const struct { _type offset; _type number; } *restrict surface_data;\
		const _type *restrict fragment_data;\
		uintptr_t i, n, m, t, it, in;\
		uintptr_t ii, nn, pp, idx, nid;\
		uintptr_t *restrict *restrict vtn_index;\
		const uintptr_t *restrict vtn_count;\
		const uintptr_t *restrict vtn_ndump;\
		surface_data = test->surface_data;\
		fragment_data = (const _type *) test->fragment_data;\
		n = test->surface_count;\
		m = test->fragment_count;\
		t = test->length_of_fragment;\
		in = (t <= 3)?t:3;\
		vtn_index = c->vtn_index;\
		vtn_count = c->vtn_count;\
		vtn_ndump = c->vtn_ndump;\
		for (i = 0; i < n; ++i)\
		{\
			ii = (uintptr_t) surface_data[i].offset;\
			nn = (uintptr_t) surface_data[i].number;\
			if ((nn += ii) <= m)\
			{\
				mlog_printf(test->mlog, "f");\
				while (ii < nn)\
				{\
					pp = ii * t;\
					for (it = 0; it < in; ++it)\
					{\
						mlog_printf(test->mlog, it?"/":" ");\
						idx = (uintptr_t) fragment_data[pp + it];\
						if (idx < vtn_count[it] && (nid = vtn_index[it][idx]) < vtn_ndump[it])\
							mlog_printf(test->mlog, "%zu", nid + 1);\
					}\
					ii += 1;\
				}\
				mlog_printf(test->mlog, "\n");\
			}\
		}\
	}\

d_dump(iphyee_loader_build_obj_test_dump32, float)
d_dump(iphyee_loader_build_obj_test_dump64, double)

d_index(iphyee_loader_build_obj_test_index16, uint16_t)
d_index(iphyee_loader_build_obj_test_index32, uint32_t)
d_index(iphyee_loader_build_obj_test_index64, uint64_t)

d_fragment(iphyee_loader_build_obj_fragment16, uint16_t)
d_fragment(iphyee_loader_build_obj_fragment32, uint32_t)
d_fragment(iphyee_loader_build_obj_fragment64, uint64_t)

#undef d_dump
#undef d_index
#undef d_fragment

static void iphyee_loader_build_obj_surface(iphyee_loader_build_obj_t *restrict c, iphyee_loader_build_obj_test_t *restrict t, const char *restrict name, iphyee_loader_model_view_surface_s *restrict surface)
{
	mlog_printf(t->mlog, "o %s\n", name);
	t->surface_data = surface->view.data;
	t->surface_count = surface->view.size / (c->view->bits_value / 8 * 2);
	// test v
	t->value_name = "v";
	t->value_data = c->view->vertex.data;
	t->value_index = c->vtn_index[0];
	t->value_ndump = &c->vtn_ndump[0];
	t->value_count = c->vtn_count[0];
	t->index_at_fragment = c->view->vertex_at_fragment;
	t->number_of_value = 3;
	t->index(t);
	// test t
	t->value_name = "vt";
	t->value_data = c->view->texture.data;
	t->value_index = c->vtn_index[1];
	t->value_ndump = &c->vtn_ndump[1];
	t->value_count = c->vtn_count[1];
	t->index_at_fragment = c->view->texture_at_fragment;
	t->number_of_value = 2;
	t->index(t);
	// test n
	t->value_name = "vn";
	t->value_data = c->view->normal.data;
	t->value_index = c->vtn_index[2];
	t->value_ndump = &c->vtn_ndump[2];
	t->value_count = c->vtn_count[2];
	t->index_at_fragment = c->view->normal_at_fragment;
	t->number_of_value = 3;
	t->index(t);
	// fragment
	t->value_name = NULL;
	t->value_data = NULL;
	t->value_index = NULL;
	t->value_ndump = NULL;
	t->value_count = 0;
	t->index_at_fragment = 0;
	t->number_of_value = 0;
	t->fragment(c, t);
}

static void iphyee_loader_build_obj_test_initial(iphyee_loader_build_obj_test_t *restrict t, iphyee_loader_build_obj_t *restrict c, mlog_s *restrict mlog)
{
	t->mlog = mlog;
	switch (c->view->bits_value)
	{
		case 32: t->dump = iphyee_loader_build_obj_test_dump32; break;
		case 64: t->dump = iphyee_loader_build_obj_test_dump64; break;
		default: t->dump = NULL; break;
	}
	switch (c->view->bits_index)
	{
		case 16:
			t->index = iphyee_loader_build_obj_test_index16;
			t->fragment = iphyee_loader_build_obj_fragment16;
			break;
		case 32:
			t->index = iphyee_loader_build_obj_test_index32;
			t->fragment = iphyee_loader_build_obj_fragment32;
			break;
		case 64:
			t->index = iphyee_loader_build_obj_test_index64;
			t->fragment = iphyee_loader_build_obj_fragment64;
			break;
		default:
			t->index = NULL;
			t->fragment = NULL;
			break;
	}
	t->value_name = NULL;
	t->surface_data = NULL;
	t->fragment_data = c->view->fragment.data;
	t->value_data = NULL;
	t->value_index = NULL;
	t->value_ndump = NULL;
	t->surface_count = 0;
	t->fragment_count = c->fcount;
	t->value_count = 0;
	t->length_of_fragment = c->view->length_of_fragment;
	t->index_at_fragment = 0;
	t->number_of_value = 0;
}

static mlog_s* iphyee_loader_build_obj_func(const layer_model_item_s *restrict model)
{
	iphyee_loader_build_obj_t c;
	iphyee_loader_build_obj_test_t test;
	vattr_vlist_t *restrict vl;
	mlog_s *restrict mlog;
	mlog_s *result;
	uintptr_t n;
	result = NULL;
	if ((mlog = mlog_alloc(0)))
	{
		if ((c.view = iphyee_loader_model_view_alloc(model)))
		{
			c.vtn_count[0] = c.view->vertex.size / (c.view->bits_value / 8 * 3);
			c.vtn_count[1] = c.view->texture.size / (c.view->bits_value / 8 * 2);
			c.vtn_count[2] = c.view->normal.size / (c.view->bits_value / 8 * 3);
			c.fcount = c.view->fragment.size / (c.view->bits_value / 8 * c.view->length_of_fragment);
			if (exbuffer_init(&c.buffer, n = (c.vtn_count[0] + c.vtn_count[1] + c.vtn_count[2]) * sizeof(uintptr_t)))
			{
				memset(c.buffer.data, 0xff, n);
				c.vtn_index[0] = (uintptr_t *) c.buffer.data;
				c.vtn_index[1] = c.vtn_index[0] + c.vtn_count[0];
				c.vtn_index[2] = c.vtn_index[1] + c.vtn_count[1];
				c.vtn_ndump[0] = 0;
				c.vtn_ndump[1] = 0;
				c.vtn_ndump[2] = 0;
				iphyee_loader_build_obj_test_initial(&test, &c, mlog);
				for (vl = c.view->surface->vattr; vl; vl = vl->vattr_next)
					iphyee_loader_build_obj_surface(&c, &test, vl->vslot->key, (iphyee_loader_model_view_surface_s *) vl->value);
				result = (mlog_s *) refer_save(mlog);
				exbuffer_uini(&c.buffer);
			}
			refer_free(c.view);
		}
		refer_free(mlog);
	}
	return result;
}

layer_file_s* iphyee_loader_build_obj_to_file(const layer_model_item_s *restrict model, layer_file_s *restrict file)
{
	mlog_s *restrict mlog;
	if (model && file && (mlog = iphyee_loader_build_obj_func(model)))
	{
		file = iphyee_loader_data_memory2file(file, mlog->mlog, mlog->length);
		refer_free(mlog);
		return file;
	}
	return NULL;
}

const char* iphyee_loader_build_obj_to_path(const layer_model_item_s *restrict model, const char *restrict path)
{
	mlog_s *restrict mlog;
	if (model && path && (mlog = iphyee_loader_build_obj_func(model)))
	{
		path = iphyee_loader_data_memory2path(path, mlog->mlog, mlog->length);
		refer_free(mlog);
		return path;
	}
	return NULL;
}
