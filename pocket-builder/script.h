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

script_t* script_alloc(void);
void script_free(script_t *restrict s);
const char* script_build(script_t *restrict script, pocket_saver_s *restrict saver, const char *restrict s, const char *restrict *restrict linker);
const char* script_link(pocket_s *restrict pocket, const char *restrict s);

#endif
