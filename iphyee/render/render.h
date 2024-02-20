#ifndef _iphyee_render_pri_h_
#define _iphyee_render_pri_h_

#include "../iphyee.h"
#include "../iphyee.worker.h"
#include "../iphyee.glslc.h"
#include <queue/queue.h>
#include <vattr.h>

struct iphyee_loader_model_view_s;

typedef struct iphyee_render_screen_s iphyee_render_screen_s;
typedef struct iphyee_render_texture_s iphyee_render_texture_s;
typedef struct iphyee_render_tpool_s iphyee_render_tpool_s;
typedef struct iphyee_render_rasterize_s iphyee_render_rasterize_s;
typedef struct iphyee_render_model_inst_part_s iphyee_render_model_inst_part_s;
typedef struct iphyee_render_model_inst_s iphyee_render_model_inst_s;
typedef struct iphyee_render_model_part_t iphyee_render_model_part_t;
typedef struct iphyee_render_model_s iphyee_render_model_s;

struct iphyee_render_screen_s {
	iphyee_worker_buffer_s *screen_host;
	iphyee_worker_buffer_s *buffer_device;
	uint32_t *screen_data;
	iphyee_glslc_pointer_array_pixel_t screen_address;
	iphyee_glslc_pointer_array_depth_t depth_address;
	uint64_t screen_offset;
	uint64_t screen_size;
	uint64_t depth_offset;
	uint64_t depth_size;
	uint32_t width;
	uint32_t height;
};

struct iphyee_render_texture_s {
	iphyee_worker_s *worker;
	refer_string_t texture_name;
	iphyee_worker_buffer_s *texture_host;
	iphyee_worker_buffer_s *texture_device;
	uint32_t *texture_data;
	iphyee_glslc_pointer_array_pixel_t texture_address;
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
	iphyee_glslc_pointer_texture_pool_t tpool_address;
	uint64_t tpool_size;
	uintptr_t need_update;
	uint32_t texture_max_count;
	uint32_t texture_use_count;
};

struct iphyee_render_rasterize_s {
	iphyee_render_screen_s *screen;
	iphyee_render_tpool_s *tpool;
	iphyee_worker_buffer_s *rasterize_host;
	iphyee_worker_buffer_s *rasterize_device;
	iphyee_glslc_rasterize_t *rasterize_data;
	iphyee_glslc_rasterize_block_t *block_normal;
	iphyee_glslc_rasterize_block_t *block_alpha;
	iphyee_glslc_pointer_rasterize_t rasterize_address;
	uint64_t rasterize_reset_size;
};

struct iphyee_render_model_inst_part_s {
	refer_nstring_t texture_name;
	uint32_t fusion_offset;
	uint32_t fusion_number;
	uint32_t model_part_index;
	uint32_t texture_alpha;
};

struct iphyee_render_model_inst_s {
	iphyee_worker_s *worker;
	vattr_s *model_part_pool;
	iphyee_worker_buffer_s *model_inst_host;
	iphyee_worker_buffer_s *model_inst_device;
	iphyee_glslc_model_inst_t *model_inst_data;
	uint64_t model_inst_address;
	uint64_t model_inst_size;
	uintptr_t model_part_count;
	uintptr_t model_tri3_count;
};

struct iphyee_render_model_part_t {
	const iphyee_render_model_inst_part_s *part;
	const iphyee_render_texture_s *texture;
	iphyee_glslc_model_t *model_part_data;
	iphyee_glslc_pointer_model_t model_part_address;
};

struct iphyee_render_model_s {
	const iphyee_render_model_inst_s *inst;
	const iphyee_render_tpool_s *tpool;
	iphyee_worker_buffer_s *model_host;
	iphyee_worker_buffer_s *model_device;
	iphyee_glslc_model_t *model_part_array;
	uint64_t model_part_address;
	uint64_t model_update_size;
	uintptr_t model_part_count;
	iphyee_render_model_part_t part[];
};

iphyee_render_screen_s* iphyee_render_screen_alloc(iphyee_worker_s *restrict worker, uint32_t width, uint32_t height);

iphyee_render_texture_s* iphyee_render_texture_alloc(iphyee_worker_s *restrict worker, const char *restrict name, const uint32_t *restrict pixels, uint32_t width, uint32_t height);
iphyee_render_texture_s* iphyee_render_texture_device_enable(iphyee_render_texture_s *restrict r);
void iphyee_render_texture_device_disable(iphyee_render_texture_s *restrict r);

iphyee_render_tpool_s* iphyee_render_tpool_alloc(iphyee_worker_s *restrict worker, uint32_t texture_max_count, iphyee_render_texture_s *restrict miss);
const iphyee_render_texture_s* iphyee_render_tpool_find(const iphyee_render_tpool_s *restrict r, const char *restrict texture_name);
iphyee_render_tpool_s* iphyee_render_tpool_insert(iphyee_render_tpool_s *restrict r, iphyee_render_texture_s *restrict texture);
iphyee_render_tpool_s* iphyee_render_tpool_delete(iphyee_render_tpool_s *restrict r, const char *restrict texture_name);
void iphyee_render_tpool_update(iphyee_render_tpool_s *restrict r, iphyee_worker_command_buffer_s *restrict cb_transfer);

iphyee_render_rasterize_s* iphyee_render_rasterize_alloc(iphyee_render_screen_s *restrict screen, iphyee_render_tpool_s *restrict tpool, uint32_t block_size, uint32_t rasterize_max_tri3, uint32_t block_max_tri3);

iphyee_render_model_inst_s* iphyee_render_model_inst_alloc(iphyee_worker_s *restrict worker, const struct iphyee_loader_model_view_s *restrict model_view);

iphyee_render_model_s* iphyee_render_model_alloc(const iphyee_render_model_inst_s *restrict inst, const iphyee_render_tpool_s *restrict tpool);

#endif
