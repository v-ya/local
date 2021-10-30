#ifndef _tparse_tmapping_h_
#define _tparse_tmapping_h_

#include <refer.h>

typedef struct tparse_tmapping_single_s tparse_tmapping_single_s;
typedef struct tparse_tmapping_multi_s tparse_tmapping_multi_s;
typedef struct tparse_tmapping_mixing_s tparse_tmapping_mixing_s;

tparse_tmapping_single_s* tparse_tmapping_alloc_single(void);
tparse_tmapping_multi_s* tparse_tmapping_alloc_multi(void);
tparse_tmapping_mixing_s* tparse_tmapping_alloc_mixing(void);

tparse_tmapping_single_s* tparse_tmapping_add_single(tparse_tmapping_single_s *restrict mp, char name, refer_t value);
tparse_tmapping_multi_s* tparse_tmapping_add_multi(tparse_tmapping_multi_s *restrict mp, const char *restrict name, refer_t value);
tparse_tmapping_mixing_s* tparse_tmapping_add_mixing(tparse_tmapping_mixing_s *restrict mp, const char *restrict name, refer_t value);

refer_t tparse_tmapping_test_single(tparse_tmapping_single_s *restrict mp, char c);
refer_t tparse_tmapping_test_multi(tparse_tmapping_multi_s *restrict mp, char c);
refer_t tparse_tmapping_test_mixing(tparse_tmapping_mixing_s *restrict mp, char c);

void tparse_tmapping_clear_single(tparse_tmapping_single_s *restrict mp);
void tparse_tmapping_clear_multi(tparse_tmapping_multi_s *restrict mp);
void tparse_tmapping_clear_mixing(tparse_tmapping_mixing_s *restrict mp);

#endif
