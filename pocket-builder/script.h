#ifndef _script_h_
#define _script_h_

#include <pocket/pocket-saver.h>
#include <hashmap.h>
#include <json.h>
#include "buffer.h"

typedef struct script_t {
	hashmap_t header;
	hashmap_t ptag;
	buffer_t *buffer;
} script_t;

typedef script_t* (*script_header_f)(script_t *restrict script, pocket_saver_s *restrict saver, const char *s);
typedef pocket_saver_s* (*script_set_f)(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align);

script_t* script_alloc(void);
void script_free(script_t *restrict s);
const char* script_build(script_t *restrict script, pocket_saver_s *restrict saver, const char *restrict s);

#endif
