#ifndef _iphyee_bonex_cache_h_
#define _iphyee_bonex_cache_h_

#include "bonex.h"

typedef struct iphyee_bonex_cache_s iphyee_bonex_cache_s;
typedef struct iphyee_bonex_cache_ja_t iphyee_bonex_cache_ja_t;
typedef struct iphyee_bonex_cache_fa_t iphyee_bonex_cache_fa_t;
typedef struct iphyee_bonex_cache_ia_t iphyee_bonex_cache_ia_t;
typedef struct iphyee_bonex_cache_va_t iphyee_bonex_cache_va_t;

struct iphyee_bonex_cache_s {
	iphyee_mat4x4_t *joint_p_array;
	iphyee_mat4x4_t *joint_n_array;
	iphyee_mat4x4_t *fixed_p_array;
	iphyee_mat4x4_t *fixed_n_array;
	iphyee_bonex_cache_va_t *va_array;
	iphyee_bonex_cache_ia_t *ia_array;
	iphyee_bonex_cache_fa_t *fa_array;
	iphyee_bonex_cache_ja_t *ja_array;
	float *value_array;
	uint32_t joint_count;
	uint32_t fixed_count;
	uint32_t inode_count;
	uint32_t value_count;
};

struct iphyee_bonex_cache_ja_t {
	uint32_t need_update;
	uint32_t base_joint_index;
	uint32_t link_joint_start;
	uint32_t link_joint_count;
	uint32_t link_fixed_start;
	uint32_t link_fixed_count;
};

struct iphyee_bonex_cache_fa_t {
	uint32_t need_update;
	uint32_t base_joint_index;
	uint32_t link_value_start;
	uint32_t link_value_count;
};

struct iphyee_bonex_cache_ia_t {
	float value_minimum;
	float value_maximum;
	float value_multiplier;
	float value_addend;
};

struct iphyee_bonex_cache_va_t {
	uint32_t need_update;
	uint32_t base_joint_index;
	uint32_t base_fixed_index;
	uint32_t base_inode_index;
	iphyee_bonex_simple_f value_simple;
	iphyee_bonex_complex_s *value_complex;
};

#endif
