#ifndef _mpeg4_h_
#define _mpeg4_h_

#include <refer.h>
#include <mlog.h>

typedef struct mpeg4_s mpeg4_s;

mpeg4_s* mpeg4_alloc(void);
const mpeg4_s* mpeg4_dump(const mpeg4_s *restrict inst, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size);

#endif
