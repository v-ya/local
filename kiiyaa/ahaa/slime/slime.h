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
	uintptr_t flags;
	uintptr_t box_size;
	const void *box_data;
};

extern slime_attr_t slime_attr;

slime_t* slime_motto(slime_do_f func, void *arg, const slime_attr_t *restrict attr);

// only used at parent

void slime_catch(slime_t *restrict slime);
void slime_byebye(slime_t *restrict slime);

// only used at slime

slime_t* slime_me(void);

// only used at self or catcher

void* slime_box(slime_t *restrict slime, uintptr_t *restrict size);
uintptr_t slime_box_put(slime_t *restrict slime, uintptr_t offset, const void *data, uintptr_t size);
uintptr_t slime_box_get(slime_t *restrict slime, uintptr_t offset, void *data, uintptr_t size);

#endif
