#ifndef _iphyee_bvh_sah_h_
#define _iphyee_bvh_sah_h_

#include "bvh.h"
#include <exbuffer.h>

typedef struct iphyee_bvh_sah_inode_t iphyee_bvh_sah_inode_t;
typedef struct iphyee_bvh_sah_input_s iphyee_bvh_sah_input_s;
typedef struct iphyee_bvh_sah_bucket_s iphyee_bvh_sah_bucket_s;
typedef struct iphyee_bvh_sah_bucket3_s iphyee_bvh_sah_bucket3_s;

struct iphyee_bvh_sah_inode_t {
	iphyee_bvh_inode_t inode;
	iphyee_glslc_bvh_box_t box;
	iphyee_bvh_sah_input_s *list;
	int32_t layer;
};

struct iphyee_bvh_sah_input_s {
	iphyee_bvh_sah_input_s *next;
	iphyee_glslc_pointer_bvh_refer_t *ref;
	iphyee_glslc_bvh_box_t box;
	uint32_t tri3index;
	uint32_t input_index;
	uint32_t inode_index;
};

struct iphyee_bvh_sah_bucket_s {
	exbuffer_t bucket;
	const iphyee_bvh_sah_input_s **bucket_array;
	uintptr_t bucket_count;
	iphyee_glslc_bvh_box_t box;
};

struct iphyee_bvh_sah_bucket3_s {
	iphyee_bvh_sah_bucket_s *x;
	iphyee_bvh_sah_bucket_s *y;
	iphyee_bvh_sah_bucket_s *z;
};

struct iphyee_bvh_sah_blayer_s {
	exbuffer_t inode;
	iphyee_bvh_sah_inode_t *inode_array;
	uintptr_t inode_count;
	iphyee_bvh_sah_bucket_s *input;
	uintptr_t bucket_number;
	iphyee_glslc_pointer_t dev_bvh_address;
	iphyee_glslc_pointer_t dev_target_address;
	iphyee_bvh_sah_bucket3_s *bucket[];
};

struct iphyee_bvh_sah_tlayer_s {
	exbuffer_t inode;
	iphyee_bvh_inode_t *inode_array;
	uintptr_t inode_count;
	iphyee_bvh_sah_bucket_s *input;
	uintptr_t bucket_number;
	iphyee_glslc_pointer_t dev_bvh_address;
};

#endif
