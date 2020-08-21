#ifndef _graph_type_pri_h_
#define _graph_type_pri_h_

#include "type.h"
#include <vulkan/vulkan.h>

#define graph_array_number(_arr)  (sizeof(_arr) / sizeof(_arr[0]))

#define graph_type_red    "\e[31m"
#define graph_type_green  "\e[32m"
#define graph_type_blue   "\e[34m"
#define graph_type_yellow "\e[33m"
#define graph_type_aqua   "\e[36m"
#define graph_type_purple "\e[35m"
#define graph_type_back   "\e[0m"

graph_format_t graph_format4vk(VkFormat r);
VkFormat graph_format2vk(graph_format_t r);

const char* graph_bool$string(graph_bool_t r);
const char* graph_physical_device_type$string(graph_physical_device_type_t r);
const char* graph_format$string(graph_format_t r);
const char* graph_color_space$string(graph_color_space_t r);
const char* graph_present_mode$string(graph_present_mode_t r);
char* graph_type_list(char *restrict s, uint32_t r, const char *array[], uint32_t n);
// 128 bytes
char* graph_queue_flags$list(register char *restrict s, register graph_queue_flags_t r);
// 256 bytes
char* graph_image_usage$list(register char *restrict s, register graph_image_usage_t r);
// 128 bytes
char* graph_surface_transform$list(register char *restrict s, register graph_surface_transform_t r);
// 64 bytes
char* graph_composite_alpha$list(register char *restrict s, register graph_composite_alpha_t r);

#endif
