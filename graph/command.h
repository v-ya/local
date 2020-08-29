#ifndef _graph_command_h_
#define _graph_command_h_

#include <refer.h>
#include "type.h"

typedef struct graph_command_pool_s graph_command_pool_s;

struct graph_device_queue_t;
struct graph_dev_s;

graph_command_pool_s* graph_command_pool_alloc(struct graph_dev_s *restrict dev, const struct graph_device_queue_t *restrict queue, graph_command_pool_flags_t flags);

#endif
