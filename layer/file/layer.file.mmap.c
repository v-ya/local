#include "layer.file.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>

struct layer_file_mmap_s {
	layer_file_s file;
	uint8_t *data;
	uintptr_t size;
	uintptr_t pos;
	uintptr_t rpos;
};

static uint64_t layer_file_size_mmap(struct layer_file_mmap_s *restrict f)
{
	return (uint64_t) f->size;
}
static uintptr_t layer_file_read_mmap(struct layer_file_mmap_s *restrict f, void *restrict data, uintptr_t size)
{
	if (size > f->rpos) size = f->rpos;
	if (size)
	{
		memcpy(data, f->data + f->pos, size);
		f->pos += size;
		f->rpos -= size;
	}
	return size;
}
static uintptr_t layer_file_write_mmap(struct layer_file_mmap_s *restrict f, const void *restrict data, uintptr_t size)
{
	if (size > f->rpos) size = f->rpos;
	if (size)
	{
		memcpy(f->data + f->pos, data, size);
		f->pos += size;
		f->rpos -= size;
	}
	return size;
}
static uint64_t layer_file_getpos_mmap(struct layer_file_mmap_s *restrict f)
{
	return (uint64_t) f->pos;
}
static uint64_t layer_file_setpos_mmap(struct layer_file_mmap_s *restrict f, uint64_t pos)
{
	if (pos > (uint64_t) f->size) pos = (uint64_t) f->size;
	f->rpos = (f->size - (f->pos = (uintptr_t) pos));
	return pos;
}
static layer_file_memory_s* layer_file_mmap_mmap(struct layer_file_mmap_s *restrict f, uint64_t offset, uintptr_t size, uintptr_t align, layer_file_io_t io)
{
	layer_file_memory_s *restrict m;
	m = NULL;
	if (size && offset < (uint64_t) f->size && size <= ((uint64_t) f->size - offset) &&
		(!(io & layer_file_io_write) || f->file.msync))
	{
		if (layer_file_memory_align(f->data + offset, align))
		{
			if ((m = layer_file_memory_alloc(0, 0)))
			{
				m->data = f->data + offset;
				m->size = size;
			}
		}
		else
		{
			if ((m = layer_file_memory_alloc(size, align)))
				memcpy(m->data, f->data + offset, size);
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
static layer_file_s* layer_file_msync_mmap(struct layer_file_mmap_s *restrict f, const layer_file_memory_s *restrict m)
{
	if ((uintptr_t) (f->data + m->offset) != (uintptr_t) m->data)
		memcpy(f->data + m->offset, m->data, m->size);
	return &f->file;
}

static void layer_file_mmap_free_func(struct layer_file_mmap_s *restrict r)
{
	if (r->data != MAP_FAILED)
		munmap(r->data, r->size);
	layer_file_free_func(&r->file);
}
layer_file_s* layer_file_alloc_mmap(const char *restrict path, layer_file_io_t io)
{
	struct layer_file_mmap_s *restrict r;
	uintptr_t size;
	struct stat st;
	int fd, oflag, prot;
	r = NULL;
	oflag = O_RDONLY ^ O_WRONLY ^ O_RDWR; // O_ACCMODE
	prot = 0;
	if (io & layer_file_io_read)
		oflag ^= O_WRONLY ^ O_RDWR, prot |= PROT_READ;
	if (io & layer_file_io_write)
		oflag ^= O_RDONLY ^ O_RDWR, prot |= PROT_WRITE;
	if (~(fd = open(path, oflag)))
	{
		if (!fstat(fd, &st))
		{
			if (((uint64_t) (size = (uintptr_t) st.st_size) == (uint64_t) st.st_size) &&
				(r = (struct layer_file_mmap_s *) layer_file_alloc_empty(sizeof(struct layer_file_mmap_s))))
			{
				r->data = MAP_FAILED;
				refer_set_free(r, (refer_free_f) layer_file_mmap_free_func);
				if ((r->data = mmap(NULL, size, prot, MAP_PRIVATE, fd, 0)) != MAP_FAILED)
				{
					r->size = size;
					r->rpos = size;
					r->file.size = (layer_file_size_f) layer_file_size_mmap;
					r->file.read = (layer_file_read_f) layer_file_read_mmap;
					r->file.write = (layer_file_write_f) layer_file_write_mmap;
					r->file.getpos = (layer_file_getpos_f) layer_file_getpos_mmap;
					r->file.setpos = (layer_file_setpos_f) layer_file_setpos_mmap;
					r->file.mmap = (layer_file_mmap_f) layer_file_mmap_mmap;
					if (io & layer_file_io_write)
						r->file.msync = (layer_file_msync_f) layer_file_msync_mmap;
				}
				else refer_free(r), r = NULL;
			}
		}
		close(fd);
	}
	return &r->file;
}
