#include "io.h"
#include <exbuffer.h>
#include <memory.h>

// memory

struct media_io_memory_s {
	struct media_io_s io;
	uintptr_t offset;
	exbuffer_t buffer;
};

#define d_offset_deal(_m_offset, _m_size, ...)  \
	if (offset)\
	{\
		if ((io->_m_offset = *offset) > io->_m_size)\
			io->_m_offset = io->_m_size;\
		__VA_ARGS__\
	}\
	return io->_m_offset

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
	d_offset_deal(offset, buffer.used);
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
static struct media_io_s* media_io_sync__memory(struct media_io_memory_s *restrict io)
{
	return &io->io;
}

// memory const

struct media_io_memory_const_s {
	struct media_io_s io;
	const uint8_t *data;
	uintptr_t size;
	uintptr_t offset;
	refer_t data_source;
};

static void media_io_free__memory_const(struct media_io_memory_const_s *restrict io)
{
	if (io->data_source) refer_free(io->data_source);
}
static uint64_t media_io_size__memory_const(struct media_io_memory_const_s *restrict io)
{
	return io->size;
}
static uint64_t media_io_offset__memory_const(struct media_io_memory_const_s *restrict io, const uint64_t *restrict offset)
{
	d_offset_deal(offset, size);
}
static uintptr_t media_io_read__memory_const(struct media_io_memory_const_s *restrict io, void *data, uintptr_t size)
{
	if (io->offset + size > io->size)
		size = io->size - io->offset;
	if (size)
		memcpy(data, io->data + io->offset, size);
	io->offset += size;
	return size;
}
static uintptr_t media_io_write__memory_const(struct media_io_memory_const_s *restrict io, const void *data, uintptr_t size)
{
	return 0;
}
static void* media_io_map__memory_const(struct media_io_memory_const_s *restrict io, uintptr_t *restrict rsize)
{
	return NULL;
}
static struct media_io_s* media_io_sync__memory_const(struct media_io_memory_const_s *restrict io)
{
	return &io->io;
}

// fsys

struct media_io_fsys_pri_cache_s {
	uint8_t *data;
	uint64_t cache_id;
	uintptr_t size;
	uint32_t used;
	uint32_t dirty;
};

struct media_io_fsys_s {
	struct media_io_s io;
	fsys_file_s *fp;
	uint64_t size;
	uint64_t offset;
	refer_t cache_data;
	uintptr_t cache_size;
	uintptr_t cache_index;
	uintptr_t cache_number;
	struct media_io_fsys_pri_cache_s cache[];
};

static inline void media_io_fsys__recache(struct media_io_fsys_s *restrict r)
{
	r->cache_index = (r->offset / r->cache_size) % r->cache_number;
}
static struct media_io_fsys_s* media_io_fsys__clean_cache(struct media_io_fsys_s *restrict r, struct media_io_fsys_pri_cache_s *restrict c)
{
	uint64_t roffset, n;
	uintptr_t rsize;
	if (!c->used || !c->dirty)
		goto label_okay;
	if (fsys_file_seek_by_head(r->fp, n = c->cache_id * r->cache_size, &roffset) && roffset == n &&
		fsys_file_write(r->fp, c->data, c->size, &rsize) && rsize == c->size)
	{
		label_okay:
		c->dirty = 0;
		return r;
	}
	return NULL;
}
static struct media_io_fsys_s* media_io_fsys__clean_all_cache(struct media_io_fsys_s *restrict r)
{
	struct media_io_fsys_pri_cache_s *restrict c;
	uintptr_t i, n;
	c = r->cache;
	n = r->cache_number;
	for (i = 0; i < n; ++i)
	{
		if (c[i].used && c[i].dirty && !media_io_fsys__clean_cache(r, c + i))
			r = NULL;
	}
	return r;
}
static uintptr_t media_io_fsys__touch_cache(struct media_io_fsys_s *restrict r, uint8_t *restrict p, uintptr_t n)
{
	struct media_io_fsys_pri_cache_s *restrict c;
	uint64_t cache_id, cache_offset, roffset;
	uintptr_t cpos, rsize;
	c = r->cache + r->cache_index;
	cache_id = r->offset / r->cache_size;
	cache_offset = cache_id * r->cache_size;
	cpos = (uintptr_t) (r->offset - cache_offset);
	if (c->used && cache_id == c->cache_id)
	{
		label_hit:
		if (c->size > cpos)
		{
			if ((rsize = c->size - cpos) > n)
				rsize = n;
			memcpy(p, c->data + cpos, rsize);
			r->offset += rsize;
			media_io_fsys__recache(r);
			return rsize;
		}
	}
	else if (r->offset <= r->size)
	{
		media_io_fsys__clean_cache(r, c);
		c->used = 1;
		c->cache_id = cache_id;
		c->size = r->size - cache_offset;
		if (c->size > r->cache_size)
			c->size = r->cache_size;
		if (fsys_file_seek_by_head(r->fp, cache_offset, &roffset) && roffset == cache_offset &&
			fsys_file_read(r->fp, c->data, c->size, &rsize) && rsize == c->size)
			goto label_hit;
		c->used = 0;
	}
	return 0;
}
static uintptr_t media_io_fsys__fixed_cache(struct media_io_fsys_s *restrict r, const uint8_t *restrict p, uintptr_t n)
{
	struct media_io_fsys_pri_cache_s *restrict c;
	uint64_t cache_id, cache_offset, roffset;
	uintptr_t cpos, rsize;
	c = r->cache + r->cache_index;
	cache_id = r->offset / r->cache_size;
	cache_offset = cache_id * r->cache_size;
	cpos = (uintptr_t) (r->offset - cache_offset);
	if (!cpos && n >= r->cache_size)
	{
		if (c->cache_id != cache_id)
			media_io_fsys__clean_cache(r, c);
		memcpy(c->data, p, r->cache_size);
		c->cache_id = cache_id;
		c->size = rsize = r->cache_size;
		c->used = 1;
		c->dirty = 1;
		goto label_okay;
	}
	else if (c->used && cache_id == c->cache_id)
	{
		label_hit:
		if ((rsize = r->cache_size - cpos) > n)
			rsize = n;
		memcpy(c->data + cpos, p, rsize);
		if (c->size < cpos + rsize)
			c->size = cpos + rsize;
		c->dirty = 1;
		label_okay:
		if ((r->offset += rsize) > r->size)
			r->size = r->offset;
		media_io_fsys__recache(r);
		return rsize;
	}
	else
	{
		media_io_fsys__clean_cache(r, c);
		c->used = 1;
		c->cache_id = cache_id;
		c->size = r->size - cache_offset;
		if (c->size > r->cache_size)
			c->size = r->cache_size;
		if (fsys_file_seek_by_head(r->fp, cache_offset, &roffset) && roffset == cache_offset &&
			fsys_file_read(r->fp, c->data, c->size, &rsize) && rsize == c->size)
			goto label_hit;
		c->used = 0;
	}
	return 0;
}

static void media_io_free__fsys(struct media_io_fsys_s *restrict io)
{
	media_io_fsys__clean_all_cache(io);
	if (io->fp) refer_free(io->fp);
	if (io->cache_data) refer_free(io->cache_data);
}
static uint64_t media_io_size__fsys(struct media_io_fsys_s *restrict io)
{
	return io->size;
}
static uint64_t media_io_offset__fsys(struct media_io_fsys_s *restrict io, const uint64_t *restrict offset)
{
	d_offset_deal(offset, size, media_io_fsys__recache(io););
}
static uintptr_t media_io_read__fsys(struct media_io_fsys_s *restrict io, void *data, uintptr_t size)
{
	uintptr_t n, sn;
	sn = 0;
	while ((n = media_io_fsys__touch_cache(io, (uint8_t *) data, size)))
	{
		data = (uint8_t *) data + n;
		size -= n;
		sn += n;
	}
	return sn;
}
static uintptr_t media_io_write__fsys(struct media_io_fsys_s *restrict io, const void *data, uintptr_t size)
{
	uintptr_t n, sn;
	sn = 0;
	while ((n = media_io_fsys__fixed_cache(io, (const uint8_t *) data, size)))
	{
		data = (const uint8_t *) data + n;
		size -= n;
		sn += n;
	}
	return sn;
}
static void* media_io_map__fsys(struct media_io_fsys_s *restrict io, uintptr_t *restrict rsize)
{
	return NULL;
}
static struct media_io_s* media_io_sync__fsys(struct media_io_fsys_s *restrict io)
{
	return &media_io_fsys__clean_all_cache(io)->io;
}

// interface

#define d_set_func(_r, _name)  \
	(_r)->io.size = (media_io_size_f) media_io_size__##_name;\
	(_r)->io.offset = (media_io_offset_f) media_io_offset__##_name;\
	(_r)->io.read = (media_io_read_f) media_io_read__##_name;\
	(_r)->io.write = (media_io_write_f) media_io_write__##_name;\
	(_r)->io.map = (media_io_map_f) media_io_map__##_name;\
	(_r)->io.sync = (media_io_sync_f) media_io_sync__##_name;\
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

struct media_io_s* media_io_create_memory_const(const void *restrict data, uintptr_t size, refer_t data_source)
{
	struct media_io_memory_const_s *restrict r;
	if ((r = (struct media_io_memory_const_s *) refer_alloz(sizeof(struct media_io_memory_const_s))))
	{
		d_set_func(r, memory_const);
		r->data = data;
		r->size = size;
		r->data_source = refer_save(data_source);
		return &r->io;
	}
	return NULL;
}

struct media_io_s* media_io_create_fsys(const char *restrict path, fsys_file_flag_t flag, uintptr_t cache_number, uintptr_t cache_size)
{
	struct media_io_fsys_s *restrict r;
	uint8_t *cache_data;
	uintptr_t i;
	if (!cache_number) cache_number = 4;
	if (!cache_size) cache_size = 4096;
	cache_size = (cache_size + 511) >> 9 << 9;
	if (path && (r = (struct media_io_fsys_s *) refer_alloz(sizeof(struct media_io_fsys_s) + sizeof(struct media_io_fsys_pri_cache_s) * cache_number)))
	{
		d_set_func(r, fsys);
		if (cache_size && cache_number)
		{
			cache_data = (uint8_t *) (r->cache_data = refer_alloc(cache_size * cache_number));
			if (!cache_data) goto label_fail;
			r->cache_size = cache_size;
			r->cache_number = cache_number;
			for (i = 0; i < cache_number; ++i)
			{
				r->cache[i].data = cache_data;
				cache_data += cache_size;
			}
		}
		if ((r->fp = fsys_file_alloc(path, flag)) && fsys_file_attr_size(r->fp, &r->size))
			return &r->io;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

#undef d_set_func
