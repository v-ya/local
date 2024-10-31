#include "miko.wink.gomi.h"
#include "miko.wink.search.h"
#include "miko.wink.gomi.request.h"
#include <memory.h>

typedef struct miko_wink_gomi_env_t miko_wink_gomi_env_t;

struct miko_wink_gomi_env_t {
	miko_wink_report_s *report;
	uintptr_t miko_gomi_msec;
	uint64_t timestamp_curr_msec;
	uint64_t timestamp_next_msec;
	miko_wink_report_t data;
};

static void miko_wink_gomi_env_initial(miko_wink_gomi_env_t *restrict env)
{
	memset(env, 0, sizeof(*env));
	env->miko_gomi_msec = 1000;
}

static void miko_wink_gomi_env_finally(miko_wink_gomi_env_t *restrict env)
{
	refer_ck_free(env->report);
}

static miko_wink_gomi_s* miko_wink_gomi_go(miko_wink_gomi_s *restrict gomi, miko_wink_gomi_env_t *restrict env)
{
	miko_wink_gomi_s *result;
	uintptr_t batch_skip;
	result = NULL;
	batch_skip = gomi->skip;
	if ((env->timestamp_next_msec += env->miko_gomi_msec) < env->timestamp_curr_msec)
		env->timestamp_next_msec = env->timestamp_curr_msec;
	memset(&env->data, 0, sizeof(env->data));
	miko_wink_searched_clear(gomi->searched);
	if (miko_wink_gomi_visible_initial(gomi, gomi->search, &env->data) &&
		miko_wink_gomi_visible_layer(gomi, gomi->search, gomi->cache, &env->data) &&
		gomi->skip == batch_skip)
	{
		miko_wink_gomi_visible_finally(gomi, &env->data);
		env->data.timestamp_msec_start = env->timestamp_curr_msec;
		env->data.timestamp_msec_stop = yaw_timestamp_msec();
		if (env->report) env->report->report(env->report, &env->data);
		result = gomi;
	}
	miko_wink_search_clear(gomi->search);
	miko_wink_search_clear(gomi->cache);
	miko_wink_searched_clear(gomi->searched);
	return result;
}

static void miko_wink_gomi_callee(miko_wink_gomi_s *restrict gomi, miko_wink_gomi_env_t *restrict env)
{
	miko_wink_gomi_request_s *restrict req;
	queue_s *restrict request;
	request = gomi->request;
	while ((req = (miko_wink_gomi_request_s *) request->pull(request)))
	{
		#define _req_(_type_)  ((miko_wink_gomi_request_##_type_##_s *) req)
		switch (req->type)
		{
			case miko_wink_gomi_request_type__gomi:
				env->timestamp_curr_msec = env->timestamp_next_msec = yaw_timestamp_msec();
				if (miko_wink_gomi_go(gomi, env))
				{
					memcpy(&_req_(gomi)->report, &env->data, sizeof(env->data));
					req->result = miko_wink_gomi_request_result__okay;
					break;
				}
				req->result = miko_wink_gomi_request_result__fail;
				break;
			case miko_wink_gomi_request_type__cycle:
				env->timestamp_next_msec = env->timestamp_curr_msec;
				env->miko_gomi_msec = _req_(cycle)->miko_gomi_msec;
				req->result = miko_wink_gomi_request_result__okay;
				break;
			case miko_wink_gomi_request_type__report:
				refer_ck_free(env->report);
				env->report = (miko_wink_report_s *) refer_save(_req_(report)->report);
				req->result = miko_wink_gomi_request_result__okay;
				break;
			default:
				req->result = miko_wink_gomi_request_result__fail;
				break;
		}
		#undef _req_
		yaw_lock_unlock(req->notify);
		refer_free(req);
	}
}

void miko_wink_gomi_daemon(yaw_s *restrict yaw)
{
	miko_wink_gomi_s *restrict gomi;
	yaw_signal_s *restrict signal;
	miko_wink_gomi_env_t env;
	uint32_t status;
	gomi = (miko_wink_gomi_s *) yaw->data;
	signal = gomi->signal;
	miko_wink_gomi_env_initial(&env);
	for (;;)
	{
		status = yaw_signal_get(signal);
		if (!yaw->running) break;
		miko_wink_gomi_callee(gomi, &env);
		if ((env.timestamp_curr_msec = yaw_timestamp_msec()) >= env.timestamp_next_msec)
			miko_wink_gomi_go(gomi, &env);
		if (env.timestamp_curr_msec < env.timestamp_next_msec)
			yaw_signal_wait_time(signal, status, 50000);
	}
	miko_wink_gomi_env_finally(&env);
}
