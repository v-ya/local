#ifndef _miko_wink_miko_wink_gomi_h_
#define _miko_wink_miko_wink_gomi_h_

#include "miko.wink.h"
#include <queue/queue.h>
#include <yaw.h>

struct miko_wink_gomi_s {
	miko_list_t *root;
	yaw_signal_s *signal;
	queue_s *request;
	miko_wink_search_s *search;
	miko_wink_search_s *cache;
	miko_wink_batch_t batch;
	yaw_s *daemon;
};

miko_wink_search_s* miko_wink_gomi_visible_layer(miko_wink_search_s *restrict search, miko_wink_search_s *restrict cache);
miko_wink_gomi_s* miko_wink_gomi_visible_initial(miko_wink_gomi_s *restrict gomi, miko_wink_report_t *restrict report, miko_wink_search_s *restrict search, uintptr_t batch_modify);
void miko_wink_gomi_visible_finally(miko_wink_gomi_s *restrict gomi, miko_wink_report_t *restrict report);

void miko_wink_gomi_daemon(yaw_s *restrict yaw);

#endif
