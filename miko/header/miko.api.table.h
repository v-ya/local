#ifndef _miko_api_table_h_
#define _miko_api_table_h_

#include "miko.type.table.h"

struct vattr_s;

miko_table_s* miko_table_create_single(struct vattr_s *restrict pool);
miko_table_s* miko_table_create_multi(struct vattr_s *restrict pool);
miko_index_t miko_table_index(const miko_table_s *restrict r, const char *restrict name);
miko_count_t miko_table_count(const miko_table_s *restrict r, const char *restrict name);

#endif
