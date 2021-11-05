#ifndef _tparse_tmapping_h_
#define _tparse_tmapping_h_

#include <refer.h>

typedef struct tparse_tmapping_s tparse_tmapping_s;
typedef tparse_tmapping_s* (*tparse_tmapping_add_f)(tparse_tmapping_s *restrict mp, const char *restrict name, refer_t value);
typedef refer_t (*tparse_tmapping_test_f)(tparse_tmapping_s *restrict mp, char c);
typedef void (*tparse_tmapping_clear_f)(tparse_tmapping_s *restrict mp);

struct tparse_tmapping_s {
	tparse_tmapping_add_f add;
	tparse_tmapping_test_f test;
	tparse_tmapping_clear_f clear;
};

tparse_tmapping_s* tparse_tmapping_alloc_single(void);
tparse_tmapping_s* tparse_tmapping_alloc_multi(void);
tparse_tmapping_s* tparse_tmapping_alloc_mixing(void);

#endif
