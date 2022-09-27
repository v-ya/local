#include "container.h"

// judge

static d_media_container__initial_judge(image, bmp)
{
	return judge;
}

#define media_container__create_pri__image__bmp NULL

static d_media_container__parse_head(image, bmp)
{
	const struct media_s *restrict m;
	struct media_io_s *restrict io;
	uint8_t bmp_header[14];
	m = c->media;
	io = c->io;
	media_info(m, "image/bmp parse head ...");
	if (media_io_read(io, bmp_header, sizeof(bmp_header)) != sizeof(bmp_header))
		goto label_fail;
	media_mlog_print_rawdata(m->mlog_verbose, "BMP header", bmp_header, sizeof(bmp_header));
	return c;
	label_fail:
	media_warning(m, "image/bmp parse head ... fail");
	return NULL;
}

#define media_container__parse_tail__image__bmp NULL
#define media_container__build_head__image__bmp NULL
#define media_container__build_tail__image__bmp NULL

struct media_container_id_s* media_container_create_image_bmp(void)
{
	struct media_container_id_func_t func;
	d_media_container_func_initial(func, image, bmp);
	return media_container_id_alloc(media_nc_bmp, &func);
}
