#include "miko.wink.see.h"
#include "miko.wink.gomi.h"
#include "miko.list.api.h"

void miko_wink_see_wink_to_gomi(miko_wink_gomi_s *restrict gomi, miko_wink_s *restrict wink)
{
	miko_wink_see_s *restrict r;
	if ((r = (miko_wink_see_s *) refer_alloz(sizeof(miko_wink_see_s))))
	{
		r->wink = wink;
		miko_list_insert(&gomi->linked, &r->inode);
	}
	else __sync_fetch_and_add(&gomi->skip, 1);  // skip this gomi
}
