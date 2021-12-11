#include "xwindow.private.h"

xwindow_s* xwindow_set_fullscreen(xwindow_s *restrict r, uint32_t enable)
{
	xcb_generic_error_t *error;
	if (r->atom.list[xwindow_atom_id___net_wm_state] && r->atom.list[xwindow_atom_id___net_wm_state_fullscreen])
	{
		if (!(error = xcb_request_check(r->connection, xcb_send_event_checked(
				r->connection, 0, r->screen->root,
				XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
				(char *) (xcb_client_message_event_t [1]) {{
					.response_type = XCB_CLIENT_MESSAGE,
					.format = 32,
					.sequence = 0,
					.window = r->window,
					.type = r->atom.list[xwindow_atom_id___net_wm_state],
					.data = {.data32 = {
							enable?1:0,
							r->atom.list[xwindow_atom_id___net_wm_state_fullscreen],
							0, 0, 0
						}}
				}}))))
			return r;
		free(error);
	}
	return NULL;
}
