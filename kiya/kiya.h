#ifndef _kiya_h_
#define _kiya_h_

#include <pocket/pocket.h>

typedef pocket_s* (*kiya_parse_f)(pocket_s *restrict pocket, pocket_attr_t *restrict root, const char *restrict name);

#endif
