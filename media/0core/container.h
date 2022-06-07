#ifndef _media_core_container_h_
#define _media_core_container_h_

#include <refer.h>
#include <vattr.h>
#include "container_id.h"
#include "attr.h"

struct media_container_inst_s {
	refer_t pri_data;
	vattr_s *stream;
};

struct media_container_s {
	const struct media_container_id_s *id;
	struct media_container_inst_s *inst;
	struct media_attr_s *attr;
};

#endif
