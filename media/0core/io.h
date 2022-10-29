#ifndef _media_core_io_h_
#define _media_core_io_h_

#include <refer.h>
#include <fsys.h>

struct media_io_s;

typedef uint64_t (*media_io_size_f)(struct media_io_s *restrict io);
typedef uint64_t (*media_io_offset_f)(struct media_io_s *restrict io, const uint64_t *restrict offset);
typedef uintptr_t (*media_io_read_f)(struct media_io_s *restrict io, void *data, uintptr_t size);
typedef uintptr_t (*media_io_write_f)(struct media_io_s *restrict io, const void *data, uintptr_t size);
typedef void* (*media_io_map_f)(struct media_io_s *restrict io, uintptr_t *restrict rsize);
typedef struct media_io_s* (*media_io_sync_f)(struct media_io_s *restrict io);

struct media_io_s {
	media_io_size_f size;
	media_io_offset_f offset;
	media_io_read_f read;
	media_io_write_f write;
	media_io_map_f map;
	media_io_sync_f sync;
};

static inline uint64_t media_io_size(struct media_io_s *restrict io)
{
	return io->size(io);
}
static inline uint64_t media_io_offset(struct media_io_s *restrict io, const uint64_t *restrict offset)
{
	return io->offset(io, offset);
}
static inline uintptr_t media_io_read(struct media_io_s *restrict io, void *data, uintptr_t size)
{
	return io->read(io, data, size);
}
static inline uintptr_t media_io_write(struct media_io_s *restrict io, const void *data, uintptr_t size)
{
	return io->write(io, data, size);
}
static inline void* media_io_map(struct media_io_s *restrict io, uintptr_t *restrict rsize)
{
	return io->map(io, rsize);
}
static inline struct media_io_s* media_io_sync(struct media_io_s *restrict io)
{
	return io->sync(io);
}

struct media_io_s* media_io_create_memory(const void *restrict pre_data, uintptr_t pre_size);
struct media_io_s* media_io_create_memory_const(const void *restrict data, uintptr_t size, refer_t data_source);
struct media_io_s* media_io_create_fsys(const char *restrict path, fsys_file_flag_t flag, uintptr_t cache_number, uintptr_t cache_size);

struct media_io_s* media_io_inner_padding_align(struct media_io_s *restrict io, uint64_t offset, uintptr_t align);

#endif
