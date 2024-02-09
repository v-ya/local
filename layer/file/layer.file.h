#ifndef _layer_file_pri_h_
#define _layer_file_pri_h_

#include "../layer.file.h"
#include <rbtree.h>

typedef struct layer_file_memory_s layer_file_memory_s;
typedef uint64_t (*layer_file_size_f)(layer_file_s *restrict f);
typedef uintptr_t (*layer_file_read_f)(layer_file_s *restrict f, void *restrict data, uintptr_t size);
typedef uintptr_t (*layer_file_write_f)(layer_file_s *restrict f, const void *restrict data, uintptr_t size);
typedef uint64_t (*layer_file_getpos_f)(layer_file_s *restrict f);
typedef uint64_t (*layer_file_setpos_f)(layer_file_s *restrict f, uint64_t pos);
typedef layer_file_memory_s* (*layer_file_mmap_f)(layer_file_s *restrict f, uint64_t offset, uintptr_t size, uintptr_t align, layer_file_io_t io);
typedef layer_file_s* (*layer_file_msync_f)(layer_file_s *restrict f, const layer_file_memory_s *restrict m);

struct layer_file_memory_s {
	void *data;
	uintptr_t size;
	uint64_t offset;
	layer_file_s *msync;
};

struct layer_file_s {
	rbtree_t *memory;
	layer_file_size_f size;
	layer_file_read_f read;
	layer_file_write_f write;
	layer_file_getpos_f getpos;
	layer_file_setpos_f setpos;
	layer_file_mmap_f mmap;
	layer_file_msync_f msync;
};

// memory

void layer_file_memory_sync_func(layer_file_memory_s *restrict r);
void layer_file_memory_free_func(layer_file_memory_s *restrict r);
layer_file_memory_s* layer_file_memory_alloc_empty(uintptr_t msize);
layer_file_memory_s* layer_file_memory_initial(layer_file_memory_s *restrict m, uintptr_t size, uintptr_t align);
layer_file_memory_s* layer_file_memory_alloc(uintptr_t size, uintptr_t align);
const void* layer_file_memory_align(const void *ptr, uintptr_t align);

// file

void layer_file_free_func(layer_file_s *restrict r);
layer_file_s* layer_file_alloc_empty(uintptr_t fsize);

#endif
