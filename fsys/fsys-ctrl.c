#define _GNU_SOURCE
#include "fsys-type.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

fsys_type_t fsys_ctrl_exist(const char *restrict path, uint64_t *restrict rsize)
{
	struct stat st;
	if (path && !stat(path, &st))
	{
		if (rsize) *rsize = (uint64_t) st.st_size;
		return fsys_dt2type((fsys_dt_type_t) IFTODT(st.st_mode));
	}
	if (rsize) *rsize = 0;
	return fsys_type_null;
}

int fsys_ctrl_remove(const char *restrict path)
{
	int ret;
	if (path && (!(ret = unlink(path)) || ret == ENOENT))
		return !!ret;
	return -1;
}

int fsys_ctrl_rmdir(const char *restrict path)
{
	int ret;
	if (path && (!(ret = rmdir(path)) || ret == ENOENT))
		return !!ret;
	return -1;
}

int fsys_ctrl_mkdir(const char *restrict path)
{
	int ret;
	if (path && (!(ret = rmdir(path)) || ret == EEXIST))
		return !!ret;
	return -1;
}

int fsys_ctrl_copy(const char *restrict dst, const char *restrict src)
{
	struct stat st;
	off_t size;
	ssize_t length;
	int fs, fd, ret;
	ret = -1;
	if (dst && src && ~(fs = open(src, O_RDONLY)))
	{
		if (!fstat(fs, &st) && ~(fd = open(dst, O_CREAT | O_EXCL | O_WRONLY, fsys_default_open_mode)))
		{
			size = st.st_size;
			while (size > 0)
			{
				length = copy_file_range(fs, NULL, fd, NULL, (size_t) size, 0);
				if (length < 0)
					goto label_fail;
				size -= length;
			}
			ret = 0;
			label_fail:
			close(fd);
		}
		close(fs);
	}
	return ret;
}

int fsys_ctrl_make_parent_path(fsys_rpath_s *restrict rp, const char *restrict rpath)
{
	const char *ps, *p, *last_path;
	int last_ret;
	if (rp && rpath)
	{
		ps = rpath;
		last_path = NULL;
		last_ret = 0;
		while ((p = strchr(ps, '/')))
		{
			last_path = fsys_rpath_get_full_path_with_length(rp, rpath, (uintptr_t) p - (uintptr_t) rpath);
			if (!last_path)
				goto label_fail;
			last_ret = fsys_ctrl_mkdir(last_path);
			if (last_ret < 0)
				goto label_fail;
			ps = p + 1;
		}
		if (!last_ret || !last_path || fsys_ctrl_exist(last_path, NULL) == fsys_type_dirent)
			return 0;
	}
	label_fail:
	return -1;
}

int fsys_ctrl_mkpath_copy(fsys_rpath_s *restrict dst_rp, const char *restrict dst_rpath, const char *restrict src)
{
	if (src && !fsys_ctrl_make_parent_path(dst_rp, dst_rpath) &&
		!fsys_ctrl_copy(fsys_rpath_get_full_path(dst_rp, dst_rpath), src))
		return 0;
	return -1;
}

