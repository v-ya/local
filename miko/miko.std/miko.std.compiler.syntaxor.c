#include "miko.std.compiler.h"

#define _alphabet_      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"
#define _digital_       "0123456789"
#define _operator_      "!#$%^&*+-./:<=>?@\\^`|~"
#define _strlist_(...)  (const char *const []) {__VA_ARGS__}

miko_std_syntaxor_s* miko_std_syntaxor_create_spec(mlog_s *restrict mlog)
{
	miko_std_syntaxor_s *restrict r;
	if ((r = miko_std_syntaxor_alloc(mlog)))
	{
		if (
			miko_std_syntaxor_add_tword(r,
				miko_std_syntax__name,
				_alphabet_ "_",
				_alphabet_ _digital_ "_-.",
				_alphabet_ _digital_ "_") &&
			miko_std_syntaxor_add_tword(r,
				miko_std_syntax__operator,
				_operator_,
				_operator_,
				_operator_) &&
			miko_std_syntaxor_add_tstring(r,
				miko_std_syntax__string, "\"",
				tparse_tstring_alloc_c_parse_multi_quotes) &&
			miko_std_syntaxor_add_tstring(r,
				miko_std_syntax__multichar, "\'",
				tparse_tstring_alloc_c_parse_single_quotes) &&
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
				"{", "}", "{}", 1, _strlist_(
					miko_std_syntax__name,
					miko_std_syntax__operator,
					miko_std_syntax__string,
					miko_std_syntax__multichar,
					miko_std_syntax__comma,
					miko_std_syntax__semicolon,
					miko_std_syntax__scope,
					miko_std_syntax__param,
					miko_std_syntax__index,
					NULL
				)) &&
			miko_std_syntaxor_set_scope(r,
				miko_std_syntax__param,
				"(", ")", "()", 1, _strlist_(
					miko_std_syntax__name,
					miko_std_syntax__operator,
					miko_std_syntax__string,
					miko_std_syntax__multichar,
					miko_std_syntax__comma,
					miko_std_syntax__semicolon,
					miko_std_syntax__scope,
					miko_std_syntax__param,
					miko_std_syntax__index,
					NULL
				)) &&
			miko_std_syntaxor_set_scope(r,
				miko_std_syntax__index,
				"[", "]", "[]", 1, _strlist_(
					miko_std_syntax__name,
					miko_std_syntax__operator,
					miko_std_syntax__string,
					miko_std_syntax__multichar,
					miko_std_syntax__comma,
					miko_std_syntax__semicolon,
					miko_std_syntax__scope,
					miko_std_syntax__param,
					miko_std_syntax__index,
					NULL
				)) &&
			miko_std_syntaxor_okay(r)
		) return r;
		refer_free(r);
	}
	return NULL;
}
