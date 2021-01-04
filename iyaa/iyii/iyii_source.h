#ifndef _iyii_source_h_
#define _iyii_source_h_

#include "iyii_graph.h"

typedef struct iyii_source_s {
	graph_memory_heap_s *mheap;
	graph_command_pool_s *cpool_tran;
	graph_sampler_param_s *sampler_param;
	graph_sampler_s *sampler;
	graph_image_s *texture;
	graph_buffer_s *buffer_texture;
	graph_buffer_s *buffer_host;
	graph_buffer_s *buffer_dev;
	graph_buffer_s *buffer_uniform;
	graph_image_view_param_s *texture_view_param;
	graph_image_view_s *texture_view;
	uintptr_t texture$width;
	uintptr_t texture$height;
	uintptr_t texture$linesize;
	uintptr_t texture$size;
	uintptr_t vertex$offset;
	uintptr_t vertex$size;
	uintptr_t index$offset;
	uintptr_t index$size;
	uintptr_t transfer$size;
	uintptr_t uniform$size;
	uintptr_t uniform$number;
} iyii_source_s;

typedef enum iyii_source_transfer_e {
	iyii_source_transfer$transfer,
	iyii_source_transfer$vertex,
	iyii_source_transfer$index,
	iyii_source_transfer$texture,
	iyii_source_transfer_number
} iyii_source_transfer_t;

iyii_source_s* iyii_source_alloc(graph_dev_s *restrict dev, const graph_device_queue_t *transfer);
iyii_source_s* iyii_source_ready(iyii_source_s *restrict source, uintptr_t texture_width, uintptr_t texture_height, uintptr_t vertex_size, uintptr_t index_size, uintptr_t uniform_size, uintptr_t uniform_number);
iyii_source_s* iyii_source_build_transfer(iyii_source_s *restrict source);
iyii_source_s* iyii_source_copy_vertex(iyii_source_s *restrict source, const void *restrict data, uint64_t offset, uint64_t size);
iyii_source_s* iyii_source_copy_index(iyii_source_s *restrict source, const void *restrict data, uint64_t offset, uint64_t size);
iyii_source_s* iyii_source_submit(iyii_source_s *restrict source, graph_queue_t *restrict transfer, iyii_source_transfer_t type);

#endif
