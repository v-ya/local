#ifndef _phoneme_type_h_
#define _phoneme_type_h_

#include <refer.h>
#include <hashmap.h>
#include <json.h>

typedef const char *phoneme_src_name_s;
typedef json_inode_t *phoneme_arg_s;
typedef struct phoneme_s phoneme_s;

extern uint8_t phoneme_alpha_table_space[256];

void phoneme_hashmap_free_refer_func(hashmap_vlist_t *restrict vl);
phoneme_src_name_s phoneme_src_name_dump(const char *restrict s);
phoneme_arg_s* phoneme_arg_alloc(json_inode_t *restrict j);
phoneme_arg_s* phoneme_arg_dump(phoneme_arg_s *restrict a);
char* phoneme_read_string(char *restrict buffer, size_t n, const char **restrict ps, const char *restrict endc);

#endif
