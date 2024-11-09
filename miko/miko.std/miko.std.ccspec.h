#ifndef _miko_std_miko_std_ccspec_h_
#define _miko_std_miko_std_ccspec_h_

#include "miko.std.h"
#include <vattr.h>

typedef enum miko_std_ccspec_major_t miko_std_ccspec_major_t;
typedef struct miko_std_ccspec_reader_s miko_std_ccspec_reader_s;
typedef struct miko_std_ccspec_major_s miko_std_ccspec_major_s;
typedef struct miko_std_ccspec_type_s miko_std_ccspec_type_s;
typedef struct miko_std_ccspec_op_s miko_std_ccspec_op_s;
typedef struct miko_std_ccspec_s miko_std_ccspec_s;

typedef miko_std_ccspec_reader_s* (*miko_std_ccspec_reader_create_reader_f)(void);
typedef refer_t (*miko_std_ccspec_reader_create_context_f)(miko_std_ccspec_s *restrict r);
typedef miko_std_ccspec_s* (*miko_std_ccspec_reader_call_f)(miko_std_ccspec_s *restrict r, refer_t context, const miko_std_syntax_s *restrict *restrict syntax_param);

enum miko_std_ccspec_major_t {
	miko_std_ccspec_major__miko,
	miko_std_ccspec_major__fake
};

struct miko_std_ccspec_reader_s {
	refer_string_t name;
	miko_std_syntax_reader_s *reader;
	miko_count_t slot_count;
	miko_std_ccspec_reader_call_f call;
	refer_t context;
};

struct miko_std_ccspec_major_s {
	refer_string_t major;
	miko_std_ccspec_major_t ctype;
	miko_major_vtype_t vtype;
};

struct miko_std_ccspec_type_s {
	refer_string_t major;
	refer_string_t minor;
	miko_std_ccspec_major_t ctype;
	miko_major_vtype_t vtype;
};

struct miko_std_ccspec_s {
	miko_log_s *log;                 // ccspec's log
	vattr_s *reader;                 // spec's keyword => reader => parse
	vattr_s *name;                   // placeholder (type, keyword)
	vattr_s *major;                  // miko.major.name => (miko_std_ccspec_major_s *)
	vattr_s *type;                   // miko.std.type.name => (miko_std_ccspec_type_s *)
	miko_std_ccspec_major_s *class;  // class link major
};

#endif
