#include "xwindow.private.h"
#include "xwindow.atom.h"

typedef struct xwindow_motif_wm_hints_t {
	uint32_t flags;
	uint32_t functions;
	uint32_t decorations;
	int32_t input_mode;
	uint32_t status;
} xwindow_motif_wm_hints_t;

xwindow_s* xwindow_set_hint_decorations(xwindow_s *restrict r, uint32_t enable)
{
	xcb_generic_error_t *error;
	xcb_atom_t atom;
	if (xwindow_atom_get(r->atom, xatom___MOTIF_WM_HINTS, &atom, r->connection))
	{
		if (!(error = xcb_request_check(r->connection, xcb_change_property_checked(
			r->connection, XCB_PROP_MODE_REPLACE, r->window, atom, atom, 32,
			sizeof(xwindow_motif_wm_hints_t) / sizeof(uint32_t),
			(xwindow_motif_wm_hints_t [1]) {{
				.flags = 2,
				.functions = 0,
				.decorations = !!enable,
				.input_mode = 0,
				.status = 0
			}}))))
		return r;
		free(error);
	}
	return NULL;
}
