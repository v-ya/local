#ifndef _graph_support_pri_h_
#define _graph_support_pri_h_

#include "support.h"
#include <vulkan/vulkan.h>

// success return NULL
const graph_layer_t* graph_layer_mapping_length(const graph_layer_t *restrict p, uint32_t *restrict length);
const graph_extension_t* graph_extension_mapping_length(const graph_extension_t *restrict p, uint32_t *restrict length);
// must success
const char** graph_layer_mapping_copy(const graph_layer_t *restrict p, const char* d[]);
const char** graph_extension_mapping_copy(const graph_extension_t *restrict p, const char* d[]);

#endif
