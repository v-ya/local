#ifndef _gvcx_file_h_
#define _gvcx_file_h_

#include <refer.h>

typedef struct gvcx_file_s gvcx_file_s;
typedef const struct gvcx_file_memory_s *gvcx_file_memory_t;
typedef enum gvcx_file_io_t gvcx_file_io_t;
enum gvcx_file_io_t {
	gvcx_file_io_read = 1,
	gvcx_file_io_write = 2,
	gvcx_file_io_rw = 3,
};

gvcx_file_s* gvcx_file_alloc_mmap(const char *restrict path, gvcx_file_io_t io);

uint64_t gvcx_file_size(gvcx_file_s *restrict f);
uintptr_t gvcx_file_read(gvcx_file_s *restrict f, void *restrict data, uintptr_t nblock, uintptr_t bsize);
uintptr_t gvcx_file_write(gvcx_file_s *restrict f, const void *restrict data, uintptr_t nblock, uintptr_t bsize);
uint64_t gvcx_file_getpos(gvcx_file_s *restrict f);
uint64_t gvcx_file_setpos(gvcx_file_s *restrict f, uint64_t pos);
gvcx_file_memory_t gvcx_file_mmap(gvcx_file_s *restrict f, uint64_t offset, uintptr_t size, uintptr_t align, gvcx_file_io_t io);
gvcx_file_s* gvcx_file_munmap(gvcx_file_s *restrict f, gvcx_file_memory_t m);
gvcx_file_s* gvcx_file_msync(gvcx_file_s *restrict f, gvcx_file_memory_t m);

#endif
