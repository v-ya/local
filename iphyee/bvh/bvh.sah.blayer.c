#include "bvh.sah.h"

static void iphyee_bvh_sah_blayer_free_func(iphyee_bvh_sah_blayer_s *restrict r)
{
	iphyee_bvh_sah_bucket_s **restrict p;
	uintptr_t i, n;
	p = r->bucket_array;
	n = r->bucket_count;
	for (i = 0; i < n; ++i)
	{
		if (p[i])
			refer_free(p[i]);
	}
	if (r->inode) refer_free(r->inode);
	if (r->input) refer_free(r->input);
}

iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_alloc(uintptr_t bucket_number)
{
	iphyee_bvh_sah_blayer_s *restrict r;
	iphyee_bvh_sah_bucket_s **restrict p;
	uintptr_t i, n;
	if (bucket_number >= 2 && (r = (iphyee_bvh_sah_blayer_s *) refer_alloz(
		sizeof(iphyee_bvh_sah_blayer_s) + sizeof(iphyee_bvh_sah_bucket_s *) * 3 * bucket_number)))
	{
		refer_set_free(r, (refer_free_f) iphyee_bvh_sah_blayer_free_func);
		if ((r->input = iphyee_bvh_sah_bucket_alloc()) &&
			(r->inode = iphyee_bvh_sah_inode_alloc()))
		{
			r->bucket_number = bucket_number;
			r->bucket_count = n = bucket_number * 3;
			p = r->bucket_array;
			for (i = 0; i < n; ++i)
			{
				if (!(p[i] = iphyee_bvh_sah_bucket_alloc()))
					goto label_fail;
			}
			r->bucket_x = p;
			p += bucket_number;
			r->bucket_y = p;
			p += bucket_number;
			r->bucket_z = p;
			return r;
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}

iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_tri3(iphyee_bvh_sah_blayer_s *restrict r, const iphyee_glslc_model_vec3xyz_t *restrict v1, const iphyee_glslc_model_vec3xyz_t *restrict v2, const iphyee_glslc_model_vec3xyz_t *restrict v3, uint32_t tri3index)
{
	iphyee_glslc_bvh_box_t box;
	iphyee_glslc_model_vec3xyz_t pos;
	iphyee_bvh_sah_input_s *restrict input;
	uintptr_t index;
	iphyee_bvh_box_tri3(&box, v1, v2, v3);
	#define _avg3coord_(u)  pos.u = (v1->u + v2->u + v3->u) / 3
	_avg3coord_(x);
	_avg3coord_(y);
	_avg3coord_(z);
	#undef _avg3coord_
	index = r->input->bucket_count;
	if ((input = iphyee_bvh_sah_input_alloc(NULL, 0, &box, &pos, tri3index, index, index + 1)))
	{
		if (!iphyee_bvh_sah_bucket_append(r->input, input))
			r = NULL;
		refer_free(input);
		return r;
	}
	return NULL;
}

static void iphyee_bvh_sah_blayer_bucket3clear(iphyee_bvh_sah_blayer_s *restrict r)
{
	iphyee_bvh_sah_bucket_s **restrict p;
	uintptr_t i, n;
	p = r->bucket_array;
	n = r->bucket_count;
	for (i = 0; i < n; ++i)
		iphyee_bvh_sah_bucket_clear(p[i]);
}

static iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_bucket3fill(iphyee_bvh_sah_blayer_s *restrict r, iphyee_bvh_sah_input_s *restrict list, const iphyee_glslc_bvh_box_t *restrict box)
{
	uintptr_t i, n;
	intptr_t j;
	float xa, ya, za, kx, ky, kz;
	n = r->bucket_number;
	xa = box->p1.x;
	ya = box->p1.y;
	za = box->p1.z;
	kx = n / (box->p2.x - xa);
	ky = n / (box->p2.y - ya);
	kz = n / (box->p2.z - za);
	n -= 1;
	while (list)
	{
		#define _push_(u) \
			j = (intptr_t) ((list->pos.u - u##a) * k##u);\
			if (j < 0) j = 0;\
			i = (uintptr_t) j;\
			if (i > n) i = n;\
			if (!iphyee_bvh_sah_bucket_append(r->bucket_##u[i], list))\
				goto label_fail
		_push_(x);
		_push_(y);
		_push_(z);
		#undef _push_
		list = list->next;
	}
	return r;
	label_fail:
	return NULL;
}

typedef struct iphyee_bvh_sah_blayer_split_t iphyee_bvh_sah_blayer_split_t;
struct iphyee_bvh_sah_blayer_split_t {
	iphyee_bvh_sah_bucket_s **bucket_array;
	uintptr_t bucket_number;
	uintptr_t split_middle; // 0 < split_middle < bucket_number
	double cost_min;        // (Sa * Na + Sb * Nb) / (Sn * Nn)
};

static double iphyee_bvh_sah_blayer_bucket3split_cost(iphyee_bvh_sah_bucket_s **restrict p, uintptr_t m, uintptr_t n, double sn)
{
	iphyee_glslc_bvh_box_t boxa, boxb;
	uintptr_t i, na, nb;
	iphyee_bvh_box_initial(&boxa);
	iphyee_bvh_box_initial(&boxb);
	na = nb = 0;
	#define _scan_(u, ia, ib)  \
		for (i = ia; i < ib; ++i)\
		{\
			iphyee_bvh_box_append(&box##u, &p[i]->box);\
			n##u += p[i]->bucket_count;\
		}
	_scan_(a, 0, m)
	_scan_(b, m, n)
	#undef _scan_
	if (na && nb)
		return (iphyee_bvh_box_area(&boxa) * na +
			iphyee_bvh_box_area(&boxb) * nb) /
			(sn * (na + nb));
	return 2;
}

static iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_bucket3split(iphyee_bvh_sah_blayer_s *restrict r, iphyee_bvh_sah_blayer_split_t *restrict split, const iphyee_glslc_bvh_box_t *restrict box)
{
	iphyee_bvh_sah_bucket_s **restrict p;
	uintptr_t n, m;
	double cost, sn;
	n = r->bucket_number;
	split->bucket_array = NULL;
	split->bucket_number = n;
	split->split_middle = 0;
	split->cost_min = 1;
	sn = iphyee_bvh_box_area(box);
	for (m = 1; m < n; ++m)
	{
		#define _cost_min_set_(u)  \
			if ((cost = iphyee_bvh_sah_blayer_bucket3split_cost(p = r->bucket_##u, m, n, sn)) < split->cost_min)\
			{\
				split->bucket_array = p;\
				split->split_middle = m;\
				split->cost_min = cost;\
			}
		_cost_min_set_(x)
		_cost_min_set_(y)
		_cost_min_set_(z)
		#undef _cost_min_set_
	}
	return split->bucket_array?r:NULL;
}

static iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_split(iphyee_bvh_sah_blayer_s *restrict r, iphyee_bvh_sah_blayer_split_t *restrict split, uintptr_t inode_index)
{
	iphyee_glslc_bvh_box_t boxa, boxb;
	iphyee_bvh_sah_bucket_s **restrict p;
	iphyee_bvh_sah_input_s **plist;
	iphyee_bvh_sah_input_s *alist;
	iphyee_bvh_sah_input_s *blist;
	uintptr_t i, n, m, na, nb;
	p = split->bucket_array;
	n = split->bucket_number;
	m = split->split_middle;
	iphyee_bvh_box_initial(&boxa);
	iphyee_bvh_box_initial(&boxb);
	alist = blist = NULL;
	na = nb = 0;
	#define _scan_(u, ia, ib)  \
		plist = &u##list;\
		for (i = ia; i < ib; ++i)\
		{\
			iphyee_bvh_box_append(&box##u, &p[i]->box);\
			n##u += p[i]->bucket_count;\
			plist = iphyee_bvh_sah_bucket_list(p[i], plist);\
		}
	#define _split_(u)  \
		if (u##list)\
		{\
			if (n##u > 1)\
			{\
				if (!iphyee_bvh_sah_inode_append_split(r->inode, inode_index, &box##u, u##list, n##u))\
					goto label_fail;\
			}\
			else\
			{\
				if (!iphyee_bvh_sah_inode_touch_tri3(r->inode, inode_index, u##list))\
					goto label_fail;\
			}\
		}
	_scan_(a, 0, m)
	_scan_(b, m, n)
	_split_(a)
	_split_(b)
	#undef _scan_
	#undef _split_
	return r;
	label_fail:
	return NULL;
}

static iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_build_initial(iphyee_bvh_sah_blayer_s *restrict r)
{
	iphyee_bvh_sah_inode_t inode;
	iphyee_bvh_sah_bucket_s *restrict input;
	iphyee_bvh_sah_input_s **restrict p;
	uintptr_t i, n;
	input = r->input;
	iphyee_bvh_inode_initial(&inode.inode);
	// push root
	inode.inode.nleaf = input->bucket_count;
	inode.box = input->box;
	inode.list = NULL;
	iphyee_bvh_sah_bucket_list(input, &inode.list);
	if (!iphyee_bvh_sah_inode_append(r->inode, &inode))
		goto label_fail;
	// push tri3
	inode.inode.nleaf = 1;
	inode.list = NULL;
	p = input->bucket_array;
	n = input->bucket_count;
	for (i = 0; i < n; ++i)
	{
		inode.inode.tri3index = p[i]->tri3index;
		inode.box = p[i]->box;
		if (p[i]->inode_index != r->inode->inode_count)
			goto label_fail;
		if (!iphyee_bvh_sah_inode_append(r->inode, &inode))
			goto label_fail;
	}
	return r;
	label_fail:
	return NULL;
}

static iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_build_inner(iphyee_bvh_sah_blayer_s *restrict r)
{
	iphyee_bvh_sah_inode_s *restrict inode;
	iphyee_bvh_sah_inode_t *restrict p;
	iphyee_bvh_sah_blayer_split_t split;
	uintptr_t i, n;
	inode = r->inode;
	i = 0;
	do {
		n = (volatile uintptr_t) inode->inode_count;
		while (i < n)
		{
			p = inode->inode_array + i;
			if (p->list)
			{
				if (p->inode.nleaf > 2)
				{
					if (!iphyee_bvh_sah_blayer_bucket3fill(r, p->list, &p->box))
						goto label_fail;
					if (iphyee_bvh_sah_blayer_bucket3split(r, &split, &p->box))
					{
						if (!iphyee_bvh_sah_blayer_split(r, &split, i))
							goto label_fail;
					}
					else if (!iphyee_bvh_sah_inode_touch_tri3(inode, i, p->list))
						goto label_fail;
					iphyee_bvh_sah_blayer_bucket3clear(r);
				}
				else
				{
					if (!iphyee_bvh_sah_inode_touch_tri3(inode, i, p->list))
						goto label_fail;
				}
			}
			i += 1;
		}
	} while (n < (volatile uintptr_t) inode->inode_count);
	return r;
	label_fail:
	return NULL;
}

iphyee_bvh_sah_blayer_s* iphyee_bvh_sah_blayer_build(iphyee_bvh_sah_blayer_s *restrict r)
{
	if (iphyee_bvh_sah_blayer_build_initial(r) &&
		iphyee_bvh_sah_blayer_build_inner(r))
		return r;
	iphyee_bvh_sah_blayer_bucket3clear(r);
	return NULL;
}
