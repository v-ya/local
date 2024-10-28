#include "miko.wink.report.h"
#include <memory.h>
#include <yaw.h>

static void miko_wink_report_default_report(miko_wink_report_default_s *restrict r, miko_wink_report_t *restrict report)
{
	double time_start, time_cost;
	#define miko_ck_diff(_field_)  (report->_field_ != r->last._field_)
	if (!r->only_modify_output || (
		miko_ck_diff(former_root_inode) ||
		miko_ck_diff(former_lost_inode) ||
		miko_ck_diff(latter_root_inode) ||
		miko_ck_diff(latter_lost_inode) ||
		miko_ck_diff(visible_visit_inode) ||
		miko_ck_diff(visible_wink_inode) ||
		miko_ck_diff(visible_lost_inode)
	))
	{
		time_start = (double) (int64_t) (report->timestamp_msec_start - r->last.timestamp_msec_start) / 1000;
		time_cost = (double) (int64_t) (report->timestamp_msec_stop - report->timestamp_msec_start) / 1000;
		mlog_printf(r->mlog, "[+%.3f:+%.3f] root:lost=(%zu:%zu => %zu:%zu) visit=%zu, wink=%zu, lost=%zu, layer=%zu, have=%zd\n",
			time_start, time_cost,
			report->former_root_inode,
			report->former_lost_inode,
			report->latter_root_inode,
			report->latter_lost_inode,
			report->visible_visit_inode,
			report->visible_wink_inode,
			report->visible_lost_inode,
			report->layer_done_count,
			(intptr_t) (report->latter_root_inode + report->latter_lost_inode - report->visible_lost_inode));
		memcpy(&r->last, report, sizeof(*report));
	}
	#undef miko_ck_diff
}

static void miko_wink_report_default_free_func(miko_wink_report_default_s *restrict r)
{
	refer_ck_free(r->mlog);
}

miko_wink_report_s* miko_wink_report_create_default(mlog_s *restrict mlog, uintptr_t only_modify_output)
{
	miko_wink_report_default_s *restrict r;
	if (mlog && (r = (miko_wink_report_default_s *) refer_alloz(sizeof(miko_wink_report_default_s))))
	{
		refer_hook_free(r, report_default);
		r->report.report = (miko_wink_report_f) miko_wink_report_default_report;
		r->mlog = (mlog_s *) refer_save(mlog);
		r->last.timestamp_msec_start = r->last.timestamp_msec_stop = yaw_timestamp_msec();
		r->only_modify_output = !!only_modify_output;
		return &r->report;
	}
	return NULL;
}
