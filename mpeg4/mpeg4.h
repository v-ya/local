#ifndef _mpeg4_h_
#define _mpeg4_h_

#include <refer.h>
#include <mlog.h>

typedef struct mpeg4_s mpeg4_s;
typedef struct mpeg4_stuff_s mpeg4_stuff_s;

const mpeg4_s* mpeg4_alloc(void);
const mpeg4_s* mpeg4_dump(const mpeg4_s *restrict inst, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, uint32_t dump_samples);
mpeg4_stuff_s* mpeg4_create_root_stuff(const mpeg4_s *restrict inst);
mpeg4_stuff_s* mpeg4_create_stuff(const mpeg4_stuff_s *restrict container, const char *restrict type);
mpeg4_stuff_s* mpeg4_link_stuff(mpeg4_stuff_s *restrict container, mpeg4_stuff_s *restrict stuff);
mpeg4_stuff_s* mpeg4_append_stuff(mpeg4_stuff_s *restrict container, const char *restrict type);
mpeg4_stuff_s* mpeg4_find_stuff(mpeg4_stuff_s *restrict container, const char *restrict type);
mpeg4_stuff_s* mpeg4_stuff_next(mpeg4_stuff_s *restrict stuff);
mpeg4_stuff_s* mpeg4_stuff_next_same(mpeg4_stuff_s *restrict stuff);
mpeg4_stuff_s* mpeg4_parse(const mpeg4_s *restrict inst, const uint8_t *restrict data, uint64_t size);
mpeg4_stuff_s* mpeg4_calc(mpeg4_stuff_s *restrict stuff);
uint64_t mpeg4_size(const mpeg4_stuff_s *restrict stuff);
const mpeg4_stuff_s* mpeg4_build(const mpeg4_stuff_s *restrict stuff, uint8_t *restrict data);

const mpeg4_stuff_s* mpeg4_stuff__set_major_brand(mpeg4_stuff_s *restrict r, const char *restrict major_brand);
const mpeg4_stuff_s* mpeg4_stuff__set_minor_version(mpeg4_stuff_s *restrict r, uint32_t minor_version);
const mpeg4_stuff_s* mpeg4_stuff__add_compatible_brand(mpeg4_stuff_s *restrict r, const char *restrict compatible_brand);

#endif
