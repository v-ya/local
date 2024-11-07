#include "miko.std.compiler.h"

#define _alphabet_      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"
#define _digital_       "0123456789"
#define _operator_      "!#$%^&*+-./:<=>?@\\^`|~"
#define _strlist_(...)  (const char *const []) {__VA_ARGS__}

miko_std_syntaxor_s* miko_std_compiler_create_syntaxor_spec(mlog_s *restrict mlog, miko_std_compiler_spec_index_t *restrict index)
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
