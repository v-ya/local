#ifndef _vkaa_tpool_h_
#define _vkaa_tpool_h_

#include "vkaa.h"
#include <vattr.h>
#include <rbtree.h>

struct vkaa_tpool_s {
	vattr_s *n2t;
	rbtree_t *i2t;
	uintptr_t id_next;
};

vkaa_tpool_s* vkaa_tpool_alloc(void);
uintptr_t vkaa_tpool_genid(vkaa_tpool_s *restrict tpool);
const vkaa_type_s* vkaa_tpool_find_name(vkaa_tpool_s *restrict tpool, const char *restrict name);
const vkaa_type_s* vkaa_tpool_find_id(vkaa_tpool_s *restrict tpool, uintptr_t id);
vkaa_tpool_s* vkaa_tpool_insert(vkaa_tpool_s *restrict tpool, const vkaa_type_s *restrict type);
void vkaa_tpool_remove_by_name(vkaa_tpool_s *restrict tpool, const char *restrict name);
void vkaa_tpool_remove_by_id(vkaa_tpool_s *restrict tpool, uintptr_t id);
void vkaa_tpool_clear(vkaa_tpool_s *restrict tpool);

#endif