#ifndef _script_parse_h_
#define _script_parse_h_

#include "buffer.h"

#define skip_space(s)    while (*s == ' ' || *s == '\t') ++s
#define skip_comment(s)  while (*s && *s != '\n') ++s

const char* parse_key(buffer_t *restrict buffer, const char *restrict *restrict p);
buffer_t* parse_value(buffer_t *restrict buffer, const char *restrict *restrict p, uintptr_t *restrict align);
buffer_t* load_file(buffer_t *restrict buffer, const char *restrict path);
buffer_t* load_text(buffer_t *restrict buffer, const char *restrict path);

#endif
