#ifndef _miko_wink_miko_wink_report_h_
#define _miko_wink_miko_wink_report_h_

#include "miko.wink.h"

typedef struct miko_wink_report_default_s miko_wink_report_default_s;

struct miko_wink_report_default_s {
	miko_wink_report_s report;
	mlog_s *mlog;
	miko_wink_report_t last;
	uintptr_t only_modify_output;
};

miko_wink_report_s* miko_wink_report_create_default(mlog_s *restrict mlog, uintptr_t only_modify_output);

#endif
