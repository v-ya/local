#ifndef _iphyee_bvh_pri_h_
#define _iphyee_bvh_pri_h_

#include "../iphyee.glslc.h"
#include <refer.h>

typedef struct iphyee_bvh_inode_t iphyee_bvh_inode_t;
typedef struct iphyee_bvh_sah_blayer_s iphyee_bvh_sah_blayer_s;
typedef struct iphyee_bvh_sah_tlayer_s iphyee_bvh_sah_tlayer_s;

struct iphyee_bvh_inode_t {
	uint32_t parent;    // 上层父节点索引
	uint32_t next;      // 平层下一节点索引
	uint32_t child;     // 下层子节点索引
	uint32_t nleaf;     // 下方最底部的叶节点数
	int32_t layer;      // 层次级别
	uint32_t tri3index; // 指向的三角形索引
};

// blayer: root, tri3, tri3, ..., inner, inner, ...
// tlayer: root, inner, inner, ...

// box
void iphyee_bvh_box_tri3(iphyee_glslc_bvh_box_t *restrict box, const iphyee_glslc_model_vec3xyz_t *restrict v1, const iphyee_glslc_model_vec3xyz_t *restrict v2, const iphyee_glslc_model_vec3xyz_t *restrict v3);
void iphyee_bvh_box_initial(iphyee_glslc_bvh_box_t *restrict box);
void iphyee_bvh_box_append(iphyee_glslc_bvh_box_t *restrict dst, const iphyee_glslc_bvh_box_t *restrict src);
double iphyee_bvh_box_area(const iphyee_glslc_bvh_box_t *restrict box);

// inode
void iphyee_bvh_inode_initial(iphyee_bvh_inode_t *restrict inode);

// blayer
iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_alloc(uintptr_t bucket_number);
iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_tri3(iphyee_bvh_sah_blayer_s *restrict r, const iphyee_glslc_model_vec3xyz_t *restrict v1, const iphyee_glslc_model_vec3xyz_t *restrict v2, const iphyee_glslc_model_vec3xyz_t *restrict v3, uint32_t tri3index);
iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_build(iphyee_bvh_sah_blayer_s *restrict r);

#endif
