#include "fsys.h"
#include <exbuffer.h>
#include <string.h>

struct fsys_rpath_s {
	exbuffer_t path;
	uintptr_t p_end;
	uintptr_t auto_insert_delimiter;
	uintptr_t stack_index;
	uintptr_t stack_size;
	uintptr_t stack[];
};

static void fsys_rpath_free_func(fsys_rpath_s *restrict r)
{
	exbuffer_uini(&r->path);
}

fsys_rpath_s* fsys_rpath_alloc(const char *restrict root_path, uintptr_t stack_size)
{
	fsys_rpath_s *restrict r;
	r = (fsys_rpath_s *) refer_alloc(sizeof(fsys_rpath_s) + stack_size * sizeof(uintptr_t));
	if (r)
	{
		if (exbuffer_init(&r->path, 0))
		{
			refer_set_free(r, (refer_free_f) fsys_rpath_free_func);
			r->p_end = 0;
			r->auto_insert_delimiter = '/';
			r->stack_index = 0;
			r->stack_size = stack_size;
			if (!root_path || fsys_rpath_set_root(r, root_path))
				return r;
		}
		refer_free(r);
	}
	return NULL;
}

void fsys_rpath_set_delimiter(fsys_rpath_s *restrict rp, char delimiter)
{
	rp->auto_insert_delimiter = (uintptr_t) delimiter;
}

fsys_rpath_s* fsys_rpath_set_root_with_length(fsys_rpath_s *restrict rp, const char *restrict root_path, uintptr_t length)
{
	char *p;
	char c;
	if (length)
	{
		if ((p = exbuffer_need(&rp->path, length + 2)))
		{
			memcpy(p, root_path, length);
			if ((c = (char) rp->auto_insert_delimiter))
			{
				if (p[length - 1] != c)
					p[length++] = c;
			}
			rp->p_end = length;
			label_okay:
			rp->stack_index = 0;
			return rp;
		}
	}
	else
	{
		rp->p_end = 0;
		goto label_okay;
	}
	return NULL;
}

fsys_rpath_s* fsys_rpath_set_root(fsys_rpath_s *restrict rp, const char *restrict root_path)
{
	return fsys_rpath_set_root_with_length(rp, root_path, root_path?strlen(root_path):0);
}

const char* fsys_rpath_get_full_path_with_length(fsys_rpath_s *restrict rp, const char *restrict rpath, uintptr_t length)
{
	char *p;
	uintptr_t index, n;
	n = (index = rp->p_end) + length;
	if (length)
	{
		if ((p = (char *) exbuffer_need(&rp->path, n + 1)))
		{
			memcpy(p + index, rpath, length);
			label_okay:
			p[n] = 0;
			return p;
		}
	}
	else
	{
		p = (char *) rp->path.data;
		goto label_okay;
	}
	return NULL;
}

const char* fsys_rpath_get_full_path(fsys_rpath_s *restrict rp, const char *restrict rpath)
{
	return fsys_rpath_get_full_path_with_length(rp, rpath, rpath?strlen(rpath):0);
}

const char* fsys_rpath_push_with_length(fsys_rpath_s *restrict rp, const char *restrict rpath, uintptr_t length)
{
	char *p;
	uintptr_t si;
	if ((si = rp->stack_index) < rp->stack_size)
	{
		uintptr_t index, n;
		char c;
		n = (index = rp->p_end) + length;
		if (length)
		{
			if ((p = (char *) exbuffer_need(&rp->path, n + 2)))
			{
				if (length)
					memcpy(p + index, rpath, length);
				if ((c = (char) rp->auto_insert_delimiter))
				{
					if (p[n - 1] != c)
						p[n++] = c;
				}
				label_okay:
				p[n] = 0;
				rp->stack[si] = index;
				rp->p_end = n;
				rp->stack_index = si + 1;
				return p;
			}
		}
		else
		{
			p = (char *) rp->path.data;
			goto label_okay;
		}
	}
	return NULL;
}

const char* fsys_rpath_push(fsys_rpath_s *restrict rp, const char *restrict rpath)
{
	return fsys_rpath_push_with_length(rp, rpath, rpath?strlen(rpath):0);
}

const char* fsys_rpath_pop(fsys_rpath_s *restrict rp)
{
	char *p;
	uintptr_t si;
	if ((si = rp->stack_index))
	{
		p = (char *) rp->path.data;
		p[rp->p_end = rp->stack[rp->stack_index = si - 1]] = 0;
		return p;
	}
	return NULL;
}
