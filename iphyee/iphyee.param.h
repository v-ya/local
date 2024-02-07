#ifndef _iphyee_param_h_
#define _iphyee_param_h_

#include <stdint.h>
#include <refer.h>

// mat4x4

struct iphyee_mat4x4_t;

// param bonex

typedef enum iphyee_bonex_fix_method_t iphyee_bonex_fix_method_t;
typedef enum iphyee_bonex_fix_property_t iphyee_bonex_fix_property_t;
typedef struct iphyee_bonex_complex_s iphyee_bonex_complex_s;

typedef void (*iphyee_bonex_simple_f)(struct iphyee_mat4x4_t *restrict dst, float value);
typedef void (*iphyee_bonex_complex_f)(struct iphyee_bonex_complex_s *restrict c, struct iphyee_mat4x4_t *restrict dst, float value);

enum iphyee_bonex_fix_method_t {
	iphyee_bonex_fix_method__e,
	iphyee_bonex_fix_method__tx,
	iphyee_bonex_fix_method__ty,
	iphyee_bonex_fix_method__tz,
	iphyee_bonex_fix_method__rx,
	iphyee_bonex_fix_method__ry,
	iphyee_bonex_fix_method__rz,
	iphyee_bonex_fix_method__custom
};

enum iphyee_bonex_fix_property_t {
	iphyee_bonex_fix_property__fixed,
	iphyee_bonex_fix_property__dynamic
};

struct iphyee_bonex_complex_s {
	iphyee_bonex_complex_f mat4x4gen;
};

typedef struct iphyee_param_bonex_coord_t iphyee_param_bonex_coord_t;
typedef struct iphyee_param_bonex_inode_t iphyee_param_bonex_inode_t;
typedef struct iphyee_param_bonex_joint_t iphyee_param_bonex_joint_t;

struct iphyee_param_bonex_coord_t {
	const char *this_coord_name;
	iphyee_bonex_fix_method_t method;
	iphyee_bonex_fix_property_t property;
};

struct iphyee_param_bonex_inode_t {
	const char *this_inode_name;
	iphyee_bonex_fix_method_t method;
	iphyee_bonex_fix_property_t property;
	iphyee_bonex_simple_f fix_custom_simple;
	iphyee_bonex_complex_s *fix_custom_complex;
	float default_value_minimum;
	float default_value_maximum;
	float default_value_multiplier;
	float default_value_addend;
};

struct iphyee_param_bonex_joint_t {
	const char *this_joint_name;
	const char *base_joint_name;
	const iphyee_param_bonex_coord_t *base_coord_array;
	uintptr_t base_coord_count;
	const iphyee_param_bonex_inode_t *this_inode_array;
	uintptr_t this_inode_count;
};

struct iphyee_param_bonex_t {
	const char *this_bonex_name;
	const char *base_bonex_name;
	const iphyee_param_bonex_joint_t *joint_array;
	uintptr_t joint_count;
};

#endif
