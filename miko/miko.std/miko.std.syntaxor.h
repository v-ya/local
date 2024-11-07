#ifndef _miko_std_miko_std_syntaxor_h_
#define _miko_std_miko_std_syntaxor_h_

#include "miko.std.h"
#include <rbtree.h>

typedef struct vattr_s miko_std_syntaxor_pool_s;
typedef struct miko_std_syntaxor_item_s miko_std_syntaxor_item_s;
typedef struct miko_std_syntaxor_scope_s miko_std_syntaxor_scope_s;
typedef struct miko_std_syntaxor_scope_t miko_std_syntaxor_scope_t;

// return skip (must <= n - pos), 0 fail, other okay
typedef uintptr_t (*miko_std_syntaxor_test_f)(miko_std_syntaxor_item_s *restrict r, const char *restrict p, uintptr_t n, uintptr_t pos);
typedef const char* (*miko_std_syntaxor_data_f)(miko_std_syntaxor_item_s *restrict r, uintptr_t *restrict length);

struct miko_std_syntaxor_s {
	mlog_s *mlog;
	tparse_tmapping_s *tester;  // char => (miko_std_syntaxor_pool_s *)
	vattr_s *syntax;            // name => (miko_std_syntaxor_item_s *)
	vattr_s *route;             // char => (miko_std_syntaxor_pool_s *)
	vattr_s *scope_start;       // syntax_start => (miko_std_syntaxor_scope_s *)
	vattr_s *scope_stop;        // syntax_stop => (miko_std_syntaxor_scope_s *)
};

struct miko_std_syntaxor_item_s {
	miko_std_syntax_id_t id;
	miko_std_syntaxor_test_f test;
	miko_std_syntaxor_data_f data;
};

struct miko_std_syntaxor_scope_s {
	miko_std_syntax_id_t id;  // new syntax id
	refer_nstring_t syntax;   // scope syntax
	rbtree_t *allow;          // index => null
	uintptr_t must_match;     // is must match
};

struct miko_std_syntaxor_scope_t {
	const miko_std_syntaxor_scope_s *scope;
	vattr_vlist_t *vlist_start;
};

miko_std_syntaxor_item_s* miko_std_syntaxor_item_create_keyword(const char *const restrict keyword[]);
miko_std_syntaxor_item_s* miko_std_syntaxor_item_create_tword(miko_std_syntaxor_chars_t head, miko_std_syntaxor_chars_t inner, miko_std_syntaxor_chars_t tail);
miko_std_syntaxor_item_s* miko_std_syntaxor_item_create_tstring(tparse_tstring_s *restrict tstring);

miko_std_syntaxor_scope_s* miko_std_syntaxor_scope_alloc(const miko_std_syntax_id_t *restrict id, const char *restrict syntax_scope, uintptr_t must_match);
miko_std_syntaxor_scope_s* miko_std_syntaxor_scope_add_allow(miko_std_syntaxor_scope_s *restrict r, miko_index_t index);
const miko_std_syntaxor_scope_s* miko_std_syntaxor_scope_allow_test(const miko_std_syntaxor_scope_s *restrict r, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop);
const miko_std_syntaxor_scope_s* miko_std_syntaxor_scope_replace(const miko_std_syntaxor_scope_s *restrict r, miko_std_syntax_s *restrict scope, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop);

miko_std_syntax_s* miko_std_syntax_create_syntax(const miko_std_syntax_id_t *restrict id, const miko_std_syntax_source_t *restrict source, const char *restrict syntax, uintptr_t length);
miko_std_syntax_s* miko_std_syntax_create_scope(const miko_std_syntax_id_t *restrict id, const miko_std_syntax_source_t *restrict source);
miko_std_syntax_s* miko_std_syntax_create_scope_by_vlist(const miko_std_syntax_id_t *restrict id, const miko_std_syntax_source_t *restrict source, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop);
miko_std_syntax_source_t* miko_std_syntax_fetch_source_by_vlist(miko_std_syntax_source_t *restrict source, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop);
vattr_s* miko_std_syntax_scope_append_vlist(vattr_s *restrict scope, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop);
vattr_s* miko_std_syntax_scope_append_vlist_copy(vattr_s *restrict scope, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop);
vattr_s* miko_std_syntax_scope_delete_vlist(vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop);
vattr_s* miko_std_syntax_scope_replace_vlist2syntax(vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop, const miko_std_syntax_s *restrict syntax);
vattr_s* miko_std_syntax_scope_replace_vlist2vlist(vattr_vlist_t *restrict target_start, vattr_vlist_t *restrict target_stop, vattr_vlist_t *restrict source_start, vattr_vlist_t *restrict source_stop);
vattr_vlist_t* miko_std_syntax_scope_find_id(vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop, miko_index_t id_index);
vattr_vlist_t* miko_std_syntax_scope_find_syntax(vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop, miko_index_t id_index, const char *restrict syntax);

#endif
