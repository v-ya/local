#ifndef _miko_std_miko_std_marco_h_
#define _miko_std_miko_std_marco_h_

#include "miko.std.h"
#include <vattr.h>
#include <rbtree.h>

typedef struct miko_std_marco_index_t miko_std_marco_index_t;
typedef struct miko_std_marco_pool_s miko_std_marco_pool_s;
typedef struct miko_std_marco_s miko_std_marco_s;
typedef const struct vattr_s miko_std_marco_args_s;  // marco.args.name => (miko_std_marco_argv_s *)
typedef const struct vattr_s miko_std_marco_argv_s;  // syntax.id.name => (const miko_std_syntax_s *)
typedef struct vattr_s miko_std_marco_scope_s;       // syntax.id.name => (const miko_std_syntax_s *)

struct miko_std_marco_index_t {
	miko_index_t name;
	miko_index_t comma;
	miko_index_t param;
};

struct miko_std_marco_pool_s {
	vattr_s *pool;                 // marco.name => (miko_std_marco_s *)
	miko_std_marco_index_t index;  // syntax.id.index
};

struct miko_std_marco_s {
	miko_vector_s_t name_cache;
	const refer_nstring_t *name_array;
	miko_count_t name_count;
	const miko_std_marco_scope_s *marco;
};

miko_std_marco_pool_s* miko_std_marco_pool_alloc(const miko_std_marco_index_t *restrict index);
miko_std_marco_pool_s* miko_std_marco_pool_add(miko_std_marco_pool_s *restrict r, const miko_std_syntax_s *restrict name, const miko_std_syntax_s *restrict param, const miko_std_marco_scope_s *restrict marco);
miko_std_marco_pool_s* miko_std_marco_pool_del(miko_std_marco_pool_s *restrict r, const miko_std_syntax_s *restrict name);
const miko_std_marco_s* miko_std_marco_pool_exist(miko_std_marco_pool_s *restrict r, const miko_std_syntax_s *restrict name, miko_bool_t *restrict need_param);
miko_std_marco_scope_s* miko_std_marco_pool_save_result(miko_std_marco_pool_s *restrict r, const miko_std_marco_s *restrict marco, const miko_std_syntax_s *restrict param);
miko_std_marco_pool_s* miko_std_marco_pool_replace_scope(miko_std_marco_pool_s *restrict r, miko_std_marco_scope_s *restrict scope);
miko_std_marco_pool_s* miko_std_marco_pool_replace_vlist(miko_std_marco_pool_s *restrict r, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop);

miko_std_marco_s* miko_std_marco_alloc(const miko_std_marco_index_t *restrict index, const miko_std_syntax_s *restrict param, const miko_std_marco_scope_s *restrict marco);
miko_std_marco_args_s* miko_std_marco_create_args(const miko_std_marco_s *restrict r, const miko_std_marco_index_t *restrict index, const miko_std_syntax_s *restrict param);
miko_std_marco_scope_s* miko_std_marco_create_scope(const miko_std_marco_s *restrict r);

void miko_std_marco_print(mlog_s *restrict mlog, const miko_source_s *restrict source, uintptr_t pos);

#endif
