#ifndef _kiya_args_h_
#define _kiya_args_h_

#include <stdint.h>

typedef struct kiya_args_t kiya_args_t;
typedef struct kiya_args_pool_t kiya_args_pool_t;

struct kiya_args_t {
	uintptr_t n;
	uintptr_t size;
	const char **argv;
};

kiya_args_pool_t* kiya_args_pool_alloc(uintptr_t size);
kiya_args_t* kiya_args_alloc(void);
void kiya_args_pool_free(kiya_args_pool_t *restrict r);
void kiya_args_free(kiya_args_t *restrict r);
void kiya_args_idol(kiya_args_t *restrict args, const char *restrict idol);
kiya_args_pool_t* kiya_args_set(kiya_args_pool_t *restrict pool, kiya_args_t *restrict args, const char *restrict value);

#endif
