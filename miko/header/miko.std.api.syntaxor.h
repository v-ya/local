#ifndef _miko_std_api_syntaxor_h_
#define _miko_std_api_syntaxor_h_

#include <mlog.h>
#include "miko.std.type.syntaxor.h"

miko_std_syntaxor_s* miko_std_syntaxor_alloc(mlog_s *restrict mlog);
miko_index_t miko_std_syntaxor_add_keyword(miko_std_syntaxor_s *restrict r, const char *restrict id_name, miko_std_syntaxor_chars_t head, const char *const restrict keyword[]);
miko_std_syntaxor_s* miko_std_syntaxor_okay(miko_std_syntaxor_s *restrict r);
void miko_std_syntaxor_print(miko_std_syntaxor_s *restrict r, const miko_source_s *restrict source, uintptr_t pos);

miko_std_syntax_s* miko_std_syntaxor_create(miko_std_syntaxor_s *restrict r, const miko_source_s *restrict source);

#endif
