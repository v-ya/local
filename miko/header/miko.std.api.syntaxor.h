#ifndef _miko_std_api_syntaxor_h_
#define _miko_std_api_syntaxor_h_

#include <mlog.h>
#include "miko.std.type.syntaxor.h"

tparse_tstring_s* miko_std_tstring_create_number(void);

miko_std_syntaxor_s* miko_std_syntaxor_alloc(mlog_s *restrict mlog);
miko_index_t miko_std_syntaxor_name2index(const miko_std_syntaxor_s *restrict r, const char *restrict id_name);
miko_index_t miko_std_syntaxor_add_none(miko_std_syntaxor_s *restrict r, const char *restrict id_name);
miko_index_t miko_std_syntaxor_add_keyword(miko_std_syntaxor_s *restrict r, const char *restrict id_name, miko_std_syntaxor_chars_t head, const char *const restrict keyword[]);
miko_index_t miko_std_syntaxor_add_tword(miko_std_syntaxor_s *restrict r, const char *restrict id_name, miko_std_syntaxor_chars_t head, miko_std_syntaxor_chars_t inner, miko_std_syntaxor_chars_t tail);
miko_index_t miko_std_syntaxor_add_tstring(miko_std_syntaxor_s *restrict r, const char *restrict id_name, miko_std_syntaxor_chars_t head, tparse_tstring_s* (*tstring_create_func)(void));
miko_std_syntaxor_s* miko_std_syntaxor_set_scope(miko_std_syntaxor_s *restrict r, const char *restrict id_name, const char *restrict syntax_start, const char *restrict syntax_stop, const char *restrict syntax_scope, uintptr_t must_match, const char *const restrict allow_syntax_name[]);
miko_std_syntaxor_s* miko_std_syntaxor_okay(miko_std_syntaxor_s *restrict r);

miko_std_syntax_s* miko_std_syntaxor_create(miko_std_syntaxor_s *restrict r, const miko_source_s *restrict source);

#endif
