#include "entry_args.h"
#include "builder.h"
#include <pocket/pocket.h>
#include <pocket/pocket-verify.h>
#include <hashmap.h>
#include <fsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct scan_env_s scan_env_s;

struct scan_env_s {
	pocket_verify_s *verify;
	source_builder_s *sb;
	hashmap_t name;
};

static void env_name_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		free(vl->value);
}

static void scan_env_free_func(scan_env_s *restrict r)
{
	if (r->verify)
		refer_free(r->verify);
	if (r->sb)
		refer_free(r->sb);
	hashmap_uini(&r->name);
}

static scan_env_s *scan_env_alloc(void)
{
	scan_env_s *restrict r;
	r = (scan_env_s *) refer_alloz(sizeof(scan_env_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) scan_env_free_func);
		if ((r->verify = pocket_verify_default()) &&
			hashmap_init(&r->name))
			return r;
		refer_free(r);
	}
	return NULL;
}

static void scan_env_set_builder(scan_env_s *restrict env, source_builder_s *restrict sb)
{
	if (env->sb)
		refer_free(env->sb);
	env->sb = (source_builder_s *) refer_save(sb);
}

static hashmap_vlist_t* env_set_name_path(scan_env_s *restrict env, const char *restrict name, const char *restrict path)
{
	hashmap_vlist_t *restrict r;
	void *restrict v;
	uintptr_t length;
	r = NULL;
	length = strlen(path) + 1;
	if ((v = malloc(length)))
	{
		memcpy(v, path, length);
		if (!(r = hashmap_set_name(&env->name, name, v, env_name_free_func)))
			free(v);
	}
	return r;
}

static scan_env_s* report_path(scan_env_s *restrict env, const char *restrict path)
{
	pocket_s *restrict p;
	const pocket_attr_t *restrict v;
	hashmap_vlist_t *restrict vl;
	if ((p = pocket_load(path, env->verify)))
	{
		// [$ "kiya"] "name" @string
		if ((v = pocket_find_path_tag(
				p,
				pocket_system(p),
				(const char *[]) {"kiya", "name", NULL},
				pocket_tag$string, NULL)) &&
			v->data.ptr)
		{
			if ((vl = hashmap_find_name(&env->name, (const char *) v->data.ptr)))
				goto label_fail_conflict;
			if (!env_set_name_path(env, (const char *) v->data.ptr, path))
				goto label_fail_memless;
			env->sb->source(env->sb, (const char *) v->data.ptr, path);
		}
		refer_free(p);
	}
	return env;
	label_fail:
	return NULL;
	label_fail_conflict:
	printf("kiya id name(%s) conflict [%s, %s]\n", (const char *) v->data.ptr, path, (const char *) vl->value);
	goto label_fail;
	label_fail_memless:
	printf("maybe memory less\n");
	goto label_fail;
}

static scan_env_s* scan_dirent(scan_env_s *restrict env, const char *restrict path)
{
	fsys_dir_s *restrict dir;
	fsys_dir_item_t item;
	dir = fsys_dir_alloc(path, fsys_type_file,
		fsys_dir_flag_looping |
		fsys_dir_flag_discard_same_iparent |
		fsys_dir_flag_link_type_parse);
	if (!dir) goto label_fail_opendir;
	goto label_entry;
	while (item.type)
	{
		if (!report_path(env, item.path))
			goto label_fail;
		label_entry:
		if (!fsys_dir_read(dir, &item))
			goto label_fail_scandir;
	}
	refer_free(dir);
	return env;
	label_fail:
	if (dir) refer_free(dir);
	return NULL;
	label_fail_opendir:
	printf("open dir(%s) fail\n", path);
	goto label_fail;
	label_fail_scandir:
	printf("scan dir(%s) fail\n", path);
	goto label_fail;
}

int main(int argc, const char *argv[])
{
	int ret = 1;
	args_t args;
	if (args_get(&args, argc, argv))
	{
		scan_env_s *restrict env;
		source_builder_s *restrict sb;
		if ((env = scan_env_alloc()))
		{
			if (args.o_builder_srcipt)
				sb = source_builder_alloc_script(args.output);
			else sb = source_builder_alloc_pocket(env->verify);
			if (sb)
			{
				scan_env_set_builder(env, sb);
				if (args.verify) sb->verify(sb, args.verify);
				if (args.version) sb->version(sb, args.version);
				if (args.author) sb->author(sb, args.author);
				if (args.time) sb->time(sb, args.time);
				if (args.description) sb->description(sb, args.description);
				if (args.flag) sb->flag(sb, args.flag);
				if (scan_dirent(env, args.input))
				{
					if (sb->save(sb, args.output)) ret = 0;
					else printf("save kiya source(%s) fail\n", args.output);
				}
				else printf("build kiya source fail\n");
				refer_free(sb);
			}
			refer_free(env);
		}
	}
	return ret;
}
