#include "builder.h"
#include <exbuffer.h>
#include <json.h>
#include <stdio.h>

typedef struct source_builder_script_s {
	source_builder_s builder;
	exbuffer_t buffer;
	FILE *need_free;
	FILE *fp;
	uintptr_t started;
} source_builder_script_s;

static exbuffer_t* buffer_set_string(exbuffer_t *restrict eb, const char *restrict s)
{
	json_inode_t *restrict js;
	uint64_t length;
	js = json_create_string(s);
	if (js)
	{
		length = json_length(js) + 1;
		if (exbuffer_need(eb, (uintptr_t) length))
		{
			if (json_encode(js, (char *) eb->data))
			{
				json_free(js);
				return eb;
			}
		}
		json_free(js);
	}
	return NULL;
}

static source_builder_s* set_verify(source_builder_script_s *restrict r, const char *restrict attr)
{
	if (buffer_set_string(&r->buffer, attr))
	{
		fprintf(r->fp, ": verify   = %s\n", (const char *) r->buffer.data);
		return &r->builder;
	}
	return NULL;
}

static source_builder_s* set_version(source_builder_script_s *restrict r, const char *restrict attr)
{
	if (buffer_set_string(&r->buffer, attr))
	{
		fprintf(r->fp, ": version  = %s\n", (const char *) r->buffer.data);
		return &r->builder;
	}
	return NULL;
}

static source_builder_s* set_author(source_builder_script_s *restrict r, const char *restrict attr)
{
	if (buffer_set_string(&r->buffer, attr))
	{
		fprintf(r->fp, ": author   = %s\n", (const char *) r->buffer.data);
		return &r->builder;
	}
	return NULL;
}

static source_builder_s* set_time(source_builder_script_s *restrict r, const char *restrict attr)
{
	if (buffer_set_string(&r->buffer, attr))
	{
		fprintf(r->fp, ": time     = %s\n", (const char *) r->buffer.data);
		return &r->builder;
	}
	return NULL;
}

static source_builder_s* set_description(source_builder_script_s *restrict r, const char *restrict attr)
{
	if (buffer_set_string(&r->buffer, attr))
	{
		fprintf(r->fp, "[$] \"description\" @text => @%s\n", (const char *) r->buffer.data);
		fprintf(r->fp, ": desc     = \"description\"\n");
		return &r->builder;
	}
	return NULL;
}

static source_builder_s* set_flag(source_builder_script_s *restrict r, const char *restrict attr)
{
	if (buffer_set_string(&r->buffer, attr))
	{
		fprintf(r->fp, ": flag     = %s\n", (const char *) r->buffer.data);
		return &r->builder;
	}
	return NULL;
}

static source_builder_s* set_source(source_builder_script_s *restrict r, const char *restrict name, const char *restrict path)
{
	if (!r->started)
	{
		fprintf(r->fp, "\n[]\n");
		r->started = 1;
	}
	if (buffer_set_string(&r->buffer, name))
	{
		fprintf(r->fp, "\t%s @string", (const char *) r->buffer.data);
		if (buffer_set_string(&r->buffer, path))
		{
			fprintf(r->fp, "=> %s\n", (const char *) r->buffer.data);
			return &r->builder;
		}
	}
	return NULL;
}

static source_builder_s* save(source_builder_script_s *restrict r, const char *restrict path)
{
	return &r->builder;
}

static void source_builder_script_free_func(source_builder_script_s *restrict r)
{
	if (r->need_free)
		fclose(r->need_free);
	exbuffer_uini(&r->buffer);
}

source_builder_s* source_builder_alloc_script(const char *restrict output)
{
	source_builder_script_s *restrict r;
	r = (source_builder_script_s *) refer_alloz(sizeof(source_builder_script_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) source_builder_script_free_func);
		if (exbuffer_init(&r->buffer, 0))
		{
			if (output)
				r->fp = r->need_free = fopen(output, "wb");
			else r->fp = stdout;
			if (r->fp)
			{
				fprintf(r->fp, ": tag      = \"source\"\n");
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
		}
		refer_free(r);
	}
	return NULL;
}
