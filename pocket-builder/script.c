#include "script.h"
#include "script_header.h"
#include "script_ptag.h"
#include "script_parse.h"
#include <stdlib.h>

script_t* script_alloc(void)
{
	script_t *restrict r;
	r = (script_t *) calloc(1, sizeof(script_t));
	if (r)
	{
		if (
			hashmap_init(&r->header) &&
			hashmap_init(&r->ptag) &&
			script_header_init(&r->header) &&
			script_ptag_init(&r->ptag) &&
			(r->buffer = buffer_alloc())
		) return r;
		script_free(r);
	}
	return NULL;
}

void script_free(script_t *restrict s)
{
	hashmap_uini(&s->header);
	hashmap_uini(&s->ptag);
	if (s->buffer) buffer_free(s->buffer);
	free(s);
}

typedef void* (*script_parse_stream_f)(void *restrict pri, const char *restrict name);

static void* script_string_stream(const char *restrict *restrict p, script_parse_stream_f func, void *restrict pri)
{
	const char *restrict s;
	json_inode_t *v;
	s = *p;
	v = NULL;
	while (*s == '\"')
	{
		if (!(s = json_decode(*p = s, &v)))
		{
			pri = NULL;
			break;
		}
		if (!(pri = func(pri, v->value.string)))
			break;
		skip_space(s);
	}
	if (v) json_free(v);
	*p = s;
	return pri;
}

static void* script_working_switch(const char *restrict *restrict p, script_parse_stream_f func, void *restrict user, void *restrict system)
{
	const char *restrict s;
	if (*(s = *p) == '[')
	{
		if (*++s == '$')
		{
			user = system;
			++s;
		}
		skip_space(s);
		if ((user = script_string_stream(&s, func, user)))
		{
			skip_space(s);
			if (*s == ']')
			{
				*p = s + 1;
				return user;
			}
		}
	}
	*p = s;
	return NULL;
}

typedef struct script_saver_cd_pri_t {
	pocket_saver_s *saver;
	pocket_saver_index_t *index;
	const char *path[2];
} script_saver_cd_pri_t;

static script_saver_cd_pri_t* script_saver_cd_func(script_saver_cd_pri_t *restrict pri, const char *restrict name)
{
	pocket_saver_index_t *restrict index;
	pri->path[0] = name;
	if ((index = pri->index) &&
		pocket_saver_create_index(pri->saver, index, pri->path) &&
		(pri->index = pocket_saver_cd(index, pri->path)))
		return pri;
	return NULL;
}

static pocket_saver_index_t* script_working_change(pocket_saver_s *restrict saver, const char *restrict *restrict p)
{
	script_saver_cd_pri_t *r;
	script_saver_cd_pri_t user, system;
	user.saver = system.saver = saver;
	user.path[1] = system.path[1] = NULL;
	user.index = pocket_saver_root_user(saver);
	system.index = pocket_saver_root_system(saver);
	if ((r = (script_saver_cd_pri_t *) script_working_switch(p, (script_parse_stream_f) script_saver_cd_func, &user, &system)))
		return r->index;
	return NULL;
}

static script_t* script_working_create(script_t *restrict script, pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict p)
{
	const char *path[2];
	script_t *r;
	const char *restrict s;
	json_inode_t *v, *tag;
	r = NULL;
	s = *p;
	v = tag = NULL;
	path[1] = NULL;
	while (index && *s == '\"')
	{
		if (!(s = json_decode(*p = s, &v)))
			break;
		path[0] = v->value.string;
		skip_space(s);
		if (*s == '@')
		{
			buffer_t *restrict buffer;
			uintptr_t align;
			buffer = NULL;
			align = 0;
			++s;
			if (*s == '\"')
			{
				if (!(s = json_decode(*p = s, &tag)))
					break;
				skip_space(s);
				if (*s == '=' && !(buffer = parse_value(script->buffer, &s, &align)))
					break;
				if (!pocket_saver_create_custom(saver, index, path, tag->value.string,
					buffer?buffer->data:NULL, buffer?buffer->used:0, align))
					break;
			}
			else
			{
				script_set_f func;
				if (!parse_key(script->buffer, &s))
					break;
				if (!(func = hashmap_get_name(&script->ptag, (const char *) script->buffer->data)))
					break;
				skip_space(s);
				if (*s == '=' && !(buffer = parse_value(script->buffer, &s, &align)))
					break;
				if (!func(saver, index, path, buffer, align))
					break;
			}
			r = script;
			break;
		}
		if (!pocket_saver_create_index(saver, index, path))
			break;
		index = pocket_saver_cd(index, path);
	}
	if (v) json_free(v);
	if (tag) json_free(tag);
	if (s) *p = s;
	return r;
}

const char* script_build(script_t *restrict script, pocket_saver_s *restrict saver, const char *restrict s, const char *restrict *restrict linker)
{
	pocket_saver_index_t *restrict working;
	*linker = NULL;
	working = pocket_saver_root_user(saver);
	if (!working) goto label_fail;
	while (*s)
	{
		switch (*s)
		{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				++s;
				continue;
			case '#':
				skip_comment(s);
				continue;
			case ':':
				if (!script_header_parse(script, saver, &s))
					goto label_fail;
				continue;
			case '[':
				working = script_working_change(saver, &s);
				if (!working) goto label_fail;
				continue;
			case '\"':
				if (!script_working_create(script, saver, working, &s))
					goto label_fail;
				continue;
			case '~':
				*linker = s + 1;
				return NULL;
			default:
				goto label_fail;
		}
	}
	return NULL;
	label_fail:
	return s;
}

typedef struct script_linker_cd_pri_t {
	pocket_s *pocket;
	const pocket_attr_t *index;
} script_linker_cd_pri_t;

static script_linker_cd_pri_t* script_linker_cd_func(script_linker_cd_pri_t *restrict pri, const char *restrict name)
{
	if (pri->index && (pri->index = pocket_find(pri->pocket, pri->index, name)))
		return pri;
	return NULL;
}

static const pocket_attr_t* script_linker_working_change(pocket_s *restrict pocket, const pocket_header_t *restrict header, const char *restrict *restrict p)
{
	script_linker_cd_pri_t *r;
	script_linker_cd_pri_t user, system;
	user.pocket = system.pocket = pocket;
	user.index = (const pocket_attr_t *) header->user.ptr;
	system.index = (const pocket_attr_t *) header->system.ptr;
	if ((r = (script_linker_cd_pri_t *) script_working_switch(p, (script_parse_stream_f) script_linker_cd_func, &user, &system)))
		return r->index;
	return NULL;
}

static pocket_s* script_link_update(pocket_s *restrict pocket, const pocket_attr_t *restrict from, const pocket_attr_t *restrict to, const char *restrict *restrict p)
{
	const char *restrict s;
	script_linker_cd_pri_t cd;
	uint64_t offset, size;
	pocket_tag_t tf, tt;
	s = *p;
	cd.pocket = pocket;
	// get from and check
	if (*s != '\"')
		goto label_fail;
	cd.index = from;
	if (!script_string_stream(&s, (script_parse_stream_f) script_linker_cd_func, &cd))
		goto label_fail;
	if (!(from = cd.index))
		goto label_fail;
	if ((tf = pocket_preset_tag(pocket, from)) <= pocket_tag$string || from->data.ptr || from->size)
		goto label_fail;
	skip_space(s);
	// get offset and size
	offset = size = 0;
	if (*s != '-')
		goto label_fail;
	++s;
	if (*s >= '0' && *s <= '9')
		offset = strtoull(s, (char **) &s, 0);
	if (*s == '+')
	{
		++s;
		if (*s >= '0' && *s <= '9')
			size = strtoull(s, (char **) &s, 0);
	}
	if (*s != '>')
		goto label_fail;
	++s;
	skip_space(s);
	// get to and check
	if (*s != '\"')
		goto label_fail;
	cd.index = to;
	if (!script_string_stream(&s, (script_parse_stream_f) script_linker_cd_func, &cd))
		goto label_fail;
	if (!(to = cd.index))
		goto label_fail;
	if ((tt = pocket_preset_tag(pocket, to)) <= pocket_tag$string || !to->data.ptr || !to->size)
		goto label_fail;
	if (offset >= to->size)
		goto label_fail;
	if (tf == pocket_tag$text && (tt != pocket_tag$text || size))
		goto label_fail;
	if (!size)
		size = to->size - offset;
	if (to->size - offset < size)
		goto label_fail;
	// link
	((pocket_attr_t *) from)->data.offset = to->data.offset + offset;
	((pocket_attr_t *) from)->size = size;
	*p = s;
	return pocket;
	label_fail:
	*p = s;
	return NULL;
}

const char* script_link(pocket_s *restrict pocket, const char *restrict s)
{
	const pocket_header_t *restrict header;
	const pocket_attr_t *from;
	const pocket_attr_t *to;
	const pocket_attr_t **restrict select;
	header = pocket_header(pocket);
	from = to = (const pocket_attr_t *) header->user.ptr;
	select = &from;
	while (*s)
	{
		switch (*s)
		{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				label_skip_char:
				++s;
				continue;
			case '#':
				skip_comment(s);
				continue;
			case '<':
				select = &from;
				goto label_skip_char;
			case '>':
				select = &to;
				goto label_skip_char;
			case '[':
				if (!(*select = script_linker_working_change(pocket, header, &s)))
					goto label_fail;
				continue;
			case '\"':
				if (!script_link_update(pocket, from, to, &s))
					goto label_fail;
				continue;
			default:
				goto label_fail;
		}
	}
	return NULL;
	label_fail:
	return s;
}
