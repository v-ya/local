#ifndef _phoneme_arg_h_
#define _phoneme_arg_h_

#include "phoneme_type.h"

typedef refer_t (*phoneme_arg2pri_f)(json_inode_t *arg);

typedef struct phoneme_arg_pool_s {
	// 根据 json 转换成 pri
	phoneme_arg2pri_f arg2pri;
	// arg_name => refer_t json_inode_t **arg
	hashmap_t json;
	// arg_name => refer_t *pri
	hashmap_t pri;
} phoneme_arg_pool_s;

phoneme_arg_pool_s* phoneme_arg_pool_alloc(void);
phoneme_arg_s* phoneme_arg_pool_set(phoneme_arg_pool_s *restrict pool, const char *restrict name, phoneme_arg_s *restrict arg);
phoneme_arg_s* phoneme_arg_pool_get_arg(phoneme_arg_pool_s *restrict pool, const char *restrict name);
refer_t phoneme_arg_pool_get_pri(phoneme_arg_pool_s *restrict pool, const char *restrict name);

#endif
