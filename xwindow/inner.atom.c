#include "xwindow.private.h"
#include <string.h>

static xwindow_atom_s* xwindow_atom_inner_set(xwindow_atom_s *restrict r, const char *restrict name, xcb_atom_t atom)
{
	yaw_lock_s *restrict lock;
	hashmap_vlist_t *vl;
	lock = r->write;
	yaw_lock_lock(lock);
	vl = hashmap_set_name(&r->atom, name, (void *) (uintptr_t) atom, NULL);
	yaw_lock_unlock(lock);
	return vl?r:NULL;
}

static xwindow_atom_s* xwindow_atom_inner_get(xwindow_atom_s *restrict r, const char *restrict name, xcb_atom_t *restrict atom)
{
	yaw_lock_s *restrict lock;
	hashmap_vlist_t *vl;
	lock = r->read;
	yaw_lock_lock(lock);
	if ((vl = hashmap_find_name(&r->atom, name)) && atom)
		*atom = (xcb_atom_t) (uintptr_t) vl->value;
	yaw_lock_unlock(lock);
	return vl?r:NULL;
}

static void xwindow_atom_free_func(xwindow_atom_s *restrict r)
{
	if (r->read) refer_free(r->read);
	if (r->write) refer_free(r->write);
	hashmap_uini(&r->atom);
}

xwindow_atom_s* xwindow_atom_alloc(void)
{
	xwindow_atom_s *restrict r;
	if ((r = (xwindow_atom_s *) refer_alloz(sizeof(xwindow_atom_s))))
	{
		refer_set_free(r, (refer_free_f) xwindow_atom_free_func);
		if (!yaw_lock_alloc_rwlock(&r->read, &r->write) &&
			hashmap_init(&r->atom))
			return r;
		refer_free(r);
	}
	return NULL;
}

void xwindow_atom_touch(xwindow_atom_s *restrict r, xcb_connection_t *restrict connection, uintptr_t number, const char *const restrict name_list[])
{
	xcb_intern_atom_cookie_t cookies[number];
	xcb_intern_atom_reply_t *restrict ratom;
	uintptr_t i;
	for (i = 0; i < number; ++i)
		cookies[i] = xcb_intern_atom(connection, 1, (uint16_t) strlen(name_list[i]), name_list[i]);
	for (i = 0; i < number; ++i)
	{
		if ((ratom = xcb_intern_atom_reply(connection, cookies[i], NULL)))
		{
			xwindow_atom_inner_set(r, name_list[i], ratom->atom);
			free(ratom);
		}
	}
}

xwindow_atom_s* xwindow_atom_get(xwindow_atom_s *restrict r, const char *restrict name, xcb_atom_t *restrict atom, xcb_connection_t *restrict connection)
{
	if (xwindow_atom_inner_get(r, name, atom))
		return r;
	if (connection)
	{
		xwindow_atom_touch(r, connection, 1, &name);
		if (xwindow_atom_inner_get(r, name, atom))
			return r;
	}
	return NULL;
}
