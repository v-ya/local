#define _DEFAULT_SOURCE
#include "entry_args.h"
#include "builder.h"
#include <pocket/pocket.h>
#include <pocket/pocket-verify.h>
#include <exbuffer.h>
#include <hashmap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

typedef struct scan_env_s scan_env_s;

typedef void (*report_path_f)(scan_env_s *restrict env, const char *restrict path);

struct scan_env_s {
	pocket_verify_s *verify;
	source_builder_s *sb;
	exbuffer_t path;
	hashmap_t name;
	report_path_f report;
	uintptr_t running;
	uintptr_t depth;
	uintptr_t max_depth;
};

static __attribute__ ((noinline)) exbuffer_t* set_name(exbuffer_t *restrict path, uintptr_t pos, const char *restrict name)
{
	uintptr_t n;
	n = strlen(name) + 1;
	if (exbuffer_need(path, pos + n))
	{
		memcpy(path->data + pos, name, n);
		return path;
	}
	return NULL;
}

static __attribute__ ((noinline)) exbuffer_t* set_dir_suffix(exbuffer_t *restrict path, uintptr_t *restrict pos)
{
	uintptr_t p = *pos;
	uint8_t *restrict d = path->data;
	if (!p) goto label_okay;
	while (p && d[p - 1] == '/')
		--p;
	if ((d = exbuffer_need(path, p + 2)))
	{
		d[p++] = '/';
		d[p] = 0;
		*pos = p;
		label_okay:
		return path;
	}
	return NULL;
}

static __attribute__ ((noinline)) exbuffer_t* set_back(exbuffer_t *restrict path, uintptr_t pos)
{
	if (exbuffer_need(path, pos + 1))
	{
		path->data[pos] = 0;
		return path;
	}
	return NULL;
}

static __attribute__ ((noinline)) unsigned char get_link_type(const char *restrict path)
{
	struct stat st;
	if (!stat(path, &st))
		return IFTODT(st.st_mode);
	return 0;
}

void scan_dirent(scan_env_s *restrict env, uintptr_t pos)
{
	DIR *restrict dir;
	struct dirent *restrict item;
	if (env->depth >= env->max_depth)
		goto label_return;
	if (!(dir = opendir((const char *) env->path.data)))
		goto label_fail_opendir;
	pos += strlen((const char *) env->path.data + pos);
	if (!set_dir_suffix(&env->path, &pos))
		goto label_fail_memless;
	while (env->running && (item = readdir(dir)))
	{
		if (item->d_name[0] != '.')
		{
			if (!set_name(&env->path, pos, item->d_name))
				goto label_fail_memless;
			if (item->d_type == DT_REG)
			{
				label_file:
				// file
				env->report(env, (const char *) env->path.data);
			}
			else if (item->d_type == DT_DIR)
			{
				label_dir:
				// dir
				env->depth += 1;
				scan_dirent(env, pos);
				env->depth -= 1;
				set_back(&env->path, pos);
			}
			else if (item->d_type == DT_LNK)
			{
				// link
				switch (get_link_type((const char *) env->path.data))
				{
					case DT_REG:
						goto label_file;
					case DT_DIR:
						goto label_dir;
					default:
						break;
				}
			}
		}
	}
	closedir(dir);
	label_return:
	return ;
	label_fail_opendir:
	printf("open dir(%s) fail\n", (const char *) env->path.data);
	env->running = 0;
	goto label_return;
	label_fail_memless:
	printf("maybe memory less\n");
	env->running = 0;
	goto label_return;
}

static void env_name_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		free(vl->value);
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

static void report_path(scan_env_s *restrict env, const char *restrict path)
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
	label_return:
	return ;
	label_fail_conflict:
	printf("kiya id name(%s) conflict [%s, %s]\n", (const char *) v->data.ptr, path, (const char *) vl->value);
	env->running = 0;
	goto label_return;
	label_fail_memless:
	printf("maybe memory less\n");
	env->running = 0;
	goto label_return;
}

static void scan_env_free_func(scan_env_s *restrict r)
{
	if (r->verify)
		refer_free(r->verify);
	if (r->sb)
		refer_free(r->sb);
	exbuffer_uini(&r->path);
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
			exbuffer_init(&r->path, 0) &&
			hashmap_init(&r->name))
		{
			r->report = report_path;
			r->running = 1;
			r->max_depth = 16;
			return r;
		}
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
				if (set_name(&env->path, 0, args.input))
				{
					if (scan_dirent(env, 0), env->running)
					{
						if (sb->save(sb, args.output)) ret = 0;
						else printf("save kiya source(%s) fail\n", args.output);
					}
					else printf("build kiya source fail\n");
				}
				refer_free(sb);
			}
			refer_free(env);
		}
	}
	return ret;
}
