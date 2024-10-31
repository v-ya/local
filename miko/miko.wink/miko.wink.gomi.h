#ifndef _miko_wink_miko_wink_gomi_h_
#define _miko_wink_miko_wink_gomi_h_

#include "miko.wink.h"
#include <queue/queue.h>
#include <yaw.h>

struct miko_wink_gomi_s {
	miko_list_t *root;               // list => (miko_wink_w *)
	miko_list_t *lost;               // list => (miko_wink_w *)
	miko_list_t *linked;             // list => (miko_wink_see_t *)
	yaw_signal_s *signal;            // user call signal
	queue_s *request;                // user call queue
	miko_wink_searched_s *searched;  // used by search
	miko_wink_search_s *search;      // used by search
	miko_wink_search_s *cache;       // used by search
	miko_wink_batch_t skip;          // control skip this gomi
	yaw_s *daemon;                   // gomi daemon thread
};

miko_wink_gomi_s* miko_wink_gomi_visible_layer(miko_wink_gomi_s *restrict gomi, miko_wink_search_s *restrict search, miko_wink_search_s *restrict cache, miko_wink_report_t *restrict report);
miko_wink_gomi_s* miko_wink_gomi_visible_initial(miko_wink_gomi_s *restrict gomi, miko_wink_search_s *restrict search, miko_wink_report_t *restrict report);
void miko_wink_gomi_visible_finally(miko_wink_gomi_s *restrict gomi, miko_wink_report_t *restrict report);

void miko_wink_gomi_daemon(yaw_s *restrict yaw);

#endif
