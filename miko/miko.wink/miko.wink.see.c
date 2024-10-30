#include "miko.wink.see.h"
#include "miko.wink.gomi.h"
#include "miko.list.api.h"

void miko_wink_see_wink_to_gomi(miko_wink_gomi_s *restrict gomi, miko_wink_w *restrict wink)
{
	miko_wink_see_t *restrict r;
	if ((r = (miko_wink_see_t *) malloc(sizeof(miko_wink_see_t))))
	{
		r->inode.next = NULL;
		r->wink = wink;
		miko_list_insert(&gomi->linked, &r->inode);
	}
	else __sync_fetch_and_add(&gomi->skip, 1);  // skip this gomi
}
