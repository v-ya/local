#include "gvcx.file.h"
#include <stdlib.h>

// static

void gvcx_file_rbtree_memory_free_func(rbtree_t *restrict rbv)
{
	if (rbv->value) refer_free(rbv->value);
}

// memory

void gvcx_file_memory_sync_func(gvcx_file_memory_s *restrict r)
{
	if (r->msync) gvcx_file_msync(r->msync, r);
}

void gvcx_file_memory_free_func(gvcx_file_memory_s *restrict r)
{
	if (r->msync) gvcx_file_msync(r->msync, r);
	free(r->data);
}

gvcx_file_memory_s* gvcx_file_memory_alloc_empty(uintptr_t msize)
{
	gvcx_file_memory_s *restrict r;
	if (msize >= sizeof(gvcx_file_memory_s) && (r = (gvcx_file_memory_s *) refer_alloz(msize)))
		return r;
	return NULL;
}

gvcx_file_memory_s* gvcx_file_memory_initial(gvcx_file_memory_s *restrict m, uintptr_t size, uintptr_t align)
{
	if (!size)
	{
		refer_set_free(m, (refer_free_f) gvcx_file_memory_sync_func);
		return m;
	}
	else if ((m->data = (align?aligned_alloc(align, size):malloc(size))))
	{
		refer_set_free(m, (refer_free_f) gvcx_file_memory_sync_func);
		return m;
	}
	return NULL;
}

gvcx_file_memory_s* gvcx_file_memory_alloc(uintptr_t size, uintptr_t align)
{
	gvcx_file_memory_s *restrict r;
	if ((r = gvcx_file_memory_alloc_empty(sizeof(gvcx_file_memory_s))))
	{
		if (gvcx_file_memory_initial(r, size, align))
			return r;
		refer_free(r);
	}
	return NULL;
}

const void* gvcx_file_memory_align(const void *ptr, uintptr_t align)
{
	if (!align || !((uintptr_t) ptr & (align - 1)))
		return ptr;
	return NULL;
}

// file

void gvcx_file_free_func(gvcx_file_s *restrict r)
{
	rbtree_clear(&r->memory);
}

gvcx_file_s* gvcx_file_alloc_empty(uintptr_t fsize)
{
	gvcx_file_s *restrict r;
	if (fsize >= sizeof(gvcx_file_s) && (r = (gvcx_file_s *) refer_alloz(fsize)))
	{
		refer_set_free(r, (refer_free_f) gvcx_file_free_func);
		return r;
	}
	return NULL;
}

uint64_t gvcx_file_size(gvcx_file_s *restrict f)
{
	return f->size(f);
}

uintptr_t gvcx_file_read(gvcx_file_s *restrict f, void *restrict data, uintptr_t nblock, uintptr_t bsize)
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

uintptr_t gvcx_file_write(gvcx_file_s *restrict f, const void *restrict data, uintptr_t nblock, uintptr_t bsize)
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

uint64_t gvcx_file_getpos(gvcx_file_s *restrict f)
{
	return f->getpos(f);
}

uint64_t gvcx_file_setpos(gvcx_file_s *restrict f, uint64_t pos)
{
	return f->setpos(f, pos);
}

gvcx_file_memory_t gvcx_file_mmap(gvcx_file_s *restrict f, uint64_t offset, uintptr_t size, uintptr_t align, gvcx_file_io_t io)
{
	gvcx_file_memory_s *restrict m;
	if ((m = f->mmap(f, offset, size, align, io)))
	{
		if (rbtree_insert(&f->memory, NULL, (uint64_t) (uintptr_t) m, m, gvcx_file_rbtree_memory_free_func))
			return m;
		refer_free(m);
	}
	return NULL;
}

gvcx_file_s* gvcx_file_munmap(gvcx_file_s *restrict f, gvcx_file_memory_t m)
{
	rbtree_t *restrict rbv;
	if ((rbv = rbtree_find(&f->memory, NULL, (uint64_t) (uintptr_t) m)))
	{
		rbtree_delete_by_pointer(&f->memory, rbv);
		return f;
	}
	return NULL;
}

gvcx_file_s* gvcx_file_msync(gvcx_file_s *restrict f, gvcx_file_memory_t m)
{
	if ((uintptr_t) f == (uintptr_t) m->msync && f->msync)
		return f->msync(f, m);
	return NULL;
}
