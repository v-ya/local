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
const mpeg4_stuff_s* mpeg4_stuff__set_data(mpeg4_stuff_s *restrict r, const void *data, uintptr_t size);
const mpeg4_stuff_s* mpeg4_stuff__set_version_and_flags(mpeg4_stuff_s *restrict r, uint32_t version, uint32_t flags);
const mpeg4_stuff_s* mpeg4_stuff__set_creation_time(mpeg4_stuff_s *restrict r, uint64_t creation_time);
const mpeg4_stuff_s* mpeg4_stuff__set_modification_time(mpeg4_stuff_s *restrict r, uint64_t modification_time);
const mpeg4_stuff_s* mpeg4_stuff__set_timescale(mpeg4_stuff_s *restrict r, uint32_t timescale);
const mpeg4_stuff_s* mpeg4_stuff__set_duration(mpeg4_stuff_s *restrict r, uint64_t duration);
const mpeg4_stuff_s* mpeg4_stuff__set_rate(mpeg4_stuff_s *restrict r, double rate);
const mpeg4_stuff_s* mpeg4_stuff__set_volume(mpeg4_stuff_s *restrict r, double volume);
const mpeg4_stuff_s* mpeg4_stuff__set_matrix(mpeg4_stuff_s *restrict r, double matrix[9]);
const mpeg4_stuff_s* mpeg4_stuff__set_next_track_id(mpeg4_stuff_s *restrict r, uint32_t next_track_id);
const mpeg4_stuff_s* mpeg4_stuff__set_track_id(mpeg4_stuff_s *restrict r, uint32_t track_id);
const mpeg4_stuff_s* mpeg4_stuff__set_track_layer(mpeg4_stuff_s *restrict r, int16_t track_layer);
const mpeg4_stuff_s* mpeg4_stuff__set_alternate_group(mpeg4_stuff_s *restrict r, int16_t alternate_group);
const mpeg4_stuff_s* mpeg4_stuff__set_resolution(mpeg4_stuff_s *restrict r, double width, double height);
const mpeg4_stuff_s* mpeg4_stuff__set_language(mpeg4_stuff_s *restrict r, const char *restrict language);
const mpeg4_stuff_s* mpeg4_stuff__set_name(mpeg4_stuff_s *restrict r, const char *restrict name);

#endif
