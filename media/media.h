#ifndef _media_h_
#define _media_h_

#include <refer.h>

struct mlog_s;

typedef struct media_s media_s;
typedef struct media_attr_s media_attr_s;
typedef struct media_frame_s media_frame_s;
typedef struct media_container_s media_container_s;

typedef enum media_loglevel_t {
	media_loglevel_bit_error   = 0x0001,
	media_loglevel_bit_warning = 0x0002,
	media_loglevel_bit_info    = 0x0004,
	media_loglevel_bit_verbose = 0x0008,
	media_loglevel_error       = 0x0001,
	media_loglevel_warning     = 0x0003,
	media_loglevel_info        = 0x0007,
	media_loglevel_verbose     = 0x000f,
} media_loglevel_t;

const media_s* media_alloc(media_loglevel_t loglevel, struct mlog_s *restrict mlog);
struct mlog_s* media_get_mlog(const media_s *restrict media, media_loglevel_t loglevel);

// attr

media_attr_s* media_attr_set_int(media_attr_s *restrict attr, const char *restrict name, int64_t value);
media_attr_s* media_attr_set_float(media_attr_s *restrict attr, const char *restrict name, double value);
media_attr_s* media_attr_set_string(media_attr_s *restrict attr, const char *restrict name, const char *restrict string);
media_attr_s* media_attr_set_data(media_attr_s *restrict attr, const char *restrict name, const void *data, uintptr_t size);
media_attr_s* media_attr_refer_string(media_attr_s *restrict attr, const char *restrict name, refer_string_t string);
media_attr_s* media_attr_refer_data(media_attr_s *restrict attr, const char *restrict name, refer_nstring_t data);

const media_attr_s* media_attr_get_int(const media_attr_s *restrict attr, const char *restrict name, int64_t *restrict value);
const media_attr_s* media_attr_get_float(const media_attr_s *restrict attr, const char *restrict name, double *restrict value);
const media_attr_s* media_attr_get_string(const media_attr_s *restrict attr, const char *restrict name, refer_string_t *restrict string);
const media_attr_s* media_attr_get_data(const media_attr_s *restrict attr, const char *restrict name, refer_nstring_t *restrict data);

void media_attr_dump(const media_attr_s *restrict attr, const media_s *restrict media, media_loglevel_t loglevel, const char *restrict name);

// frame

media_frame_s* media_create_frame(const media_s *restrict media, const char *restrict frame_name, uintptr_t d, const uintptr_t dv[]);
media_frame_s* media_create_frame_1d(const media_s *restrict media, const char *restrict frame_name, uintptr_t n);
media_frame_s* media_create_frame_2d(const media_s *restrict media, const char *restrict frame_name, uintptr_t width, uintptr_t height);
media_frame_s* media_create_frame_3d(const media_s *restrict media, const char *restrict frame_name, uintptr_t width, uintptr_t height, uintptr_t depth);

// container

media_attr_s* media_container_get_attr(const media_container_s *restrict container);
media_container_s* media_create_container(const media_s *restrict media, const char *restrict frame_name);
media_container_s* media_create_input_by_memory(const media_s *restrict media, const char *restrict frame_name, const void *data, uintptr_t size);

#endif
