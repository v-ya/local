#ifndef _tparse_tstring_h_
#define _tparse_tstring_h_

#include <refer.h>

typedef struct tparse_tstring_s tparse_tstring_s;
typedef struct tparse_tstring_trigger_s tparse_tstring_trigger_s;
typedef struct tparse_tstring_value_t tparse_tstring_value_t;
typedef struct tparse_tstring_cache_s tparse_tstring_cache_s;

typedef tparse_tstring_trigger_s* (*tparse_tstring_trigger_f)(tparse_tstring_trigger_s *restrict trigger, tparse_tstring_value_t *restrict value, const char *restrict data, uintptr_t size, uintptr_t *restrict pos);

typedef enum tparse_tstring_trigger_t {
	tparse_tstring_trigger_head,
	tparse_tstring_trigger_inner,
	tparse_tstring_trigger_tail
} tparse_tstring_trigger_t;

struct tparse_tstring_trigger_s {
	tparse_tstring_trigger_f trigger;
};

// value

tparse_tstring_value_t* tparse_tstring_value_append(tparse_tstring_value_t *restrict value, const char *restrict string, uintptr_t length);
void tparse_tstring_value_rollback(tparse_tstring_value_t *restrict value, uintptr_t n);
void tparse_tstring_value_finally(tparse_tstring_value_t *restrict value);

// inst build

tparse_tstring_s* tparse_tstring_alloc_empty(void);
tparse_tstring_s* tparse_tstring_set_trigger(tparse_tstring_s *restrict ts, tparse_tstring_trigger_t type, const char *restrict inner_name, tparse_tstring_trigger_s *trigger);
tparse_tstring_s* tparse_tstring_set_trigger_func(tparse_tstring_s *restrict ts, tparse_tstring_trigger_t type, const char *restrict inner_name, tparse_tstring_trigger_f trigger_func);

// inst do

void tparse_tstring_clear(tparse_tstring_s *restrict ts);
tparse_tstring_s* tparse_tstring_transport(tparse_tstring_s *restrict ts, const char *restrict data, uintptr_t size, uintptr_t pos, uintptr_t *restrict rpos);
const char* tparse_tstring_get_data(tparse_tstring_s *restrict ts, uintptr_t *restrict length);
const char* tparse_tstring_get_string(tparse_tstring_s *restrict ts, uintptr_t *restrict length);

// inst preset

tparse_tstring_s* tparse_tstring_alloc_c_parse_multi_quotes(void);
tparse_tstring_s* tparse_tstring_alloc_c_parse_single_quotes(void);

// string cache

tparse_tstring_cache_s* tparse_tstring_cache_alloc(void);
const char* tparse_tstring_cache_get(tparse_tstring_cache_s *restrict cache, const char *restrict text, uintptr_t length);

#endif
