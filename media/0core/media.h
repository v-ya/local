#ifndef _media_core_media_h_
#define _media_core_media_h_

#include <refer.h>
#include "stack_id.h"
#include "frame_id.h"
#include "container_id.h"
#include "transform_id.h"
#include "mlog.h"
#include <hashmap.h>

struct media_s {
	hashmap_t string;
	hashmap_t stack;
	hashmap_t frame;
	hashmap_t component;
	hashmap_t container;
	hashmap_t transform;
	mlog_s *mlog_error;
	mlog_s *mlog_warning;
	mlog_s *mlog_info;
	mlog_s *mlog_verbose;
};

struct media_s* media_alloc_empty(void);

void media_hashmap_free_refer_func(hashmap_vlist_t *restrict vl);

struct media_s* media_initial_add_string(struct media_s *restrict media, const char *restrict string);
struct media_s* media_initial_add_stack(struct media_s *restrict media, const struct media_stack_id_s *restrict stack_id);
struct media_s* media_initial_add_frame(struct media_s *restrict media, struct media_frame_id_s *restrict frame_id);
struct media_s* media_initial_add_component(struct media_s *restrict media, const char *restrict name, refer_t component);
struct media_s* media_initial_add_container(struct media_s *restrict media, const struct media_container_id_s *restrict container_id);
struct media_s* media_initial_add_transform(struct media_s *restrict media, const struct media_transform_id_s *restrict transform_id);

const struct media_transform_id_s* media_get_transform(const struct media_s *restrict media, const char *restrict src_frame_compat, const char *restrict dst_frame_compat);

struct media_s* media_initial_set_mlog(struct media_s *restrict media, mlog_s *restrict mlog, uint32_t loglevel);
mlog_s* media_get_mlog_by_loglevel(const struct media_s *restrict media, uint32_t loglevel);

#define media_get_string(_media, _s)           ((refer_string_t) hashmap_get_name(&(_media)->string, _s))
#define media_save_string(_media, _s)          ((refer_string_t) refer_save(hashmap_get_name(&(_media)->string, _s)))
#define media_get_stack(_media, _id)           ((const struct media_stack_id_s *) hashmap_get_name(&(_media)->stack, _id))
#define media_save_stack(_media, _id)          ((const struct media_stack_id_s *) refer_save(hashmap_get_name(&(_media)->stack, _id)))
#define media_get_frame(_media, _id)           ((const struct media_frame_id_s *) hashmap_get_name(&(_media)->frame, _id))
#define media_save_frame(_media, _id)          ((const struct media_frame_id_s *) refer_save(hashmap_get_name(&(_media)->frame, _id)))
#define media_get_component(_media, _id, _t)   ((const _t *) hashmap_get_name(&(_media)->component, _id))
#define media_save_component(_media, _id, _t)  ((const _t *) refer_save(hashmap_get_name(&(_media)->component, _id)))
#define media_get_container(_media, _id)       ((const struct media_container_id_s *) hashmap_get_name(&(_media)->container, _id))
#define media_save_container(_media, _id)      ((const struct media_container_id_s *) refer_save(hashmap_get_name(&(_media)->container, _id)))

#define media_error(_media, _format, ...)   (void) ((_media)->mlog_error   && mlog_printf((_media)->mlog_error,   _format, ##__VA_ARGS__))
#define media_warning(_media, _format, ...) (void) ((_media)->mlog_warning && mlog_printf((_media)->mlog_warning, _format, ##__VA_ARGS__))
#define media_info(_media, _format, ...)    (void) ((_media)->mlog_info    && mlog_printf((_media)->mlog_info,    _format, ##__VA_ARGS__))
#define media_verbose(_media, _format, ...) (void) ((_media)->mlog_verbose && mlog_printf((_media)->mlog_verbose, _format, ##__VA_ARGS__))

#endif
