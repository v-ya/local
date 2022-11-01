#ifndef _media_image_container_h_
#define _media_image_container_h_

#include <refer.h>
#include "../0core/media.h"
#include "../0core/stream.h"
#include "../0core/container.h"
#include "../0bits/bits.h"
#include "../media.container.h"
#include "../media.frame.h"
#include "../media.attr.h"
#include "../media.h"

struct media_container_id_s* media_container_create_image_bmp(const struct media_s *restrict media);
struct media_container_id_s* media_container_create_image_jpeg(const struct media_s *restrict media);

// inner

struct media_frame_s* media_container_inner_iframe_touch_data(struct media_frame_s *restrict frame, uintptr_t width, uintptr_t height);

#endif
