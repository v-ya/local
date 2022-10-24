#ifndef _media_image_stream_h_
#define _media_image_stream_h_

#include <refer.h>
#include "../0core/media.h"
#include "../0core/stream.h"
#include "../media.stream.h"
#include "../media.frame.h"

struct media_stream_id_s* media_stream_create_image_oz_bgra32(const struct media_s *restrict media);

#endif
