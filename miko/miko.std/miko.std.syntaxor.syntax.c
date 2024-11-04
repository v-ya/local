#include "miko.std.syntaxor.h"

static void miko_std_syntax_free_func(miko_std_syntax_s *restrict r)
{
	refer_ck_free(r->id.name);
	refer_ck_free(r->data.syntax);
	refer_ck_free(r->data.scope);
	refer_ck_free(r->source.source);
}

static miko_std_syntax_s* miko_std_syntax_alloc(const miko_std_syntax_id_t *restrict id, const miko_std_syntax_source_t *restrict source)
{
	miko_std_syntax_s *restrict r;
	if ((r = (miko_std_syntax_s *) refer_alloz(sizeof(miko_std_syntax_s))))
	{
		refer_hook_free(r, syntax);
		if (id)
		{
			r->id.name = (refer_string_t) refer_save(id->name);
			r->id.index = id->index;
		}
		if (source)
		{
			r->source.source = (const miko_source_s *) refer_save(source->source);
			r->source.offset = source->offset;
			r->source.tail = source->tail;
		}
		return r;
	}
	return NULL;
}

miko_std_syntax_s* miko_std_syntax_create_syntax(const miko_std_syntax_id_t *restrict id, const miko_std_syntax_source_t *restrict source, const char *restrict syntax, uintptr_t length)
{
	miko_std_syntax_s *restrict r;
	if ((r = miko_std_syntax_alloc(id, source)))
	{
		if ((r->data.syntax = (refer_nstring_t) refer_dump_nstring_with_length(syntax, length)))
			return r;
		refer_free(r);
	}
	return NULL;
}

miko_std_syntax_s* miko_std_syntax_create_scope(const miko_std_syntax_id_t *restrict id, const miko_std_syntax_source_t *restrict source)
{
	miko_std_syntax_s *restrict r;
	if ((r = miko_std_syntax_alloc(id, source)))
	{
		if ((r->data.scope = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}
