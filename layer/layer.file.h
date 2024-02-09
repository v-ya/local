#ifndef _layer_file_h_
#define _layer_file_h_

#include <refer.h>

typedef struct layer_file_s layer_file_s;
typedef const struct layer_file_memory_s *layer_file_memory_t;
typedef enum layer_file_io_t layer_file_io_t;
enum layer_file_io_t {
	layer_file_io_read = 1,
	layer_file_io_write = 2,
	layer_file_io_rw = 3,
};

layer_file_s* layer_file_alloc_mmap(const char *restrict path, layer_file_io_t io);
layer_file_s* layer_file_alloc_mbuf(const void *data, uintptr_t size);

uint64_t layer_file_size(layer_file_s *restrict f);
uintptr_t layer_file_read(layer_file_s *restrict f, void *restrict data, uintptr_t nblock, uintptr_t bsize);
uintptr_t layer_file_write(layer_file_s *restrict f, const void *restrict data, uintptr_t nblock, uintptr_t bsize);
uint64_t layer_file_getpos(layer_file_s *restrict f);
uint64_t layer_file_setpos(layer_file_s *restrict f, uint64_t pos);
layer_file_memory_t layer_file_mmap(layer_file_s *restrict f, uint64_t offset, uintptr_t size, uintptr_t align, layer_file_io_t io);
layer_file_s* layer_file_munmap(layer_file_s *restrict f, layer_file_memory_t m);
void* layer_file_mdata(layer_file_s *restrict f, layer_file_memory_t m, uintptr_t *restrict size);
layer_file_s* layer_file_msync(layer_file_s *restrict f, layer_file_memory_t m);

#endif
