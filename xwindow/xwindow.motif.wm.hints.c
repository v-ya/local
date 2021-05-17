#include "xwindow.private.h"

typedef struct xwindow_motif_wm_hints_t {
	uint32_t flags;
	uint32_t functions;
	uint32_t decorations;
	int32_t input_mode;
	uint32_t status;
} xwindow_motif_wm_hints_t;

static const char *s_atom_motif_wm_hints = "_MOTIF_WM_HINTS";

xwindow_s* xwindow_set_hint_decorations(xwindow_s *restrict r, uint32_t enable)
{
	xcb_generic_error_t *error;
	if (!r->atom_hint)
	{
		if (!xwindow_inner_get_atom(r->connection, s_atom_motif_wm_hints, &r->atom_hint))
			goto label_fail;
	}
	if (!(error = xcb_request_check(r->connection, xcb_change_property_checked(
			r->connection, XCB_PROP_MODE_REPLACE, r->window,
			r->atom_hint, r->atom_hint, 32,
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
	label_fail:
	return NULL;
}
