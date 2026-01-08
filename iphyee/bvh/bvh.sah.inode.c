#include "bvh.sah.h"

static void iphyee_bvh_sah_inode_free_func(iphyee_bvh_sah_inode_s *restrict r)
{
	exbuffer_uini(&r->inode);
}

iphyee_bvh_sah_inode_s* iphyee_bvh_sah_inode_alloc(void)
{
	iphyee_bvh_sah_inode_s *restrict r;
	if ((r = (iphyee_bvh_sah_inode_s *) refer_alloz(sizeof(iphyee_bvh_sah_inode_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_bvh_sah_inode_free_func);
		if (exbuffer_init(&r->inode, 0))
			return r;
		refer_free(r);
	}
	return NULL;
}

iphyee_bvh_sah_inode_t* iphyee_bvh_sah_inode_append(iphyee_bvh_sah_inode_s *restrict r, const iphyee_bvh_sah_inode_t *restrict inode)
{
	iphyee_bvh_sah_inode_t *restrict p;
	uintptr_t index;
	if (inode && exbuffer_append(&r->inode, inode, sizeof(iphyee_bvh_sah_inode_t)))
	{
		p = (iphyee_bvh_sah_inode_t *) r->inode.data;
		index = r->inode_count;
		r->inode_array = p;
		r->inode_count = index + 1;
		return p + index;
	}
	return NULL;
}
