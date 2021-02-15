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

static pocket_saver_index_t* script_cd(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict p)
{
	const char *path[2];
	const char *restrict s;
	json_inode_t *v;
	s = *p;
	v = NULL;
	path[1] = NULL;
	while (index && *s == '\"')
	{
		if (!(s = json_decode(*p = s, &v)))
		{
			label_fail:
			index = NULL;
			break;
		}
		path[0] = v->value.string;
		if (!pocket_saver_create_index(saver, index, path))
			goto label_fail;
		index = pocket_saver_cd(index, path);
		skip_space(s);
	}
	if (v) json_free(v);
	*p = s;
	return index;
}

static pocket_saver_index_t* script_working_change(pocket_saver_s *restrict saver, const char *restrict *restrict p)
{
	const char *restrict s;
	pocket_saver_index_t *restrict index;
	if (*(s = *p) == '[')
	{
		if (*++s == '$')
		{
			index = pocket_saver_root_system(saver);
			++s;
		}
		else index = pocket_saver_root_user(saver);
		skip_space(s);
		index = script_cd(saver, index, &s);
		if (index)
		{
			skip_space(s);
			if (*s == ']')
			{
				*p = s + 1;
				return index;
			}
		}
	}
	*p = s;
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

const char* script_build(script_t *restrict script, pocket_saver_s *restrict saver, const char *restrict s)
{
	pocket_saver_index_t *restrict working;
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
				while (*s && *s != '\n') ++s;
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
			default:
				goto label_fail;
		}
	}
	return NULL;
	label_fail:
	return s;
}
