#define _GNU_SOURCE
#include "fsys-type.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

struct fsys_file_s {
	int fd;
	unsigned int attr_update;
	struct stat64 attr_file;
};

#define attr_getdaze(_r)  (_r->attr_update || fsys_file_get_attr(_r))

static int fsys_file_flags2oflag(fsys_file_flag_t flags)
{
	static const uint32_t of_map[] = {
		[0] = O_WRONLY ^ O_RDWR, // fsys_file_flag_read
		[1] = O_RDONLY ^ O_RDWR, // fsys_file_flag_write
		[2] = O_APPEND,          // fsys_file_flag_append
		[3] = 0,                 // -
		[4] = O_CREAT,           // fsys_file_flag_create
		[5] = O_EXCL,            // fsys_file_flag_must_create & ~fsys_file_flag_create
		[6] = O_TRUNC,           // fsys_file_flag_truncate
		[7] = O_TMPFILE,         // fsys_file_flag_temporary
		[8] = O_NOATIME,         // fsys_file_flag_no_access_time
	};
	register uint32_t o = O_RDONLY ^ O_WRONLY ^ O_RDWR; // == O_ACCMODE
	for (register uint32_t i = 0; i < (sizeof(of_map) / sizeof(*of_map)); ++i)
		if (flags & (1u << i))
			o ^= of_map[i];
	return (int) o;
}

static void fsys_file_free_func(fsys_file_s *restrict r)
{
	if (~r->fd)
		close(r->fd);
}

fsys_file_s* fsys_file_alloc(const char *restrict path, fsys_file_flag_t flags)
{
	fsys_file_s *restrict r;
	r = (fsys_file_s *) refer_alloc(sizeof(fsys_file_s));
	if (r)
	{
		r->fd = ~0;
		r->attr_update = 0;
		refer_set_free(r, (refer_free_f) fsys_file_free_func);
		r->fd = open(path, fsys_file_flags2oflag(flags), fsys_default_open_mode);
		if (~r->fd)
			return r;
		refer_free(r);
	}
	return NULL;
}

fsys_file_s* fsys_file_read(fsys_file_s *restrict file, void *data, uintptr_t size, uintptr_t *restrict rsize)
{
	ssize_t n;
	if (~(n = read(file->fd, data, size)))
	{
		if (rsize)
			*rsize = (uintptr_t) n;
		return file;
	}
	return NULL;
}

fsys_file_s* fsys_file_write(fsys_file_s *restrict file, const void *data, uintptr_t size, uintptr_t *restrict rsize)
{
	ssize_t n;
	if (~(n = write(file->fd, data, size)))
	{
		if (rsize)
			*rsize = (uintptr_t) n;
		return file;
	}
	return NULL;
}

refer_nstring_t fsys_file_aread(fsys_file_s *restrict file, uintptr_t size)
{
	struct refer_nstring_s *restrict r;
	ssize_t n;
	r = (struct refer_nstring_s *) refer_alloc(sizeof(struct refer_nstring_s) + 1 + size);
	if (r)
	{
		if (~(n = read(file->fd, r->string, size)))
		{
			r->string[r->length = (uintptr_t) n] = 0;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

refer_nstring_t fsys_file_aread_all(fsys_file_s *restrict file)
{
	if (attr_getdaze(file) && ~lseek64(file->fd, 0, SEEK_SET))
		return fsys_file_aread(file, (uintptr_t) file->attr_file.st_size);
	return NULL;
}

fsys_file_s* fsys_file_seek_by_head(fsys_file_s *restrict file, uint64_t offset, uint64_t *restrict roffset)
{
	loff_t rp;
	if (~(rp = lseek64(file->fd, (loff_t) offset, SEEK_SET)))
	{
		if (roffset)
			*roffset = (uint64_t) rp;
		return file;
	}
	return NULL;
}

fsys_file_s* fsys_file_seek_by_curr(fsys_file_s *restrict file, int64_t offset, uint64_t *restrict roffset)
{
	loff_t rp;
	if (~(rp = lseek64(file->fd, (loff_t) offset, SEEK_CUR)))
	{
		if (roffset)
			*roffset = (uint64_t) rp;
		return file;
	}
	return NULL;
}

fsys_file_s* fsys_file_seek_by_tail(fsys_file_s *restrict file, int64_t offset, uint64_t *restrict roffset)
{
	loff_t rp;
	if (~(rp = lseek64(file->fd, (loff_t) offset, SEEK_END)))
	{
		if (roffset)
			*roffset = (uint64_t) rp;
		return file;
	}
	return NULL;
}

fsys_file_s* fsys_file_truncate(fsys_file_s *restrict file, uint64_t size)
{
	if (!ftruncate64(file->fd, (loff_t) size))
		return file;
	return NULL;
}

fsys_file_s* fsys_file_get_attr(fsys_file_s *restrict file)
{
	if (!fstat64(file->fd, &file->attr_file))
	{
		file->attr_update = 1;
		return file;
	}
	return NULL;
}

fsys_file_s* fsys_file_attr_size(fsys_file_s *restrict file, uint64_t *restrict size)
{
	if (size && attr_getdaze(file))
	{
		*size = (uint64_t) file->attr_file.st_size;
		return file;
	}
	return NULL;
}

fsys_file_s* fsys_file_attr_rdev(fsys_file_s *restrict file, uint64_t *restrict rdev)
{
	if (rdev && attr_getdaze(file))
	{
		*rdev = (uint64_t) file->attr_file.st_rdev;
		return file;
	}
	return NULL;
}

fsys_file_s* fsys_file_attr_time_access(fsys_file_s *restrict file, uint64_t *restrict posixtimestamp_secend, uint64_t *restrict posixtimestamp_nanosecend)
{
	if ((posixtimestamp_secend || posixtimestamp_nanosecend) && attr_getdaze(file))
	{
		if (posixtimestamp_secend)
			*posixtimestamp_secend = (uint64_t) file->attr_file.st_atim.tv_sec;
		if (posixtimestamp_nanosecend)
			*posixtimestamp_nanosecend = (uint64_t) file->attr_file.st_atim.tv_nsec;
		return file;
	}
	return NULL;
}

fsys_file_s* fsys_file_attr_time_modify(fsys_file_s *restrict file, uint64_t *restrict posixtimestamp_secend, uint64_t *restrict posixtimestamp_nanosecend)
{
	if ((posixtimestamp_secend || posixtimestamp_nanosecend) && attr_getdaze(file))
	{
		if (posixtimestamp_secend)
			*posixtimestamp_secend = (uint64_t) file->attr_file.st_mtim.tv_sec;
		if (posixtimestamp_nanosecend)
			*posixtimestamp_nanosecend = (uint64_t) file->attr_file.st_mtim.tv_nsec;
		return file;
	}
	return NULL;
}

fsys_file_s* fsys_file_attr_time_create(fsys_file_s *restrict file, uint64_t *restrict posixtimestamp_secend, uint64_t *restrict posixtimestamp_nanosecend)
{
	if ((posixtimestamp_secend || posixtimestamp_nanosecend) && attr_getdaze(file))
	{
		if (posixtimestamp_secend)
			*posixtimestamp_secend = (uint64_t) file->attr_file.st_ctim.tv_sec;
		if (posixtimestamp_nanosecend)
			*posixtimestamp_nanosecend = (uint64_t) file->attr_file.st_ctim.tv_nsec;
		return file;
	}
	return NULL;
}
