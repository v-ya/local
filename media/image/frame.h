#ifndef _media_image_frame_h_
#define _media_image_frame_h_

#include <refer.h>
#include "../0core/media.h"
#include "../0core/frame.h"
#include "../media.frame.h"

struct media_frame_id_s* media_frame_create_image_bgra32(void);
struct media_frame_id_s* media_frame_create_image_bgr24(void);
struct media_frame_id_s* media_frame_create_image_bgr24_p4(void);

#endif
