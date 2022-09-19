#include "io.h"
#include <exbuffer.h>
#include <fsys.h>
#include <memory.h>

// memory

struct media_io_memory_s {
	struct media_io_s io;
	uintptr_t offset;
	exbuffer_t buffer;
};

static void media_io_free__memory(struct media_io_memory_s *restrict io)
{
	exbuffer_uini(&io->buffer);
}
static uint64_t media_io_size__memory(struct media_io_memory_s *restrict io)
{
	return io->buffer.used;
}
static uint64_t media_io_offset__memory(struct media_io_memory_s *restrict io, const uint64_t *restrict offset)
{
	if (offset)
	{
		if ((io->offset = *offset) > io->buffer.used)
			io->offset = io->buffer.used;
	}
	return io->offset;
}
static uintptr_t media_io_read__memory(struct media_io_memory_s *restrict io, void *data, uintptr_t size)
{
	if (io->offset + size > io->buffer.used)
		size = io->buffer.used - io->offset;
	if (size)
		memcpy(data, io->buffer.data + io->offset, size);
	io->offset += size;
	return size;
}
static uintptr_t media_io_write__memory(struct media_io_memory_s *restrict io, const void *data, uintptr_t size)
{
	uintptr_t tail;
	tail = io->offset + size;
	if (tail > io->buffer.size)
		exbuffer_need(&io->buffer, tail);
	if (tail <= io->buffer.size)
		io->buffer.used = tail;
	else io->buffer.used = io->buffer.size;
	if (tail > io->buffer.used)
		size = io->buffer.used - io->offset;
	if (size)
		memcpy(io->buffer.data + io->offset, data, size);
	io->offset += size;
	return size;
}
static void* media_io_map__memory(struct media_io_memory_s *restrict io, uintptr_t *restrict rsize)
{
	if (rsize) *rsize = io->buffer.used;
	return io->buffer.data;
}

// fsys

struct media_io_fsys_s {
	struct media_io_s io;
	fsys_file_s *fp;
};

// interface

#define d_set_func(_r, _name)  \
	(_r)->io.size = (media_io_size_f) media_io_size__##_name;\
	(_r)->io.offset = (media_io_offset_f) media_io_offset__##_name;\
	(_r)->io.read = (media_io_read_f) media_io_read__##_name;\
	(_r)->io.write = (media_io_write_f) media_io_write__##_name;\
	(_r)->io.map = (media_io_map_f) media_io_map__##_name;\
	refer_set_free(_r, (refer_free_f) media_io_free__##_name)

struct media_io_s* media_io_create_memory(const void *restrict pre_data, uintptr_t pre_size)
{
	struct media_io_memory_s *restrict r;
	if ((r = (struct media_io_memory_s *) refer_alloz(sizeof(struct media_io_memory_s))))
	{
		d_set_func(r, memory);
		if (exbuffer_init(&r->buffer, pre_size))
		{
			if (pre_data)
				memcpy(r->buffer.data, pre_data, r->buffer.used = pre_size);
			return &r->io;
		}
		refer_free(r);
	}
	return NULL;
}

#undef d_set_func
