#ifndef _script_header_h_
#define _script_header_h_

#include "script.h"

typedef script_t* (*script_header_f)(script_t *restrict script, pocket_saver_s *restrict saver, const char *s);

hashmap_t* script_header_init(hashmap_t *restrict h);
script_t* script_header_parse(script_t *restrict script, pocket_saver_s *restrict saver, const char *restrict *restrict p);

#endif
