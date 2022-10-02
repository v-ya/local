#include "media.h"

static void media_free_func(struct media_s *restrict r)
{
	hashmap_uini(&r->frame);
	hashmap_uini(&r->stream);
	hashmap_uini(&r->container);
	if (r->mlog_error) refer_free(r->mlog_error);
	if (r->mlog_warning) refer_free(r->mlog_warning);
	if (r->mlog_info) refer_free(r->mlog_info);
	if (r->mlog_verbose) refer_free(r->mlog_verbose);
}

struct media_s* media_alloc_empty(void)
{
	struct media_s *restrict r;
	if ((r = (struct media_s *) refer_alloz(sizeof(struct media_s))))
	{
		refer_set_free(r, (refer_free_f) media_free_func);
		if (hashmap_init(&r->frame) &&
			hashmap_init(&r->stream) &&
			hashmap_init(&r->container))
			return r;
		refer_free(r);
	}
	return NULL;
}

void media_hashmap_free_refer_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value) refer_free(vl->value);
}

static refer_t media_initial_hashmap_add_refer(hashmap_t *restrict hm, const char *restrict name, refer_t value)
{
	if (!hashmap_find_name(hm, name) && hashmap_set_name(hm, name, value, media_hashmap_free_refer_func))
		return refer_save(value);
	return NULL;
}

struct media_s* media_initial_add_frame(struct media_s *restrict media, const struct media_frame_id_s *restrict frame_id)
{
	if (frame_id && media_initial_hashmap_add_refer(&media->frame, frame_id->name, frame_id))
		return media;
	return NULL;
}

struct media_s* media_initial_add_stream(struct media_s *restrict media, const struct media_stream_id_s *restrict stream_id)
{
	if (stream_id && media_initial_hashmap_add_refer(&media->stream, stream_id->name, stream_id))
		return media;
	return NULL;
}

struct media_s* media_initial_add_container(struct media_s *restrict media, const struct media_container_id_s *restrict container_id)
{
	if (container_id && media_initial_hashmap_add_refer(&media->container, container_id->name, container_id))
		return media;
	return NULL;
}

#include "../media.h"
#include "mlog.h"
#include <yaw.h>

static mlog_s** media_initial_set_mlog_replace(mlog_s **restrict pm, mlog_s *restrict mlog, const char *restrict loglevel, uint64_t tsms_start, yaw_lock_s *restrict lock)
{
	if (*pm)
	{
		refer_free(*pm);
		*pm = NULL;
	}
	if (!mlog || (*pm = media_mlog_alloc(mlog, loglevel, tsms_start, lock)))
		return pm;
	return NULL;
}

#define media_log_red    "\e[31m"
#define media_log_green  "\e[32m"
#define media_log_yellow "\e[33m"
#define media_log_blue   "\e[34m"
#define media_log_purple "\e[35m"
#define media_log_aqua   "\e[36m"
#define media_log_white  "\e[37m"
#define media_log_back   "\e[0m"

struct media_s* media_initial_set_mlog(struct media_s *restrict media, mlog_s *restrict mlog, uint32_t loglevel)
{
	static const char *const loglevel_error   = media_log_red    "  error" media_log_back;
	static const char *const loglevel_warning = media_log_yellow "warning" media_log_back;
	static const char *const loglevel_info    = media_log_blue   "   info" media_log_back;
	static const char *const loglevel_verbose = media_log_aqua   "verbose" media_log_back;
	media_s *rr;
	yaw_lock_s *restrict lock;
	uint64_t tsms_start;
	uint32_t test;
	rr = media;
	test = 1;
	lock = yaw_lock_alloc_mutex();
	tsms_start = yaw_timestamp_msec();
	while (loglevel)
	{
		#define d_test(_lv)  \
			case media_loglevel_bit_##_lv:\
				if (!media_initial_set_mlog_replace(&media->mlog_##_lv, mlog, loglevel_##_lv, tsms_start, lock))\
					rr = NULL;\
				break
		switch (loglevel & test)
		{
			d_test(error);
			d_test(warning);
			d_test(info);
			d_test(verbose);
			default: break;
		}
		#undef d_test
		loglevel &= ~test;
		test <<= 1;
	}
	if (lock) refer_free(lock);
	return rr;
}

mlog_s* media_get_mlog_by_loglevel(const struct media_s *restrict media, uint32_t loglevel)
{
	mlog_s *restrict mlog;
	uint32_t test;
	mlog = NULL;
	test = 1;
	while (loglevel)
	{
		#define d_test(_lv)  \
			case media_loglevel_bit_##_lv:\
				if (media->mlog_##_lv) mlog = media->mlog_##_lv;\
				break
		switch (loglevel & test)
		{
			d_test(error);
			d_test(warning);
			d_test(info);
			d_test(verbose);
			default: break;
		}
		#undef d_test
		loglevel &= ~test;
		test <<= 1;
	}
	return mlog;
}
