#include "miko.std.compiler.h"
#include "miko.std.syntaxor.h"
#include "miko.std.marco.h"
#include "miko.std.spec.h"
#include <string.h>

const miko_source_s* miko_std_compiler_create_default_spec(void)
{
	miko_source_s *restrict source;
	refer_nstring_t spec;
	source = NULL;
	if ((spec = refer_dump_nstring_with_length(_miko_std_spec_data_, (uintptr_t) _miko_std_spec_size_)))
	{
		source = miko_source_alloc("miko.std.spec", spec);
		refer_free(spec);
	}
	return source;
}

static miko_std_marco_pool_s* miko_std_compiler_syntaxor_marco_spec_add(miko_std_marco_pool_s *restrict mpool, vattr_vlist_t *restrict vlist, vattr_vlist_t *restrict vstop, const miko_std_compiler_spec_index_t *restrict index)
{
	miko_std_syntax_s *restrict name;
	miko_std_syntax_s *restrict param;
	miko_std_syntax_s *restrict assign;
	miko_std_syntax_s *restrict scope;
	miko_std_marco_scope_s *restrict marco;
	if (!vlist || vlist == vstop)
		goto label_fail;
	name = (miko_std_syntax_s *) vlist->value;
	vlist = vlist->vattr_next;
	if (!vlist || vlist == vstop)
		goto label_fail;
	param = (miko_std_syntax_s *) vlist->value;
	if (param->id.index == mpool->index.param)
	{
		vlist = vlist->vattr_next;
		if (!vlist || vlist == vstop)
			goto label_fail;
	}
	else param = NULL;
	assign = (miko_std_syntax_s *) vlist->value;
	if (assign->id.index != index->operator || !assign->data.syntax ||
		strcmp(assign->data.syntax->string, "="))
		goto label_fail;
	vlist = vlist->vattr_next;
	if (!vlist || vlist == vstop)
		goto label_fail;
	scope = (miko_std_syntax_s *) vlist->value;
	if (scope->id.index == index->scope && vlist->vattr_next == vstop)
		marco = (miko_std_marco_scope_s *) refer_save(scope->data.scope);
	else
	{
		if (!(marco = vattr_alloc()))
			goto label_fail;
		if (!miko_std_syntax_scope_append_vlist(marco, vlist, vstop))
			goto label_fail_marco;
	}
	mpool = miko_std_marco_pool_add(mpool, name, param, marco);
	refer_free(marco);
	return mpool;
	label_fail_marco:
	refer_free(marco);
	label_fail:
	return NULL;
}

miko_std_syntax_s* miko_std_compiler_syntaxor_marco_spec(miko_std_syntaxor_s *restrict syntaxor, const miko_std_compiler_spec_index_t *restrict index, miko_std_syntax_s *restrict syntax)
{
	miko_std_marco_pool_s *restrict mpool;
	miko_std_syntax_s *restrict key;
	vattr_vlist_t *restrict vlist;
	vattr_vlist_t *restrict vstop;
	miko_std_marco_index_t midx;
	if (index && syntax && syntax->data.scope)
	{
		midx.name = index->name;
		midx.comma = index->comma;
		midx.param = index->param;
		if ((mpool = miko_std_marco_pool_alloc(&midx)))
		{
			for (vlist = syntax->data.scope->vattr; vlist; vlist = vstop)
			{
				vstop = miko_std_syntax_scope_find_id(vlist, NULL, index->semicolon);
				key = (miko_std_syntax_s *) vlist->value;
				if (key->id.index == index->name && key->data.syntax &&
					!strcmp(key->data.syntax->string, "marco.def"))
				{
					if (!miko_std_compiler_syntaxor_marco_spec_add(mpool, vlist->vattr_next, vstop, index))
						goto label_fail;
					if (vstop) vstop = vstop->vattr_next;
					miko_std_syntax_scope_delete_vlist(vlist, vstop);
					continue;
				}
				else if (!miko_std_marco_pool_replace_vlist(mpool, vlist, vstop))
					goto label_fail;
				if (vstop) vstop = vstop->vattr_next;
			}
			refer_free(mpool);
			return syntax;
			label_fail:
			miko_std_marco_print(syntaxor->mlog, key->source.source, key->source.offset);
			refer_free(mpool);
		}
	}
	return NULL;
}
