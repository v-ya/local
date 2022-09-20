#include "mlog.h"
#include <yaw.h>

static void media_mlog_data_free(struct media_mlog_data_s *restrict r)
{
	if (r->mlog) refer_free(r->mlog);
	if (r->loglevel) refer_free(r->loglevel);
}

static struct media_mlog_data_s* media_mlog_data_alloc(mlog_s *restrict mlog, const char *restrict loglevel, uint64_t tsms_start)
{
	struct media_mlog_data_s *restrict r;
	if ((r = (struct media_mlog_data_s *) refer_alloz(sizeof(struct media_mlog_data_s))))
	{
		refer_set_free(r, (refer_free_f) media_mlog_data_free);
		r->mlog = (mlog_s *) refer_save(mlog);
		r->loglevel = refer_dump_string(loglevel);
		r->tsms_start = tsms_start;
		if (r->mlog && r->loglevel)
			return r;
		refer_free(r);
	}
	return NULL;
}

static int media_mlog_report_func(const char *restrict msg, size_t length, struct media_mlog_data_s *restrict data)
{
	uint64_t duration_msec;
	duration_msec = yaw_timestamp_msec() - data->tsms_start;
	mlog_printf(data->mlog,
		"[%zu.%03u %s] %s\n",
		(uintptr_t) (duration_msec / 1000),
		(uint32_t) (duration_msec % 1000),
		data->loglevel, msg);
	return 1;
}

mlog_s* media_mlog_alloc(mlog_s *restrict mlog, const char *restrict loglevel, uint64_t tsms_start, struct yaw_lock_s *restrict lock)
{
	struct media_mlog_data_s *restrict data;
	mlog_s *restrict r;
	r = NULL;
	if ((data = media_mlog_data_alloc(mlog, loglevel, tsms_start)))
	{
		if ((r = mlog_alloc(0)))
		{
			mlog_set_report(r, (mlog_report_f) media_mlog_report_func, data);
			if (lock) mlog_set_locker(r, lock);
		}
		refer_free(data);
	}
	return r;
}
