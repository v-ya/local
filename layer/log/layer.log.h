#ifndef _layer_log_pri_h_
#define _layer_log_pri_h_

#include "../layer.log.h"

typedef struct layer_log_inst_s layer_log_inst_s;
typedef struct layer_log_data_s layer_log_data_s;

struct layer_log_inst_s {
	layer_log_s log;
	layer_log_data_s *data;
};

struct layer_log_data_s {
	mlog_s *input;
	mlog_s *output;
	uint32_t nindex;
	uint32_t nspace;
	uint32_t mindex;
	uint32_t mspace;
};

#endif
