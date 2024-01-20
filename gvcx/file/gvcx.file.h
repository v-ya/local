#ifndef _gvcx_file_pri_h_
#define _gvcx_file_pri_h_

#include "../gvcx.file.h"
#include <rbtree.h>

typedef struct gvcx_file_memory_s gvcx_file_memory_s;
typedef uint64_t (*gvcx_file_size_f)(gvcx_file_s *restrict f);
typedef uintptr_t (*gvcx_file_read_f)(gvcx_file_s *restrict f, void *restrict data, uintptr_t size);
typedef uintptr_t (*gvcx_file_write_f)(gvcx_file_s *restrict f, const void *restrict data, uintptr_t size);
typedef uint64_t (*gvcx_file_getpos_f)(gvcx_file_s *restrict f);
typedef uint64_t (*gvcx_file_setpos_f)(gvcx_file_s *restrict f, uint64_t pos);
typedef gvcx_file_memory_s* (*gvcx_file_mmap_f)(gvcx_file_s *restrict f, uint64_t offset, uintptr_t size, uintptr_t align, gvcx_file_io_t io);
typedef gvcx_file_s* (*gvcx_file_msync_f)(gvcx_file_s *restrict f, const gvcx_file_memory_s *restrict m);

struct gvcx_file_memory_s {
	void *data;
	uintptr_t size;
	uint64_t offset;
	gvcx_file_s *msync;
};

struct gvcx_file_s {
	rbtree_t *memory;
	gvcx_file_size_f size;
	gvcx_file_read_f read;
	gvcx_file_write_f write;
	gvcx_file_getpos_f getpos;
	gvcx_file_setpos_f setpos;
	gvcx_file_mmap_f mmap;
	gvcx_file_msync_f msync;
};

// memory

void gvcx_file_memory_sync_func(gvcx_file_memory_s *restrict r);
void gvcx_file_memory_free_func(gvcx_file_memory_s *restrict r);
gvcx_file_memory_s* gvcx_file_memory_alloc_empty(uintptr_t msize);
gvcx_file_memory_s* gvcx_file_memory_initial(gvcx_file_memory_s *restrict m, uintptr_t size, uintptr_t align);
gvcx_file_memory_s* gvcx_file_memory_alloc(uintptr_t size, uintptr_t align);
const void* gvcx_file_memory_align(const void *ptr, uintptr_t align);

// file

void gvcx_file_free_func(gvcx_file_s *restrict r);
gvcx_file_s* gvcx_file_alloc_empty(uintptr_t fsize);

#endif
