#ifndef _graph_type_pri_h_
#define _graph_type_pri_h_

#include "type.h"
#include <vulkan/vulkan.h>

#define graph_array_number(_arr)  (sizeof(_arr) / sizeof(*_arr))

#define graph_type_red    "\e[31m"
#define graph_type_green  "\e[32m"
#define graph_type_blue   "\e[34m"
#define graph_type_yellow "\e[33m"
#define graph_type_aqua   "\e[36m"
#define graph_type_purple "\e[35m"
#define graph_type_back   "\e[0m"

char* graph_type_list(char *restrict s, uint32_t r, const char *array[], uint32_t n);

const char* graph_bool$string(graph_bool_t r);

const char* graph_physical_device_type$string(graph_physical_device_type_t r);

graph_format_t graph_format4vk(VkFormat r);
VkFormat graph_format2vk(graph_format_t r);

const char* graph_format$string(graph_format_t r);
const char* graph_color_space$string(graph_color_space_t r);
const char* graph_present_mode$string(graph_present_mode_t r);

// 128 bytes
char* graph_queue_flags$list(char *restrict s, graph_queue_flags_t r);

// 256 bytes
char* graph_image_usage$list(char *restrict s, graph_image_usage_t r);

graph_image_view_type_t graph_image_view_type4vk(VkImageViewType r);
VkImageViewType graph_image_view_type2vk(graph_image_view_type_t r);

// 128 bytes
char* graph_surface_transform$list(char *restrict s, graph_surface_transform_t r);

// 64 bytes
char* graph_composite_alpha$list(char *restrict s, graph_composite_alpha_t r);

VkVertexInputRate graph_vertex_input_rate2vk(graph_vertex_input_rate_t r);
VkPrimitiveTopology graph_primitive_topology2vk(graph_primitive_topology_t r);
VkPolygonMode graph_polygon_mode2vk(graph_polygon_mode_t r);
VkBlendFactor graph_blend_factor2vk(graph_blend_factor_t r);
VkBlendOp graph_blend_op2vk(graph_blend_op_t r);
VkLogicOp graph_logic_op2vk(graph_logic_op_t r);
VkAttachmentLoadOp graph_attachment_load_op2vk(graph_attachment_load_op_t r);
VkAttachmentStoreOp graph_attachment_store_op2vk(graph_attachment_store_op_t r);
VkImageLayout graph_image_layout2vk(graph_image_layout_t r);
VkPipelineBindPoint graph_pipeline_bind_point2vk(graph_pipeline_bind_point_t r);
VkDynamicState graph_dynamic2vk(graph_dynamic_t r);
VkSubpassContents graph_subpass_contents2vk(graph_subpass_contents_t r);

#endif
