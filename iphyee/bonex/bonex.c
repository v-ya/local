#include "bonex.h"

static void iphyee_bonex_free_func(iphyee_bonex_s *restrict r)
{
	if (r->joint) refer_free(r->joint);
	if (r->coord) refer_free(r->coord);
	if (r->inode) refer_free(r->inode);
}

iphyee_bonex_s* iphyee_bonex_alloc(uintptr_t joint_nsize, uintptr_t coord_nsize, uintptr_t inode_nsize)
{
	iphyee_bonex_s *restrict r;
	uintptr_t size;
	size = sizeof(iphyee_bonex_s) + (joint_nsize + coord_nsize + inode_nsize) * sizeof(refer_t);
	if ((r = (iphyee_bonex_s *) refer_alloz(size)))
	{
		refer_set_free(r, (refer_free_f) iphyee_bonex_free_func);
		if ((r->joint = vattr_alloc()) && (r->coord = vattr_alloc()) && (r->inode = vattr_alloc()))
		{
			r->joint_nsize = joint_nsize;
			r->coord_nsize = coord_nsize;
			r->inode_nsize = inode_nsize;
			r->joint_array = (iphyee_bonex_joint_s **) (r + 1);
			r->coord_array = (iphyee_bonex_coord_s **) (r->joint_array + joint_nsize);
			r->inode_array = (iphyee_bonex_inode_s **) (r->coord_array + coord_nsize);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

#define d_find_field(_field)  {\
		return (iphyee_bonex_##_field##_s *) vattr_get_first(r->_field, name);\
	}
iphyee_bonex_joint_s* iphyee_bonex_find_joint(iphyee_bonex_s *restrict r, const char *restrict name)
	d_find_field(joint)
iphyee_bonex_coord_s* iphyee_bonex_find_coord(iphyee_bonex_s *restrict r, const char *restrict name)
	d_find_field(coord)
iphyee_bonex_inode_s* iphyee_bonex_find_inode(iphyee_bonex_s *restrict r, const char *restrict name)
	d_find_field(inode)
#undef d_find_field

#define d_insert_field(_field)  {\
		vattr_vlist_t *restrict vpos;\
		vattr_vlist_t *restrict item;\
		refer_string_t base;\
		if (_field && !vattr_get_vslot(r->_field, _field->name) &&\
			(!(base = _field->base) || vattr_get_vslot(r->joint, base)))\
		{\
			for (vpos = r->_field->vattr; vpos; vpos = vpos->vattr_next)\
			{\
				if (((iphyee_bonex_##_field##_s *) vpos->value)->base == base)\
				{\
					while ((item = vpos->vattr_next) && (((iphyee_bonex_##_field##_s *) item->value)->base == base))\
						vpos = item;\
					break;\
				}\
			}\
			if ((item = vattr_insert_tail(r->_field, _field->name, _field)))\
			{\
				if (vpos) vattr_moveto_vattr_next(vpos, item);\
				return r;\
			}\
		}\
		return NULL;\
	}
static iphyee_bonex_s* iphyee_bonex_insert_joint(iphyee_bonex_s *restrict r, iphyee_bonex_joint_s *restrict joint)
	d_insert_field(joint)
static iphyee_bonex_s* iphyee_bonex_insert_coord(iphyee_bonex_s *restrict r, iphyee_bonex_coord_s *restrict coord)
	d_insert_field(coord)
static iphyee_bonex_s* iphyee_bonex_insert_inode(iphyee_bonex_s *restrict r, iphyee_bonex_inode_s *restrict inode)
	d_insert_field(inode)
#undef d_insert_field

iphyee_bonex_s* iphyee_bonex_append_bonex(iphyee_bonex_s *restrict r, const iphyee_bonex_s *restrict bonex)
{
	vattr_vlist_t *restrict vl;
	refer_t v;
	if (!r->bonex_index_okay && bonex)
	{
		#define d_append_field(_field)  \
			for (vl = bonex->_field->vattr; vl; vl = vl->vattr_next)\
			{\
				if (!(v = iphyee_bonex_##_field##_dump((const iphyee_bonex_##_field##_s *) vl->value)))\
					goto label_fail;\
				if (!iphyee_bonex_insert_##_field(r, (iphyee_bonex_##_field##_s *) v))\
					goto label_fail_free_v;\
				refer_free(v);\
			}
		d_append_field(joint)
		d_append_field(coord)
		d_append_field(inode)
		#undef d_append_field
		return r;
	}
	label_fail:
	return NULL;
	label_fail_free_v:
	refer_free(v);
	return NULL;
}

iphyee_bonex_joint_s* iphyee_bonex_append_joint(iphyee_bonex_s *restrict r, const char *this_joint_name, const char *base_joint_name)
{
	iphyee_bonex_joint_s *restrict joint;
	iphyee_bonex_joint_s *restrict base;
	if (!r->bonex_index_okay && this_joint_name)
	{
		base = NULL;
		if (base_joint_name)
		{
			if (!(base = iphyee_bonex_find_joint(r, base_joint_name)))
				goto label_fail;
		}
		if ((joint = iphyee_bonex_joint_alloc(this_joint_name, base)))
		{
			r = iphyee_bonex_insert_joint(r, joint);
			refer_free(joint);
			if (r) return joint;
		}
	}
	label_fail:
	return NULL;
}

iphyee_bonex_coord_s* iphyee_bonex_append_coord(iphyee_bonex_s *restrict r, const iphyee_bonex_joint_s *restrict joint, const iphyee_param_bonex_coord_t *restrict param)
{
	iphyee_bonex_coord_s *restrict coord;
	if (!r->bonex_index_okay && (coord = iphyee_bonex_coord_alloc(param, joint)))
	{
		r = iphyee_bonex_insert_coord(r, coord);
		refer_free(coord);
		if (r) return coord;
	}
	return NULL;
}

iphyee_bonex_inode_s* iphyee_bonex_append_inode(iphyee_bonex_s *restrict r, const iphyee_bonex_joint_s *restrict joint, const iphyee_param_bonex_inode_t *restrict param)
{
	iphyee_bonex_inode_s *restrict inode;
	if (!r->bonex_index_okay && (inode = iphyee_bonex_inode_alloc(param, joint)))
	{
		r = iphyee_bonex_insert_inode(r, inode);
		refer_free(inode);
		if (r) return inode;
	}
	return NULL;
}

static uintptr_t* iphyee_bonex_okay_link_udpate(uintptr_t *restrict start, uintptr_t *restrict count, uintptr_t index)
{
	if (*start + *count == index)
	{
		*count += 1;
		return count;
	}
	else if (!~*start && !*count)
	{
		*start = index;
		*count = 1;
		return start;
	}
	return NULL;
}

static iphyee_bonex_s* iphyee_bonex_okay_joint(iphyee_bonex_s *restrict r, iphyee_bonex_joint_s *restrict joint)
{
	iphyee_bonex_joint_s *restrict base;
	uintptr_t p;
	if ((p = r->joint_count) < r->joint_nsize)
	{
		base = NULL;
		if (joint->base && !(base = iphyee_bonex_find_joint(r, joint->base)))
			goto label_fail;
		if (base && base->index.this_joint_index >= p)
			goto label_fail;
		joint->index.this_joint_index = p;
		joint->index.base_joint_index = base?base->index.this_joint_index:~(uintptr_t) 0;
		r->joint_array[p] = joint;
		r->joint_count = p + 1;
		if (base && !iphyee_bonex_okay_link_udpate(&base->index.link_joint_start, &base->index.link_joint_count, p))
			goto label_fail;
		return r;
	}
	label_fail:
	return NULL;
}

static iphyee_bonex_s* iphyee_bonex_okay_coord(iphyee_bonex_s *restrict r, iphyee_bonex_coord_s *restrict coord)
{
	iphyee_bonex_joint_s *restrict base;
	uintptr_t p;
	if ((p = r->coord_count) < r->coord_nsize)
	{
		if (!(base = iphyee_bonex_find_joint(r, coord->base)))
			goto label_fail;
		coord->index.base_joint_index = base->index.this_joint_index;
		coord->index.this_coord_index = p;
		r->coord_array[p] = coord;
		r->coord_count = p + 1;
		if (!iphyee_bonex_okay_link_udpate(&base->index.link_coord_start, &base->index.link_coord_count, p))
			goto label_fail;
		return r;
	}
	label_fail:
	return NULL;
}

static iphyee_bonex_s* iphyee_bonex_okay_inode(iphyee_bonex_s *restrict r, iphyee_bonex_inode_s *restrict inode)
{
	iphyee_bonex_joint_s *restrict base;
	uintptr_t p;
	if ((p = r->inode_count) < r->inode_nsize)
	{
		if (!(base = iphyee_bonex_find_joint(r, inode->base)))
			goto label_fail;
		inode->index.base_joint_index = base->index.this_joint_index;
		inode->index.this_inode_index = p;
		r->inode_array[p] = inode;
		r->inode_count = p + 1;
		if (!iphyee_bonex_okay_link_udpate(&base->index.link_inode_start, &base->index.link_inode_count, p))
			goto label_fail;
		return r;
	}
	label_fail:
	return NULL;
}

static iphyee_bonex_s* iphyee_bonex_okay_value(iphyee_bonex_s *restrict r, iphyee_bonex_joint_s *restrict joint)
{
	iphyee_bonex_coord_s **restrict pc;
	iphyee_bonex_inode_s **restrict pi;
	iphyee_bonex_coord_s *restrict coord;
	iphyee_bonex_inode_s *restrict inode;
	uintptr_t i, n, fixed;
	joint->index.link_value_start = r->value_count;
	joint->index.link_value_count = 0;
	joint->index.fix_mat4x4_start = r->fix_mat4x4_count;
	joint->index.fix_mat4x4_count = 0;
	#define d_done_loop(_field, _pp)  \
		if ((n = joint->index.link_##_field##_count))\
		{\
			i = joint->index.link_##_field##_start;\
			_pp = r->_field##_array;\
			fixed = 0;\
			for (n += i; i < n; ++i)\
			{\
				_field = _pp[i];\
				_field->index.this_value_index = r->value_count;\
				r->value_count += 1;\
				joint->index.link_value_count += 1;\
				if ((fixed = _field->property?0:(fixed + 1)) > 1)\
					_field->index.fix_mat4x4_index = _pp[i - 1]->index.fix_mat4x4_index;\
				else\
				{\
					_field->index.fix_mat4x4_index = r->fix_mat4x4_count;\
					r->fix_mat4x4_count += 1;\
					joint->index.fix_mat4x4_count += 1;\
				}\
			}\
		}
	d_done_loop(coord, pc)
	d_done_loop(inode, pi)
	#undef d_done_loop
	return r;
}

iphyee_bonex_s* iphyee_bonex_okay(iphyee_bonex_s *restrict r)
{
	vattr_vlist_t *restrict vl;
	if (!r->bonex_index_okay)
	{
		#define d_okay_field(_field, _method)  \
			for (vl = r->_field->vattr; vl; vl = vl->vattr_next)\
			{\
				if (!iphyee_bonex_okay_##_method(r, (iphyee_bonex_##_field##_s *) vl->value))\
					goto label_fail;\
			}
		d_okay_field(joint, joint)
		d_okay_field(coord, coord)
		d_okay_field(inode, inode)
		d_okay_field(joint, value)
		#undef d_okay_field
	}
	return r;
	label_fail:
	return NULL;
}
