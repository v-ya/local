#ifndef _miko_std_api_compiler_h_
#define _miko_std_api_compiler_h_

#include "miko.type.source.h"
#include "miko.std.type.compiler.h"
#include "miko.std.type.syntaxor.h"

const miko_source_s* miko_std_compiler_create_default_spec(void);

miko_std_syntaxor_s* miko_std_compiler_create_syntaxor_spec(mlog_s *restrict mlog, miko_std_compiler_spec_index_t *restrict index);
miko_std_syntax_s* miko_std_compiler_syntaxor_marco_spec(miko_std_syntaxor_s *restrict syntaxor, const miko_std_compiler_spec_index_t *restrict index, miko_std_syntax_s *restrict syntax);

#endif
