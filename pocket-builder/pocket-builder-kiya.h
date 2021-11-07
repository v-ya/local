#ifndef _pocket_builder_kiya_h_
#define _pocket_builder_kiya_h_

#include <pocket/pocket-saver.h>

typedef pocket_saver_index_t* (*pocket_builder_kiya_main_f)(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, uintptr_t argc, const char *const argv[]);

#endif
