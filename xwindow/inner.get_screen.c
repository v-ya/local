#include "xwindow.private.h"

const xcb_screen_t* xwindow_inner_get_screen(xcb_connection_t *restrict c, uint32_t depth, xcb_visualid_t *restrict visual)
{
	const xcb_setup_t *restrict setup;
	const xcb_screen_t *restrict screen;
	const xcb_depth_t *restrict d;
	const xcb_visualtype_t *vt;
	setup = xcb_get_setup(c);
	if (setup)
	{
		xcb_screen_iterator_t screen_iterator;
		xcb_depth_iterator_t depth_iterator;
		for (screen_iterator = xcb_setup_roots_iterator(setup);
			(screen = screen_iterator.data) && screen_iterator.rem > 0;
			xcb_screen_next(&screen_iterator))
		{
			if (!depth || screen->root_depth == depth)
			{
				*visual = screen->root_visual;
				return screen;
			}
			if (screen->allowed_depths_len)
			{
				for (depth_iterator = xcb_screen_allowed_depths_iterator(screen_iterator.data);
					(d = depth_iterator.data) && depth_iterator.rem > 0;
					xcb_depth_next(&depth_iterator))
				{
					if (d->visuals_len && d->depth == depth && (vt = xcb_depth_visuals(d)))
					{
						*visual = vt->visual_id;
						return screen;
					}
				}
			}
		}
	}
	return NULL;
}
