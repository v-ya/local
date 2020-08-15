#ifndef _graph_vulkan_debug_h_
#define _graph_vulkan_debug_h_

#include <refer.h>
#include <mlog.h>
#include <vulkan/vulkan.h>
#include "type.h"
#include "allocator.h"

typedef struct graph_vulkan_debug_utils_messenger_s graph_vulkan_debug_utils_messenger_s;

graph_vulkan_debug_utils_messenger_s* graph_value_debug_utils_messenger_alloc(VkInstance instance, mlog_s *ml, graph_debug_message_level_t level, graph_debug_message_type_t type, graph_allocator_s *ga);

#endif
