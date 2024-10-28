#ifndef _miko_wink_miko_wink_gomi_request_h_
#define _miko_wink_miko_wink_gomi_request_h_

#include "miko.wink.gomi.h"

typedef enum miko_wink_gomi_request_type_t miko_wink_gomi_request_type_t;
typedef enum miko_wink_gomi_request_result_t miko_wink_gomi_request_result_t;
typedef struct miko_wink_gomi_request_s miko_wink_gomi_request_s;
typedef struct miko_wink_gomi_request_gomi_s miko_wink_gomi_request_gomi_s;
typedef struct miko_wink_gomi_request_cycle_s miko_wink_gomi_request_cycle_s;
typedef struct miko_wink_gomi_request_report_s miko_wink_gomi_request_report_s;

enum miko_wink_gomi_request_type_t {
	miko_wink_gomi_request_type__null,
	miko_wink_gomi_request_type__gomi,
	miko_wink_gomi_request_type__cycle,
	miko_wink_gomi_request_type__report
};

enum miko_wink_gomi_request_result_t {
	miko_wink_gomi_request_result__unknow,
	miko_wink_gomi_request_result__okay,
	miko_wink_gomi_request_result__fail
};

struct miko_wink_gomi_request_s {
	miko_wink_gomi_request_type_t type;
	miko_wink_gomi_request_result_t result;
	yaw_lock_s *notify;
};

struct miko_wink_gomi_request_gomi_s {
	miko_wink_gomi_request_s request;
	miko_wink_report_t report;
};

struct miko_wink_gomi_request_cycle_s {
	miko_wink_gomi_request_s request;
	uintptr_t miko_gomi_msec;
};

struct miko_wink_gomi_request_report_s {
	miko_wink_gomi_request_s request;
	miko_wink_report_s *report;
};

miko_wink_gomi_request_s* miko_wink_gomi_request_wait(miko_wink_gomi_s *restrict gomi, miko_wink_gomi_request_s *restrict req);

miko_wink_gomi_request_gomi_s* miko_wink_gomi_request_create_gomi(void);
miko_wink_gomi_request_cycle_s* miko_wink_gomi_request_create_cycle(uintptr_t miko_gomi_msec);
miko_wink_gomi_request_report_s* miko_wink_gomi_request_create_report(miko_wink_report_s *restrict report);

#endif
