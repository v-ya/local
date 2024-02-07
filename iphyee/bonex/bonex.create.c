#include "bonex.h"

static void iphyee_bonex_create_get_count(const iphyee_param_bonex_t *restrict param, uintptr_t *restrict coord_nsize, uintptr_t *restrict inode_nsize)
{
	const iphyee_param_bonex_joint_t *restrict p;
	uintptr_t i, n;
	*coord_nsize = *inode_nsize = 0;
	p = param->joint_array;
	n = param->joint_count;
	for (i = 0; i < n; ++i)
	{
		*coord_nsize += p[i].base_coord_count;
		*inode_nsize += p[i].this_inode_count;
	}
}

static iphyee_bonex_s* iphyee_bonex_create_append(iphyee_bonex_s *restrict r, const iphyee_param_bonex_t *restrict param)
{
	const iphyee_param_bonex_joint_t *restrict p;
	const iphyee_param_bonex_coord_t *restrict pc;
	const iphyee_param_bonex_inode_t *restrict pi;
	const iphyee_bonex_joint_s *restrict joint;
	uintptr_t i, n, j, m;
	p = param->joint_array;
	n = param->joint_count;
	for (i = 0; i < n; ++i)
	{
		if (!(joint = iphyee_bonex_append_joint(r, p[i].this_joint_name, p[i].base_joint_name)))
			goto label_fail;
		pc = p[i].base_coord_array;
		m = p[i].base_coord_count;
		for (j = 0; j < m; ++j)
		{
			if (!iphyee_bonex_append_coord(r, joint, &pc[j]))
				goto label_fail;
		}
		pi = p[i].this_inode_array;
		m = p[i].this_inode_count;
		for (j = 0; j < m; ++j)
		{
			if (!iphyee_bonex_append_inode(r, joint, &pi[j]))
				goto label_fail;
		}
	}
	return r;
	label_fail:
	return NULL;
}

iphyee_bonex_s* iphyee_bonex_create(vattr_s *restrict bonex_pool, const iphyee_param_bonex_t *restrict param)
{
	const iphyee_bonex_s *restrict base;
	iphyee_bonex_s *restrict r;
	uintptr_t joint_nsize;
	uintptr_t coord_nsize;
	uintptr_t inode_nsize;
	base = NULL;
	if (!bonex_pool || !param || !param->this_bonex_name || vattr_get_first(bonex_pool, param->this_bonex_name))
		goto label_fail;
	if (param->base_bonex_name && !(base = (const iphyee_bonex_s *) vattr_get_first(bonex_pool, param->base_bonex_name)))
		goto label_fail;
	joint_nsize = param->joint_count;
	iphyee_bonex_create_get_count(param, &coord_nsize, &inode_nsize);
	if (base)
	{
		joint_nsize += base->joint_count;
		coord_nsize += base->coord_count;
		inode_nsize += base->inode_count;
	}
	if ((r = iphyee_bonex_alloc(joint_nsize, coord_nsize, inode_nsize)))
	{
		if ((!base || iphyee_bonex_append_bonex(r, base)) &&
			iphyee_bonex_create_append(r, param) &&
			iphyee_bonex_okay(r) &&
			vattr_insert_tail(bonex_pool, param->this_bonex_name, r))
			bonex_pool = NULL;
		refer_free(r);
		if (!bonex_pool)
			return r;
	}
	label_fail:
	return NULL;
}
