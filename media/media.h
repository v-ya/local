#ifndef _media_h_
#define _media_h_

#include <refer.h>
#include <fsys.h>

struct mlog_s;

typedef struct media_s media_s;
typedef struct media_io_s media_io_s;
typedef struct media_attr_s media_attr_s;
typedef struct media_frame_s media_frame_s;
typedef struct media_stream_s media_stream_s;
typedef struct media_container_s media_container_s;
typedef struct media_transform_s media_transform_s;
typedef struct media_runtime_s media_runtime_s;
typedef struct media_runtime_task_s media_runtime_task_s;

typedef enum media_loglevel_t {
	media_loglevel_bit_error   = 0x0001,
	media_loglevel_bit_warning = 0x0002,
	media_loglevel_bit_info    = 0x0004,
	media_loglevel_bit_verbose = 0x0008,
	media_loglevel_error       = 0x0001,
	media_loglevel_warning     = 0x0003,
	media_loglevel_info        = 0x0007,
	media_loglevel_verbose     = 0x000f,
} media_loglevel_t;

const media_s* media_alloc(media_loglevel_t loglevel, struct mlog_s *restrict mlog);
struct mlog_s* media_get_mlog(const media_s *restrict media, media_loglevel_t loglevel);

// attr

media_attr_s* media_attr_set_int(media_attr_s *restrict attr, const char *restrict name, int64_t value);
media_attr_s* media_attr_set_float(media_attr_s *restrict attr, const char *restrict name, double value);
media_attr_s* media_attr_set_string(media_attr_s *restrict attr, const char *restrict name, const char *restrict string);
media_attr_s* media_attr_set_data(media_attr_s *restrict attr, const char *restrict name, const void *data, uintptr_t size);
media_attr_s* media_attr_refer_string(media_attr_s *restrict attr, const char *restrict name, refer_string_t string);
media_attr_s* media_attr_refer_data(media_attr_s *restrict attr, const char *restrict name, refer_nstring_t data);

const media_attr_s* media_attr_get_int(const media_attr_s *restrict attr, const char *restrict name, int64_t *restrict value);
const media_attr_s* media_attr_get_float(const media_attr_s *restrict attr, const char *restrict name, double *restrict value);
const media_attr_s* media_attr_get_string(const media_attr_s *restrict attr, const char *restrict name, refer_string_t *restrict string);
const media_attr_s* media_attr_get_data(const media_attr_s *restrict attr, const char *restrict name, refer_nstring_t *restrict data);

void media_attr_dump(const media_attr_s *restrict attr, const media_s *restrict media, media_loglevel_t loglevel, const char *restrict name);

// frame

media_frame_s* media_create_frame(const media_s *restrict media, const char *restrict frame_name, uintptr_t d, const uintptr_t *restrict dv);
media_frame_s* media_create_frame_1d(const media_s *restrict media, const char *restrict frame_name, uintptr_t n);
media_frame_s* media_create_frame_2d(const media_s *restrict media, const char *restrict frame_name, uintptr_t width, uintptr_t height);
media_frame_s* media_create_frame_3d(const media_s *restrict media, const char *restrict frame_name, uintptr_t width, uintptr_t height, uintptr_t depth);
refer_string_t media_frame_get_name(const media_frame_s *restrict frame);
refer_string_t media_frame_get_compat(const media_frame_s *restrict frame);

// container

media_attr_s* media_container_get_attr(const media_container_s *restrict container);
media_io_s* media_container_get_io(const media_container_s *restrict container);
media_container_s* media_container_write_head(media_container_s *restrict container);
media_container_s* media_container_write_tail(media_container_s *restrict container);
media_container_s* media_container_set_input(media_container_s *restrict container, media_io_s *restrict io);
media_container_s* media_container_set_output(media_container_s *restrict container, media_io_s *restrict io);
media_container_s* media_create_container(const media_s *restrict media, const char *restrict container_name);
media_container_s* media_create_input(const media_s *restrict media, const char *restrict container_name, media_io_s *restrict io);
media_container_s* media_create_output(const media_s *restrict media, const char *restrict container_name, media_io_s *restrict io);
media_container_s* media_create_input_by_memory(const media_s *restrict media, const char *restrict container_name, const void *data, uintptr_t size);
media_container_s* media_create_output_by_memory(const media_s *restrict media, const char *restrict container_name);
media_container_s* media_create_input_by_path(const media_s *restrict media, const char *restrict container_name, const char *restrict path);
media_container_s* media_create_output_by_path(const media_s *restrict media, const char *restrict container_name, const char *restrict path);

// don't free `media_container_new_stream` return value
struct media_stream_s* media_container_new_stream(struct media_container_s *restrict container, const char *restrict stream_type, const char *restrict frame_name);
struct media_stream_s* media_container_find_stream(const struct media_container_s *restrict container, const char *restrict stream_type, uintptr_t index);

// stream

refer_string_t media_stream_get_frame_name(const media_stream_s *restrict stream);
refer_string_t media_stream_get_frame_compat(const media_stream_s *restrict stream);
media_attr_s* media_stream_get_attr(const media_stream_s *restrict stream);

struct media_frame_s* media_stream_create_frame(struct media_stream_s *restrict stream);
struct media_frame_s* media_stream_read_frame_by_index(struct media_stream_s *restrict stream, struct media_frame_s *restrict frame, uintptr_t index);
struct media_frame_s* media_stream_read_frame(struct media_stream_s *restrict stream, struct media_frame_s *restrict frame);
struct media_stream_s* media_stream_write_frame(struct media_stream_s *restrict stream, const struct media_frame_s *restrict frame);

// io

uint64_t media_io_get_size(media_io_s *restrict io);
uint64_t media_io_get_offset(media_io_s *restrict io);
uint64_t media_io_set_offset(media_io_s *restrict io, uint64_t offset);
uintptr_t media_io_read_data(media_io_s *restrict io, void *data, uintptr_t size);
uintptr_t media_io_write_data(media_io_s *restrict io, const void *data, uintptr_t size);
void* media_io_map_data(media_io_s *restrict io, uintptr_t *restrict rsize);
media_io_s* media_io_sync_data(media_io_s *restrict io);

struct media_io_s* media_io_create_memory(const void *restrict pre_data, uintptr_t pre_size);
struct media_io_s* media_io_create_memory_const(const void *restrict data, uintptr_t size, refer_t data_source);
struct media_io_s* media_io_create_fsys(const char *restrict path, fsys_file_flag_t flag, uintptr_t cache_number, uintptr_t cache_size);

// runtime

struct media_runtime_s* media_runtime_alloc(uintptr_t unit_core_number, uintptr_t task_queue_limit, uintptr_t friendly);
void media_runtime_stop(struct media_runtime_s *restrict runtime);

struct media_runtime_task_s* media_runtime_task_cancel(struct media_runtime_task_s *restrict task);
const struct media_runtime_task_s* media_runtime_task_is_finish(const struct media_runtime_task_s *restrict task);
const struct media_runtime_task_s* media_runtime_task_is_okay(const struct media_runtime_task_s *restrict task);
struct media_runtime_task_s* media_runtime_task_wait_time(struct media_runtime_task_s *restrict task, uintptr_t usec);
void media_runtime_task_wait(struct media_runtime_task_s *restrict task);

// transform

struct media_transform_s* media_create_transform(const media_s *restrict media, const char *restrict src_frame_compat, const char *restrict dst_frame_compat);
struct media_frame_s* media_conver_frame(const media_s *restrict media, media_runtime_s *restrict runtime, const media_frame_s *restrict frame, const char *restrict target_frame_name, const uintptr_t *restrict timeout_usec);

struct media_transform_s* media_transform_open(struct media_transform_s *restrict transform);
void media_transform_close(struct media_transform_s *restrict transform);
struct media_frame_s* media_transform_alloc_conver(struct media_runtime_s *restrict runtime, struct media_transform_s *restrict transform, const struct media_frame_s *restrict src_frame, const char *restrict dst_frame_name, const uintptr_t *restrict timeout_usec);

#endif
