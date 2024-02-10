#include "loader.model.h"
#include <mlog.h>
#include <memory.h>

static mlog_s* iphyee_loader_build_obj_func(const layer_model_item_s *restrict model)
{
	mlog_s *restrict mlog;
	// mlog_s *result;
	// result = NULL;
	if ((mlog = mlog_alloc(0)))
	{
		;
		refer_free(mlog);
	}
	return NULL;
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

/*
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

*/
