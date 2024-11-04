#ifndef _miko_std_miko_std_syntaxor_h_
#define _miko_std_miko_std_syntaxor_h_

#include "miko.std.h"

typedef struct vattr_s miko_std_syntaxor_pool_s;
typedef struct miko_std_syntaxor_item_s miko_std_syntaxor_item_s;

// return skip (must <= n - pos), 0 fail, other okay
typedef uintptr_t (*miko_std_syntaxor_test_f)(miko_std_syntaxor_item_s *restrict r, const char *restrict p, uintptr_t n, uintptr_t pos);
typedef const char* (*miko_std_syntaxor_data_f)(miko_std_syntaxor_item_s *restrict r, uintptr_t *restrict length);

struct miko_std_syntaxor_s {
	mlog_s *mlog;
	tparse_tmapping_s *tester;  // char => (miko_std_syntaxor_pool_s *)
	vattr_s *syntax;            // name => (miko_std_syntaxor_item_s *)
	vattr_s *route;             // char => (miko_std_syntaxor_pool_s *)
};

struct miko_std_syntaxor_item_s {
	miko_std_syntax_id_t id;
	miko_std_syntaxor_test_f test;
	miko_std_syntaxor_data_f data;
};

miko_std_syntaxor_item_s* miko_std_syntaxor_item_create_keyword(const char *const restrict keyword[]);

miko_std_syntax_s* miko_std_syntax_create_syntax(const miko_std_syntax_id_t *restrict id, const miko_std_syntax_source_t *restrict source, const char *restrict syntax, uintptr_t length);
miko_std_syntax_s* miko_std_syntax_create_scope(const miko_std_syntax_id_t *restrict id, const miko_std_syntax_source_t *restrict source);

#endif
