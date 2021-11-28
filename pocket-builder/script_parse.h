#ifndef _script_parse_h_
#define _script_parse_h_

#include <exbuffer.h>

#define skip_space(s)    while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') ++s
#define skip_comment(s)  while (*s && *s != '\n') ++s

int parse_is_key(const char *restrict s);
const char* parse_key(exbuffer_t *restrict buffer, const char *restrict *restrict p);
exbuffer_t* parse_value(exbuffer_t *restrict buffer, const char *restrict *restrict p, uintptr_t *restrict align);
exbuffer_t* parse_load_file(exbuffer_t *restrict buffer, const char *restrict path);
exbuffer_t* parse_load_text(exbuffer_t *restrict buffer, const char *restrict path);

#endif
