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

static miko_std_syntax_reader_s* miko_std_compiler_syntaxor_marco_spec_create_reader(miko_std_syntaxor_s *restrict syntaxor, const miko_std_compiler_spec_index_t *restrict index)
{
	// | -> [slot:0] name -> [slot:1] (ignore) param -> [slot:-] "=" -> [slot:2] (finish, scope) scope -> |
	miko_std_syntax_reader_s *restrict reader;
	if ((reader = miko_std_syntax_reader_alloc(syntaxor, 3)))
	{
		if (miko_std_syntax_reader_new_group(reader) &&
			miko_std_syntax_reader_add_inode_by_nstring(reader, index->name, NULL, 0) &&
			miko_std_syntax_reader_add_inode_by_nstring(reader, index->param, NULL, 1) &&
			miko_std_syntax_reader_enable_inode_allow_ignore(reader) &&
			miko_std_syntax_reader_add_inode_by_cstring(reader, index->operator, "=", ~(miko_offset_t) 0) &&
			miko_std_syntax_reader_add_inode_by_cstring(reader, index->scope, "{}", 2) &&
			miko_std_syntax_reader_enable_inode_must_finish(reader) &&
			miko_std_syntax_reader_enable_inode_replace_scope(reader) &&
			miko_std_syntax_reader_finish(reader))
			return reader;
		refer_free(reader);
	}
	return NULL;
}

miko_std_syntax_s* miko_std_compiler_syntaxor_marco_spec(miko_std_syntaxor_s *restrict syntaxor, const miko_std_compiler_spec_index_t *restrict index, miko_std_syntax_s *restrict syntax)
{
	miko_std_syntax_reader_s *restrict reader;
	miko_std_marco_pool_s *restrict mpool;
	miko_std_syntax_s *restrict key;
	vattr_vlist_t *restrict vlist;
	vattr_vlist_t *restrict vstop;
	miko_std_marco_index_t midx;
	miko_std_syntax_s *marco_slot[3];
	if (index && syntax && syntax->data.scope)
	{
		if ((reader = miko_std_compiler_syntaxor_marco_spec_create_reader(syntaxor, index)))
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
						if (!miko_std_syntax_reader_fetch(reader, marco_slot, 3, vlist->vattr_next, vstop, NULL))
							goto label_fail;
						if (!miko_std_marco_pool_add(mpool, marco_slot[0], marco_slot[1], marco_slot[2]->data.scope))
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
				refer_free(reader);
				return syntax;
				label_fail:
				miko_std_marco_print(syntaxor->mlog, key->source.source, key->source.offset);
				refer_free(mpool);
			}
			refer_free(reader);
		}
	}
	return NULL;
}
