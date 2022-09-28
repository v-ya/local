#ifndef _media_core_media_h_
#define _media_core_media_h_

#include <refer.h>
#include "frame_id.h"
#include "stream_id.h"
#include "container_id.h"
#include "mlog.h"
#include <hashmap.h>

struct media_s {
	hashmap_t frame;
	hashmap_t stream;
	hashmap_t container;
	mlog_s *mlog_error;
	mlog_s *mlog_warning;
	mlog_s *mlog_info;
	mlog_s *mlog_verbose;
};

struct media_s* media_alloc_empty(void);
struct media_s* media_initial_add_frame(struct media_s *restrict media, const struct media_frame_id_s *restrict frame_id);
struct media_s* media_initial_add_stream(struct media_s *restrict media, const struct media_stream_id_s *restrict stream_id);
struct media_s* media_initial_add_container(struct media_s *restrict media, const struct media_container_id_s *restrict container_id);

struct media_s* media_initial_set_mlog(struct media_s *restrict media, mlog_s *restrict mlog, uint32_t loglevel);
mlog_s* media_get_mlog_by_loglevel(const struct media_s *restrict media, uint32_t loglevel);

#define media_error(_media, _format, ...)   (void) ((_media)->mlog_error   && mlog_printf((_media)->mlog_error,   _format, ##__VA_ARGS__))
#define media_warning(_media, _format, ...) (void) ((_media)->mlog_warning && mlog_printf((_media)->mlog_warning, _format, ##__VA_ARGS__))
#define media_info(_media, _format, ...)    (void) ((_media)->mlog_info    && mlog_printf((_media)->mlog_info,    _format, ##__VA_ARGS__))
#define media_verbose(_media, _format, ...) (void) ((_media)->mlog_verbose && mlog_printf((_media)->mlog_verbose, _format, ##__VA_ARGS__))

#endif
