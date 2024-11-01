#ifndef _miko_api_major_h_
#define _miko_api_major_h_

#include "miko.type.major.h"
#include "miko.type.minor.h"

miko_major_s* miko_major_add_minor(miko_major_s *restrict r, miko_minor_s *restrict minor);
miko_minor_s* miko_major_save_minor_by_index(const miko_major_s *restrict r, miko_index_t index);
miko_minor_s* miko_major_save_minor_by_name(const miko_major_s *restrict r, const char *restrict name);

#endif
