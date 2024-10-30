#include "miko.wink.view.h"

#define miko_wink_fetch(_wink_)  ((miko_wink_w *) (_wink_) - 1)

miko_wink_view_s* miko_wink_view_add(miko_wink_view_s *restrict r, miko_wink_t wink)
{
	miko_wink_w *restrict item;
	if (wink && (item = miko_wink_fetch(wink))->visible == miko_wink_visible__outside)
		return miko_wink_search_append(r, item);
	return r;
}

miko_wink_view_s* miko_wink_view_add_array(miko_wink_view_s *restrict r, miko_wink_t *restrict wink_array, uintptr_t wink_count)
{
	miko_wink_w *restrict wink;
	uintptr_t i;
	for (i = 0; r && i < wink_count; ++i)
	{
		if (wink_array[i] && (wink = miko_wink_fetch(wink_array[i]))->visible == miko_wink_visible__outside)
			r = miko_wink_search_append(r, wink);
	}
	return r;
}
