#ifndef _iphyee_bonex_pri_h_
#define _iphyee_bonex_pri_h_

#include "../iphyee.pri.h"

typedef struct iphyee_bonex_joint_t iphyee_bonex_joint_t;
typedef struct iphyee_bonex_joint_s iphyee_bonex_joint_s;
typedef struct iphyee_bonex_coord_t iphyee_bonex_coord_t;
typedef struct iphyee_bonex_coord_s iphyee_bonex_coord_s;
typedef struct iphyee_bonex_inode_t iphyee_bonex_inode_t;
typedef struct iphyee_bonex_inode_s iphyee_bonex_inode_s;

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
	float value_minimum;
	float value_maximum;
	float value_multiplier;
	float value_addend;
	iphyee_bonex_inode_t index;
};

// method

iphyee_bonex_simple_f iphyee_bonex_fix_method(iphyee_bonex_fix_method_t method);

// bonex

iphyee_bonex_s* iphyee_bonex_alloc(uintptr_t joint_nsize, uintptr_t coord_nsize, uintptr_t inode_nsize);
iphyee_bonex_joint_s* iphyee_bonex_find_joint(iphyee_bonex_s *restrict r, const char *restrict name);
iphyee_bonex_coord_s* iphyee_bonex_find_coord(iphyee_bonex_s *restrict r, const char *restrict name);
iphyee_bonex_inode_s* iphyee_bonex_find_inode(iphyee_bonex_s *restrict r, const char *restrict name);
iphyee_bonex_s* iphyee_bonex_append_bonex(iphyee_bonex_s *restrict r, const iphyee_bonex_s *restrict bonex);
iphyee_bonex_joint_s* iphyee_bonex_append_joint(iphyee_bonex_s *restrict r, const char *this_joint_name, const char *base_joint_name);
iphyee_bonex_coord_s* iphyee_bonex_append_coord(iphyee_bonex_s *restrict r, const iphyee_bonex_joint_s *restrict joint, const iphyee_param_bonex_coord_t *restrict param);
iphyee_bonex_inode_s* iphyee_bonex_append_inode(iphyee_bonex_s *restrict r, const iphyee_bonex_joint_s *restrict joint, const iphyee_param_bonex_inode_t *restrict param);
iphyee_bonex_s* iphyee_bonex_okay(iphyee_bonex_s *restrict r);

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

#endif
