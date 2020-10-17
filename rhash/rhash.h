#ifndef _rhash_h_
#define _rhash_h_

#include <stdint.h>
#include <refer.h>
#include <random/random.h>

typedef struct rhash_s {
	random_s *random;
	size_t index;
	size_t size;
} rhash_s;

typedef struct rhash64_s {
	rhash_s header;
	uint64_t rdata[];
} rhash64_s;

rhash64_s* rhash64_alloc_by_r64(random64_s *restrict r64, size_t n, uint64_t key);
rhash64_s* rhash64_alloc(const char *restrict method, size_t n, uint64_t key);
void rhash64_reinit(rhash64_s *restrict r, uint64_t key);
void rhash64_send(rhash64_s *restrict r, const uint64_t *restrict data, size_t n);
uint64_t* rhash64_final(rhash64_s *restrict r);

#endif
