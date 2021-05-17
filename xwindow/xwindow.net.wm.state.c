#include "xwindow.private.h"

static const char *s_atom_net_wm_state = "_NET_WM_STATE";
static const char *s_atom_net_wm_state_fullscreen = "_NET_WM_STATE_FULLSCREEN";

xwindow_s* xwindow_set_fullscreen(xwindow_s *restrict r, uint32_t enable)
{
	xcb_generic_error_t *error;
	if (!r->atom_state)
	{
		if (!xwindow_inner_get_atom(r->connection, s_atom_net_wm_state, &r->atom_state))
			goto label_fail;
	}
	if (!r->atom_state_fullscreen)
	{
		if (!xwindow_inner_get_atom(r->connection, s_atom_net_wm_state_fullscreen, &r->atom_state_fullscreen))
			goto label_fail;
	}
	error = xcb_request_check(r->connection, xcb_send_event_checked(
		r->connection, 0, r->screen->root,
		XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
		(char *) (xcb_client_message_event_t [1]) {{
			.response_type = XCB_CLIENT_MESSAGE,
			.format = 32,
			.sequence = 0,
			.window = r->window,
			.type = r->atom_state,
			.data = {.data32 = {enable?1:0, r->atom_state_fullscreen, 0, 0, 0}}
		}})
	);
	if (!error) return r;
	free(error);
	label_fail:
	return NULL;
}
