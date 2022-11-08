#include "media.h"
#include <string.h>
#include <alloca.h>

struct media_transform_src2dst_s {
	hashmap_t transform;
	const struct media_transform_id_s *restrict any;
};

static void media_transform_src2dst_free_func(struct media_transform_src2dst_s *restrict r)
{
	hashmap_uini(&r->transform);
	if (r->any) refer_free(r->any);
}

static struct media_transform_src2dst_s* media_transform_src2dst_alloc(void)
{
	struct media_transform_src2dst_s *restrict r;
	if ((r = (struct media_transform_src2dst_s *) refer_alloz(sizeof(struct media_transform_src2dst_s))))
	{
		refer_set_free(r, (refer_free_f) media_transform_src2dst_free_func);
		if (hashmap_init(&r->transform))
			return r;
		refer_free(r);
	}
	return NULL;
}

// media

static void media_free_func(struct media_s *restrict r)
{
	hashmap_uini(&r->string);
	hashmap_uini(&r->stack);
	hashmap_uini(&r->frame);
	hashmap_uini(&r->container);
	hashmap_uini(&r->transform);
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
		if (hashmap_init(&r->string) &&
			hashmap_init(&r->stack) &&
			hashmap_init(&r->frame) &&
			hashmap_init(&r->container) &&
			hashmap_init(&r->transform))
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

struct media_s* media_initial_add_string(struct media_s *restrict media, const char *restrict string)
{
	refer_string_t rs;
	if (string && (rs = refer_dump_string(string)))
	{
		if (!media_initial_hashmap_add_refer(&media->string, string, rs))
			media = NULL;
		refer_free(rs);
		return media;
	}
	return NULL;
}

struct media_s* media_initial_add_stack(struct media_s *restrict media, const struct media_stack_id_s *restrict stack_id)
{
	refer_string_t name;
	if (stack_id && (name = stack_id->name) && media_initial_hashmap_add_refer(&media->string, name, name) &&
		media_initial_hashmap_add_refer(&media->stack, name, stack_id))
		return media;
	return NULL;
}

static refer_string_t media_initial_add_frame_initial_compat(struct media_s *restrict media, struct media_frame_id_s *restrict frame_id, refer_string_t name)
{
	const char *restrict compat_endptr;
	char *restrict compat_cache;
	uintptr_t length;
	if (!frame_id->compat)
	{
		if (!(compat_endptr = strchr(name, ';')))
			frame_id->compat = (refer_string_t) refer_save(name);
		else if ((compat_cache = (char *) alloca((length = (uintptr_t) compat_endptr - (uintptr_t) name) + 1)))
		{
			((char *) memcpy(compat_cache, name, length))[length] = 0;
			frame_id->compat = media_save_string(media, compat_cache);
		}
		return frame_id->compat;
	}
	return NULL;
}

struct media_s* media_initial_add_frame(struct media_s *restrict media, struct media_frame_id_s *restrict frame_id)
{
	refer_string_t name;
	if (frame_id && (name = frame_id->name) && media_initial_hashmap_add_refer(&media->string, name, name) &&
		media_initial_add_frame_initial_compat(media, frame_id, name) &&
		media_initial_hashmap_add_refer(&media->frame, name, frame_id))
		return media;
	return NULL;
}

struct media_s* media_initial_add_container(struct media_s *restrict media, const struct media_container_id_s *restrict container_id)
{
	refer_string_t name;
	if (container_id && (name = container_id->name) && media_initial_hashmap_add_refer(&media->string, name, name) &&
		media_initial_hashmap_add_refer(&media->container, name, container_id))
		return media;
	return NULL;
}

struct media_s* media_initial_add_transform(struct media_s *restrict media, const struct media_transform_id_s *restrict transform_id)
{
	struct media_transform_src2dst_s *restrict s2d;
	if (transform_id->src_frame_compat && media_get_frame(media, transform_id->src_frame_compat))
	{
		if ((s2d = (struct media_transform_src2dst_s *) hashmap_get_name(&media->transform, transform_id->src_frame_compat)))
		{
			label_find_s2d:
			if (transform_id->dst_frame_compat)
			{
				if (media_get_frame(media, transform_id->dst_frame_compat) &&
					media_initial_hashmap_add_refer(&s2d->transform, transform_id->dst_frame_compat, transform_id))
					goto label_okay;
			}
			else if (!s2d->any)
			{
				s2d->any = (const struct media_transform_id_s *) refer_save(transform_id);
				label_okay:
				return media;
			}
		}
		else if ((s2d = media_transform_src2dst_alloc()))
		{
			if (hashmap_set_name(&media->transform, transform_id->src_frame_compat, s2d, media_hashmap_free_refer_func))
				goto label_find_s2d;
			refer_free(s2d);
		}
	}
	return NULL;
}

const struct media_transform_id_s* media_get_transform(const struct media_s *restrict media, const char *restrict src_frame_compat, const char *restrict dst_frame_compat)
{
	const struct media_transform_src2dst_s *restrict s2d;
	if (src_frame_compat && (s2d = (const struct media_transform_src2dst_s *) hashmap_get_name(&media->transform, src_frame_compat)))
		return dst_frame_compat?((const struct media_transform_id_s *) hashmap_get_name(&s2d->transform, dst_frame_compat)):s2d->any;
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
