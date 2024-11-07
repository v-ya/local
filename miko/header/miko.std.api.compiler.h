#ifndef _miko_std_api_compiler_h_
#define _miko_std_api_compiler_h_

#include "miko.type.source.h"
#include "miko.std.type.compiler.h"
#include "miko.std.type.syntaxor.h"

const miko_source_s* miko_std_compiler_create_default_spec(void);

miko_std_syntaxor_s* miko_std_syntaxor_create_spec(mlog_s *restrict mlog);

#endif
