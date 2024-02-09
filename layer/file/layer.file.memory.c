#include "layer.file.h"
#include <exbuffer.h>
#include <memory.h>

struct layer_file_mbuf_s {
	layer_file_s file;
	exbuffer_t mbuf;
	uintptr_t pos;
	uintptr_t rpos;
};

static uint64_t layer_file_size_mbuf(struct layer_file_mbuf_s *restrict f)
{
	return (uint64_t) f->mbuf.used;
}
static uintptr_t layer_file_read_mbuf(struct layer_file_mbuf_s *restrict f, void *restrict data, uintptr_t size)
{
	if (size > f->rpos) size = f->rpos;
	if (size)
	{
		memcpy(data, f->mbuf.data + f->pos, size);
		f->pos += size;
		f->rpos -= size;
	}
	return size;
}
static uintptr_t layer_file_write_mbuf(struct layer_file_mbuf_s *restrict f, const void *restrict data, uintptr_t size)
{
	if (size > f->rpos)
	{
		if (!f->file.memory && exbuffer_need(&f->mbuf, f->pos + size))
			f->mbuf.used = f->pos + (f->rpos = size);
		else size = f->rpos;
	}
	if (size)
	{
		memcpy(f->mbuf.data + f->pos, data, size);
		f->pos += size;
		f->rpos -= size;
	}
	return size;
}
static uint64_t layer_file_getpos_mbuf(struct layer_file_mbuf_s *restrict f)
{
	return (uint64_t) f->pos;
}
static uint64_t layer_file_setpos_mbuf(struct layer_file_mbuf_s *restrict f, uint64_t pos)
{
	if (pos > (uint64_t) f->mbuf.used)
	{
		if (!f->file.memory && (uint64_t) (uintptr_t) pos == pos && exbuffer_need(&f->mbuf, (uintptr_t) pos))
			f->mbuf.used = (uintptr_t) pos;
		else pos = (uint64_t) f->mbuf.used;
	}
	f->rpos = (f->mbuf.used - (f->pos = (uintptr_t) pos));
	return pos;
}
static layer_file_memory_s* layer_file_mmap_mbuf(struct layer_file_mbuf_s *restrict f, uint64_t offset, uintptr_t size, uintptr_t align, layer_file_io_t io)
{
	layer_file_memory_s *restrict m;
	m = NULL;
	if (size && offset < (uint64_t) f->mbuf.used && size <= ((uint64_t) f->mbuf.used - offset) &&
		(!(io & layer_file_io_write) || f->file.msync))
	{
		if (layer_file_memory_align(f->mbuf.data + offset, align))
		{
			if ((m = layer_file_memory_alloc(0, 0)))
			{
				m->data = f->mbuf.data + offset;
				m->size = size;
			}
		}
		else
		{
			if ((m = layer_file_memory_alloc(size, align)))
				memcpy(m->data, f->mbuf.data + offset, size);
		}
		if (m)
		{
			m->offset = offset;
			if ((io & layer_file_io_write) && f->file.msync)
				m->msync = &f->file;
		}
	}
	return m;
}
static layer_file_s* layer_file_msync_mbuf(struct layer_file_mbuf_s *restrict f, const layer_file_memory_s *restrict m)
{
	if ((uintptr_t) (f->mbuf.data + m->offset) != (uintptr_t) m->data)
		memcpy(f->mbuf.data + m->offset, m->data, m->size);
	return &f->file;
}

static void layer_file_mbuf_free_func(struct layer_file_mbuf_s *restrict r)
{
	exbuffer_uini(&r->mbuf);
	layer_file_free_func(&r->file);
}
layer_file_s* layer_file_alloc_mbuf(const void *data, uintptr_t size)
{
	struct layer_file_mbuf_s *restrict r;
	if ((r = (struct layer_file_mbuf_s *) layer_file_alloc_empty(sizeof(struct layer_file_mbuf_s))))
	{
		if (exbuffer_init(&r->mbuf, size))
		{
			refer_set_free(r, (refer_free_f) layer_file_mbuf_free_func);
			r->mbuf.used = size;
			r->pos = 0;
			r->rpos = size;
			r->file.size = (layer_file_size_f) layer_file_size_mbuf;
			r->file.read = (layer_file_read_f) layer_file_read_mbuf;
			r->file.write = (layer_file_write_f) layer_file_write_mbuf;
			r->file.getpos = (layer_file_getpos_f) layer_file_getpos_mbuf;
			r->file.setpos = (layer_file_setpos_f) layer_file_setpos_mbuf;
			r->file.mmap = (layer_file_mmap_f) layer_file_mmap_mbuf;
			r->file.msync = (layer_file_msync_f) layer_file_msync_mbuf;
			if (data && size)
				memcpy(r->mbuf.data, data, size);
			return &r->file;
		}
		refer_free(r);
	}
	return NULL;
}
