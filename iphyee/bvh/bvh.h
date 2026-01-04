#ifndef _iphyee_bvh_pri_h_
#define _iphyee_bvh_pri_h_

#include "../iphyee.glslc.h"

typedef struct iphyee_bvh_inode_t iphyee_bvh_inode_t;
typedef struct iphyee_bvh_sah_s iphyee_bvh_sah_s;

struct iphyee_bvh_inode_t {
	uint32_t parent;
	uint32_t next;
	uint32_t child;
	uint32_t tri3index;
};

#endif
