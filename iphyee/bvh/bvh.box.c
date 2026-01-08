#include "bvh.h"
#include <math.h>

#define _min2_(x, y)     ((x < y)?x:y)
#define _max2_(x, y)     ((x > y)?x:y)
#define _min3_(x, y, z)  _min2_(_min2_(x, y), z)
#define _max3_(x, y, z)  _max2_(_max2_(x, y), z)

void iphyee_bvh_box_tri3(iphyee_glslc_bvh_box_t *restrict box, const iphyee_glslc_model_vec3xyz_t *restrict v1, const iphyee_glslc_model_vec3xyz_t *restrict v2, const iphyee_glslc_model_vec3xyz_t *restrict v3)
{
	#define _v3coord_min_(u)  box->p1.u = _min3_(v1->u, v2->u, v3->u)
	#define _v3coord_max_(u)  box->p2.u = _max3_(v1->u, v2->u, v3->u)
	_v3coord_min_(x);
	_v3coord_min_(y);
	_v3coord_min_(z);
	_v3coord_max_(x);
	_v3coord_max_(y);
	_v3coord_max_(z);
	#undef _v3coord_min_
	#undef _v3coord_max_
}

#undef _min2_
#undef _max2_
#undef _min3_
#undef _max3_

void iphyee_bvh_box_initial(iphyee_glslc_bvh_box_t *restrict box)
{
	box->p1.z = box->p1.y = box->p1.x = INFINITY;
	box->p2.z = box->p2.y = box->p2.x = -INFINITY;
}

void iphyee_bvh_box_append(iphyee_glslc_bvh_box_t *restrict dst, const iphyee_glslc_bvh_box_t *restrict src)
{
	#define _compare_set_(p, u, op)  if (src->p.u op dst->p.u) dst->p.u = src->p.u
	_compare_set_(p1, x, <);
	_compare_set_(p1, y, <);
	_compare_set_(p1, z, <);
	_compare_set_(p2, x, >);
	_compare_set_(p2, y, >);
	_compare_set_(p2, z, >);
	#undef _compare_set_
}

double iphyee_bvh_box_area(const iphyee_glslc_bvh_box_t *restrict box)
{
	#define _d3coord_(u)  (box->p2.u - box->p1.u)
	double dx, dy, dz;
	if ((dx = (double) _d3coord_(x)) < 0) dx = 0;
	if ((dy = (double) _d3coord_(y)) < 0) dy = 0;
	if ((dz = (double) _d3coord_(z)) < 0) dz = 0;
	return (dx * dy + dy * dz + dz * dx) * 2;
	#undef _d3coord_
}
