#ifndef _random_check_h_
#define _random_check_h_

#include <refer.h>
#include <mlog.h>

typedef struct random_check_s random_check_s;

random_check_s* random_check_alloc(mlog_s *ml);
void random_check_check(random_check_s *restrict rc, uint32_t random);
void random_check_clear(random_check_s *restrict rc);
void random_check_dump(random_check_s *restrict rc);

random_check_s* random_check_layer_bits(random_check_s *restrict rc, uint32_t bits);
random_check_s* random_check_layer_mod(random_check_s *restrict rc, uint32_t mod);

#endif
