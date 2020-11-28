#ifndef _largeN_h_
#define _largeN_h_

#include <stdint.h>

typedef struct largeN_t {
	uint64_t n;
	uint64_t q[];
} largeN_t;

typedef struct largeN_cache_t {
	uint64_t *cache;
	uint64_t size;
	uint64_t number;
} largeN_cache_t;

largeN_cache_t* largeN_cache_alloc(uint64_t size);
void largeN_cache_free(largeN_cache_t *restrict cache);
uint64_t largeN_cache_get(largeN_cache_t *restrict cache);
void largeN_cache_set(largeN_cache_t *restrict cache, uint64_t stack);
uint64_t* largeN_cache_pull(largeN_cache_t *restrict cache, uint64_t n);
uint64_t* largeN_cache_zpull(largeN_cache_t *restrict cache, uint64_t n);

largeN_t* largeN_alloc(uint64_t n);
largeN_t* largeN_alloz(uint64_t n);
void largeN_free(largeN_t *ln);

largeN_t* largeN_add_n(largeN_t *restrict r, uint64_t v, uint64_t i);
largeN_t* largeN_add(largeN_t *restrict r, const largeN_t *restrict a);
largeN_t* largeN_sub_n(largeN_t *restrict r, uint64_t v, uint64_t i);
largeN_t* largeN_sub(largeN_t *restrict r, const largeN_t *restrict a);

#endif
