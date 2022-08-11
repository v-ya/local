#include "xwindow.private.h"
#include "xwindow.atom.h"

void xwindow_inner_atom_touch_common(xwindow_atom_s *restrict r, xcb_connection_t *restrict c)
{
	const char *const name_list[] = {
		xatom__UTF8_STRING,
		xatom__WM_PROTOCOLS,
		xatom__WM_DELETE_WINDOW,
		xatom___MOTIF_WM_HINTS,
		xatom___NET_WM_ICON,
		xatom___NET_WM_NAME,
		xatom___NET_WM_STATE,
		xatom___NET_WM_STATE_FULLSCREEN,
	};
	xwindow_atom_touch(r, c, sizeof(name_list) / sizeof(const char *), name_list);
}
