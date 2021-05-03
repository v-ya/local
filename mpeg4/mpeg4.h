#ifndef _mpeg4_h_
#define _mpeg4_h_

#include <refer.h>
#include <mlog.h>

typedef struct mpeg4_s mpeg4_s;
typedef struct mpeg4_stuff_s mpeg4_stuff_s;

const mpeg4_s* mpeg4_alloc(void);
void mpeg4_verbose_dump(const mpeg4_s *restrict inst, mlog_s *restrict mlog);
const mpeg4_s* mpeg4_dump(const mpeg4_s *restrict inst, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, uint32_t dump_samples);
mpeg4_stuff_s* mpeg4_create_root_stuff(const mpeg4_s *restrict inst);
mpeg4_stuff_s* mpeg4_create_stuff(const mpeg4_stuff_s *restrict container, const char *restrict type);
mpeg4_stuff_s* mpeg4_link_stuff(mpeg4_stuff_s *restrict container, mpeg4_stuff_s *restrict stuff);
mpeg4_stuff_s* mpeg4_link_before_stuff(mpeg4_stuff_s *restrict after, mpeg4_stuff_s *restrict stuff);
mpeg4_stuff_s* mpeg4_append_stuff(mpeg4_stuff_s *restrict container, const char *restrict type);
mpeg4_stuff_s* mpeg4_find_stuff(mpeg4_stuff_s *restrict container, const char *restrict type);
mpeg4_stuff_s* mpeg4_find_path_stuff(mpeg4_stuff_s *restrict container, const char *const restrict *restrict path);
mpeg4_stuff_s* mpeg4_stuff_container(mpeg4_stuff_s *restrict stuff);
mpeg4_stuff_s* mpeg4_stuff_next(mpeg4_stuff_s *restrict stuff);
mpeg4_stuff_s* mpeg4_stuff_next_same(mpeg4_stuff_s *restrict stuff);
mpeg4_stuff_s* mpeg4_parse(const mpeg4_s *restrict inst, const uint8_t *restrict data, uint64_t size);
mpeg4_stuff_s* mpeg4_calc(mpeg4_stuff_s *restrict stuff);
uint64_t mpeg4_size(const mpeg4_stuff_s *restrict stuff);
const mpeg4_stuff_s* mpeg4_build(const mpeg4_stuff_s *restrict stuff, uint8_t *restrict data);

const mpeg4_stuff_s* mpeg4_stuff__set_major_brand(mpeg4_stuff_s *restrict r, const char *restrict major_brand);
const mpeg4_stuff_s* mpeg4_stuff__set_minor_brand(mpeg4_stuff_s *restrict r, const char *restrict minor_brand);
const mpeg4_stuff_s* mpeg4_stuff__set_minor_version(mpeg4_stuff_s *restrict r, uint32_t minor_version);
const mpeg4_stuff_s* mpeg4_stuff__add_compatible_brand(mpeg4_stuff_s *restrict r, const char *restrict compatible_brand);
const mpeg4_stuff_s* mpeg4_stuff__set_version_and_flags(mpeg4_stuff_s *restrict r, uint32_t version, uint32_t flags);
const mpeg4_stuff_s* mpeg4_stuff__get_version_and_flags(mpeg4_stuff_s *restrict r, uint32_t *restrict version, uint32_t *restrict flags);
const mpeg4_stuff_s* mpeg4_stuff__set_data(mpeg4_stuff_s *restrict r, const void *data, uint64_t size);
const mpeg4_stuff_s* mpeg4_stuff__add_data(mpeg4_stuff_s *restrict r, const void *data, uint64_t size, uint64_t *restrict offset);
const mpeg4_stuff_s* mpeg4_stuff__calc_offset(mpeg4_stuff_s *restrict r, uint64_t *restrict offset);
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
const mpeg4_stuff_s* mpeg4_stuff__set_location(mpeg4_stuff_s *restrict r, const char *restrict location);
const mpeg4_stuff_s* mpeg4_stuff__set_graphicsmode(mpeg4_stuff_s *restrict r, uint16_t graphicsmode);
const mpeg4_stuff_s* mpeg4_stuff__set_opcolor(mpeg4_stuff_s *restrict r, uint16_t red, uint16_t green, uint16_t blue);
const mpeg4_stuff_s* mpeg4_stuff__set_balance(mpeg4_stuff_s *restrict r, double balance);
const mpeg4_stuff_s* mpeg4_stuff__add_edit_list_item(mpeg4_stuff_s *restrict r, uint64_t segment_duration, int64_t media_time, uint16_t media_rate_integer, uint16_t media_rate_fraction);
const mpeg4_stuff_s* mpeg4_stuff__add_chunk_offset(mpeg4_stuff_s *restrict r, mpeg4_stuff_s *restrict mdat, uint64_t offset, uint32_t *restrict chunk_id);
const mpeg4_stuff_s* mpeg4_stuff__set_sample_count(mpeg4_stuff_s *restrict r, uint32_t sample_size, uint32_t sample_count);
const mpeg4_stuff_s* mpeg4_stuff__add_sample_size(mpeg4_stuff_s *restrict r, uint32_t sample_size, uint32_t *restrict sample_id);
const mpeg4_stuff_s* mpeg4_stuff__add_sample_to_chunk(mpeg4_stuff_s *restrict r, uint32_t first_chunk, uint32_t samples_per_chunk, uint32_t sample_description_index);
const mpeg4_stuff_s* mpeg4_stuff__add_time_to_sample(mpeg4_stuff_s *restrict r, uint32_t sample_count, uint32_t sample_delta);
const mpeg4_stuff_s* mpeg4_stuff__add_composition_offset(mpeg4_stuff_s *restrict r, uint32_t sample_count, int32_t sample_offset);
const mpeg4_stuff_s* mpeg4_stuff__add_sync_sample(mpeg4_stuff_s *restrict r, uint32_t sample_id);
const mpeg4_stuff_s* mpeg4_stuff__add_sample_to_group(mpeg4_stuff_s *restrict r, uint32_t sample_count, uint32_t group_description_index);
const mpeg4_stuff_s* mpeg4_stuff__set_ilst_data_text(mpeg4_stuff_s *restrict r, const char *restrict text);

#endif
