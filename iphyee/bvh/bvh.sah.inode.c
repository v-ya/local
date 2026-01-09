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

static iphyee_bvh_sah_inode_t* iphyee_bvh_sah_inode_fetch_inner(iphyee_bvh_sah_inode_s *restrict r, uintptr_t inode_index, int32_t *restrict layer, uint32_t *restrict *restrict ppnext)
{
	iphyee_bvh_sah_inode_t *restrict pa;
	iphyee_bvh_sah_inode_t *restrict p;
	uint32_t *restrict pnext;
	uint32_t i, n;
	pa = r->inode_array;
	n = (uint32_t) r->inode_count;
	if (inode_index < (uintptr_t) n)
	{
		p = pa + inode_index;
		// clear list
		p->list = NULL;
		// get layer
		*layer = p->inode.layer + 1;
		if (*layer > r->layer_max)
			r->layer_max = *layer;
		// get pnext
		for (pnext = &p->inode.child; ~(i = *pnext); pnext = &pa[i].inode.next)
		{
			if (i >= n)
				goto label_fail;
		}
		*ppnext = pnext;
		return p;
	}
	label_fail:
	return NULL;
}

iphyee_bvh_sah_inode_s* iphyee_bvh_sah_inode_append_split(iphyee_bvh_sah_inode_s *restrict r, uintptr_t inode_index, const iphyee_glslc_bvh_box_t *restrict box, iphyee_bvh_sah_input_s *restrict list, uintptr_t list_count)
{
	iphyee_bvh_sah_inode_t inode;
	uint32_t *restrict pnext;
	int32_t layer;
	if (list && list_count && iphyee_bvh_sah_inode_fetch_inner(r, inode_index, &layer, &pnext))
	{
		inode.inode.parent = inode_index;
		inode.inode.next = *pnext;
		inode.inode.child = ~(uint32_t) 0;
		inode.inode.nleaf = (uint32_t) list_count;
		inode.inode.layer = layer;
		inode.inode.tri3index = ~(uint32_t) 0;
		inode.box = *box;
		inode.list = list;
		*pnext = (uint32_t) r->inode_count;
		if (iphyee_bvh_sah_inode_append(r, &inode))
			return r;
	}
	return NULL;
}

iphyee_bvh_sah_inode_s* iphyee_bvh_sah_inode_touch_tri3(iphyee_bvh_sah_inode_s *restrict r, uintptr_t inode_index, iphyee_bvh_sah_input_s *restrict list)
{
	iphyee_bvh_sah_inode_t *restrict pa;
	iphyee_bvh_sah_inode_t *restrict p;
	uint32_t *restrict pnext;
	uint32_t i, n;
	int32_t layer;
	if (list && iphyee_bvh_sah_inode_fetch_inner(r, inode_index, &layer, &pnext))
	{
		pa = r->inode_array;
		n = (uint32_t) r->inode_count;
		while (list)
		{
			if ((i = list->inode_index) >= n)
				goto label_fail;
			p = pa + i;
			p->inode.parent = inode_index;
			p->inode.next = *pnext;
			p->inode.nleaf = 1;
			p->inode.layer = layer;
			p->inode.tri3index = list->tri3index;
			p->list = NULL;
			*pnext = i;
			pnext = &p->inode.next;
			list = list->next;
		}
		return r;
	}
	label_fail:
	return NULL;
}
