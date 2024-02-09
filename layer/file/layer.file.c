#include "layer.file.h"
#include <stdlib.h>

// static

void layer_file_rbtree_memory_free_func(rbtree_t *restrict rbv)
{
	if (rbv->value) refer_free(rbv->value);
}

// memory

void layer_file_memory_sync_func(layer_file_memory_s *restrict r)
{
	if (r->msync) layer_file_msync(r->msync, r);
}

void layer_file_memory_free_func(layer_file_memory_s *restrict r)
{
	if (r->msync) layer_file_msync(r->msync, r);
	free(r->data);
}

layer_file_memory_s* layer_file_memory_alloc_empty(uintptr_t msize)
{
	layer_file_memory_s *restrict r;
	if (msize >= sizeof(layer_file_memory_s) && (r = (layer_file_memory_s *) refer_alloz(msize)))
		return r;
	return NULL;
}

layer_file_memory_s* layer_file_memory_initial(layer_file_memory_s *restrict m, uintptr_t size, uintptr_t align)
{
	if (!size)
	{
		refer_set_free(m, (refer_free_f) layer_file_memory_sync_func);
		return m;
	}
	else if ((m->data = (align?aligned_alloc(align, size):malloc(size))))
	{
		refer_set_free(m, (refer_free_f) layer_file_memory_sync_func);
		return m;
	}
	return NULL;
}

layer_file_memory_s* layer_file_memory_alloc(uintptr_t size, uintptr_t align)
{
	layer_file_memory_s *restrict r;
	if ((r = layer_file_memory_alloc_empty(sizeof(layer_file_memory_s))))
	{
		if (layer_file_memory_initial(r, size, align))
			return r;
		refer_free(r);
	}
	return NULL;
}

const void* layer_file_memory_align(const void *ptr, uintptr_t align)
{
	if (!align || !((uintptr_t) ptr & (align - 1)))
		return ptr;
	return NULL;
}

// file

void layer_file_free_func(layer_file_s *restrict r)
{
	rbtree_clear(&r->memory);
}

layer_file_s* layer_file_alloc_empty(uintptr_t fsize)
{
	layer_file_s *restrict r;
	if (fsize >= sizeof(layer_file_s) && (r = (layer_file_s *) refer_alloz(fsize)))
	{
		refer_set_free(r, (refer_free_f) layer_file_free_func);
		return r;
	}
	return NULL;
}

uint64_t layer_file_size(layer_file_s *restrict f)
{
	return f->size(f);
}

uintptr_t layer_file_read(layer_file_s *restrict f, void *restrict data, uintptr_t nblock, uintptr_t bsize)
{
	uintptr_t size;
	size = nblock * bsize;
	if (size && size >= nblock && size >= bsize)
	{
		size = f->read(f, data, size);
		nblock = size / bsize;
		bsize = size % bsize;
		if (bsize) f->setpos(f, f->getpos(f) - (uint64_t) bsize);
		return nblock;
	}
	return 0;
}

uintptr_t layer_file_write(layer_file_s *restrict f, const void *restrict data, uintptr_t nblock, uintptr_t bsize)
{
	uintptr_t size;
	size = nblock * bsize;
	if (size > nblock && size > bsize)
	{
		size = f->write(f, data, size);
		nblock = size / bsize;
		bsize = size % bsize;
		if (bsize) f->setpos(f, f->getpos(f) - (uint64_t) bsize);
		return nblock;
	}
	return 0;
}

uint64_t layer_file_getpos(layer_file_s *restrict f)
{
	return f->getpos(f);
}

uint64_t layer_file_setpos(layer_file_s *restrict f, uint64_t pos)
{
	return f->setpos(f, pos);
}

layer_file_memory_t layer_file_mmap(layer_file_s *restrict f, uint64_t offset, uintptr_t size, uintptr_t align, layer_file_io_t io)
{
	layer_file_memory_s *restrict m;
	if ((m = f->mmap(f, offset, size, align, io)))
	{
		if (rbtree_insert(&f->memory, NULL, (uint64_t) (uintptr_t) m, m, layer_file_rbtree_memory_free_func))
			return m;
		refer_free(m);
	}
	return NULL;
}

layer_file_s* layer_file_munmap(layer_file_s *restrict f, layer_file_memory_t m)
{
	rbtree_t *restrict rbv;
	if ((rbv = rbtree_find(&f->memory, NULL, (uint64_t) (uintptr_t) m)))
	{
		rbtree_delete_by_pointer(&f->memory, rbv);
		return f;
	}
	return NULL;
}

void* layer_file_mdata(layer_file_s *restrict f, layer_file_memory_t m, uintptr_t *restrict size)
{
	if (rbtree_find(&f->memory, NULL, (uint64_t) (uintptr_t) m))
	{
		if (size) *size = m->size;
		return m->data;
	}
	if (size) *size = 0;
	return NULL;
}

layer_file_s* layer_file_msync(layer_file_s *restrict f, layer_file_memory_t m)
{
	if (rbtree_find(&f->memory, NULL, (uint64_t) (uintptr_t) m) &&
		(uintptr_t) f == (uintptr_t) m->msync && f->msync)
		return f->msync(f, m);
	return NULL;
}
