#ifndef _miko_std_type_syntaxor_h_
#define _miko_std_type_syntaxor_h_

#include <vattr.h>
#include <tparse/tword.h>
#include <tparse/tstack.h>
#include <tparse/tstring.h>
#include <tparse/tmapping.h>
#include "miko.type.base.h"
#include "miko.type.source.h"

typedef const char *miko_std_syntaxor_chars_t;
typedef struct miko_std_syntax_source_t miko_std_syntax_source_t;
typedef struct miko_std_syntax_data_t miko_std_syntax_data_t;
typedef struct miko_std_syntax_id_t miko_std_syntax_id_t;
typedef struct miko_std_syntaxor_s miko_std_syntaxor_s;
typedef struct miko_std_syntax_s miko_std_syntax_s;
typedef struct miko_std_syntax_reader_s miko_std_syntax_reader_s;

struct miko_std_syntax_source_t {
	const miko_source_s *source;  // source code (offset <= syntax < tail)
	miko_offset_t offset;         // this syntax start at source
	miko_offset_t tail;           // this syntax stop at source
};

struct miko_std_syntax_data_t {
	refer_nstring_t syntax;  // original data
	refer_nstring_t value;   // analysis data
	vattr_s *scope;          // (maybe null) => (miko_std_syntax_s *)
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
