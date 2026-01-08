#include "bvh.sah.h"

iphyee_bvh_sah_input_s* iphyee_bvh_sah_input_alloc(iphyee_glslc_bvh_refer_t *restrict ref, iphyee_glslc_pointer_t ref_address, const iphyee_glslc_bvh_box_t *restrict box, const iphyee_glslc_model_vec3xyz_t *restrict pos, uint32_t tri3index, uint32_t input_index, uint32_t inode_index)
{
	iphyee_bvh_sah_input_s *restrict r;
	if ((r = (iphyee_bvh_sah_input_s *) refer_alloc(sizeof(iphyee_bvh_sah_input_s))))
	{
		r->next = NULL;
		r->ref = ref;
		r->ref_address = ref_address;
		r->box = *box;
		r->pos = *pos;
		r->tri3index = tri3index;
		r->input_index = input_index;
		r->inode_index = inode_index;
		return r;
	}
	return NULL;
}
