#include "../vkaa.oplevel.h"

typedef struct vkaa_oplevel_list_s vkaa_oplevel_list_s;

struct vkaa_oplevel_list_s {
	vkaa_oplevel_list_s *next;
	vkaa_oplevel_list_s **p_next;
	vkaa_oplevel_list_s ***pp_tail;
	vkaa_level_s *level;
};

struct vkaa_oplevel_s {
	hashmap_t finder;
	vkaa_oplevel_list_s *list;
	vkaa_oplevel_list_s **p_tail;
};

static void vkaa_oplevel_list_drop(vkaa_oplevel_list_s *restrict r)
{
	if (*r->pp_tail == &r->next) *r->pp_tail = r->p_next;
	if (r->p_next) *r->p_next = r->next;
	if (r->next) r->next->p_next = r->p_next;
	r->next = NULL;
	r->p_next = NULL;
}

static void vkaa_oplevel_list_link(vkaa_oplevel_list_s **restrict p_next, vkaa_oplevel_list_s *restrict r)
{
	vkaa_oplevel_list_drop(r);
	r->p_next = p_next;
	if ((r->next = *p_next)) (*p_next)->p_next = &r->next;
	else *r->pp_tail = &r->next;
	*p_next = r;
}

static void vkaa_oplevel_list_free_func(vkaa_oplevel_list_s *restrict r)
{
	vkaa_oplevel_list_drop(r);
	if (r->level)
	{
		r->level = 0;
		refer_free(r->level);
	}
}

static vkaa_oplevel_list_s* vkaa_oplevel_list_alloc(vkaa_oplevel_list_s ***pp_tail)
{
	vkaa_oplevel_list_s *restrict r;
	if ((r = (vkaa_oplevel_list_s *) refer_alloz(sizeof(vkaa_oplevel_list_s))))
	{
		r->pp_tail = pp_tail;
		refer_set_free(r, (refer_free_f) vkaa_oplevel_list_free_func);
		if ((r->level = (vkaa_level_s *) refer_alloz(sizeof(vkaa_level_s))))
			return r;
		refer_free(r);
	}
	return NULL;
}

static void vkaa_oplevel_free_func(vkaa_oplevel_s *restrict r)
{
	hashmap_uini(&r->finder);
}

vkaa_oplevel_s* vkaa_oplevel_alloc(void)
{
	vkaa_oplevel_s *restrict r;
	if ((r = (vkaa_oplevel_s *) refer_alloz(sizeof(vkaa_oplevel_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_oplevel_free_func);
		r->p_tail = &r->list;
		if (hashmap_init(&r->finder))
			return r;
		refer_free(r);
	}
	return NULL;
}

void vkaa_oplevel_clear(vkaa_oplevel_s *restrict oplevel)
{
	hashmap_clear(&oplevel->finder);
}

static void vkaa_oplevel_fix(vkaa_oplevel_s *restrict r, vkaa_oplevel_list_s *restrict list)
{
	uintptr_t level;
	level = 0;
	if (list->p_next != &r->list)
		level = ((vkaa_oplevel_list_s *) ((uintptr_t) list->p_next - offsetof(vkaa_oplevel_list_s, next)))->level->level;
	while (list)
	{
		list->level->level = ++level;
		list = list->next;
	}
}

static void vkaa_oplevel_hashmap_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value) refer_free(vl->value);
}

static vkaa_level_s* vkaa_oplevel_insert(vkaa_oplevel_s *restrict r, vkaa_oplevel_list_s **restrict p_next, const char *restrict name)
{
	if (p_next && !hashmap_find_name(&r->finder, name))
	{
		vkaa_oplevel_list_s *restrict list;
		if ((list = vkaa_oplevel_list_alloc(&r->p_tail)))
		{
			if (hashmap_set_name(&r->finder, name, list, vkaa_oplevel_hashmap_free_func))
			{
				vkaa_oplevel_list_link(p_next, list);
				vkaa_oplevel_fix(r, list);
				return list->level;
			}
			refer_free(list);
		}
	}
	return NULL;
}

const vkaa_level_s* vkaa_oplevel_get(vkaa_oplevel_s *restrict oplevel, const char *restrict name)
{
	vkaa_oplevel_list_s *restrict t;
	if ((t = (vkaa_oplevel_list_s *) hashmap_get_name(&oplevel->finder, name)))
		return t->level;
	return NULL;
}

const vkaa_level_s* vkaa_oplevel_insert_first(vkaa_oplevel_s *restrict oplevel, const char *restrict name)
{
	return vkaa_oplevel_insert(oplevel, &oplevel->list, name);
}

const vkaa_level_s* vkaa_oplevel_insert_tail(vkaa_oplevel_s *restrict oplevel, const char *restrict name)
{
	return vkaa_oplevel_insert(oplevel, oplevel->p_tail, name);
}

const vkaa_level_s* vkaa_oplevel_insert_last(vkaa_oplevel_s *restrict oplevel, const char *restrict name, const char *restrict target)
{
	vkaa_oplevel_list_s *restrict t;
	if ((t = (vkaa_oplevel_list_s *) hashmap_get_name(&oplevel->finder, target)))
		return vkaa_oplevel_insert(oplevel, t->p_next, name);
	return NULL;
}

const vkaa_level_s* vkaa_oplevel_insert_next(vkaa_oplevel_s *restrict oplevel, const char *restrict name, const char *restrict target)
{
	vkaa_oplevel_list_s *restrict t;
	if ((t = (vkaa_oplevel_list_s *) hashmap_get_name(&oplevel->finder, target)))
		return vkaa_oplevel_insert(oplevel, &t->next, name);
	return NULL;
}

void vkaa_oplevel_remove(vkaa_oplevel_s *restrict oplevel, const char *restrict name)
{
	hashmap_delete_name(&oplevel->finder, name);
}
