#ifndef _iphyee_bonex_pri_h_
#define _iphyee_bonex_pri_h_

#include "../iphyee.pri.h"

typedef struct iphyee_bonex_joint_t iphyee_bonex_joint_t;
typedef struct iphyee_bonex_joint_s iphyee_bonex_joint_s;
typedef struct iphyee_bonex_coord_t iphyee_bonex_coord_t;
typedef struct iphyee_bonex_coord_s iphyee_bonex_coord_s;
typedef struct iphyee_bonex_inode_t iphyee_bonex_inode_t;
typedef struct iphyee_bonex_inode_s iphyee_bonex_inode_s;
typedef struct iphyee_bonex_cache_s iphyee_bonex_cache_s;

struct iphyee_bonex_s {
	vattr_s *joint;
	vattr_s *coord;
	vattr_s *inode;
	uintptr_t joint_nsize;
	uintptr_t coord_nsize;
	uintptr_t inode_nsize;
	iphyee_bonex_joint_s **joint_array;
	iphyee_bonex_coord_s **coord_array;
	iphyee_bonex_inode_s **inode_array;
	uintptr_t joint_count;
	uintptr_t coord_count;
	uintptr_t inode_count;
	uintptr_t value_count;
	uintptr_t fix_mat4x4_count;
	uintptr_t bonex_index_okay;
};

struct iphyee_bonex_joint_t {
	uintptr_t this_joint_index;
	uintptr_t base_joint_index;
	uintptr_t link_joint_start;
	uintptr_t link_joint_count;
	uintptr_t link_coord_start;
	uintptr_t link_coord_count;
	uintptr_t link_inode_start;
	uintptr_t link_inode_count;
	uintptr_t link_value_start;
	uintptr_t link_value_count;
	uintptr_t fix_mat4x4_start;
	uintptr_t fix_mat4x4_count;
};

struct iphyee_bonex_joint_s {
	refer_string_t name;
	refer_string_t base;
	iphyee_bonex_joint_t index;
};

struct iphyee_bonex_coord_t {
	uintptr_t base_joint_index;
	uintptr_t this_coord_index;
	uintptr_t this_value_index;
	uintptr_t fix_mat4x4_index;
};

struct iphyee_bonex_coord_s {
	refer_string_t name;
	refer_string_t base;
	iphyee_bonex_fix_method_t method;
	iphyee_bonex_fix_property_t property;
	iphyee_bonex_simple_f bonex_simple_fix;
	iphyee_bonex_coord_t index;
};

struct iphyee_bonex_inode_t {
	uintptr_t base_joint_index;
	uintptr_t this_inode_index;
	uintptr_t this_value_index;
	uintptr_t fix_mat4x4_index;
};

struct iphyee_bonex_inode_s {
	refer_string_t name;
	refer_string_t base;
	iphyee_bonex_fix_method_t method;
	iphyee_bonex_fix_property_t property;
	iphyee_bonex_simple_f bonex_simple_fix;
	iphyee_bonex_complex_s *bonex_complex_fix;
	float default_value_minimum;
	float default_value_maximum;
	float default_value_multiplier;
	float default_value_addend;
	iphyee_bonex_inode_t index;
};

// method

iphyee_bonex_simple_f iphyee_bonex_fix_method(iphyee_bonex_fix_method_t method);

// bonex

iphyee_bonex_s* iphyee_bonex_alloc(uintptr_t joint_nsize, uintptr_t coord_nsize, uintptr_t inode_nsize);
iphyee_bonex_joint_s* iphyee_bonex_find_joint(const iphyee_bonex_s *restrict r, const char *restrict name);
iphyee_bonex_coord_s* iphyee_bonex_find_coord(const iphyee_bonex_s *restrict r, const char *restrict name);
iphyee_bonex_inode_s* iphyee_bonex_find_inode(const iphyee_bonex_s *restrict r, const char *restrict name);
iphyee_bonex_s* iphyee_bonex_append_bonex(iphyee_bonex_s *restrict r, const iphyee_bonex_s *restrict bonex);
iphyee_bonex_joint_s* iphyee_bonex_append_joint(iphyee_bonex_s *restrict r, const char *this_joint_name, const char *base_joint_name);
iphyee_bonex_coord_s* iphyee_bonex_append_coord(iphyee_bonex_s *restrict r, const iphyee_bonex_joint_s *restrict joint, const iphyee_param_bonex_coord_t *restrict param);
iphyee_bonex_inode_s* iphyee_bonex_append_inode(iphyee_bonex_s *restrict r, const iphyee_bonex_joint_s *restrict joint, const iphyee_param_bonex_inode_t *restrict param);
iphyee_bonex_s* iphyee_bonex_okay(iphyee_bonex_s *restrict r);

uint32_t iphyee_bonex_index_joint(const iphyee_bonex_s *restrict r, const char *restrict name);
uint32_t iphyee_bonex_index_coord(const iphyee_bonex_s *restrict r, const char *restrict name);
uint32_t iphyee_bonex_index_inode(const iphyee_bonex_s *restrict r, const char *restrict name);

// joint

iphyee_bonex_joint_s* iphyee_bonex_joint_alloc(const char *restrict name, const iphyee_bonex_joint_s *restrict base);
iphyee_bonex_joint_s* iphyee_bonex_joint_dump(const iphyee_bonex_joint_s *restrict src);

// coord

iphyee_bonex_coord_s* iphyee_bonex_coord_alloc(const iphyee_param_bonex_coord_t *restrict param, const iphyee_bonex_joint_s *restrict base);
iphyee_bonex_coord_s* iphyee_bonex_coord_dump(const iphyee_bonex_coord_s *restrict src);

// inode

iphyee_bonex_inode_s* iphyee_bonex_inode_alloc(const iphyee_param_bonex_inode_t *restrict param, const iphyee_bonex_joint_s *restrict base);
iphyee_bonex_inode_s* iphyee_bonex_inode_dump(const iphyee_bonex_inode_s *restrict src);

// create

iphyee_bonex_s* iphyee_bonex_create(vattr_s *restrict bonex_pool, const iphyee_param_bonex_t *restrict param);

// cache

iphyee_bonex_cache_s* iphyee_bonex_cache_alloc(const iphyee_bonex_s *restrict bonex);
iphyee_bonex_cache_s* iphyee_bonex_cache_dump(const iphyee_bonex_cache_s *restrict cache);
iphyee_bonex_cache_s* iphyee_bonex_cache_set_value(iphyee_bonex_cache_s *restrict r, uint32_t value_index, float value);
const iphyee_mat4x4_t* iphyee_bonex_cache_joint_p_mat4x4(iphyee_bonex_cache_s *restrict r, uint32_t joint_index);
const iphyee_mat4x4_t* iphyee_bonex_cache_joint_n_mat4x4(iphyee_bonex_cache_s *restrict r, uint32_t joint_index);

#endif
