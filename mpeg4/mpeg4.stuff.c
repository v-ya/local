#include "mpeg4.stuff.h"
#include "mpeg4.atom.h"
#include <stdlib.h>

static inline void mpeg4_stuff_link_set(register mpeg4_stuff_t *restrict stuff, register mpeg4_stuff_t **restrict pp_pos, register mpeg4_stuff_t **restrict pp_same_pos, mpeg4_stuff_t *restrict parent, mpeg4_stuff_container_type_finder_t *restrict finder)
{
	register mpeg4_stuff_link_t *restrict link;
	link = &stuff->link;
	// list
	link->next = *(link->p_next = pp_pos);
	*link->p_next = stuff;
	// same list
	link->same_next = *(link->p_same_next = pp_same_pos);
	*link->p_same_next = stuff;
	// parent && finder
	link->container = parent;
	link->finder = finder;
}

static inline void mpeg4_stuff_link_unset(register mpeg4_stuff_link_t *restrict link)
{
	if ((*link->p_same_next = link->same_next))
		link->same_next->link.p_same_next = link->p_same_next;
	else link->finder->p_same_tail = link->p_same_next;
	if ((*link->p_next = link->next))
		link->next->link.p_next = link->p_next;
	else link->container->container.p_tail = link->p_next;
	link->next = NULL;
	link->same_next = NULL;
	link->p_next = NULL;
	link->p_same_next = NULL;
	link->container = NULL;
	link->finder = NULL;
}

static void mpeg4_stuff_container_type_finder_free_func(rbtree_t *restrict r)
{
	if (r->value)
		free(r->value);
}

static mpeg4_stuff_container_type_finder_t* mpeg4_stuff_container_type_finder_get(mpeg4_stuff_container_t *restrict r, mpeg4_box_type_t type)
{
	rbtree_t *restrict v;
	mpeg4_stuff_container_type_finder_t *restrict finder;
	if ((v = rbtree_find(&r->finder, NULL, (uint64_t) type.v)))
	{
		label_okay:
		return (mpeg4_stuff_container_type_finder_t *) v->value;
	}
	if ((finder = malloc(sizeof(mpeg4_stuff_container_type_finder_t))))
	{
		finder->same_list = NULL;
		finder->p_same_tail = &finder->same_list;
		if ((v = rbtree_insert(&r->finder, NULL, (uint64_t) type.v, finder, mpeg4_stuff_container_type_finder_free_func)))
			goto label_okay;
		free(finder);
	}
	return NULL;
}

static void mpeg4_stuff_container_clear(mpeg4_stuff_container_t *restrict r)
{
	mpeg4_stuff_t *restrict p;
	// un list refer
	while ((p = (mpeg4_stuff_t *volatile) r->list))
	{
		mpeg4_stuff_link_unset(&p->link);
		refer_free(p);
	}
	// clear type finder
	if (r->finder) rbtree_clear(&r->finder);
}

void mpeg4_stuff_free_default_func(mpeg4_stuff_t *restrict r)
{
	// clear child
	mpeg4_stuff_container_clear(&r->container);
	// parent must not exist
	// un refer save
	if (r->inst) refer_free(r->inst);
}

mpeg4_stuff_t* mpeg4_stuff_alloc(const struct mpeg4_atom_s *restrict atom, const mpeg4_t *restrict inst, mpeg4_box_type_t type, size_t size, mpeg4_stuff_init_f init_func, refer_free_f free_func)
{
	mpeg4_stuff_t *restrict r;
	if (size >= sizeof(mpeg4_stuff_t) && (r = (mpeg4_stuff_t *) refer_alloz(size)))
	{
		if (!free_func) free_func = (refer_free_f) mpeg4_stuff_free_default_func;
		refer_set_free(r, free_func);
		r->inst = (const mpeg4_t *) refer_save(inst);
		r->atom = atom;
		r->container.p_tail = &r->container.list;
		r->info.type.v = type.v;
		if (!init_func || init_func(r))
			return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4_stuff_t* mpeg4_stuff_calc_okay(mpeg4_stuff_t *restrict stuff, uint64_t box_inner_size)
{
	stuff->info.inner_size = box_inner_size;
	if (stuff->info.type.v)
	{
		stuff->info.all_size = box_inner_size + sizeof(mpeg4_box_t);
		if (stuff->info.all_size > (uint64_t) 0xffffffff)
			stuff->info.all_size += sizeof(mpeg4_box_extend_size_t);
	}
	else stuff->info.all_size = box_inner_size;
	stuff->info.calc_okay = 1;
	return stuff;
}

void mpeg4_stuff_calc_invalid(mpeg4_stuff_t *restrict stuff)
{
	while (stuff && stuff->info.calc_okay)
	{
		stuff->info.calc_okay = 0;
		stuff = stuff->link.container;
	}
}

static void mpeg4_stuff_calc_invalid_tree_func(mpeg4_stuff_t *restrict root)
{
	while (root)
	{
		if (root->container.list)
			mpeg4_stuff_calc_invalid_tree_func(root->container.list);
		root->info.calc_okay = 0;
		root = root->link.next;
	}
}

void mpeg4_stuff_calc_invalid_tree(mpeg4_stuff_t *restrict root)
{
	if (root->container.list)
		mpeg4_stuff_calc_invalid_tree_func(root->container.list);
	mpeg4_stuff_calc_invalid(root);
}

mpeg4_stuff_t* mpeg4_stuff_container_link(mpeg4_stuff_t *restrict container, mpeg4_stuff_t *restrict stuff, mpeg4_stuff_t *restrict insert)
{
	if (!stuff->link.container && (!container->atom->interface.container_testing ||
		container->atom->interface.container_testing(container, stuff->info.type)))
	{
		mpeg4_stuff_container_type_finder_t *restrict finder;
		finder = mpeg4_stuff_container_type_finder_get(&container->container, stuff->info.type);
		if (finder)
		{
			refer_save(stuff);
			mpeg4_stuff_calc_invalid(container);
			if (insert && insert->link.container == container)
			{
				mpeg4_stuff_t *pos = container->container.list;
				mpeg4_stuff_t *same_pos = finder->same_list;
				while (pos && pos != insert)
				{
					if (pos->info.type.v == stuff->info.type.v)
						same_pos = pos->link.same_next;
					pos = pos->link.next;
				}
				if (!pos) goto label_append;
				mpeg4_stuff_link_set(stuff, pos->link.p_next, same_pos?same_pos->link.p_same_next:finder->p_same_tail, container, finder);
				if (!same_pos)
					goto label_append_same;
			}
			else
			{
				label_append:
				mpeg4_stuff_link_set(stuff, container->container.p_tail, finder->p_same_tail, container, finder);
				container->container.p_tail = &stuff->link.next;
				label_append_same:
				finder->p_same_tail = &stuff->link.same_next;
			}
			if (container->atom->interface.container_update)
				container->atom->interface.container_update(container);
			if (stuff->atom->interface.link_update)
				stuff->atom->interface.link_update(stuff, NULL);
			return container;
		}
	}
	return NULL;
}

mpeg4_stuff_t* mpeg4_stuff_container_append(mpeg4_stuff_t *restrict container, mpeg4_box_type_t type)
{
	const mpeg4_atom_s *restrict atom;
	mpeg4_create_f create;
	mpeg4_stuff_t *restrict stuff;
	mpeg4_stuff_t *restrict r;
	r = NULL;
	if ((atom = mpeg4_atom_layer_find(container->atom, type.v)) &&
		(create = atom->interface.create) &&
		(stuff = create(atom, container->inst, type)))
	{
		if (mpeg4_stuff_container_link(container, stuff, NULL))
			r = stuff;
		refer_free(stuff);
	}
	return r;
}

mpeg4_stuff_t* mpeg4_stuff_container_find(mpeg4_stuff_t *restrict container, const mpeg4_box_type_t *restrict type)
{
	rbtree_t *restrict finder;
	if (type)
	{
		finder = rbtree_find(&container->container.finder, NULL, (uint64_t) type->v);
		if (finder && finder->value)
			return ((mpeg4_stuff_container_type_finder_t *) finder->value)->same_list;
		return NULL;
	}
	else return container->container.list;
}

mpeg4_stuff_t* mpeg4_stuff_container_find_path(mpeg4_stuff_t *restrict container, const mpeg4_box_type_t *restrict path)
{
	while (container && path->v)
	{
		container = mpeg4_stuff_container_find(container, path);
		++path;
	}
	return container;
}

void mpeg4_stuff_unlink(mpeg4_stuff_t *restrict stuff)
{
	mpeg4_stuff_t *restrict container;
	if ((container = stuff->link.container))
	{
		mpeg4_stuff_calc_invalid(container);
		mpeg4_stuff_link_unset(&stuff->link);
		if (container->atom->interface.container_update)
			container->atom->interface.container_update(container);
		if (stuff->atom->interface.link_update)
			stuff->atom->interface.link_update(stuff, container);
		refer_free(stuff);
	}
}

mpeg4_stuff_t* mpeg4_stuff_replace(mpeg4_stuff_t *restrict stuff, const struct mpeg4_atom_s *restrict atom, mpeg4_box_type_t type)
{
	const mpeg4_atom_s *restrict aim;
	mpeg4_stuff_t *restrict container;
	mpeg4_stuff_t *restrict insert;
	mpeg4_box_type_t orginal;
	container = stuff->link.container;
	insert = stuff->link.next;
	orginal = stuff->info.type;
	if (container)
	{
		if (!(aim = mpeg4_atom_layer_find(container->atom, type.v)) || (aim != atom))
			goto label_fail;
		refer_save(stuff);
		mpeg4_stuff_unlink(stuff);
		stuff->info.type = type;
		if (mpeg4_stuff_container_link(container, stuff, insert))
		{
			stuff->atom = aim;
			refer_free(stuff);
			goto label_ok;
		}
		else
		{
			stuff->info.type = orginal;
			mpeg4_stuff_container_link(container, stuff, insert);
			refer_free(stuff);
			goto label_fail;
		}
	}
	if (!atom) goto label_fail;
	stuff->info.type = type;
	stuff->atom = atom;
	label_ok:
	mpeg4_stuff_calc_invalid(stuff);
	return stuff;
	label_fail:
	return NULL;
}
