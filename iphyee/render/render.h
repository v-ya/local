#ifndef _iphyee_render_pri_h_
#define _iphyee_render_pri_h_

#include "../iphyee.h"
#include "../iphyee.worker.h"
#include "../iphyee.glslc.h"
#include <queue/queue.h>
#include <vattr.h>

typedef struct iphyee_render_screen_s iphyee_render_screen_s;
typedef struct iphyee_render_texture_s iphyee_render_texture_s;
typedef struct iphyee_render_tpool_s iphyee_render_tpool_s;

struct iphyee_render_screen_s {
	iphyee_worker_buffer_s *screen_host;
	iphyee_worker_buffer_s *buffer_device;
	const void *screen_data;
	uint64_t screen_address;
	uint64_t screen_offset;
	uint64_t screen_size;
	uint64_t depth_address;
	uint64_t depth_offset;
	uint64_t depth_size;
	uint64_t mutex_address;
	uint64_t mutex_offset;
	uint64_t mutex_size;
	uint32_t width;
	uint32_t height;
};

struct iphyee_render_texture_s {
	iphyee_worker_s *worker;
	refer_string_t texture_name;
	iphyee_worker_buffer_s *texture_host;
	iphyee_worker_buffer_s *texture_device;
	uint64_t texture_address;
	uint64_t texture_size;
	uint32_t texture_width;
	uint32_t texture_height;
	volatile uint32_t texture_index;
	volatile uint32_t texture_vaild;
};

struct iphyee_render_tpool_s {
	iphyee_worker_s *worker;
	iphyee_worker_buffer_s *tpool_host;
	iphyee_worker_buffer_s *tpool_device;
	iphyee_render_texture_s *texture_miss;
	iphyee_glslc_image_t *texture_array;
	vattr_s *texture_pool;
	queue_s *texture_insert;
	queue_s *texture_delete;
	uint64_t *tpool_mask;
	uint64_t tpool_address;
	uint64_t tpool_size;
	uintptr_t need_update;
	uint32_t texture_max_count;
	uint32_t texture_use_count;
};

iphyee_render_screen_s* iphyee_render_screen_alloc(iphyee_worker_s *restrict worker, uint32_t width, uint32_t height);

iphyee_render_texture_s* iphyee_render_texture_alloc(iphyee_worker_s *restrict worker, const char *restrict name, const uint32_t *restrict pixels, uint32_t width, uint32_t height);
iphyee_render_texture_s* iphyee_render_texture_device_enable(iphyee_render_texture_s *restrict r);
void iphyee_render_texture_device_disable(iphyee_render_texture_s *restrict r);

iphyee_render_tpool_s* iphyee_render_tpool_alloc(iphyee_worker_s *restrict worker, uint32_t texture_max_count, iphyee_render_texture_s *restrict miss);
iphyee_render_tpool_s* iphyee_render_tpool_insert(iphyee_render_tpool_s *restrict r, iphyee_render_texture_s *restrict texture);
iphyee_render_tpool_s* iphyee_render_tpool_delete(iphyee_render_tpool_s *restrict r, const char *restrict texture_name);
void iphyee_render_tpool_update(iphyee_render_tpool_s *restrict r, iphyee_worker_command_buffer_s *restrict cb_transfer);

#endif
