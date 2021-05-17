#include "xwindow.private.h"
#include <string.h>

xcb_connection_t* xwindow_inner_get_atom(xcb_connection_t *restrict c, const char *restrict name, xcb_atom_t *restrict atom)
{
	xcb_intern_atom_reply_t *restrict ratom;
	ratom = xcb_intern_atom_reply(c, xcb_intern_atom(c, 1, strlen(name), name), NULL);
	if (ratom)
	{
		*atom = ratom->atom;
		free(ratom);
		return c;
	}
	return NULL;
}

