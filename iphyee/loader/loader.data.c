#include "loader.h"
#include <fsys.h>

void* iphyee_loader_data_path2file(const char *restrict path, layer_file_io_t io, iphyee_loader_data_file_f func, const void *restrict pri)
{
	layer_file_s *restrict file;
	void *result;
	result = NULL;
	if (path && func && (file = layer_file_alloc_mmap(path, io)))
	{
		result = func((void *) pri, file);
		refer_free(file);
	}
	return result;
}

void* iphyee_loader_data_file2memory(layer_file_s *restrict file, uint64_t offset, uintptr_t size, layer_file_io_t io, iphyee_loader_data_memory_f func, const void *restrict pri)
{
	layer_file_memory_t m;
	void *result;
	result = NULL;
	if (file && size && func && (m = layer_file_mmap(file, offset, size, 0, io)))
	{
		result = layer_file_mdata(file, m, &size);
		result = func((void *) pri, result, size);
		layer_file_munmap(file, m);
	}
	return result;
}

void* iphyee_loader_data_path2memory(const char *restrict path, layer_file_io_t io, iphyee_loader_data_memory_f func, const void *restrict pri)
{
	layer_file_s *restrict file;
	layer_file_memory_t m;
	uintptr_t size;
	void *result;
	result = NULL;
	if (path && func && (file = layer_file_alloc_mmap(path, io)))
	{
		if ((m = layer_file_mmap(file, 0, (uintptr_t) layer_file_size(file), 0, io)))
		{
			result = layer_file_mdata(file, m, &size);
			result = func((void *) pri, result, size);
			layer_file_munmap(file, m);
		}
		refer_free(file);
	}
	return result;
}

layer_file_s* iphyee_loader_data_memory2file(layer_file_s *restrict file, const void *restrict data, uintptr_t size)
{
	if (file && layer_file_write(file, data, 1, size))
		return file;
	return NULL;
}

const char* iphyee_loader_data_memory2path(const char *restrict path, const void *restrict data, uintptr_t size)
{
	fsys_file_s *restrict fp;
	const char *result;
	uintptr_t n;
	result = NULL;
	if (path && (fp = fsys_file_alloc(path, fsys_file_flag_write | fsys_file_flag_create | fsys_file_flag_truncate)))
	{
		if (fsys_file_write(fp, data, size, &n) && n == size)
			result = path;
		refer_free(fp);
	}
	return result;
}

const char* iphyee_loader_data_file2path(const char *restrict path, layer_file_s *restrict file)
{
	fsys_file_s *restrict fp;
	layer_file_memory_t m;
	const char *result;
	const void *data;
	uintptr_t size;
	uintptr_t n;
	result = NULL;
	if (path && file && (fp = fsys_file_alloc(path, fsys_file_flag_write | fsys_file_flag_create | fsys_file_flag_truncate)))
	{
		if ((size = (uintptr_t) layer_file_size(file)))
		{
			if ((m = layer_file_mmap(file, 0, size, 0, layer_file_io_read)))
			{
				data = layer_file_mdata(file, m, &size);
				if (fsys_file_write(fp, data, size, &n) && n == size)
					result = path;
				layer_file_munmap(file, m);
			}
		}
		else result = path;
		refer_free(fp);
	}
	return result;
}
