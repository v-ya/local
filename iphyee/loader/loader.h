#ifndef _iphyee_loader_pri_h_
#define _iphyee_loader_pri_h_

#include "../iphyee.loader.h"
#include <layer/layer.model.custom.h>

struct iphyee_loader_s {
	layer_model_s *m;
};

typedef void* (*iphyee_loader_data_file_f)(void *restrict pri, layer_file_s *restrict file);
typedef void* (*iphyee_loader_data_memory_f)(void *restrict pri, void *restrict data, uintptr_t size);

void* iphyee_loader_data_path2file(const char *restrict path, layer_file_io_t io, iphyee_loader_data_file_f func, const void *restrict pri);
void* iphyee_loader_data_file2memory(layer_file_s *restrict file, uint64_t offset, uintptr_t size, layer_file_io_t io, iphyee_loader_data_memory_f func, const void *restrict pri);
void* iphyee_loader_data_path2memory(const char *restrict path, layer_file_io_t io, iphyee_loader_data_memory_f func, const void *restrict pri);

layer_file_s* iphyee_loader_data_memory2file(layer_file_s *restrict file, const void *restrict data, uintptr_t size);
const char* iphyee_loader_data_memory2path(const char *restrict path, const void *restrict data, uintptr_t size);
const char* iphyee_loader_data_file2path(const char *restrict path, layer_file_s *restrict file);

#endif
