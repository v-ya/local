#ifndef _kiya_h_
#define _kiya_h_

#include <pocket/pocket.h>
#include <mlog.h>

typedef struct kiya_t kiya_t;

typedef pocket_s* (*kiya_tag_f)(pocket_s *restrict pocket);
typedef pocket_s* (*kiya_parse_f)(pocket_s *restrict pocket, const pocket_attr_t *restrict root, const char *restrict name, const char *restrict *restrict error);

typedef const char* (*kiya_initial_f)(uintptr_t argc, const char *const argv[]);
typedef void (*kiya_finally_f)(void);
typedef int (*kiya_main_f)(uintptr_t argc, const char *const argv[]);

kiya_t* kiya_alloc(mlog_s *restrict mlog, size_t xsize);
void kiya_free(kiya_t *restrict kiya);
void kiya_set_verify(kiya_t *restrict kiya, const struct pocket_verify_s *verify);
kiya_t* kiya_set_source(kiya_t *restrict kiya, const char *restrict name, const char *restrict path);
kiya_t* kiya_set_arg(kiya_t *restrict kiya, const char *restrict name, const char *restrict value);
kiya_t* kiya_load(kiya_t *restrict kiya, pocket_s *restrict pocket);
kiya_t* kiya_load_path(kiya_t *restrict kiya, const char *restrict path);
void* kiya_symbol(kiya_t *restrict kiya, const char *name, const char *restrict symbol);
kiya_t* kiya_do(kiya_t *restrict kiya, const char *name, const char *restrict main, int *restrict ret);

#endif
