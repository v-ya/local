#ifndef _iphyee_pri_h_
#define _iphyee_pri_h_

#include "iphyee.h"
#include <vattr.h>

struct iphyee_worker_instance_s;

struct iphyee_s {
	struct iphyee_worker_instance_s *worker_instance;
	vattr_s *bonex_preset;
};

#endif
