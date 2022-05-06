#ifndef _vkaa_tpool_h_
#define _vkaa_tpool_h_

#include "vkaa.h"
#include <vattr.h>
#include <rbtree.h>

struct vkaa_tpool_s {
	vattr_s *n2t;
	rbtree_t *i2t;
	rbtree_t *i2v;
	vkaa_error_s *e;
	uintptr_t id_last;
};

vkaa_tpool_s* vkaa_tpool_alloc(void);
uintptr_t vkaa_tpool_genid(vkaa_tpool_s *restrict tpool);
const vkaa_type_s* vkaa_tpool_find_name(const vkaa_tpool_s *restrict tpool, const char *restrict name);
const vkaa_type_s* vkaa_tpool_find_id(const vkaa_tpool_s *restrict tpool, uintptr_t id);
vkaa_tpool_s* vkaa_tpool_insert(vkaa_tpool_s *restrict tpool, vkaa_type_s *restrict type);
void vkaa_tpool_remove_by_name(vkaa_tpool_s *restrict tpool, const char *restrict name);
void vkaa_tpool_remove_by_id(vkaa_tpool_s *restrict tpool, uintptr_t id);
void vkaa_tpool_clear(vkaa_tpool_s *restrict tpool);

vkaa_var_s* vkaa_tpool_var_const_enable(vkaa_tpool_s *restrict tpool, const vkaa_type_s *restrict type);
vkaa_var_s* vkaa_tpool_var_const_enable_by_name(vkaa_tpool_s *restrict tpool, const char *restrict name);
vkaa_var_s* vkaa_tpool_var_const_enable_by_id(vkaa_tpool_s *restrict tpool, uintptr_t id);
void vkaa_tpool_var_const_disable_by_name(vkaa_tpool_s *restrict tpool, const char *restrict name);
void vkaa_tpool_var_const_disable_by_id(vkaa_tpool_s *restrict tpool, uintptr_t id);

vkaa_var_s* vkaa_tpool_var_create(const vkaa_tpool_s *restrict tpool, const vkaa_type_s *restrict type);
vkaa_var_s* vkaa_tpool_var_create_by_name(const vkaa_tpool_s *restrict tpool, const char *restrict name);
vkaa_var_s* vkaa_tpool_var_create_by_id(const vkaa_tpool_s *restrict tpool, uintptr_t id);

#endif
