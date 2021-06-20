#ifndef _script_ptag_h_
#define _script_ptag_h_

#include "script.h"

typedef pocket_saver_s* (*script_set_f)(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const exbuffer_t *restrict buffer, uintptr_t align);

hashmap_t* script_ptag_init(hashmap_t *restrict p);

#endif
