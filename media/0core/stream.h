#ifndef _media_core_stream_h_
#define _media_core_stream_h_

#include <refer.h>
#include <vattr.h>
#include "stream_spec.h"
#include "io.h"

struct media_stream_s {
	const struct media_stream_spec_s *spec;
	struct media_container_inner_s *inner;
	refer_t pri_data;
	struct media_attr_s *attr;
	struct media_stack_s *stack;
};

#endif
