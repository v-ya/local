#include "miko.std.syntaxor.h"

#define _alphabet_      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"
#define _digital_       "0123456789"
#define _operator_      "!#$%^&*+-./:<=>?@\\^`|~"
#define _strlist_(...)  (const char *const []) {__VA_ARGS__}

// create

miko_std_syntaxor_s* miko_std_syntaxor_create_spec(mlog_s *restrict mlog, miko_std_syntaxor_spec_t *restrict index)
{
	const char *const *allow_syntax_name;
	miko_std_syntaxor_s *restrict r;
	if ((r = miko_std_syntaxor_alloc(mlog)))
	{
		allow_syntax_name = _strlist_(
			miko_std_syntax__name,
			miko_std_syntax__string,
			miko_std_syntax__multichar,
			miko_std_syntax__number,
			miko_std_syntax__operator,
			miko_std_syntax__comma,
			miko_std_syntax__semicolon,
			miko_std_syntax__scope,
			miko_std_syntax__param,
			miko_std_syntax__index,
			NULL
		);
		if (
			miko_std_syntaxor_add_tword(r,
				miko_std_syntax__name,
				_alphabet_ "_",
				_alphabet_ _digital_ "_-.",
				_alphabet_ _digital_ "_") &&
			miko_std_syntaxor_add_tstring(r,
				miko_std_syntax__string, "\"",
				tparse_tstring_alloc_c_parse_multi_quotes) &&
			miko_std_syntaxor_add_tstring(r,
				miko_std_syntax__multichar, "\'",
				tparse_tstring_alloc_c_parse_single_quotes) &&
			miko_std_syntaxor_add_tstring(r,
				miko_std_syntax__number,
				_digital_ "+-",
				miko_std_tstring_create_number) &&
			miko_std_syntaxor_add_tword(r,
				miko_std_syntax__operator,
				_operator_,
				_operator_,
				_operator_) &&
			miko_std_syntaxor_add_keyword(r,
				miko_std_syntax__comma,
				",", _strlist_(",", NULL)) &&
			miko_std_syntaxor_add_keyword(r,
				miko_std_syntax__semicolon,
				";", _strlist_(";", NULL)) &&
			miko_std_syntaxor_add_keyword(r,
				miko_std_syntax__scope,
				"{}", _strlist_("{", "}", NULL)) &&
			miko_std_syntaxor_add_keyword(r,
				miko_std_syntax__param,
				"()", _strlist_("(", ")", NULL)) &&
			miko_std_syntaxor_add_keyword(r,
				miko_std_syntax__index,
				"[]", _strlist_("[", "]", NULL)) &&
			miko_std_syntaxor_set_scope(r,
				miko_std_syntax__scope,
				"{", "}", "{}", 1, allow_syntax_name) &&
			miko_std_syntaxor_set_scope(r,
				miko_std_syntax__param,
				"(", ")", "()", 1, allow_syntax_name) &&
			miko_std_syntaxor_set_scope(r,
				miko_std_syntax__index,
				"[", "]", "[]", 1, allow_syntax_name) &&
			miko_std_syntaxor_okay(r)
		)
		{
			if (index)
			{
				index->name      = miko_std_syntaxor_name2index(r, miko_std_syntax__name);
				index->keyword   = miko_std_syntaxor_name2index(r, miko_std_syntax__keyword);
				index->string    = miko_std_syntaxor_name2index(r, miko_std_syntax__string);
				index->multichar = miko_std_syntaxor_name2index(r, miko_std_syntax__multichar);
				index->number    = miko_std_syntaxor_name2index(r, miko_std_syntax__number);
				index->operator  = miko_std_syntaxor_name2index(r, miko_std_syntax__operator);
				index->comma     = miko_std_syntaxor_name2index(r, miko_std_syntax__comma);
				index->semicolon = miko_std_syntaxor_name2index(r, miko_std_syntax__semicolon);
				index->scope     = miko_std_syntaxor_name2index(r, miko_std_syntax__scope);
				index->param     = miko_std_syntaxor_name2index(r, miko_std_syntax__param);
				index->index     = miko_std_syntaxor_name2index(r, miko_std_syntax__index);
			}
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

// marco

#include "miko.std.marco.h"
#include <string.h>

static miko_std_syntax_reader_s* miko_std_syntaxor_marco_spec_create_reader(miko_std_syntaxor_s *restrict syntaxor, const miko_std_syntaxor_spec_t *restrict index)
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

miko_std_syntax_s* miko_std_syntaxor_marco_spec(miko_std_syntaxor_s *restrict syntaxor, const miko_std_syntaxor_spec_t *restrict index, miko_std_syntax_s *restrict syntax)
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
		if ((reader = miko_std_syntaxor_marco_spec_create_reader(syntaxor, index)))
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
