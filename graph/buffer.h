#ifndef _graph_buffer_h_
#define _graph_buffer_h_

#include <refer.h>
#include "type.h"

typedef struct graph_memory_heap_s graph_memory_heap_s;
typedef struct graph_buffer_s graph_buffer_s;

struct graph_dev_s;
struct graph_device_queue_t;

graph_memory_heap_s* graph_memory_heap_alloc(struct graph_dev_s *restrict dev);
graph_buffer_s* graph_buffer_alloc(graph_memory_heap_s *restrict heap, uint64_t size, graph_buffer_flags_t flags, graph_buffer_usage_t usage, graph_memory_property_t property, uint32_t share_queue_number, const struct graph_device_queue_t *restrict share_queue_array);

void* graph_buffer_map(graph_buffer_s *restrict buffer, uint64_t offset, uint64_t size);
void graph_buffer_unmap(graph_buffer_s *restrict buffer);
graph_buffer_s* graph_buffer_flush(graph_buffer_s *restrict buffer, uint64_t offset, uint64_t size);
graph_buffer_s* graph_buffer_invalidate(graph_buffer_s *restrict buffer, uint64_t offset, uint64_t size);

#endif
