#define _DEFAULT_SOURCE
#include "fsys-dirent.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

typedef typeof(((struct dirent *) 0)->d_type) fsys_dir_local_type_t;

static exbuffer_t* fsys_dir_path_push(exbuffer_t *restrict ep, const char *restrict name, uintptr_t *restrict pp_end, uintptr_t *restrict pp_name)
{
	char *p;
	uintptr_t pos, length;
	pos = *pp_end;
	length = strlen(name);
	if ((p = (char *) exbuffer_need(ep, pos + length + 2)))
	{
		if (pos && p[pos - 1] != '/')
			p[pos++] = '/';
		*pp_name = pos;
		if (length)
			memcpy(p + pos, name, length);
		p[*pp_end = pos + length] = 0;
		return ep;
	}
	return NULL;
}

static inline void fsys_dir_path_pop(exbuffer_t *restrict ep, uintptr_t pp_end)
{
	((char *) ep->data)[pp_end] = 0;
}

static fsys_dir_s* fsys_dir_stack_push(fsys_dir_s *restrict r)
{
	fsys_dir_stack_t *restrict p;
	DIR *d;
	uintptr_t index, size;
	if (r->ds_pos >= r->ds_max_depth)
	{
		if (!(r->flags & fsys_dir_flag_interrupt_beyond_max_depth))
			goto label_okay;
		goto label_fail;
	}
	if (r->item.name && *r->item.name == '.' && (r->flags & fsys_dir_flag_discard_hide))
		goto label_okay;
	if ((r->flags & fsys_dir_flag_discard_same_iparent) && r->curr)
	{
		typeof(r->curr->d_ino) ino;
		ino = r->curr->d_ino;
		p = r->dstack;
		for (index = 0, size = r->ds_pos; index < size; ++index)
		{
			if (p[index].curr && p[index].curr->d_ino == ino)
				goto label_okay;
		}
	}
	d = opendir((char *) r->path_buffer.data);
	if (d)
	{
		if ((index = r->ds_pos) < r->ds_size)
		{
			p = r->dstack;
			label_push_set:
			p[index].dirent = r->dirent;
			p[index].curr = r->curr;
			p[index].pp_last_name = r->pp_name;
			p[index].pp_last_end = r->pp_end;
			r->dirent = d;
			r->curr = NULL;
			r->pp_name = r->pp_end;
			r->ds_pos = index + 1;
			label_okay:
			return r;
		}
		size = r->ds_size?(r->ds_size << 1):fsys_dir_stack_default_size;
		p = (fsys_dir_stack_t *) realloc(r->dstack, size * sizeof(fsys_dir_stack_t));
		if (p)
		{
			r->dstack = p;
			r->ds_size = size;
			goto label_push_set;
		}
		closedir(d);
	}
	else if (!(r->flags & fsys_dir_flag_interrupt_sub_dirent_fail))
		goto label_okay;
	label_fail:
	return NULL;
}

static fsys_dir_s* fsys_dir_stack_pop(fsys_dir_s *restrict r)
{
	fsys_dir_stack_t *restrict p;
	uintptr_t index;
	r->item.type = fsys_type_null;
	r->item.path = NULL;
	r->item.rpath = NULL;
	r->item.name = NULL;
	if ((index = r->ds_pos))
	{
		p = r->dstack;
		r->ds_pos = --index;
		if (r->dirent)
			closedir(r->dirent);
		r->dirent = p[index].dirent;
		r->curr = p[index].curr;
		r->pp_name = p[index].pp_last_name;
		fsys_dir_path_pop(&r->path_buffer, r->pp_end = p[index].pp_last_end);
		return r;
	}
	return NULL;
}

static inline fsys_type_t fsys_dir_get_type(fsys_dir_local_type_t type)
{
	switch (type)
	{
		case DT_DIR: return fsys_type_dirent;
		case DT_REG: return fsys_type_file;
		case DT_LNK: return fsys_type_link;
		default: return fsys_type_unknow;
	}
}

static fsys_type_t fsys_dir_get_type_by_path(const char *restrict path)
{

	struct stat st;
	if (!stat(path, &st))
		return fsys_dir_get_type((fsys_dir_local_type_t) IFTODT(st.st_mode));
	return fsys_type_unknow;
}

static inline const char* fsys_dir_is_cd_name(const char *restrict name)
{
	if (name[0] == '.' && (!name[1] || (name[1] == '.' && !name[2])))
		return name;
	return NULL;
}

static inline const fsys_dir_s* fsys_dir_check_hit(const fsys_dir_s *restrict r, const char *restrict name, fsys_type_t type)
{
	if (!(type & r->allow_type))
		goto label_miss;
	if (*name == '.' && (r->flags & fsys_dir_flag_discard_hide))
		goto label_miss;
	return r;
	label_miss:
	return NULL;
}

static void fsys_dir_free_func(fsys_dir_s *restrict r)
{
	if (r->dirent)
		closedir(r->dirent);
	if (r->dstack)
	{
		uintptr_t i;
		i = r->ds_pos;
		while (i)
		{
			--i;
			if (r->dstack[i].dirent)
				closedir(r->dstack[i].dirent);
		}
		free(r->dstack);
	}
	exbuffer_uini(&r->path_buffer);
}

fsys_dir_s* fsys_dir_alloc(const char *restrict path, fsys_type_t type, fsys_dir_flag_t flags)
{
	fsys_dir_s *restrict r;
	r = (fsys_dir_s *) refer_alloz(sizeof(fsys_dir_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) fsys_dir_free_func);
		if (fsys_dir_path_push(&r->path_buffer, path, &r->pp_end, &r->pp_name) &&
			fsys_dir_path_push(&r->path_buffer, "", &r->pp_end, &r->pp_name))
		{
			r->pp_rpath = r->pp_name;
			r->dirent = opendir(path);
			if (r->dirent)
			{
				r->ds_max_depth = fsys_dir_stack_max_depth;
				r->allow_type = type;
				r->flags = flags;
				return r;
			}
		}
		refer_free(r);
	}
	return NULL;
}

void fsys_dir_set_max_depth(fsys_dir_s *restrict dir, uintptr_t max_depth)
{
	dir->ds_max_depth = max_depth;
}

fsys_dir_s* fsys_dir_read(fsys_dir_s *restrict dir, fsys_dir_item_t *restrict item)
{
	struct dirent *restrict d;
	uintptr_t pos;
	fsys_type_t type;
	for (;;)
	{
		if ((dir->item.type == fsys_type_dirent) &&
			(dir->flags & fsys_dir_flag_looping) &&
			!fsys_dir_stack_push(dir))
			goto label_fail;
		label_skip_this:
		errno = 0;
		d = readdir(dir->dirent);
		if (errno)
			goto label_fail;
		if ((dir->curr = d))
		{
			if (fsys_dir_is_cd_name(d->d_name))
				goto label_skip_this;
			type = fsys_dir_get_type(d->d_type);
			pos = dir->pp_name;
			if (!fsys_dir_path_push(&dir->path_buffer, d->d_name, &pos, &dir->pp_name))
				goto label_fail;
			dir->pp_end = pos;
			dir->item.type = type;
			dir->item.path = (const char *) dir->path_buffer.data;
			dir->item.rpath = dir->item.path + dir->pp_rpath;
			dir->item.name = dir->item.path + dir->pp_name;
			if (type == fsys_type_link && (dir->flags & fsys_dir_flag_link_type_parse))
				dir->item.type = type = fsys_dir_get_type_by_path(dir->item.path);
			if (fsys_dir_check_hit(dir, d->d_name, type))
			{
				label_okay:
				memcpy(item, &dir->item, sizeof(fsys_dir_item_t));
				return dir;
			}
		}
		else if (!fsys_dir_stack_pop(dir))
			goto label_okay;
	}
	label_fail:
	return NULL;
}
