#include "miko.wink.gomi.h"
#include "miko.wink.search.h"
#include "miko.list.api.h"

static void miko_wink_gomi_free_func(miko_wink_gomi_s *restrict r)
{
	miko_list_t *restrict inode;
	if (r->daemon)
	{
		yaw_stop(r->daemon);
		yaw_signal_inc_wake(r->signal, 1);
		yaw_wait(r->daemon);
		refer_free(r->daemon);
	}
	while ((inode = r->root))
	{
		miko_list_unlink(&r->root, inode);
		refer_free(inode);
	}
	while ((inode = r->linked))
	{
		miko_list_unlink(&r->linked, inode);
		refer_free(inode);
	}
	refer_ck_free(r->signal);
	refer_ck_free(r->request);
	refer_ck_free(r->search);
	refer_ck_free(r->cache);
}

miko_wink_gomi_s* miko_wink_gomi_alloc(void)
{
	miko_wink_gomi_s *restrict r;
	if ((r = (miko_wink_gomi_s *) refer_alloz(sizeof(miko_wink_gomi_s))))
	{
		refer_hook_free(r, gomi);
		if ((r->signal = yaw_signal_alloc()) &&
			(r->request = queue_alloc_ring(1024)) &&
			(r->search = miko_wink_search_alloc()) &&
			(r->cache = miko_wink_search_alloc()) &&
			(r->daemon = yaw_alloc_and_start(miko_wink_gomi_daemon, NULL, r)))
			return r;
		refer_free(r);
	}
	return NULL;
}

#include "miko.wink.gomi.request.h"
#include <memory.h>

miko_wink_gomi_s* miko_wink_gomi_call_gomi(miko_wink_gomi_s *restrict gomi, miko_wink_report_t *restrict report)
{
	miko_wink_gomi_request_gomi_s *restrict r;
	if ((r = miko_wink_gomi_request_create_gomi()))
	{
		miko_wink_gomi_request_wait(gomi, &r->request);
		if (report) memcpy(report, &r->report, sizeof(*report));
		if (r->request.result != miko_wink_gomi_request_result__okay)
			gomi = NULL;
		refer_free(r);
		return gomi;
	}
	return NULL;
}

miko_wink_gomi_s* miko_wink_gomi_call_cycle(miko_wink_gomi_s *restrict gomi, uintptr_t miko_gomi_msec)
{
	miko_wink_gomi_request_cycle_s *restrict r;
	if ((r = miko_wink_gomi_request_create_cycle(miko_gomi_msec)))
	{
		miko_wink_gomi_request_wait(gomi, &r->request);
		if (r->request.result != miko_wink_gomi_request_result__okay)
			gomi = NULL;
		refer_free(r);
		return gomi;
	}
	return NULL;
}

miko_wink_gomi_s* miko_wink_gomi_call_report(miko_wink_gomi_s *restrict gomi, miko_wink_report_s *restrict report)
{
	miko_wink_gomi_request_report_s *restrict r;
	if ((r = miko_wink_gomi_request_create_report(report)))
	{
		miko_wink_gomi_request_wait(gomi, &r->request);
		if (r->request.result != miko_wink_gomi_request_result__okay)
			gomi = NULL;
		refer_free(r);
		return gomi;
	}
	return NULL;
}

#include "miko.wink.report.h"

miko_wink_gomi_s* miko_wink_gomi_default_report(miko_wink_gomi_s *restrict gomi, mlog_s *restrict mlog, uintptr_t only_modify_output)
{
	miko_wink_report_s *restrict report;
	if ((report = miko_wink_report_create_default(mlog, only_modify_output)))
	{
		gomi = miko_wink_gomi_call_report(gomi, report);
		refer_free(report);
		return gomi;
	}
	return NULL;
}
