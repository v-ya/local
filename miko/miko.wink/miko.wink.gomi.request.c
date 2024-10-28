#include "miko.wink.gomi.request.h"

miko_wink_gomi_request_s* miko_wink_gomi_request_wait(miko_wink_gomi_s *restrict gomi, miko_wink_gomi_request_s *restrict req)
{
	yaw_signal_s *restrict signal;
	queue_s *restrict request;
	yaw_lock_s *restrict notify;
	signal = gomi->signal;
	request = gomi->request;
	notify = req->notify;
	yaw_lock_lock(notify);
	if (request->push(request, req))
	{
		yaw_signal_inc_wake(signal, 1);
		yaw_lock_lock(notify);
	}
	else req = NULL;
	yaw_lock_unlock(notify);
	return req;
}

static void miko_wink_gomi_request_free_func(miko_wink_gomi_request_s *restrict r)
{
	refer_ck_free(r->notify);
}

static miko_wink_gomi_request_s* miko_wink_gomi_request_alloc(uintptr_t size, miko_wink_gomi_request_type_t type)
{
	miko_wink_gomi_request_s *restrict r;
	if (size >= sizeof(miko_wink_gomi_request_s) && (r = (miko_wink_gomi_request_s *) refer_alloz(size)))
	{
		refer_hook_free(r, gomi_request);
		r->type = type;
		r->result = miko_wink_gomi_request_result__fail;
		if ((r->notify = yaw_lock_alloc_mutex()))
			return r;
		refer_free(r);
	}
	return NULL;
}

#define miko_wink_gomi_request_create(_type_)  ((miko_wink_gomi_request_##_type_##_s *) miko_wink_gomi_request_alloc(\
							sizeof(miko_wink_gomi_request_##_type_##_s),\
							miko_wink_gomi_request_type__##_type_))

miko_wink_gomi_request_gomi_s* miko_wink_gomi_request_create_gomi(void)
{
	return miko_wink_gomi_request_create(gomi);
}

miko_wink_gomi_request_cycle_s* miko_wink_gomi_request_create_cycle(uintptr_t miko_gomi_msec)
{
	miko_wink_gomi_request_cycle_s *restrict r;
	if ((r = miko_wink_gomi_request_create(cycle)))
		r->miko_gomi_msec = miko_gomi_msec;
	return r;
}

static void miko_wink_gomi_request_report_free_func(miko_wink_gomi_request_report_s *restrict r)
{
	miko_wink_gomi_request_free_func(&r->request);
	refer_ck_free(r->report);
}

miko_wink_gomi_request_report_s* miko_wink_gomi_request_create_report(miko_wink_report_s *restrict report)
{
	miko_wink_gomi_request_report_s *restrict r;
	if ((!report || report->report) && (r = miko_wink_gomi_request_create(report)))
	{
		refer_hook_free(r, gomi_request_report);
		r->report = (miko_wink_report_s *) refer_save(report);
		return r;
	}
	return NULL;
}
