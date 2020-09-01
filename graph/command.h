#ifndef _graph_command_h_
#define _graph_command_h_

#include <refer.h>
#include "type.h"

typedef struct graph_command_pool_s graph_command_pool_s;
typedef struct graph_semaphore_s graph_semaphore_s;
typedef struct graph_fence_s graph_fence_s;

struct graph_device_queue_t;
struct graph_dev_s;
struct graph_queue_t;
struct graph_render_pass_s;
struct graph_pipe_s;
struct graph_swapchain_s;
struct graph_frame_buffer_s;
struct graph_buffer_s;

graph_command_pool_s* graph_command_pool_alloc(struct graph_dev_s *restrict dev, const struct graph_device_queue_t *restrict queue, graph_command_pool_flags_t flags, uint32_t number);
// ia: (~0) all, ((index = ia) < number) index
graph_command_pool_s* graph_command_pool_begin(graph_command_pool_s *restrict r, uint32_t ia, graph_command_buffer_usage_t usage);
graph_command_pool_s* graph_command_pool_end(graph_command_pool_s *restrict r, uint32_t ia);
void graph_command_begin_render(graph_command_pool_s *restrict r, uint32_t ia, const struct graph_render_pass_s *restrict render, struct graph_frame_buffer_s *restrict frame, int32_t x, int32_t y, uint32_t width, uint32_t height);
void graph_command_bind_pipe(graph_command_pool_s *restrict r, uint32_t ia, graph_pipeline_bind_point_t type, const struct graph_pipe_s *restrict pipe);
graph_command_pool_s* graph_command_bind_vertex_buffers(graph_command_pool_s *restrict r, uint32_t ia, uint32_t first_binding, uint32_t n, const struct graph_buffer_s *const restrict *restrict buffers, const uint64_t *restrict offsets);
void graph_command_draw(graph_command_pool_s *restrict r, uint32_t ia, uint32_t v_number, uint32_t i_number, uint32_t v_start, uint32_t i_start);
void graph_command_end_render(graph_command_pool_s *restrict r, uint32_t ia);

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
