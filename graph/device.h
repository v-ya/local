#ifndef _graph_device_h_
#define _graph_device_h_

#include <refer.h>
#include <mlog.h>
#include "type.h"

typedef struct graph_devices_s graph_devices_s;
typedef struct graph_device_t graph_device_t;
typedef struct graph_device_queues_s graph_device_queues_s;
typedef struct graph_device_queue_t graph_device_queue_t;
typedef struct graph_dev_param_s graph_dev_param_s;
typedef struct graph_dev_s graph_dev_s;
typedef struct graph_queue_t graph_queue_t;

struct graph_s;
struct graph_surface_s;

graph_devices_s* graph_instance_devices_get(const struct graph_s *restrict g);
uint32_t graph_devices_number(const graph_devices_s *restrict gds);
const graph_device_t* graph_devices_index(const graph_devices_s *restrict gds, uint32_t index);

graph_device_queues_s* graph_device_queues_get(const graph_device_t *restrict gd);
uint32_t graph_device_queues_number(const graph_device_queues_s *restrict gdqs);
const graph_device_queue_t* graph_device_queues_index(const graph_device_queues_s *restrict gdqs, uint32_t index);

graph_dev_param_s* graph_dev_param_alloc(uint32_t queue_family_number);
graph_dev_param_s* graph_dev_param_set_queue(graph_dev_param_s *restrict param, uint32_t index, const graph_device_queue_t *restrict queue, uint32_t number, const float *restrict priorities, graph_queue_create_flags_t flags);
graph_dev_param_s* graph_dev_param_set_layer(graph_dev_param_s *restrict param, const graph_layer_t *restrict layer);
graph_dev_param_s* graph_dev_param_set_extension(graph_dev_param_s *restrict param, const graph_extension_t *restrict extension);
void graph_dev_param_feature_enable(graph_dev_param_s *restrict param, graph_device_feature_t feature);
void graph_dev_param_feature_disable(graph_dev_param_s *restrict param, graph_device_feature_t feature);
void graph_dev_param_feature_enable_all(graph_dev_param_s *restrict param, const graph_device_t *restrict gd);

graph_dev_s* graph_dev_alloc(const graph_dev_param_s *restrict param, const struct graph_s *restrict g, const graph_device_t *restrict gd);
graph_queue_t* graph_dev_queue(graph_dev_s *restrict dev, const graph_device_queue_t *restrict queue, uint32_t qi);
graph_dev_s* graph_dev_wait_idle(graph_dev_s *restrict dev);

graph_physical_device_type_t graph_device_type(const graph_device_t *restrict gd);
const char* graph_device_name(const graph_device_t *restrict gd);

graph_queue_flags_t graph_device_queue_flags(const graph_device_queue_t *restrict q);
uint32_t graph_device_queue_count(const graph_device_queue_t *restrict q);
uint32_t graph_device_queue_timestamp_valid_bits(const graph_device_queue_t *restrict q);
void graph_device_queue_min_image_transfer_granularity(const graph_device_queue_t *restrict q, uint32_t *restrict width, uint32_t *restrict height, uint32_t *restrict depth);

graph_bool_t graph_device_features_test(const graph_device_t *restrict gd, graph_device_feature_t feature);
graph_bool_t graph_device_queue_surface_support(const graph_device_queue_t *restrict q, const struct graph_surface_s *restrict sf);

void graph_device_properties_header_dump(const graph_device_t *restrict gd);
void graph_device_properties_limits_dump(const graph_device_t *restrict gd);
void graph_device_properties_sparse_dump(const graph_device_t *restrict gd);
void graph_device_properties_dump(const graph_device_t *restrict gd);
void graph_device_features_dump(const graph_device_t *restrict gd);
void graph_device_dump(const graph_device_t *restrict gd);
void graph_device_queue_dump(const graph_device_queue_t *restrict gdq);

#endif
