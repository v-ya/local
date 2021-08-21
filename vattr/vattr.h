#ifndef _vattr_h_
#define _vattr_h_

#include <refer.h>
#include <hashmap.h>

typedef struct vattr_s vattr_s;
typedef struct vattr_vslot_t vattr_vslot_t;
typedef struct vattr_vlist_t vattr_vlist_t;

struct vattr_s {
	hashmap_t vslot;
	vattr_vlist_t *vattr;
	vattr_vlist_t **p_vattr_tail;
};

struct vattr_vslot_t {
	const char *key;
	uintptr_t number;
	vattr_vlist_t *vslot;
	vattr_vlist_t **p_vslot_tail;
	vattr_s *vattr;
};

struct vattr_vlist_t {
	vattr_vlist_t *vattr_next;
	vattr_vlist_t *vslot_next;
	vattr_vlist_t **p_vattr_next;
	vattr_vlist_t **p_vslot_next;
	vattr_vslot_t *vslot;
	refer_t value;
};

vattr_s* vattr_alloc(void);
void vattr_clear(vattr_s *restrict vattr);
vattr_s* vattr_append(vattr_s *restrict dst, const vattr_s *restrict src);
vattr_s* vattr_copy(vattr_s *restrict dst, const vattr_s *restrict src);

vattr_vlist_t* vattr_first(const vattr_s *restrict vattr);
vattr_vlist_t* vattr_tail(const vattr_s *restrict vattr);
vattr_vlist_t* vattr_next(const vattr_vlist_t *restrict vlist);
vattr_vlist_t* vattr_last(const vattr_vlist_t *restrict vlist);
vattr_vlist_t* vattr_next_end(const vattr_s *restrict vattr);
vattr_vlist_t* vattr_last_end(const vattr_s *restrict vattr);

vattr_vlist_t* vattr_vslot_first(const vattr_vslot_t *restrict vslot);
vattr_vlist_t* vattr_vslot_tail(const vattr_vslot_t *restrict vslot);
vattr_vlist_t* vattr_vslot_next(const vattr_vlist_t *restrict vlist);
vattr_vlist_t* vattr_vslot_last(const vattr_vlist_t *restrict vlist);
vattr_vlist_t* vattr_vslot_next_end(const vattr_vslot_t *restrict vslot);
vattr_vlist_t* vattr_vslot_last_end(const vattr_vslot_t *restrict vslot);
vattr_vlist_t* vattr_vslot_index(const vattr_vslot_t *restrict vslot, uintptr_t index);

vattr_vslot_t* vattr_get_vslot(const vattr_s *restrict vattr, const char *restrict key);

uintptr_t vattr_get_vslot_number(const vattr_s *restrict vattr, const char *restrict key);

vattr_vlist_t* vattr_get_vlist_first(const vattr_s *restrict vattr, const char *restrict key);
vattr_vlist_t* vattr_get_vlist_tail(const vattr_s *restrict vattr, const char *restrict key);
vattr_vlist_t* vattr_get_vlist_index(const vattr_s *restrict vattr, const char *restrict key, uintptr_t index);

refer_t vattr_get_first(const vattr_s *restrict vattr, const char *restrict key);
refer_t vattr_get_tail(const vattr_s *restrict vattr, const char *restrict key);
refer_t vattr_get_index(const vattr_s *restrict vattr, const char *restrict key, uintptr_t index);

vattr_vlist_t* vattr_set(vattr_s *restrict vattr, const char *restrict key, refer_t value);

vattr_vlist_t* vattr_insert_vlist_last(vattr_vlist_t *restrict vlist, refer_t value);
vattr_vlist_t* vattr_insert_vlist_next(vattr_vlist_t *restrict vlist, refer_t value);

vattr_vlist_t* vattr_insert_vslot_first_vattr(vattr_vslot_t *restrict vslot, refer_t value);
vattr_vlist_t* vattr_insert_vslot_first_vslot(vattr_vslot_t *restrict vslot, refer_t value);
vattr_vlist_t* vattr_insert_vslot_tail_vslot(vattr_vslot_t *restrict vslot, refer_t value);
vattr_vlist_t* vattr_insert_vslot_tail_vattr(vattr_vslot_t *restrict vslot, refer_t value);
vattr_vlist_t* vattr_insert_vslot_index_last(vattr_vslot_t *restrict vslot, uintptr_t index, refer_t value);
vattr_vlist_t* vattr_insert_vslot_index_next(vattr_vslot_t *restrict vslot, uintptr_t index, refer_t value);

vattr_vlist_t* vattr_insert_first(vattr_s *restrict vattr, const char *restrict key, refer_t value);
vattr_vlist_t* vattr_insert_first_vslot(vattr_s *restrict vattr, const char *restrict key, refer_t value);
vattr_vlist_t* vattr_insert_tail_vslot(vattr_s *restrict vattr, const char *restrict key, refer_t value);
vattr_vlist_t* vattr_insert_tail(vattr_s *restrict vattr, const char *restrict key, refer_t value);
vattr_vlist_t* vattr_insert_index_last(vattr_s *restrict vattr, const char *restrict key, uintptr_t index, refer_t value);
vattr_vlist_t* vattr_insert_index_next(vattr_s *restrict vattr, const char *restrict key, uintptr_t index, refer_t value);

vattr_vslot_t* vattr_delete_vlist(vattr_vlist_t *restrict vlist);

void vattr_delete_vslot(vattr_vslot_t *restrict vslot);
vattr_vslot_t* vattr_delete_vslot_first(vattr_vslot_t *restrict vslot);
vattr_vslot_t* vattr_delete_vslot_tail(vattr_vslot_t *restrict vslot);
vattr_vslot_t* vattr_delete_vslot_index(vattr_vslot_t *restrict vslot, uintptr_t index);

void vattr_delete(vattr_s *restrict vattr, const char *restrict key);
vattr_vslot_t* vattr_delete_first(vattr_s *restrict vattr, const char *restrict key);
vattr_vslot_t* vattr_delete_tail(vattr_s *restrict vattr, const char *restrict key);
vattr_vslot_t* vattr_delete_index(vattr_s *restrict vattr, const char *restrict key, uintptr_t index);

#endif
