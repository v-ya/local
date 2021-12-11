#include "xwindow.private.h"
#include <memory.h>

xcb_connection_t* xwindow_inner_get_atom(xcb_connection_t *restrict c, xwindow_atom_t *restrict atom)
{
	static const struct {
		const char *atom_name;
		uintptr_t atom_name_length;
	} atom_list[xwindow_atom_id_number] = {
		#define d_item(_id, _name)  [xwindow_atom_id__##_id] = { .atom_name = _name, .atom_name_length = sizeof(_name) - 1}
		d_item(utf8_string,              "UTF8_STRING"),
		d_item(wm_protocols,             "WM_PROTOCOLS"),
		d_item(wm_delete_window,         "WM_DELETE_WINDOW"),
		d_item(_motif_wm_hints,          "_MOTIF_WM_HINTS"),
		d_item(_net_wm_icon,             "_NET_WM_ICON"),
		d_item(_net_wm_name,             "_NET_WM_NAME"),
		d_item(_net_wm_state,            "_NET_WM_STATE"),
		d_item(_net_wm_state_fullscreen, "_NET_WM_STATE_FULLSCREEN"),
		#undef d_item
	};
	xcb_intern_atom_cookie_t cookies[xwindow_atom_id_number];
	xcb_intern_atom_reply_t *restrict ratom;
	uintptr_t i;
	memset(atom, 0, sizeof(*atom));
	for (i = 0; i < xwindow_atom_id_number; ++i)
		cookies[i] = xcb_intern_atom(c, 1, (uint16_t) atom_list[i].atom_name_length, atom_list[i].atom_name);
	for (i = 0; i < xwindow_atom_id_number; ++i)
	{
		if ((ratom = xcb_intern_atom_reply(c, cookies[i], NULL)))
		{
			atom->list[i] = ratom->atom;
			free(ratom);
		}
	}
	return c;
}
