#ifndef _mpeg4_stuff_method_h_
#define _mpeg4_stuff_method_h_

#include "mpeg4.stuff.h"

typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$major_brand)(mpeg4_stuff_t *restrict r, mpeg4_box_type_t major_brand);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$minor_brand)(mpeg4_stuff_t *restrict r, mpeg4_box_type_t minor_brand);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$minor_version)(mpeg4_stuff_t *restrict r, uint32_t minor_version);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$add$compatible_brands)(mpeg4_stuff_t *restrict r, const mpeg4_box_type_t *restrict compatible_brands, uint32_t n);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$version_and_flags)(mpeg4_stuff_t *restrict r, uint32_t version, uint32_t flags);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$get$version_and_flags)(mpeg4_stuff_t *restrict r, uint32_t *restrict version, uint32_t *restrict flags);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$data)(mpeg4_stuff_t *restrict r, const void *data, uint64_t size);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$add$data)(mpeg4_stuff_t *restrict r, const void *data, uint64_t size, uint64_t *restrict offset);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$calc$offset)(mpeg4_stuff_t *restrict r, uint64_t *restrict offset);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$creation_time)(mpeg4_stuff_t *restrict r, uint64_t creation_time);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$modification_time)(mpeg4_stuff_t *restrict r, uint64_t modification_time);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$timescale)(mpeg4_stuff_t *restrict r, uint32_t timescale);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$duration)(mpeg4_stuff_t *restrict r, uint64_t duration);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$rate)(mpeg4_stuff_t *restrict r, double rate);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$volume)(mpeg4_stuff_t *restrict r, double volume);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$matrix)(mpeg4_stuff_t *restrict r, double matrix[9]);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$next_track_id)(mpeg4_stuff_t *restrict r, uint32_t next_track_id);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$track_id)(mpeg4_stuff_t *restrict r, uint32_t track_id);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$track_layer)(mpeg4_stuff_t *restrict r, int16_t track_layer);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$alternate_group)(mpeg4_stuff_t *restrict r, int16_t alternate_group);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$track_resolution)(mpeg4_stuff_t *restrict r, double width, double height);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$language)(mpeg4_stuff_t *restrict r, const char language[3]);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$name)(mpeg4_stuff_t *restrict r, const char *restrict name);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$location)(mpeg4_stuff_t *restrict r, const char *restrict location);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$graphicsmode)(mpeg4_stuff_t *restrict r, uint16_t graphicsmode);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$opcolor)(mpeg4_stuff_t *restrict r, uint16_t red, uint16_t green, uint16_t blue);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$balance)(mpeg4_stuff_t *restrict r, double balance);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$data_reference_index)(mpeg4_stuff_t *restrict r, uint16_t data_reference_index);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$pixel_resolution)(mpeg4_stuff_t *restrict r, uint16_t width, uint16_t height);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$ppi_resolution)(mpeg4_stuff_t *restrict r, double horizre, double vertre);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$frame_per_sample)(mpeg4_stuff_t *restrict r, uint16_t frame_per_sample);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$depth)(mpeg4_stuff_t *restrict r, uint16_t depth);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$audio_channel_count)(mpeg4_stuff_t *restrict r, uint16_t audio_channel_count);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$audio_sample_size)(mpeg4_stuff_t *restrict r, uint16_t audio_sample_size);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$bitrate)(mpeg4_stuff_t *restrict r, uint32_t buffer_size_db, uint32_t max_bitrate, uint32_t avg_bitrate);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$pixel_aspect_ratio)(mpeg4_stuff_t *restrict r, uint32_t h_spacing, uint32_t v_spacing);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$es_id)(mpeg4_stuff_t *restrict r, uint16_t es_id);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$decoder_config_profile)(mpeg4_stuff_t *restrict r, uint8_t object_profile_indication, uint8_t stream_type);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$add$edit_list_item)(mpeg4_stuff_t *restrict r, uint64_t segment_duration, int64_t media_time, uint16_t media_rate_integer, uint16_t media_rate_fraction);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$add$chunk_offset)(mpeg4_stuff_t *restrict r, mpeg4_stuff_t *restrict mdat, uint64_t offset, uint32_t *restrict chunk_id);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$sample_count)(mpeg4_stuff_t *restrict r, uint32_t sample_size, uint32_t sample_count);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$add$sample_size)(mpeg4_stuff_t *restrict r, uint32_t sample_size, uint32_t *restrict sample_id);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$add$sample_to_chunk)(mpeg4_stuff_t *restrict r, uint32_t first_chunk, uint32_t samples_per_chunk, uint32_t sample_description_index);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$add$time_to_sample)(mpeg4_stuff_t *restrict r, uint32_t sample_count, uint32_t sample_delta);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$add$composition_offset)(mpeg4_stuff_t *restrict r, uint32_t sample_count, int32_t sample_offset);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$add$sync_sample)(mpeg4_stuff_t *restrict r, uint32_t sample_id);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$add$sample_to_group)(mpeg4_stuff_t *restrict r, uint32_t sample_count, uint32_t group_description_index);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$default_sample_description_index)(mpeg4_stuff_t *restrict r, uint32_t default_sample_description_index);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$roll_distance)(mpeg4_stuff_t *restrict r, int16_t roll_distance);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$ilst_data_text)(mpeg4_stuff_t *restrict r, const char *restrict text, uintptr_t length);

typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$inner$push_mdat)(mpeg4_stuff_t *restrict r, mpeg4_stuff_t *restrict mdat);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$inner$push_stco)(mpeg4_stuff_t *restrict r, mpeg4_stuff_t *restrict stco);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$inner$get_mdat)(mpeg4_stuff_t *restrict r, mpeg4_stuff_t ***restrict mdat, uintptr_t *restrict n);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$inner$get_stco)(mpeg4_stuff_t *restrict r, mpeg4_stuff_t ***restrict stco, uintptr_t *restrict n);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$inner$clear)(mpeg4_stuff_t *restrict r);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$inner$set_parse)(mpeg4_stuff_t *restrict r, const void *data, uint64_t size);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$inner$do_parse_mdat)(mpeg4_stuff_t *restrict r, const void **restrict p_data, uint64_t *restrict p_size);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$inner$do_parse_stco)(mpeg4_stuff_t *restrict r, mpeg4_stuff_t *const restrict *restrict mdat, const uint64_t *restrict offset, uintptr_t n);

typedef enum mpeg4_stuff_method_t {
	mpeg4_stuff_method$set$major_brand,
	mpeg4_stuff_method$set$minor_brand,
	mpeg4_stuff_method$set$minor_version,
	mpeg4_stuff_method$add$compatible_brands,
	mpeg4_stuff_method$set$version_and_flags,
	mpeg4_stuff_method$get$version_and_flags,
	mpeg4_stuff_method$set$data,
	mpeg4_stuff_method$add$data,
	mpeg4_stuff_method$calc$offset,
	mpeg4_stuff_method$set$creation_time,
	mpeg4_stuff_method$set$modification_time,
	mpeg4_stuff_method$set$timescale,
	mpeg4_stuff_method$set$duration,
	mpeg4_stuff_method$set$rate,
	mpeg4_stuff_method$set$volume,
	mpeg4_stuff_method$set$matrix,
	mpeg4_stuff_method$set$next_track_id,
	mpeg4_stuff_method$set$track_id,
	mpeg4_stuff_method$set$track_layer,
	mpeg4_stuff_method$set$alternate_group,
	mpeg4_stuff_method$set$track_resolution,
	mpeg4_stuff_method$set$language,
	mpeg4_stuff_method$set$name,
	mpeg4_stuff_method$set$location,
	mpeg4_stuff_method$set$graphicsmode,
	mpeg4_stuff_method$set$opcolor,
	mpeg4_stuff_method$set$balance,
	mpeg4_stuff_method$set$data_reference_index,
	mpeg4_stuff_method$set$pixel_resolution,
	mpeg4_stuff_method$set$ppi_resolution,
	mpeg4_stuff_method$set$frame_per_sample,
	mpeg4_stuff_method$set$depth,
	mpeg4_stuff_method$set$audio_channel_count,
	mpeg4_stuff_method$set$audio_sample_size,
	mpeg4_stuff_method$set$bitrate,
	mpeg4_stuff_method$set$pixel_aspect_ratio,
	mpeg4_stuff_method$set$es_id,
	mpeg4_stuff_method$set$decoder_config_profile,
	mpeg4_stuff_method$add$edit_list_item,
	mpeg4_stuff_method$add$chunk_offset,
	mpeg4_stuff_method$set$sample_count,
	mpeg4_stuff_method$add$sample_size,
	mpeg4_stuff_method$add$sample_to_chunk,
	mpeg4_stuff_method$add$time_to_sample,
	mpeg4_stuff_method$add$composition_offset,
	mpeg4_stuff_method$add$sync_sample,
	mpeg4_stuff_method$add$sample_to_group,
	mpeg4_stuff_method$set$default_sample_description_index,
	mpeg4_stuff_method$set$roll_distance,
	mpeg4_stuff_method$set$ilst_data_text,
	mpeg4_stuff_method_max,

	mpeg4_stuff_method$inner$push_mdat,
	mpeg4_stuff_method$inner$push_stco,
	mpeg4_stuff_method$inner$get_mdat,
	mpeg4_stuff_method$inner$get_stco,
	mpeg4_stuff_method$inner$clear,
	mpeg4_stuff_method$inner$set_parse,
	mpeg4_stuff_method$inner$do_parse_mdat,
	mpeg4_stuff_method$inner$do_parse_stco,
	mpeg4_stuff_method_inner_max
} mpeg4_stuff_method_t;

#define mpeg4$stuff$method$set(_atom, _name, _mid)  rbtree_insert(&(_atom)->method, NULL, mpeg4_stuff_method$##_mid, mpeg4$define(stuff, _name, _mid), NULL)
#define mpeg4$stuff$method$call(_stuff, _mid, ...)  ({register rbtree_t *restrict _rbv = rbtree_find(&(_stuff)->atom->method, NULL, mpeg4_stuff_method$##_mid);\
							(_rbv && _rbv->value)?((mpeg4_stuff_func$##_mid) _rbv->value)(_stuff, ##__VA_ARGS__):NULL;})

#endif
