#ifndef _pocket_verify_h_
#define _pocket_verify_h_

#include "pocket.h"
#include <refer.h>

typedef struct pocket_verify_s pocket_verify_s;
typedef struct pocket_verify_entry_s pocket_verify_entry_s;

typedef const pocket_verify_entry_s* (*pocket_verify_build_f)(const pocket_verify_entry_s *restrict entry, void *verify, const uint8_t *data, uint64_t size);
typedef const pocket_verify_entry_s* (*pocket_verify_check_f)(const pocket_verify_entry_s *restrict entry, void *verify, const uint8_t *data, uint64_t size);
typedef const pocket_verify_entry_s* (*pocket_verify_method_f)(const pocket_verify_s *restrict pool, const char *restrict name);

struct pocket_verify_entry_s {
	pocket_verify_build_f build;
	pocket_verify_check_f check;
	pocket_tag_t tag;
	uint32_t align;
	uint64_t size;
};

struct pocket_verify_s {
	pocket_verify_method_f method;
};

#endif
