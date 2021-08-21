#ifndef _refer_h_
#define _refer_h_

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

typedef const void* refer_t;

typedef const char* refer_string_t;
typedef const struct refer_nstring_s {
	uintptr_t length;
	char string[];
} *refer_nstring_t;

typedef void (*refer_free_f)(void *restrict v);

refer_t refer_alloc(size_t size);
refer_t refer_alloz(size_t size);
refer_t refer_free(refer_t v);
void refer_set_free(refer_t v, refer_free_f free_func);
refer_free_f refer_get_free(refer_t v);
refer_t refer_save(refer_t v);
uint64_t refer_save_number(refer_t v);

refer_string_t refer_dump_string_with_length(const char *restrict string, size_t length);
refer_string_t refer_dump_string(const char *restrict string);
refer_nstring_t refer_dump_nstring_with_length(const char *restrict string, size_t length);
refer_nstring_t refer_dump_nstring(const char *restrict string);
refer_t refer_dump_data(const void *restrict data, size_t size);

#endif
