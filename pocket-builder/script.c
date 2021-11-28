#include "script.h"
#include "script_header.h"
#include "script_ptag.h"
#include "script_parse.h"
#include <mlog.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

script_t* script_alloc(void)
{
	script_t *restrict r;
	r = (script_t *) calloc(1, sizeof(script_t));
	if (r)
	{
		if (
			hashmap_init(&r->header) &&
			hashmap_init(&r->ptag) &&
			hashmap_init(&r->pool) &&
			script_header_init(&r->header) &&
			script_ptag_init(&r->ptag) &&
			(r->buffer = exbuffer_alloc(0))
		) return r;
		script_free(r);
	}
	return NULL;
}

void script_free(script_t *restrict s)
{
	hashmap_uini(&s->header);
	hashmap_uini(&s->ptag);
	hashmap_uini(&s->pool);
	if (s->buffer) exbuffer_free(s->buffer);
	if (s->kiya && s->kiya_free)
		s->kiya_free(s->kiya);
	if (s->libkiya)
		dlclose(s->libkiya);
	free(s);
}

script_t* script_kiya_enable(script_t *restrict s, uintptr_t xmsize, struct pocket_verify_s *verify)
{
	kiya_t* (*kiya_alloc)(mlog_s *restrict mlog, uintptr_t xsize, uintptr_t asize);
	void (*kiya_set_verify)(kiya_t *restrict kiya, const struct pocket_verify_s *verify);
	mlog_s *restrict ml;
	ml = NULL;
	if (!s->libkiya && !s->kiya)
	{
		s->libkiya = dlopen("libkiya.so", RTLD_LOCAL | RTLD_NOW);
		if (s->libkiya)
		{
			kiya_alloc = dlsym(s->libkiya, "kiya_alloc");
			kiya_set_verify = dlsym(s->libkiya, "kiya_set_verify");
			s->kiya_free = dlsym(s->libkiya, "kiya_free");
			s->kiya_load_path = dlsym(s->libkiya, "kiya_load_path");
			s->kiya_symbol = dlsym(s->libkiya, "kiya_symbol");
			if (kiya_alloc && kiya_set_verify && s->kiya_free && s->kiya_load_path && s->kiya_symbol)
			{
				ml = mlog_alloc(0);
				if (ml)
				{
					mlog_set_report(ml, mlog_report_stdout_func, NULL);
					s->kiya = kiya_alloc(ml, xmsize, 0);
					refer_free(ml);
					ml = NULL;
					if (s->kiya)
					{
						kiya_set_verify(s->kiya, verify);
						return s;
					}
				}
			}
		}
	}
	if (ml) refer_free(ml);
	return NULL;
}

script_t* script_kiya_load(script_t *restrict s, const char *restrict kiya_pocket_path)
{
	if (s->kiya && s->kiya_load_path && kiya_pocket_path)
	{
		if (s->kiya_load_path(s->kiya, kiya_pocket_path))
			return s;
	}
	return NULL;
}

typedef const void* (*script_parse_stream_f)(const void *restrict inode, const void *restrict inst, const char *restrict name);

static const void* script_string_stream(const char *restrict *restrict p, script_parse_stream_f func, const void *restrict inst, const void *restrict inode)
{
	const char *restrict s;
	json_inode_t *v;
	s = *p;
	v = NULL;
	while (*s == '\"')
	{
		if (!(s = json_decode(*p = s, &v)))
		{
			inode = NULL;
			break;
		}
		if (!(inode = func(inode, inst, v->value.string)))
			break;
		skip_space(s);
	}
	if (v) json_free(v);
	*p = s;
	return inode;
}

typedef struct script_working_switch_t {
	hashmap_t *pool;
	exbuffer_t *key;
	const void *user;
	const void *system;
	const void *curr;
} script_working_switch_t;

static const void* script_working_switch(const char *restrict *restrict p, const script_working_switch_t *context, script_parse_stream_f func, const void *restrict inst)
{
	const char *restrict s, *restrict key;
	const void *inode;
	if (*(s = *p) == '[')
	{
		++s;
		if (*s == '$')
		{
			// system
			inode = context->system;
			++s;
		}
		else if (*s == '~')
		{
			// curr
			inode = context->curr;
			++s;
		}
		else if (!parse_is_key(s))
		{
			// user
			inode = context->user;
		}
		else
		{
			// key
			if (!context->pool || !context->key)
				goto label_fail;
			if (!(key = parse_key(context->key, &s)))
				goto label_fail;
			if (!(inode = hashmap_get_name(context->pool, key)))
				goto label_fail;
		}
		skip_space(s);
		if ((inode = script_string_stream(&s, func, inst, inode)))
		{
			skip_space(s);
			if (parse_is_key(s))
			{
				if (!context->pool || !context->key)
					goto label_fail;
				if (!(key = parse_key(context->key, &s)))
					goto label_fail;
				if (!hashmap_set_name(context->pool, key, inode, NULL))
					goto label_fail;
			}
			if (*s == ']')
			{
				*p = s + 1;
				return inode;
			}
		}
	}
	label_fail:
	*p = s;
	return NULL;
}

typedef struct script_saver_cd_pri_t {
	pocket_saver_s *saver;
	const char *path[2];
} script_saver_cd_pri_t;

static pocket_saver_index_t* script_saver_cd_func(pocket_saver_index_t *restrict index, script_saver_cd_pri_t *restrict pri, const char *restrict name)
{
	pri->path[0] = name;
	if (index && pocket_saver_create_index(pri->saver, index, pri->path))
		return pocket_saver_cd(index, pri->path);
	return NULL;
}

static pocket_saver_index_t* script_working_change(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict curr, hashmap_t *restrict pool, exbuffer_t *restrict cache, const char *restrict *restrict p)
{
	script_working_switch_t context;
	script_saver_cd_pri_t inst;
	inst.saver = saver;
	inst.path[1] = NULL;
	context.pool = pool;
	context.key = cache;
	context.user = pocket_saver_root_user(saver);
	context.system = pocket_saver_root_system(saver);
	context.curr = curr;
	return (pocket_saver_index_t *) script_working_switch(p, &context, (script_parse_stream_f) script_saver_cd_func, &inst);
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
			exbuffer_t *restrict buffer;
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

typedef struct script_working_kiya_call_arg_t {
	exbuffer_t data;
	exbuffer_t pointer;
	uintptr_t argc;
	const char **argv;
} script_working_kiya_call_arg_t;

static script_working_kiya_call_arg_t* script_working_kiya_call_get_arg_func(script_working_kiya_call_arg_t *restrict arg, void *restrict inst, const char *restrict name)
{
	uintptr_t size;
	uintptr_t pos;
	size = strlen(name) + 1;
	pos = arg->data.used;
	if (exbuffer_append(&arg->data, name, size) &&
		(arg->argv = exbuffer_append(&arg->pointer, &pos, sizeof(pos))))
	{
		arg->argc += 1;
		return arg;
	}
	return NULL;
}

static script_working_kiya_call_arg_t* script_working_kiya_call_get_arg(script_working_kiya_call_arg_t *restrict arg, const char *restrict *restrict p)
{
	uintptr_t data_start, i;
	const char *restrict s;
	if (*(s = *p) == '(')
	{
		++s;
		skip_space(s);
		if ((script_string_stream(&s, (script_parse_stream_f) script_working_kiya_call_get_arg_func, NULL, arg)))
		{
			skip_space(s);
			if (*s == ')')
			{
				*p = s + 1;
				data_start = (uintptr_t) arg->data.data;
				for (i = 0; i < arg->argc; ++i)
					arg->argv[i] += data_start;
				return arg;
			}
		}
	}
	*p = s;
	return NULL;
}

static pocket_saver_index_t* script_working_kiya_call(script_t *restrict script, pocket_saver_s *restrict saver, pocket_saver_index_t *restrict working, const char *restrict *restrict p)
{
	script_working_kiya_call_arg_t arg;
	pocket_builder_kiya_main_f kmain;
	pocket_saver_index_t *rw;
	rw = NULL;
	if (script->kiya && script->kiya_symbol)
	{
		if (!exbuffer_init(&arg.data, 0))
			goto label_fail_data;
		if (!exbuffer_init(&arg.pointer, 0))
			goto label_fail_pointer;
		arg.argc = 0;
		arg.argv = NULL;
		if (script_working_kiya_call_get_arg(&arg, p) && arg.argc && arg.argv)
		{
			kmain = (pocket_builder_kiya_main_f) script->kiya_symbol(script->kiya, NULL, arg.argv[0]);
			if (kmain)
				rw = kmain(saver, working, arg.argc, arg.argv);
		}
		exbuffer_uini(&arg.pointer);
		label_fail_pointer:
		exbuffer_uini(&arg.data);
		label_fail_data:
		;
	}
	return rw;
}

pocket_builder_kiya_main_f script_kiya_symbol(script_t *restrict s, const char *restrict symbol)
{
	if (s->kiya && s->kiya_symbol && symbol)
		return (pocket_builder_kiya_main_f) s->kiya_symbol(s->kiya, NULL, symbol);
	return NULL;
}

const char* script_build(script_t *restrict script, pocket_saver_s *restrict saver, const char *restrict s, const char *restrict *restrict linker)
{
	pocket_saver_index_t *restrict working;
	*linker = NULL;
	hashmap_clear(&script->pool);
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
				working = script_working_change(saver, working, &script->pool, script->buffer, &s);
				if (!working) goto label_fail;
				continue;
			case '\"':
				if (!script_working_create(script, saver, working, &s))
					goto label_fail;
				continue;
			case '(':
				working = script_working_kiya_call(script, saver, working, &s);
				if (!working) goto label_fail;
				continue;
			case '~':
				*linker = s + 1;
				return NULL;
			default:
				goto label_fail;
		}
	}
	hashmap_clear(&script->pool);
	return NULL;
	label_fail:
	hashmap_clear(&script->pool);
	return s;
}

static const pocket_attr_t* script_linker_cd_func(const pocket_attr_t *restrict inode, pocket_s *restrict inst, const char *restrict name)
{
	if (inode)
		return pocket_find(inst, inode, name);
	return NULL;
}

static const pocket_attr_t* script_linker_working_change(pocket_s *restrict pocket, const pocket_attr_t *restrict curr, hashmap_t *restrict pool, exbuffer_t *restrict cache, const char *restrict *restrict p)
{
	script_working_switch_t context;
	context.pool = pool;
	context.key = cache;
	context.user = pocket_user(pocket);
	context.system = pocket_system(pocket);
	context.curr = curr;
	return script_working_switch(p, &context, (script_parse_stream_f) script_linker_cd_func, pocket);
}

static pocket_s* script_link_update(pocket_s *restrict pocket, const pocket_attr_t *restrict from, const pocket_attr_t *restrict to, const char *restrict *restrict p)
{
	const char *restrict s;
	uint64_t offset, size;
	pocket_tag_t tf, tt;
	s = *p;
	// get from and check
	if (*s != '\"')
		goto label_fail;
	if (!(from = script_string_stream(&s, (script_parse_stream_f) script_linker_cd_func, pocket, from)))
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
	if (!(to = script_string_stream(&s, (script_parse_stream_f) script_linker_cd_func, pocket, to)))
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

const char* script_link(script_t *restrict script, pocket_s *restrict pocket, const char *restrict s)
{
	const pocket_attr_t *from;
	const pocket_attr_t *to;
	const pocket_attr_t **restrict select;
	hashmap_clear(&script->pool);
	from = to = pocket_user(pocket);
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
				if (!(*select = script_linker_working_change(pocket, *select, &script->pool, script->buffer, &s)))
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
	hashmap_clear(&script->pool);
	return NULL;
	label_fail:
	hashmap_clear(&script->pool);
	return s;
}
