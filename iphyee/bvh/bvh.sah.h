#ifndef _iphyee_bvh_sah_h_
#define _iphyee_bvh_sah_h_

#include "bvh.h"
#include <exbuffer.h>

typedef struct iphyee_bvh_sah_inode_t iphyee_bvh_sah_inode_t;
typedef struct iphyee_bvh_sah_input_s iphyee_bvh_sah_input_s;
typedef struct iphyee_bvh_sah_bucket_s iphyee_bvh_sah_bucket_s;
typedef struct iphyee_bvh_sah_inode_s iphyee_bvh_sah_inode_s;

struct iphyee_bvh_sah_inode_t {
	iphyee_bvh_inode_t inode;
	iphyee_glslc_bvh_box_t box;
	iphyee_bvh_sah_input_s *list;
};

struct iphyee_bvh_sah_input_s {
	iphyee_bvh_sah_input_s *next;
	iphyee_glslc_bvh_refer_t *ref;
	iphyee_glslc_pointer_t ref_address;
	iphyee_glslc_bvh_box_t box;
	iphyee_glslc_model_vec3xyz_t pos;
	uint32_t tri3index;
	uint32_t input_index;
	uint32_t inode_index;
};

struct iphyee_bvh_sah_bucket_s {
	exbuffer_t bucket;
	iphyee_bvh_sah_input_s **bucket_array;
	uintptr_t bucket_count;
	iphyee_glslc_bvh_box_t box;
};

struct iphyee_bvh_sah_inode_s {
	exbuffer_t inode;
	iphyee_bvh_sah_inode_t *inode_array;
	uintptr_t inode_count;
	int32_t layer_max;
};

struct iphyee_bvh_sah_blayer_s {
	iphyee_bvh_sah_bucket_s *input;
	iphyee_bvh_sah_inode_s *inode;
	uintptr_t bucket_number;
	iphyee_bvh_sah_bucket_s **bucket_x;
	iphyee_bvh_sah_bucket_s **bucket_y;
	iphyee_bvh_sah_bucket_s **bucket_z;
	uintptr_t bucket_count;
	iphyee_bvh_sah_bucket_s *bucket_array[];
};

struct iphyee_bvh_sah_tlayer_s {
	iphyee_bvh_sah_bucket_s *input;
	iphyee_bvh_sah_inode_s *inode;
};

// bvh.sah.input.c
iphyee_bvh_sah_input_s* iphyee_bvh_sah_input_alloc(iphyee_glslc_bvh_refer_t *restrict ref, iphyee_glslc_pointer_t ref_address, const iphyee_glslc_bvh_box_t *restrict box, const iphyee_glslc_model_vec3xyz_t *restrict pos, uint32_t tri3index, uint32_t input_index, uint32_t inode_index);

// bvh.sah.bucket.c
iphyee_bvh_sah_bucket_s* iphyee_bvh_sah_bucket_alloc(void);
iphyee_bvh_sah_bucket_s* iphyee_bvh_sah_bucket_append(iphyee_bvh_sah_bucket_s *restrict r, iphyee_bvh_sah_input_s *restrict input);
void iphyee_bvh_sah_bucket_clear(iphyee_bvh_sah_bucket_s *restrict r);
iphyee_bvh_sah_input_s** iphyee_bvh_sah_bucket_list(iphyee_bvh_sah_bucket_s *restrict r, iphyee_bvh_sah_input_s **restrict ptail);

// bvh.sah.inode.c
iphyee_bvh_sah_inode_s* iphyee_bvh_sah_inode_alloc(void);
iphyee_bvh_sah_inode_t* iphyee_bvh_sah_inode_append(iphyee_bvh_sah_inode_s *restrict r, const iphyee_bvh_sah_inode_t *restrict inode);
iphyee_bvh_sah_inode_s* iphyee_bvh_sah_inode_append_split(iphyee_bvh_sah_inode_s *restrict r, uintptr_t inode_index, const iphyee_glslc_bvh_box_t *restrict box, iphyee_bvh_sah_input_s *restrict list, uintptr_t list_count);
iphyee_bvh_sah_inode_s* iphyee_bvh_sah_inode_touch_tri3(iphyee_bvh_sah_inode_s *restrict r, uintptr_t inode_index, iphyee_bvh_sah_input_s *restrict list);

// bvh.sah.blayer.c
iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_alloc(uintptr_t bucket_number);
iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_tri3(iphyee_bvh_sah_blayer_s *restrict r, const iphyee_glslc_model_vec3xyz_t *restrict v1, const iphyee_glslc_model_vec3xyz_t *restrict v2, const iphyee_glslc_model_vec3xyz_t *restrict v3, uint32_t tri3index);
iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_build(iphyee_bvh_sah_blayer_s *restrict r);

#endif
