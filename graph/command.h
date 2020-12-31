#ifndef _graph_command_h_
#define _graph_command_h_

#include <refer.h>
#include "type.h"

typedef struct graph_command_pool_s graph_command_pool_s;
typedef struct graph_command_pipe_barrier_param_s graph_command_pipe_barrier_param_s;
typedef struct graph_semaphore_s graph_semaphore_s;
typedef struct graph_fence_s graph_fence_s;

struct graph_device_queue_t;
struct graph_dev_s;
struct graph_queue_t;
struct graph_render_pass_s;
struct graph_pipe_s;
struct graph_viewports_scissors_s;
struct graph_swapchain_s;
struct graph_frame_buffer_s;
struct graph_buffer_s;
struct graph_image_s;
struct graph_pipe_layout_s;
struct graph_descriptor_sets_s;

graph_command_pool_s* graph_command_pool_alloc(struct graph_dev_s *restrict dev, const struct graph_device_queue_t *restrict queue, graph_command_pool_flags_t flags, uint32_t number);
// ia: (~0) all, ((index = ia) < number) index
graph_command_pool_s* graph_command_pool_begin(graph_command_pool_s *restrict r, uint32_t ia, graph_command_buffer_usage_t usage);
graph_command_pool_s* graph_command_pool_end(graph_command_pool_s *restrict r, uint32_t ia);
void graph_command_begin_render(graph_command_pool_s *restrict r, uint32_t ia, const struct graph_render_pass_s *restrict render, struct graph_frame_buffer_s *restrict frame, int32_t x, int32_t y, uint32_t width, uint32_t height);
void graph_command_bind_pipe(graph_command_pool_s *restrict r, uint32_t ia, graph_pipeline_bind_point_t type, const struct graph_pipe_s *restrict pipe);
void graph_command_set_viewport(graph_command_pool_s *restrict r, uint32_t ia, const struct graph_viewports_scissors_s *restrict vs);
void graph_command_set_scissor(graph_command_pool_s *restrict r, uint32_t ia, const struct graph_viewports_scissors_s *restrict vs);
graph_command_pool_s* graph_command_bind_vertex_buffers(graph_command_pool_s *restrict r, uint32_t ia, uint32_t first_binding, uint32_t n, const struct graph_buffer_s *const restrict *restrict buffers, const uint64_t *restrict offsets);
graph_command_pool_s* graph_command_bind_index_buffer(graph_command_pool_s *restrict r, uint32_t ia, const struct graph_buffer_s *restrict buffer, uint64_t offset, graph_index_type_t type);
graph_command_pool_s* graph_command_bind_desc_sets(graph_command_pool_s *restrict r, uint32_t ia, graph_pipeline_bind_point_t point, const struct graph_pipe_layout_s *restrict layout, const struct graph_descriptor_sets_s *restrict sets, uint32_t set_start, uint32_t set_number, uint32_t dy_offset_number, const uint32_t *restrict dy_offset);
void graph_command_draw(graph_command_pool_s *restrict r, uint32_t ia, uint32_t v_number, uint32_t i_number, uint32_t v_start, uint32_t i_start);
void graph_command_draw_index(graph_command_pool_s *restrict r, uint32_t ia, uint32_t idx_number, uint32_t i_number, uint32_t idx_start, uint32_t v_start, uint32_t i_start);
void graph_command_end_render(graph_command_pool_s *restrict r, uint32_t ia);
void graph_command_copy_buffer(graph_command_pool_s *restrict r, uint32_t ia, struct graph_buffer_s *restrict dst, const struct graph_buffer_s *restrict src, uint64_t dst_offset, uint64_t src_offset, uint64_t size);
graph_command_pool_s* graph_command_copy_buffer_to_image(graph_command_pool_s *restrict r, uint32_t ia, struct graph_image_s *restrict dst, const struct graph_buffer_s *restrict src, graph_image_layout_t layout, uint64_t buffer_offset, graph_image_aspect_flags_t flags, const int32_t *restrict image_offset, const uint32_t *restrict image_extent);

graph_command_pipe_barrier_param_s* graph_command_pipe_barrier_param_alloc(uint32_t memory_number, uint32_t buffer_number, uint32_t image_number);
void graph_command_pipe_barrier_param_set_image(graph_command_pipe_barrier_param_s *restrict param, uint32_t index, const struct graph_image_s *restrict image, graph_image_aspect_flags_t flags, uint32_t mip_level_base, uint32_t mip_level_number, uint32_t layer_base, uint32_t layer_number);
void graph_command_pipe_barrier_param_set_image_access(graph_command_pipe_barrier_param_s *restrict param, uint32_t index, graph_access_flags_t src, graph_access_flags_t dst);
void graph_command_pipe_barrier_param_set_image_layout(graph_command_pipe_barrier_param_s *restrict param, uint32_t index, graph_image_layout_t old, graph_image_layout_t new);
void graph_command_pipe_barrier_param_set_image_queue(graph_command_pipe_barrier_param_s *restrict param, uint32_t index, const struct graph_device_queue_t *restrict src, const struct graph_device_queue_t *restrict dst);
void graph_command_pipe_barrier(graph_command_pool_s *restrict r, uint32_t ia, graph_pipeline_stage_flags_t src, graph_pipeline_stage_flags_t dst, graph_dependency_flags_t flags, const graph_command_pipe_barrier_param_s *restrict param);

struct graph_queue_t* graph_queue_submit(struct graph_queue_t *restrict queue, graph_command_pool_s *restrict pool, uint32_t index, graph_semaphore_s *restrict wait, graph_semaphore_s *restrict signal, graph_fence_s *restrict fence, graph_pipeline_stage_flags_t wait_mask);
struct graph_queue_t* graph_queue_present(struct graph_queue_t *restrict queue, struct graph_swapchain_s *restrict swapchain, uint32_t index, graph_semaphore_s *restrict wait);
struct graph_queue_t* graph_queue_wait_idle(struct graph_queue_t *restrict queue);

graph_semaphore_s* graph_semaphore_alloc(struct graph_dev_s *restrict dev);
graph_semaphore_s* graph_semaphore_signal(graph_semaphore_s *restrict semaphore, uint64_t value);
graph_semaphore_s* graph_semaphore_wait(graph_semaphore_s *restrict semaphore, uint64_t timeout, uint64_t value);
graph_fence_s* graph_fence_alloc(struct graph_dev_s *restrict dev, graph_fence_flags_t flags);
graph_fence_s* graph_fence_reset(graph_fence_s *restrict fence);
graph_fence_s* graph_fence_wait(graph_fence_s *restrict fence, uint64_t timeout);

#endif
