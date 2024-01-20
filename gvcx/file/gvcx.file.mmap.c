#include "gvcx.file.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>

struct gvcx_file_mmap_s {
	gvcx_file_s file;
	uint8_t *data;
	uintptr_t size;
	uintptr_t pos;
	uintptr_t rpos;
};

static uint64_t gvcx_file_size_mmap(struct gvcx_file_mmap_s *restrict f)
{
	return (uint64_t) f->size;
}
static uintptr_t gvcx_file_read_mmap(struct gvcx_file_mmap_s *restrict f, void *restrict data, uintptr_t size)
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
static uintptr_t gvcx_file_write_mmap(struct gvcx_file_mmap_s *restrict f, const void *restrict data, uintptr_t size)
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
static uint64_t gvcx_file_getpos_mmap(struct gvcx_file_mmap_s *restrict f)
{
	return (uint64_t) f->pos;
}
static uint64_t gvcx_file_setpos_mmap(struct gvcx_file_mmap_s *restrict f, uint64_t pos)
{
	if (pos > (uint64_t) f->size) pos = (uint64_t) f->size;
	f->rpos = (f->size - (f->pos = (uintptr_t) pos));
	return pos;
}
static gvcx_file_memory_s* gvcx_file_mmap_mmap(struct gvcx_file_mmap_s *restrict f, uint64_t offset, uintptr_t size, uintptr_t align, gvcx_file_io_t io)
{
	gvcx_file_memory_s *restrict m;
	m = NULL;
	if (size && offset < (uint64_t) f->size && size <= ((uint64_t) f->size - offset) &&
		(!(io & gvcx_file_io_write) || f->file.msync))
	{
		if (gvcx_file_memory_align(f->data + offset, align))
		{
			if ((m = gvcx_file_memory_alloc(0, 0)))
			{
				m->data = f->data + offset;
				m->size = size;
			}
		}
		else
		{
			if ((m = gvcx_file_memory_alloc(size, align)))
				memcpy(m->data, f->data + offset, size);
		}
		if (m)
		{
			m->offset = offset;
			if ((io & gvcx_file_io_write) && f->file.msync)
				m->msync = &f->file;
		}
	}
	return m;
}
static gvcx_file_s* gvcx_file_msync_mmap(struct gvcx_file_mmap_s *restrict f, const gvcx_file_memory_s *restrict m)
{
	if ((uintptr_t) (f->data + m->offset) != (uintptr_t) m->data)
		memcpy(f->data + m->offset, m->data, m->size);
	return &f->file;
}

static void gvcx_file_mmap_free_func(struct gvcx_file_mmap_s *restrict r)
{
	if (r->data != MAP_FAILED)
		munmap(r->data, r->size);
	gvcx_file_free_func(&r->file);
}
gvcx_file_s* gvcx_file_alloc_mmap(const char *restrict path, gvcx_file_io_t io)
{
	struct gvcx_file_mmap_s *restrict r;
	uintptr_t size;
	struct stat st;
	int fd, oflag, prot;
	r = NULL;
	oflag = O_RDONLY ^ O_WRONLY ^ O_RDWR; // O_ACCMODE
	prot = 0;
	if (io & gvcx_file_io_read)
		oflag ^= O_WRONLY ^ O_RDWR, prot |= PROT_READ;
	if (io & gvcx_file_io_write)
		oflag ^= O_RDONLY ^ O_RDWR, prot |= PROT_WRITE;
	if (~(fd = open(path, oflag)))
	{
		if (!fstat(fd, &st))
		{
			if (((uint64_t) (size = (uintptr_t) st.st_size) == (uint64_t) st.st_size) &&
				(r = (struct gvcx_file_mmap_s *) gvcx_file_alloc_empty(sizeof(struct gvcx_file_mmap_s))))
			{
				r->data = MAP_FAILED;
				refer_set_free(r, (refer_free_f) gvcx_file_mmap_free_func);
				if ((r->data = mmap(NULL, size, prot, MAP_PRIVATE, fd, 0)) != MAP_FAILED)
				{
					r->size = size;
					r->rpos = size;
					r->file.size = (gvcx_file_size_f) gvcx_file_size_mmap;
					r->file.read = (gvcx_file_read_f) gvcx_file_read_mmap;
					r->file.write = (gvcx_file_write_f) gvcx_file_write_mmap;
					r->file.getpos = (gvcx_file_getpos_f) gvcx_file_getpos_mmap;
					r->file.setpos = (gvcx_file_setpos_f) gvcx_file_setpos_mmap;
					r->file.mmap = (gvcx_file_mmap_f) gvcx_file_mmap_mmap;
					if (io & gvcx_file_io_write)
						r->file.msync = (gvcx_file_msync_f) gvcx_file_msync_mmap;
				}
				else refer_free(r), r = NULL;
			}
		}
		close(fd);
	}
	return &r->file;
}
