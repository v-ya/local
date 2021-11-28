#ifndef _script_h_
#define _script_h_

#include <pocket/pocket-saver.h>
#include <hashmap.h>
#include <json.h>
#include <exbuffer.h>
#include <kiya.h>
#include "pocket-builder-kiya.h"

struct pocket_verify_s;

typedef struct script_t {
	hashmap_t header;
	hashmap_t ptag;
	hashmap_t pool;
	exbuffer_t *buffer;
	kiya_t *kiya;
	void *libkiya;
	void (*kiya_free)(kiya_t *restrict kiya);
	kiya_t* (*kiya_load_path)(kiya_t *restrict kiya, const char *restrict path);
	void* (*kiya_symbol)(kiya_t *restrict kiya, const char *name, const char *restrict symbol);
} script_t;

script_t* script_alloc(void);
void script_free(script_t *restrict s);
script_t* script_kiya_enable(script_t *restrict s, uintptr_t xmsize, struct pocket_verify_s *verify);
script_t* script_kiya_load(script_t *restrict s, const char *restrict kiya_pocket_path);
const char* script_build(script_t *restrict script, pocket_saver_s *restrict saver, const char *restrict s, const char *restrict *restrict linker);
const char* script_link(script_t *restrict script, pocket_s *restrict pocket, const char *restrict s);

#endif
