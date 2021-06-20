#include "builder.h"
#include <pocket/pocket-saver.h>
#include <pocket/pocket-verify.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct source_builder_pocket_s {
	source_builder_s builder;
	pocket_verify_s *verify;
	pocket_saver_s *saver;
	pocket_saver_index_t *root;
} source_builder_pocket_s;

static char* load_text(const char *restrict path)
{
	FILE *restrict fp;
	char *restrict r;
	size_t n;
	r = NULL;
	fp = fopen(path, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		if ((n = ftell(fp)))
		{
			r = (char *) malloc(n + 1);
			if (r)
			{
				fseek(fp, 0, SEEK_SET);
				if (fread(r, 1, n, fp) == n)
					r[n] = 0;
				else
				{
					free(r);
					r = NULL;
				}
			}
		}
		fclose(fp);
	}
	return r;
}

static source_builder_s* set_verify(source_builder_pocket_s *restrict r, const char *restrict attr)
{
	if (pocket_saver_set_verify(r->saver, attr))
		return &r->builder;
	return NULL;
}

static source_builder_s* set_version(source_builder_pocket_s *restrict r, const char *restrict attr)
{
	if (pocket_saver_set_version(r->saver, attr))
		return &r->builder;
	return NULL;
}

static source_builder_s* set_author(source_builder_pocket_s *restrict r, const char *restrict attr)
{
	if (pocket_saver_set_author(r->saver, attr))
		return &r->builder;
	return NULL;
}

static source_builder_s* set_time(source_builder_pocket_s *restrict r, const char *restrict attr)
{
	if (pocket_saver_set_time(r->saver, attr))
		return &r->builder;
	return NULL;
}

static source_builder_s* set_description(source_builder_pocket_s *restrict r, const char *restrict attr)
{
	char *restrict text;
	if ((text = load_text(attr)))
	{
		if (pocket_saver_create_text(
				r->saver,
				pocket_saver_root_system(r->saver),
				(const char *[]) {"description", NULL},
				text,
				0) &&
			pocket_saver_set_description(r->saver, "description"))
		{
			free(text);
			return &r->builder;
		}
		free(text);
	}
	return NULL;
}

static source_builder_s* set_flag(source_builder_pocket_s *restrict r, const char *restrict attr)
{
	if (pocket_saver_set_flag(r->saver, attr))
		return &r->builder;
	return NULL;
}

static source_builder_s* set_source(source_builder_pocket_s *restrict r, const char *restrict name, const char *restrict path)
{
	if (pocket_saver_create_string(
			r->saver,
			r->root,
			(const char *[]) {name, NULL},
			path))
		return &r->builder;
	return NULL;
}

static source_builder_s* save(source_builder_pocket_s *restrict r, const char *restrict path)
{
	if (pocket_saver_save(r->saver, path, r->verify))
		return &r->builder;
	return NULL;
}

static void source_builder_pocket_free_func(source_builder_pocket_s *restrict r)
{
	if (r->verify)
		refer_free(r->verify);
	if (r->saver)
		refer_free(r->saver);
}

static pocket_saver_s* saver_set_time(pocket_saver_s *restrict saver)
{
	char st[64];
	time_t this;
	struct tm tm;
	time(&this);
	localtime_r(&this, &tm);
	sprintf(st, "%04d-%02d-%02d %02d:%02d:%02d",
		tm.tm_year + 1900,
		tm.tm_mon + 1,
		tm.tm_mday,
		tm.tm_hour,
		tm.tm_min,
		tm.tm_sec);
	return pocket_saver_set_time(saver, st);
}

source_builder_s* source_builder_alloc_pocket(struct pocket_verify_s *restrict verify)
{
	source_builder_pocket_s *restrict r;
	r = (source_builder_pocket_s *) refer_alloz(sizeof(source_builder_pocket_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) source_builder_pocket_free_func);
		r->verify = (pocket_verify_s *) refer_save(verify);
		if ((r->saver = pocket_saver_alloc()) &&
			(r->root = pocket_saver_root_user(r->saver)) &&
			pocket_saver_set_tag(r->saver, "source") &&
			saver_set_time(r->saver))
		{
			r->builder.verify = (source_builder_set_attr_f) set_verify;
			r->builder.version = (source_builder_set_attr_f) set_version;
			r->builder.author = (source_builder_set_attr_f) set_author;
			r->builder.time = (source_builder_set_attr_f) set_time;
			r->builder.description = (source_builder_set_attr_f) set_description;
			r->builder.flag = (source_builder_set_attr_f) set_flag;
			r->builder.source = (source_builder_set_source_f) set_source;
			r->builder.save = (source_builder_save_f) save;
			return &r->builder;
		}
		refer_free(r);
	}
	return NULL;
}
