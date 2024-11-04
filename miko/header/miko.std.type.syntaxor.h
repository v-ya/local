#ifndef _miko_std_type_syntaxor_h_
#define _miko_std_type_syntaxor_h_

#include <vattr.h>
#include "miko.type.base.h"
#include "miko.type.source.h"

typedef struct miko_std_syntax_source_t miko_std_syntax_source_t;
typedef struct miko_std_syntax_data_t miko_std_syntax_data_t;
typedef struct miko_std_syntax_id_t miko_std_syntax_id_t;
typedef struct miko_std_syntaxor_s miko_std_syntaxor_s;
typedef struct miko_std_syntax_s miko_std_syntax_s;

struct miko_std_syntax_source_t {
	miko_source_s *source;  // source code (offset <= syntax < tail)
	miko_offset_t offset;   // this syntax start at source
	miko_offset_t tail;     // this syntax stop at source
};

struct miko_std_syntax_data_t {
	refer_nstring_t syntax;  // only used one
	vattr_s *scope;          // only used one
};

struct miko_std_syntax_id_t {
	refer_string_t name;
	miko_index_t index;
};

struct miko_std_syntax_s {
	miko_std_syntax_id_t id;
	miko_std_syntax_data_t data;
	miko_std_syntax_source_t source;
};

#endif
