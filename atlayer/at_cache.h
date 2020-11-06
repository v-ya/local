#ifndef _at_cache_h_
#define _at_cache_h_

#include <stdint.h>

typedef struct at_cache_t {
	void *list;
	void *ptr;
	uintptr_t size;
	uintptr_t number;
} at_cache_t;

void at_cache_init(at_cache_t *restrict atc, uintptr_t size, uintptr_t number);
void at_cache_uini(at_cache_t *restrict atc);
void* at_cache_pull(at_cache_t *restrict atc);
void at_cache_push(at_cache_t *restrict atc, void *v);
uintptr_t at_cache_b_leak(const at_cache_t *restrict atc);

#define atc_def(_type)           at_cache_t __atc$##_type
#define atc_init(_r, _type, _n)  at_cache_init(&(_r)->__atc$##_type, sizeof(_type), _n)
#define atc_uini(_r, _type)      at_cache_uini(&(_r)->__atc$##_type)
#define atc_pull(_r, _type)      ((_type *) at_cache_pull(&(_r)->__atc$##_type))
#define atc_push(_r, _type, _v)  at_cache_push(&(_r)->__atc$##_type, _v)
#define atc_leak(_r, _type)      at_cache_b_leak(&(_r)->__atc$##_type)

typedef struct at_cache_array_t at_cache_array_t;

at_cache_array_t* at_cache_array_alloc(uintptr_t size, uintptr_t number, uintptr_t offset, uintptr_t nc);
void at_cache_array_free(at_cache_array_t *restrict atca);
void* at_cache_array_pull(at_cache_array_t *restrict atca, uintptr_t n);
void at_cache_array_push(at_cache_array_t *restrict atca, void *v, uintptr_t n);
uintptr_t at_cache_array_b_leak(const at_cache_array_t *restrict atca);

#define atca_def(_type)                              at_cache_array_t *__atca$##_type
#define atca_init(_r, _type, _offset, _number, _nc)  ((_r)->__atca$##_type = at_cache_array_alloc(sizeof(_type), _offset, _number, _nc))
#define atca_uini(_r, _type)                         if ((_r)->__atca$##_type) at_cache_array_free((_r)->__atca$##_type), (_r)->__atca$##_type = NULL
#define atca_pull(_r, _type, _n)                     ((_type *) at_cache_array_pull((_r)->__atca$##_type, _n))
#define atca_push(_r, _type, _v, _n)                 at_cache_array_push((_r)->__atca$##_type, _v, _n)
#define atca_leak(_r, _type)                         at_cache_array_b_leak((_r)->__atca$##_type)

#endif
