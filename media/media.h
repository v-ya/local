#ifndef _media_h_
#define _media_h_

#include <refer.h>

typedef struct media_s media_s;
typedef struct media_frame_s media_frame_s;

const media_s* media_alloc(void);

// frame
media_frame_s* media_create_frame(const media_s *restrict media, const char *restrict frame_name, uintptr_t d, const uintptr_t dv[]);
media_frame_s* media_create_frame_1d(const media_s *restrict media, const char *restrict frame_name, uintptr_t n);
media_frame_s* media_create_frame_2d(const media_s *restrict media, const char *restrict frame_name, uintptr_t width, uintptr_t height);
media_frame_s* media_create_frame_3d(const media_s *restrict media, const char *restrict frame_name, uintptr_t width, uintptr_t height, uintptr_t depth);

#endif
