#ifndef _ahaa_slime_h_
#define _ahaa_slime_h_

#include <stdint.h>

typedef const struct slime_t slime_t;
typedef struct slime_attr_t slime_attr_t;

// maybe void (*slime_do_f)(void *arg, slime_t *restrict slime)
typedef void (*slime_do_f)(void *arg);

typedef enum slime_flag_t {
	slime_flag$detach = 0x01,
} slime_flag_t;

struct slime_attr_t {
	uintptr_t stack_size;
	uintptr_t tls_size;
	uintptr_t flags;
	void *tls_data;
};

struct slime_t {
	struct slime_t *this;
	void *tls_start;
	void *stack_start;
	void **stack_top;
	uintptr_t tls_number;
	uintptr_t tpid;
	slime_attr_t attr;
};

extern slime_attr_t slime_attr;

slime_t* slime_motto(slime_do_f func, void *arg, const slime_attr_t *restrict attr);
void slime_catch(slime_t *restrict slime, int *restrict rstatus);
void slime_byebye(slime_t *restrict slime);

#endif
