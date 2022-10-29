#include "container.h"

struct media_frame_s* media_container_inner_iframe_touch_data(struct media_frame_s *restrict frame, uintptr_t width, uintptr_t height)
{
	uintptr_t image_size[2] = {width, height};
	if (media_frame_test_dimension(frame, 2, image_size) ||
		media_frame_set_dimension(frame, 2, image_size))
		return media_frame_touch_data(frame);
	return NULL;
}
