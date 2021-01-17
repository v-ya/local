#ifndef _iyii_h_
#define _iyii_h_

#include <refer.h>
#include <mlog.h>

typedef struct iyii_s iyii_s;

iyii_s* iyii_alloc(mlog_s *restrict mlog, uint32_t enable_validation);
void iyii_present(iyii_s *restrict iyii);
iyii_s* iyii_do_events(iyii_s *restrict iyii);
void iyii_wait_exit(iyii_s *restrict iyii);

#endif
