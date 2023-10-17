#include "adora.h"
#include <hashmap.h>
#include <rbtree.h>
#include <vattr.h>

struct abc_adora_isa_s {
	hashmap_t function;
	refer_nstring_t arch;
	vattr_s *iset;
	vattr_s *instr;
	rbtree_t *ilink;
	abc_adora_isa_t isa;
	uint32_t version;
	uint32_t error;
	uint32_t initial;
	uint32_t finally;
};

static void abc_adora_isa_rbtree_free_func(rbtree_t *restrict rbv)
{
	if (rbv->value) refer_free(rbv->value);
}

// func

static void abc_adora_isa__register_iset__func(abc_adora_isa_s *restrict isa, const char *restrict iset_name, uint64_t iset_flag)
{
	const abc_adora_iset_s *restrict iset;
	vattr_vlist_t *restrict vl;
	vl = NULL;
	if (iset_name && !vattr_get_vlist_first(isa->iset, iset_name) &&
		(iset = abc_adora_iset_alloc(iset_name, iset_flag)))
	{
		vl = vattr_set(isa->iset, iset_name, iset);
		refer_free(iset);
	}
	if (!vl) isa->error = 1;
}
static void abc_adora_isa__register_instr__func(abc_adora_isa_s *restrict isa, const char *restrict instr_name, const abc_adora_instr_t *restrict instr)
{
	const abc_adora_instr_s *restrict i;
	vattr_vlist_t *restrict vl;
	refer_nstring_t name;
	uintptr_t index;
	vl = NULL;
	if (instr_name && instr)
	{
		if ((vl = vattr_get_vlist_first(isa->instr, instr_name)))
		{
			i = (const abc_adora_instr_s *) vl->value;
			name = (refer_nstring_t) refer_save(i->instr_name);
			index = vl->vslot->number;
			vl = NULL;
		}
		else
		{
			name = refer_dump_nstring(instr_name);
			index = 0;
		}
		if (name)
		{
			if ((i = abc_adora_instr_alloc(name, index, instr)))
			{
				vl = vattr_insert_tail(isa->instr, instr_name, i);
				refer_free(i);
			}
			refer_free(name);
		}
	}
	if (!vl) isa->error = 1;
}
static void abc_adora_isa__link_instr__func(abc_adora_isa_s *restrict isa, uint64_t instr_id, uint64_t exist_iset_flags, const char *restrict instr_name)
{
	const abc_adora_ilink_s *restrict ilink;
	const abc_adora_instr_s *restrict i;
	rbtree_t *restrict rbv;
	rbv = NULL;
	if (instr_name && !rbtree_find(&isa->ilink, NULL, instr_id) &&
		(i = (const abc_adora_instr_s *) vattr_get_first(isa->instr, instr_name)) &&
		(ilink = abc_adora_ilink_alloc(i->instr_name, exist_iset_flags)))
	{
		if (!(rbv = rbtree_insert(&isa->ilink, NULL, instr_id, ilink, abc_adora_isa_rbtree_free_func)))
			refer_free(ilink);
	}
	if (!rbv) isa->error = 1;
}

// isa func

static uint32_t abc_adora_isa_version_func(abc_adora_isa_s *restrict isa)
{
	return isa->version;
}
static abc_adora_isa__none__f abc_adora_isa_function_func(abc_adora_isa_s *restrict isa, const char *restrict name)
{
	if (name) return (abc_adora_isa__none__f) hashmap_find_name(&isa->function, name);
	return NULL;
}
static void abc_adora_isa_finally_func(abc_adora_isa_s *restrict isa, const char *restrict arch)
{
	if (arch && !isa->arch && !isa->finally && (isa->arch = refer_dump_nstring(arch)))
		isa->finally = 1;
	else isa->error = 1;
}

// isa

static void abc_adora_isa_free_func(abc_adora_isa_s *restrict r)
{
	if (r->arch) refer_free(r->arch);
	if (r->iset) refer_free(r->iset);
	if (r->instr) refer_free(r->instr);
	rbtree_clear(&r->ilink);
	hashmap_uini(&r->function);
}

static abc_adora_isa_s* abc_adora_isa_alloc_empty(void)
{
	abc_adora_isa_s *restrict r;
	if ((r = (abc_adora_isa_s *) refer_alloz(sizeof(abc_adora_isa_s))))
	{
		refer_set_free(r, (refer_free_f) abc_adora_isa_free_func);
		#define d_func(_name)  hashmap_set_name(&r->function, #_name, abc_adora_isa__##_name##__func, NULL)
		if (hashmap_init(&r->function) &&
			(r->iset = vattr_alloc()) &&
			(r->instr = vattr_alloc()) &&
			d_func(register_iset) &&
			d_func(register_instr) &&
			d_func(link_instr))
		{
			r->isa.version = abc_adora_isa_version_func;
			r->isa.function = abc_adora_isa_function_func;
			r->isa.finally = abc_adora_isa_finally_func;
			r->version = abc_adora_isa_version;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

// api

abc_adora_isa_s* abc_adora_isa_alloc(abc_adora_isa_initial_f initial)
{
	abc_adora_isa_s *restrict r;
	if ((r = abc_adora_isa_alloc_empty()))
	{
		if (initial)
		{
			initial(r, &r->isa);
			r->initial = 1;
		}
		if (!r->error && r->initial && r->finally)
			return r;
		refer_free(r);
	}
	return NULL;
}

refer_nstring_t abc_adora_isa_arch(const abc_adora_isa_s *restrict isa)
{
	return isa->arch;
}

uint64_t abc_adora_isa_iset_flag(const abc_adora_isa_s *restrict isa, const char *restrict iset_name)
{
	const abc_adora_iset_s *restrict iset;
	if (iset_name && (iset = (const abc_adora_iset_s *) vattr_get_first(isa->iset, iset_name)))
		return iset->iset_flag;
	return 0;
}

const abc_adora_instr_s* abc_adora_isa_match_instr(const abc_adora_isa_s *restrict isa, uint64_t iset_flags, uint64_t instr_id, uintptr_t vtype_count, const abc_adora_vtype_t *restrict vtype_array)
{
	const abc_adora_ilink_s *restrict ilink;
	const abc_adora_instr_s *restrict instr;
	rbtree_t *restrict rbv;
	vattr_vlist_t *restrict vl;
	uint64_t exist_iset_flags;
	if ((rbv = rbtree_find(&isa->ilink, NULL, instr_id)) &&
		(ilink = (const abc_adora_ilink_s *) rbv->value))
	{
		exist_iset_flags = ilink->exist_iset_flags;
		if ((iset_flags & exist_iset_flags) == exist_iset_flags)
		{
			vl = vattr_get_vlist_first(isa->instr, ilink->instr_name->string);
			while (vl)
			{
				instr = (const abc_adora_instr_s *) vl->value;
				if (abc_adora_instr_match(instr, vtype_count, vtype_array))
					return instr;
				vl = vl->vslot_next;
			}
		}
	}
	return NULL;
}
