#ifndef _pocket_verify_h_
#define _pocket_verify_h_

#include "pocket.h"
#include <refer.h>

#define pocket_verify_attr_name  "verify"

typedef struct pocket_verify_s pocket_verify_s;
typedef struct pocket_verify_entry_s pocket_verify_entry_s;

typedef const pocket_verify_entry_s* (*pocket_verify_build_f)(const pocket_verify_entry_s *restrict entry, void *verify, const uint8_t *data, uint64_t size);
typedef const pocket_verify_entry_s* (*pocket_verify_check_f)(const pocket_verify_entry_s *restrict entry, void *verify, const uint8_t *data, uint64_t size);

typedef const pocket_verify_entry_s* (*pocket_verify_method_f)(const pocket_verify_s *restrict pool, const char *restrict name);
typedef pocket_verify_s* (*pocket_verify_set_f)(pocket_verify_s *restrict pool, const char *restrict name, const pocket_verify_entry_s *restrict entry);
typedef void (*pocket_verify_unset_f)(pocket_verify_s *restrict pool, const char *restrict name);

struct pocket_verify_entry_s {
	pocket_verify_build_f build;
	pocket_verify_check_f check;
	uint64_t size;
	pocket_tag_t tag;
	uint32_t align;
};

struct pocket_verify_s {
	pocket_verify_method_f method;
	pocket_verify_set_f set;
	pocket_verify_unset_f unset;
};

pocket_verify_entry_s* pocket_verify_entry_alloc(pocket_verify_build_f build, pocket_verify_check_f check, uint64_t size, pocket_tag_t tag, uint32_t align);

pocket_verify_s* pocket_verify_empty(void);
pocket_verify_s* pocket_verify_default(void);

#endif
