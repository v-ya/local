#ifndef _media_core_container_h_
#define _media_core_container_h_

#include <refer.h>
#include <vattr.h>
#include "container_id.h"
#include "media.h"
#include "io.h"
#include "attr.h"

struct media_container_s {
	const struct media_container_id_s *id;
	const struct media_s *media;
	refer_t pri_data;
	struct media_io_s *io;
	struct media_attr_s *attr;
	vattr_s *stream;
};

struct media_container_s* media_container_alloc(const struct media_s *restrict media, const struct media_container_id_s *restrict container_id);

#endif
