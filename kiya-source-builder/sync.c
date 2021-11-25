#include "sync.h"
#include <pocket/pocket.h>
#include <pocket/pocket-verify.h>
#include <hashmap.h>
#include <fsys.h>
#include <memory.h>

#define sync_fsys_flags  (fsys_dir_flag_looping | fsys_dir_flag_discard_same_iparent | fsys_dir_flag_discard_hide | fsys_dir_flag_link_type_parse)

hashmap_t* sync_pocket_load_dst_rpath(hashmap_t *restrict dst_rpath, const char *restrict dst_dir)
{
	fsys_dir_s *restrict dir, *rdir;
	rdir = NULL;
	dir = fsys_dir_alloc(dst_dir, fsys_type_file, sync_fsys_flags);
	if (dir)
	{
		fsys_dir_item_t item;
		while ((rdir = fsys_dir_read(dir, &item)))
		{
			if (!item.type)
				break;
			if (!hashmap_set_name(dst_rpath, item.rpath, NULL, NULL))
			{
				rdir = NULL;
				break;
			}
		}
		refer_free(dir);
	}
	return rdir?dst_rpath:NULL;
}

static inline int sync_pocket_compare_string(const char *restrict a, const char *restrict b)
{
	if ((!a && !b) || (a && b && !strcmp(a, b)))
		return 0;
	return -1;
}

static void sync_pocket_compare_reset_ignore(pocket_s *restrict p)
{
	const pocket_attr_t *restrict attr;
	((pocket_header_t *) pocket_header(p))->time.string = NULL;
	if ((attr = pocket_system(p)) &&
		(attr = pocket_find(p, attr, "verify")) &&
		pocket_preset_tag(p, attr) != pocket_tag$index &&
		attr->data.ptr && attr->size)
		memset(attr->data.ptr, 0, attr->size);
}

static int sync_pocket_compare_header(const pocket_header_t *restrict d, const pocket_header_t *restrict s)
{
	if (
		#define dsc(_n)  !sync_pocket_compare_string(d->_n.string, s->_n.string)
		dsc(verify) &&
		dsc(tag) &&
		dsc(version) &&
		dsc(author) &&
		dsc(time) &&
		dsc(description) &&
		dsc(flag)
		#undef dsc
	) return 0;
	return -1;
}

static int sync_pocket_compare_tree(const pocket_attr_t *restrict d, const pocket_attr_t *restrict s)
{
	if (!sync_pocket_compare_string(d->name.string, s->name.string) &&
		!sync_pocket_compare_string(d->tag.string, s->tag.string) &&
		d->size == s->size)
	{
		if (!d->tag.string)
			// @null
			goto label_compare;
		if (!strcmp(d->tag.string, pocket_tag_string$index))
		{
			// @index
			uint64_t i = d->size;
			d = (const pocket_attr_t *) d->data.ptr;
			s = (const pocket_attr_t *) s->data.ptr;
			while (i)
			{
				if (sync_pocket_compare_tree(d, s))
					goto label_fail;
				++d;
				++s;
				--i;
			}
			label_compare:
			return 0;
		}
		else if (!strcmp(d->tag.string, pocket_tag_string$string))
		{
			// @string
			if (!sync_pocket_compare_string((const char *) d->data.ptr, (const char *) s->data.ptr))
				goto label_compare;
		}
		else
		{
			// check data
			if (!d->size || !memcmp(d->data.ptr, s->data.ptr, (size_t) d->size))
				goto label_compare;
		}
	}
	label_fail:
	return -1;
}

static int sync_pocket_compare_tree_check(const pocket_attr_t *restrict d, const pocket_attr_t *restrict s)
{
	if (d && s)
		return sync_pocket_compare_tree(d, s);
	else if (!d && !s)
		return 0;
	return -1;
}

static int sync_pocket_compare(const pocket_verify_s *restrict verify, const char *restrict path, pocket_s *restrict sp)
{
	pocket_s *restrict dp;
	int ret;
	ret = -1;
	dp = pocket_load(path, verify);
	if (dp)
	{
		sync_pocket_compare_reset_ignore(dp);
		sync_pocket_compare_reset_ignore(sp);
		if (!sync_pocket_compare_header(pocket_header(dp), pocket_header(sp)) &&
			!sync_pocket_compare_tree_check(pocket_system(dp), pocket_system(sp)) &&
			!sync_pocket_compare_tree_check(pocket_user(dp), pocket_user(sp)))
			ret = 0;
		refer_free(dp);
	}
	return ret;
}

typedef struct sync_delete_arg_t {
	fsys_rpath_s *restrict rp;
	sync_report_f report_func;
	void *pri;
	int ret;
} sync_delete_arg_t;

static void sync_delete_src_func(hashmap_vlist_t *restrict vl, sync_delete_arg_t *restrict arg)
{
	if (vl->name && !arg->ret)
	{
		const char *restrict path;
		path = fsys_rpath_get_full_path(arg->rp, vl->name);
		if (path)
		{
			arg->ret = fsys_ctrl_remove(path);
			arg->ret = arg->report_func(arg->pri, (arg->ret >= 0)?sync_status_delete:sync_status_fail, vl->name, NULL);
		}
		else arg->ret = -1;
	}
}

int sync_pocket_ignore_time(const struct pocket_verify_s *restrict verify, const char *restrict dst_dir, const char *restrict src_dir, sync_report_f report_func, void *pri)
{
	hashmap_t dst_rpath;
	int ret, rr;
	ret = -1;
	rr = 0;
	if ((hashmap_init(&dst_rpath)))
	{
		fsys_rpath_s *restrict rp;
		fsys_dir_s *restrict dir;
		pocket_s *restrict sp;
		const char *restrict dst_path;
		sync_delete_arg_t da;
		rp = NULL;
		dir = NULL;
		sp = NULL;
		if (sync_pocket_load_dst_rpath(&dst_rpath, dst_dir))
		{
			rp = fsys_rpath_alloc(dst_dir, 0);
			dir = fsys_dir_alloc(src_dir, fsys_type_file, sync_fsys_flags);
			if (rp && dir)
			{
				fsys_dir_item_t item;
				for (;;)
				{
					if (!fsys_dir_read(dir, &item))
						goto label_fail;
					if (!item.type)
						break;
					dst_path = fsys_rpath_get_full_path(rp, item.rpath);
					if (!dst_path)
						goto label_fail;
					if ((sp = pocket_load(item.path, verify)))
					{
						if (!hashmap_find_name(&dst_rpath, item.rpath) ||
							sync_pocket_compare(verify, dst_path, sp))
						{
							// copy
							fsys_ctrl_remove(dst_path);
							rr = fsys_ctrl_mkpath_copy(rp, item.rpath, item.path);
							if ((rr = report_func(pri, (rr >= 0)?sync_status_copy:sync_status_fail, item.rpath, item.rpath)))
								goto label_fail;
						}
						else
						{
							// same pocket
							if ((rr = report_func(pri, sync_status_same, item.rpath, item.rpath)))
								goto label_fail;
						}
						refer_free(sp);
						sp = NULL;
						hashmap_delete_name(&dst_rpath, item.rpath);
					}
					else
					{
						// src not pocket
						if ((rr = report_func(pri, sync_status_fail, NULL, item.rpath)))
							goto label_fail;
					}
				}
				da.rp = rp;
				da.report_func = report_func;
				da.pri = pri;
				da.ret = 0;
				hashmap_call(&dst_rpath, (hashmap_func_call_f) sync_delete_src_func, &da);
				rr = da.ret;
				ret = 0;
			}
		}
		label_fail:
		if (sp) refer_free(sp);
		if (rp) refer_free(rp);
		if (dir) refer_free(dir);
		hashmap_uini(&dst_rpath);
	}
	return rr?rr:ret;
}
