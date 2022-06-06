#ifndef _media_h_
#define _media_h_

#include <refer.h>

typedef struct media_s media_s;
typedef struct media_attr_s media_attr_s;
typedef struct media_frame_s media_frame_s;

const media_s* media_alloc(void);

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

// frame

media_frame_s* media_create_frame(const media_s *restrict media, const char *restrict frame_name, uintptr_t d, const uintptr_t dv[]);
media_frame_s* media_create_frame_1d(const media_s *restrict media, const char *restrict frame_name, uintptr_t n);
media_frame_s* media_create_frame_2d(const media_s *restrict media, const char *restrict frame_name, uintptr_t width, uintptr_t height);
media_frame_s* media_create_frame_3d(const media_s *restrict media, const char *restrict frame_name, uintptr_t width, uintptr_t height, uintptr_t depth);

#endif
