#include "container.h"

static struct media_attr_judge_s* media_container_initial_judge_image_bmp(struct media_attr_judge_s *restrict judge)
{
	return judge;
}

struct media_container_id_s* media_container_create_image_bmp(void)
{
	struct media_container_id_func_t func;
	func.create = NULL;
	return media_container_id_alloc(media_nc_bmp, media_container_initial_judge_image_bmp, &func);
}
