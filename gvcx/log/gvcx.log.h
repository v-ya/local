#ifndef _gvcx_log_pri_h_
#define _gvcx_log_pri_h_

#include "../gvcx.log.h"

typedef struct gvcx_log_inst_s gvcx_log_inst_s;
typedef struct gvcx_log_data_s gvcx_log_data_s;

struct gvcx_log_inst_s {
	gvcx_log_s log;
	gvcx_log_data_s *data;
};

struct gvcx_log_data_s {
	mlog_s *input;
	mlog_s *output;
	uint32_t nindex;
	uint32_t nspace;
	uint32_t mindex;
	uint32_t mspace;
};

#endif
