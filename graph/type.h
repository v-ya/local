#ifndef _graph_type_h_
#define _graph_type_h_

#include <stdint.h>

typedef uint32_t graph_bool_t;

typedef enum graph_layer_e {
	graph_layer_null,
	graph_layer_validation,
	graph_layer_api_dump,
	graph_layer$number,
	graph_layer$placeholder = 0x7fffffff
} graph_layer_t;

typedef enum graph_extension_e {
	graph_extension_null,
	graph_extension_ext_debug_utils,
	graph_extension_khr_swapchain,
	graph_extension_khr_surface,
	graph_extension_khr_xcb_surface,
	graph_extension$number,
	graph_extension$placeholder = 0x7fffffff
} graph_extension_t;

typedef enum graph_debug_message_level_e {
	graph_debug_message_level_version = 0x0001,
	graph_debug_message_level_info    = 0x0010,
	graph_debug_message_level_warning = 0x0100,
	graph_debug_message_level_error   = 0x1000
} graph_debug_message_level_t;

typedef enum graph_debug_message_type_e {
	graph_debug_message_type_general     = 0x0001,
	graph_debug_message_type_validation  = 0x0002,
	graph_debug_message_type_performance = 0x0004
} graph_debug_message_type_t;

typedef enum graph_physical_device_type_e {
	graph_physical_device_type_other          = 0,
	graph_physical_device_type_integrated_gpu = 1,
	graph_physical_device_type_discrete_gpu   = 2,
	graph_physical_device_type_virtual_gpu    = 3,
	graph_physical_device_type_cpu            = 4
} graph_physical_device_type_t;

typedef enum graph_queue_flags_e {
	graph_queue_flags_graphics       = 0x0001,
	graph_queue_flags_compute        = 0x0002,
	graph_queue_flags_transfer       = 0x0004,
	graph_queue_flags_sparse_binding = 0x0008,
	graph_queue_flags_protected      = 0x0010
} graph_queue_flags_t;

typedef enum graph_queue_create_flags_e {
	graph_queue_create_flags_protected = 0x0001
} graph_queue_create_flags_t;

typedef enum graph_device_feature_e {
	graph_device_feature_robustBufferAccess,
	graph_device_feature_fullDrawIndexUint32,
	graph_device_feature_imageCubeArray,
	graph_device_feature_independentBlend,
	graph_device_feature_geometryShader,
	graph_device_feature_tessellationShader,
	graph_device_feature_sampleRateShading,
	graph_device_feature_dualSrcBlend,
	graph_device_feature_logicOp,
	graph_device_feature_multiDrawIndirect,
	graph_device_feature_drawIndirectFirstInstance,
	graph_device_feature_depthClamp,
	graph_device_feature_depthBiasClamp,
	graph_device_feature_fillModeNonSolid,
	graph_device_feature_depthBounds,
	graph_device_feature_wideLines,
	graph_device_feature_largePoints,
	graph_device_feature_alphaToOne,
	graph_device_feature_multiViewport,
	graph_device_feature_samplerAnisotropy,
	graph_device_feature_textureCompressionETC2,
	graph_device_feature_textureCompressionASTC_LDR,
	graph_device_feature_textureCompressionBC,
	graph_device_feature_occlusionQueryPrecise,
	graph_device_feature_pipelineStatisticsQuery,
	graph_device_feature_vertexPipelineStoresAndAtomics,
	graph_device_feature_fragmentStoresAndAtomics,
	graph_device_feature_shaderTessellationAndGeometryPointSize,
	graph_device_feature_shaderImageGatherExtended,
	graph_device_feature_shaderStorageImageExtendedFormats,
	graph_device_feature_shaderStorageImageMultisample,
	graph_device_feature_shaderStorageImageReadWithoutFormat,
	graph_device_feature_shaderStorageImageWriteWithoutFormat,
	graph_device_feature_shaderUniformBufferArrayDynamicIndexing,
	graph_device_feature_shaderSampledImageArrayDynamicIndexing,
	graph_device_feature_shaderStorageBufferArrayDynamicIndexing,
	graph_device_feature_shaderStorageImageArrayDynamicIndexing,
	graph_device_feature_shaderClipDistance,
	graph_device_feature_shaderCullDistance,
	graph_device_feature_shaderFloat64,
	graph_device_feature_shaderInt64,
	graph_device_feature_shaderInt16,
	graph_device_feature_shaderResourceResidency,
	graph_device_feature_shaderResourceMinLod,
	graph_device_feature_sparseBinding,
	graph_device_feature_sparseResidencyBuffer,
	graph_device_feature_sparseResidencyImage2D,
	graph_device_feature_sparseResidencyImage3D,
	graph_device_feature_sparseResidency2Samples,
	graph_device_feature_sparseResidency4Samples,
	graph_device_feature_sparseResidency8Samples,
	graph_device_feature_sparseResidency16Samples,
	graph_device_feature_sparseResidencyAliased,
	graph_device_feature_variableMultisampleRate,
	graph_device_feature_inheritedQueries,
	graph_device_feature$number
} graph_device_feature_t;

typedef enum graph_format_e {
	graph_format_unknow,
	graph_format_r4g4_unorm_pack8,
	graph_format_r4g4b4a4_unorm_pack16,
	graph_format_b4g4r4a4_unorm_pack16,
	graph_format_r5g6b5_unorm_pack16,
	graph_format_b5g6r5_unorm_pack16,
	graph_format_r5g5b5a1_unorm_pack16,
	graph_format_b5g5r5a1_unorm_pack16,
	graph_format_a1r5g5b5_unorm_pack16,
	graph_format_r8_unorm,
	graph_format_r8_snorm,
	graph_format_r8_uscaled,
	graph_format_r8_sscaled,
	graph_format_r8_uint,
	graph_format_r8_sint,
	graph_format_r8_srgb,
	graph_format_r8g8_unorm,
	graph_format_r8g8_snorm,
	graph_format_r8g8_uscaled,
	graph_format_r8g8_sscaled,
	graph_format_r8g8_uint,
	graph_format_r8g8_sint,
	graph_format_r8g8_srgb,
	graph_format_r8g8b8_unorm,
	graph_format_r8g8b8_snorm,
	graph_format_r8g8b8_uscaled,
	graph_format_r8g8b8_sscaled,
	graph_format_r8g8b8_uint,
	graph_format_r8g8b8_sint,
	graph_format_r8g8b8_srgb,
	graph_format_b8g8r8_unorm,
	graph_format_b8g8r8_snorm,
	graph_format_b8g8r8_uscaled,
	graph_format_b8g8r8_sscaled,
	graph_format_b8g8r8_uint,
	graph_format_b8g8r8_sint,
	graph_format_b8g8r8_srgb,
	graph_format_r8g8b8a8_unorm,
	graph_format_r8g8b8a8_snorm,
	graph_format_r8g8b8a8_uscaled,
	graph_format_r8g8b8a8_sscaled,
	graph_format_r8g8b8a8_uint,
	graph_format_r8g8b8a8_sint,
	graph_format_r8g8b8a8_srgb,
	graph_format_b8g8r8a8_unorm,
	graph_format_b8g8r8a8_snorm,
	graph_format_b8g8r8a8_uscaled,
	graph_format_b8g8r8a8_sscaled,
	graph_format_b8g8r8a8_uint,
	graph_format_b8g8r8a8_sint,
	graph_format_b8g8r8a8_srgb,
	graph_format_a8b8g8r8_unorm_pack32,
	graph_format_a8b8g8r8_snorm_pack32,
	graph_format_a8b8g8r8_uscaled_pack32,
	graph_format_a8b8g8r8_sscaled_pack32,
	graph_format_a8b8g8r8_uint_pack32,
	graph_format_a8b8g8r8_sint_pack32,
	graph_format_a8b8g8r8_srgb_pack32,
	graph_format_a2r10g10b10_unorm_pack32,
	graph_format_a2r10g10b10_snorm_pack32,
	graph_format_a2r10g10b10_uscaled_pack32,
	graph_format_a2r10g10b10_sscaled_pack32,
	graph_format_a2r10g10b10_uint_pack32,
	graph_format_a2r10g10b10_sint_pack32,
	graph_format_a2b10g10r10_unorm_pack32,
	graph_format_a2b10g10r10_snorm_pack32,
	graph_format_a2b10g10r10_uscaled_pack32,
	graph_format_a2b10g10r10_sscaled_pack32,
	graph_format_a2b10g10r10_uint_pack32,
	graph_format_a2b10g10r10_sint_pack32,
	graph_format_r16_unorm,
	graph_format_r16_snorm,
	graph_format_r16_uscaled,
	graph_format_r16_sscaled,
	graph_format_r16_uint,
	graph_format_r16_sint,
	graph_format_r16_sfloat,
	graph_format_r16g16_unorm,
	graph_format_r16g16_snorm,
	graph_format_r16g16_uscaled,
	graph_format_r16g16_sscaled,
	graph_format_r16g16_uint,
	graph_format_r16g16_sint,
	graph_format_r16g16_sfloat,
	graph_format_r16g16b16_unorm,
	graph_format_r16g16b16_snorm,
	graph_format_r16g16b16_uscaled,
	graph_format_r16g16b16_sscaled,
	graph_format_r16g16b16_uint,
	graph_format_r16g16b16_sint,
	graph_format_r16g16b16_sfloat,
	graph_format_r16g16b16a16_unorm,
	graph_format_r16g16b16a16_snorm,
	graph_format_r16g16b16a16_uscaled,
	graph_format_r16g16b16a16_sscaled,
	graph_format_r16g16b16a16_uint,
	graph_format_r16g16b16a16_sint,
	graph_format_r16g16b16a16_sfloat,
	graph_format_r32_uint,
	graph_format_r32_sint,
	graph_format_r32_sfloat,
	graph_format_r32g32_uint,
	graph_format_r32g32_sint,
	graph_format_r32g32_sfloat,
	graph_format_r32g32b32_uint,
	graph_format_r32g32b32_sint,
	graph_format_r32g32b32_sfloat,
	graph_format_r32g32b32a32_uint,
	graph_format_r32g32b32a32_sint,
	graph_format_r32g32b32a32_sfloat,
	graph_format_r64_uint,
	graph_format_r64_sint,
	graph_format_r64_sfloat,
	graph_format_r64g64_uint,
	graph_format_r64g64_sint,
	graph_format_r64g64_sfloat,
	graph_format_r64g64b64_uint,
	graph_format_r64g64b64_sint,
	graph_format_r64g64b64_sfloat,
	graph_format_r64g64b64a64_uint,
	graph_format_r64g64b64a64_sint,
	graph_format_r64g64b64a64_sfloat,
	graph_format_b10g11r11_ufloat_pack32,
	graph_format_e5b9g9r9_ufloat_pack32,
	graph_format_d16_unorm,
	graph_format_x8_d24_unorm_pack32,
	graph_format_d32_sfloat,
	graph_format_s8_uint,
	graph_format_d16_unorm_s8_uint,
	graph_format_d24_unorm_s8_uint,
	graph_format_d32_sfloat_s8_uint,
	graph_format$number
} graph_format_t;

typedef enum graph_color_space_e {
	graph_color_space_srgb = 0
} graph_color_space_t;

typedef enum graph_present_mode_e {
	graph_present_mode_immediate    = 0,
	graph_present_mode_mailbox      = 1,
	graph_present_mode_fifo         = 2,
	graph_present_mode_fifo_relaxed = 3
} graph_present_mode_t;

typedef enum graph_image_usage_e {
	graph_image_usage_transfer_src             = 0x0001,
	graph_image_usage_transfer_dst             = 0x0002,
	graph_image_usage_sampled                  = 0x0004,
	graph_image_usage_storage                  = 0x0008,
	graph_image_usage_color_attachment         = 0x0010,
	graph_image_usage_depth_stencil_attachment = 0x0020,
	graph_image_usage_transient_attachment     = 0x0040,
	graph_image_usage_input_attachment         = 0x0080,
	graph_image_usage_shading_rate_image_nv    = 0x0100,
	graph_image_usage_fragment_density_map_ext = 0x0200
} graph_image_usage_t;


typedef enum graph_image_view_type_e {
	graph_image_view_type_1D,
	graph_image_view_type_2D,
	graph_image_view_type_3D,
	graph_image_view_type_cube,
	graph_image_view_type_1D_array,
	graph_image_view_type_2D_array,
	graph_image_view_type_cube_array,
	graph_image_view_type$number
} graph_image_view_type_t;

typedef enum graph_sharing_mode_e {
	graph_sharing_mode_exclusive  = 0,
	graph_sharing_mode_concurrent = 1
} graph_sharing_mode_t;

typedef enum graph_surface_transform_e {
	graph_surface_transform_identity                     = 0x0001,
	graph_surface_transform_rotate_90                    = 0x0002,
	graph_surface_transform_rotate_180                   = 0x0004,
	graph_surface_transform_rotate_270                   = 0x0008,
	graph_surface_transform_horizontal_mirror            = 0x0010,
	graph_surface_transform_horizontal_mirror_rotate_90  = 0x0020,
	graph_surface_transform_horizontal_mirror_rotate_180 = 0x0040,
	graph_surface_transform_horizontal_mirror_rotate_270 = 0x0080
} graph_surface_transform_t;

typedef enum graph_composite_alpha_e {
	graph_composite_alpha_opaque          = 0x01,
	graph_composite_alpha_pre_multiplied  = 0x02,
	graph_composite_alpha_post_multiplied = 0x04,
	graph_composite_alpha_inherit         = 0x08
} graph_composite_alpha_t;

typedef enum graph_shader_type_e {
	graph_shader_type_vertex                  = 0x0001,
	graph_shader_type_tessellation_control    = 0x0002,
	graph_shader_type_tessellation_evaluation = 0x0004,
	graph_shader_type_geometry                = 0x0008,
	graph_shader_type_fragment                = 0x0010,
	graph_shader_type_compute                 = 0x0020
} graph_shader_type_t;

typedef enum graph_vertex_input_rate_e {
	graph_vertex_input_rate_vertex,
	graph_vertex_input_rate_instance,
	graph_vertex_input_rate$number
} graph_vertex_input_rate_t;

typedef enum graph_primitive_topology_e {
	graph_primitive_topology_point_list,
	graph_primitive_topology_line_list,
	graph_primitive_topology_line_strip,
	graph_primitive_topology_triangle_list,
	graph_primitive_topology_triangle_strip,
	graph_primitive_topology_triangle_fan,
	graph_primitive_topology_line_list_with_adjacency,
	graph_primitive_topology_line_strip_with_adjacency,
	graph_primitive_topology_triangle_list_with_adjacency,
	graph_primitive_topology_triangle_strip_with_adjacency,
	graph_primitive_topology_patch_list,
	graph_primitive_topology$number
} graph_primitive_topology_t;

typedef enum graph_polygon_mode_e {
	graph_polygon_mode_fill,
	graph_polygon_mode_line,
	graph_polygon_mode_point,
	graph_polygon_mode$number
} graph_polygon_mode_t;

typedef enum graph_cull_mode_flags_e {
	graph_cull_mode_flags_none           = 0x00,
	graph_cull_mode_flags_front          = 0x01,
	graph_cull_mode_flags_back           = 0x02,
	graph_cull_mode_flags_front_and_back = 0x03
} graph_cull_mode_flags_t;

typedef enum graph_front_face_e {
	graph_front_face_counter_clockwise = 0,
	graph_front_face_clockwise         = 1
} graph_front_face_t;

typedef enum graph_sample_count_e {
	graph_sample_count_1  = 0x01,
	graph_sample_count_2  = 0x02,
	graph_sample_count_4  = 0x04,
	graph_sample_count_8  = 0x08,
	graph_sample_count_16 = 0x10,
	graph_sample_count_32 = 0x20,
	graph_sample_count_64 = 0x40
} graph_sample_count_t;

typedef enum graph_color_component_mask_e {
	graph_color_component_mask_r   = 0x01,
	graph_color_component_mask_g   = 0x02,
	graph_color_component_mask_b   = 0x04,
	graph_color_component_mask_a   = 0x08,
	graph_color_component_mask_all = 0x0f,
} graph_color_component_mask_t;

typedef enum graph_blend_factor_e {
	graph_blend_factor_zero,
	graph_blend_factor_one,
	graph_blend_factor_src_color,
	graph_blend_factor_one_minus_src_color,
	graph_blend_factor_dst_color,
	graph_blend_factor_one_minus_dst_color,
	graph_blend_factor_src_alpha,
	graph_blend_factor_one_minus_src_alpha,
	graph_blend_factor_dst_alpha,
	graph_blend_factor_one_minus_dst_alpha,
	graph_blend_factor_constant_color,
	graph_blend_factor_one_minus_constant_color,
	graph_blend_factor_constant_alpha,
	graph_blend_factor_one_minus_constant_alpha,
	graph_blend_factor_src_alpha_saturate,
	graph_blend_factor_src1_color,
	graph_blend_factor_one_minus_src1_color,
	graph_blend_factor_src1_alpha,
	graph_blend_factor_one_minus_src1_alpha,
	graph_blend_factor$number
} graph_blend_factor_t;

typedef enum graph_blend_op_e {
	graph_blend_op_add,
	graph_blend_op_subtract,
	graph_blend_op_reverse_subtract,
	graph_blend_op_min,
	graph_blend_op_max,
	graph_blend_op$number
} graph_blend_op_t;

typedef enum graph_logic_op_e {
	graph_logic_op_clear,
	graph_logic_op_and,
	graph_logic_op_and_reverse,
	graph_logic_op_copy,
	graph_logic_op_and_inverted,
	graph_logic_op_no_op,
	graph_logic_op_xor,
	graph_logic_op_or,
	graph_logic_op_nor,
	graph_logic_op_equivalent,
	graph_logic_op_invert,
	graph_logic_op_or_reverse,
	graph_logic_op_copy_inverted,
	graph_logic_op_or_inverted,
	graph_logic_op_nand,
	graph_logic_op_set,
	graph_logic_op$number
} graph_logic_op_t;

typedef enum graph_attachment_load_op_e {
	graph_attachment_load_op_load,
	graph_attachment_load_op_clear,
	graph_attachment_load_op_none,
	graph_attachment_load_op$number
} graph_attachment_load_op_t;

typedef enum graph_attachment_store_op_e {
	graph_attachment_store_op_store,
	graph_attachment_store_op_none,
	graph_attachment_store_op$number
} graph_attachment_store_op_t;

typedef enum graph_image_layout_e {
	graph_image_layout_undefined,
	graph_image_layout_general,
	graph_image_layout_color_attachment_optimal,
	graph_image_layout_depth_stencil_attachment_optimal,
	graph_image_layout_depth_stencil_read_only_optimal,
	graph_image_layout_shader_read_only_optimal,
	graph_image_layout_transfer_src_optimal,
	graph_image_layout_transfer_dst_optimal,
	graph_image_layout_preinitialized,
	graph_image_layout_khr_present_src,
	graph_image_layout$number
} graph_image_layout_t;

typedef enum graph_desc_set_layout_flags_e {
	graph_desc_set_layout_flags_push_descriptor        = 0x01,
	graph_desc_set_layout_flags_update_after_bind_pool = 0x02
} graph_desc_set_layout_flags_t;

typedef enum graph_desc_type_e {
	graph_desc_type_sampler,
	graph_desc_type_combined_image_sampler,
	graph_desc_type_sampled_image,
	graph_desc_type_storage_image,
	graph_desc_type_uniform_texel_buffer,
	graph_desc_type_storage_texel_buffer,
	graph_desc_type_uniform_buffer,
	graph_desc_type_storage_buffer,
	graph_desc_type_uniform_buffer_dynamic,
	graph_desc_type_storage_buffer_dynamic,
	graph_desc_type_input_attachment,
	graph_desc_type$number
} graph_desc_type_t;

typedef enum graph_desc_pool_flags_e {
	graph_desc_pool_flags_free_descriptor_set = 0x01,
	graph_desc_pool_flags_update_after_bind   = 0x02
} graph_desc_pool_flags_t;

typedef enum graph_shader_stage_flags_e {
	graph_shader_stage_flags_vertex                  = 0x00000001,
	graph_shader_stage_flags_tessellation_control    = 0x00000002,
	graph_shader_stage_flags_tessellation_evaluation = 0x00000004,
	graph_shader_stage_flags_geometry                = 0x00000008,
	graph_shader_stage_flags_fragment                = 0x00000010,
	graph_shader_stage_flags_compute                 = 0x00000020,
	graph_shader_stage_flags_all_graphics            = 0x0000001f
} graph_shader_stage_flags_t;

typedef enum graph_pipeline_bind_point_e {
	graph_pipeline_bind_point_graphics,
	graph_pipeline_bind_point_compute,
	graph_pipeline_bind_point$number
} graph_pipeline_bind_point_t;

typedef enum graph_dynamic_e {
	graph_dynamic_viewport,
	graph_dynamic_scissor,
	graph_dynamic_line_width,
	graph_dynamic_depth_bias,
	graph_dynamic_blend_constants,
	graph_dynamic_depth_bounds,
	graph_dynamic_stencil_compare_mask,
	graph_dynamic_stencil_write_mask,
	graph_dynamic_stencil_reference,
	graph_dynamic$number
} graph_dynamic_t;

typedef enum graph_pipe_flags_e {
	graph_pipe_flags_disable_optimization         = 0x00000001,
	graph_pipe_flags_allow_derivatives            = 0x00000002,
	graph_pipe_flags_derivative                   = 0x00000004,
	graph_pipe_flags_view_index_from_device_index = 0x00000008,
	graph_pipe_flags_dispatch_base                = 0x00000010
} graph_pipe_flags_t;

typedef enum graph_command_pool_flags_e {
	graph_command_pool_flags_transient            = 0x01,
	graph_command_pool_flags_reset_command_buffer = 0x02,
	graph_command_pool_flags_protected            = 0x04
} graph_command_pool_flags_t;

typedef enum graph_command_buffer_usage_e {
	graph_command_buffer_usage_one_time_submit      = 0x01,
	graph_command_buffer_usage_render_pass_continue = 0x02,
	graph_command_buffer_usage_simultaneous_use     = 0x04
} graph_command_buffer_usage_t;

typedef enum graph_subpass_contents_e {
	graph_subpass_contents_inline,
	graph_subpass_contents_secondary,
	graph_subpass_contents$number
} graph_subpass_contents_t;

typedef enum graph_pipeline_stage_flags_e {
	graph_pipeline_stage_flags_top_of_pipe                    = 0x00000001,
	graph_pipeline_stage_flags_draw_indirect                  = 0x00000002,
	graph_pipeline_stage_flags_vertex_input                   = 0x00000004,
	graph_pipeline_stage_flags_vertex_shader                  = 0x00000008,
	graph_pipeline_stage_flags_tessellation_control_shader    = 0x00000010,
	graph_pipeline_stage_flags_tessellation_evaluation_shader = 0x00000020,
	graph_pipeline_stage_flags_geometry_shader                = 0x00000040,
	graph_pipeline_stage_flags_fragment_shader                = 0x00000080,
	graph_pipeline_stage_flags_early_fragment_tests           = 0x00000100,
	graph_pipeline_stage_flags_late_fragment_tests            = 0x00000200,
	graph_pipeline_stage_flags_color_attachment_output        = 0x00000400,
	graph_pipeline_stage_flags_compute_shader                 = 0x00000800,
	graph_pipeline_stage_flags_transfer                       = 0x00001000,
	graph_pipeline_stage_flags_bottom_of_pipe                 = 0x00002000,
	graph_pipeline_stage_flags_host                           = 0x00004000,
	graph_pipeline_stage_flags_all_graphics                   = 0x00008000,
	graph_pipeline_stage_flags_all_commands                   = 0x00010000
} graph_pipeline_stage_flags_t;

typedef enum graph_fence_flags_e {
	graph_fence_flags_signaled = 0x01
} graph_fence_flags_t;

typedef enum graph_buffer_flags_e {
	graph_buffer_flags_sparse_binding                = 0x01,
	graph_buffer_flags_sparse_residency              = 0x02,
	graph_buffer_flags_sparse_aliased                = 0x04,
	graph_buffer_flags_protected                     = 0x08,
	graph_buffer_flags_device_address_capture_replay = 0x10
} graph_buffer_flags_t;

typedef enum graph_buffer_usage_e {
	graph_buffer_usage_transfer_src                          = 0x00000001,
	graph_buffer_usage_transfer_dst                          = 0x00000002,
	graph_buffer_usage_uniform_texel_buffer                  = 0x00000004,
	graph_buffer_usage_storage_texel_buffer                  = 0x00000008,
	graph_buffer_usage_uniform_buffer                        = 0x00000010,
	graph_buffer_usage_storage_buffer                        = 0x00000020,
	graph_buffer_usage_index_buffer                          = 0x00000040,
	graph_buffer_usage_vertex_buffer                         = 0x00000080,
	graph_buffer_usage_indirect_buffer                       = 0x00000100,
	graph_buffer_usage_conditional_rendering_ext             = 0x00000200,
	graph_buffer_usage_ray_tracing_khr                       = 0x00000400,
	graph_buffer_usage_transform_feedback_buffer_ext         = 0x00000800,
	graph_buffer_usage_transform_feedback_counter_buffer_ext = 0x00001000,
	graph_buffer_usage_shader_device_address                 = 0x00020000
} graph_buffer_usage_t;

typedef enum graph_memory_property_e {
	graph_memory_property_device_local        = 0x01,
	graph_memory_property_host_visible        = 0x02,
	graph_memory_property_host_coherent       = 0x04,
	graph_memory_property_host_cached         = 0x08,
	graph_memory_property_lazily_allocated    = 0x10,
	graph_memory_property_protected           = 0x20,
	graph_memory_property_device_coherent_amd = 0x40,
	graph_memory_property_device_uncached_amd = 0x80
} graph_memory_property_t;

typedef enum graph_index_type_e {
	graph_index_type_uint16,
	graph_index_type_uint32,
	graph_index_type$number
} graph_index_type_t;

typedef enum graph_image_aspect_flags_e {
	graph_image_aspect_flags_color          = 0x0001,
	graph_image_aspect_flags_depth          = 0x0002,
	graph_image_aspect_flags_stencil        = 0x0004,
	graph_image_aspect_flags_metadata       = 0x0008,
	graph_image_aspect_flags_plane_0        = 0x0010,
	graph_image_aspect_flags_plane_1        = 0x0020,
	graph_image_aspect_flags_plane_2        = 0x0040,
	graph_image_aspect_flags_memory_plane_0 = 0x0080,
	graph_image_aspect_flags_memory_plane_1 = 0x0100,
	graph_image_aspect_flags_memory_plane_2 = 0x0200,
	graph_image_aspect_flags_memory_plane_3 = 0x0400
} graph_image_aspect_flags_t;

typedef enum graph_dependency_flags_e {
	graph_dependency_flags_by_region    = 0x01,
	graph_dependency_flags_view_local   = 0x02,
	graph_dependency_flags_device_group = 0x04
} graph_dependency_flags_t;

typedef enum graph_access_flags_e {
	graph_access_flags_indirect_command_read          = 0x00000001,
	graph_access_flags_index_read                     = 0x00000002,
	graph_access_flags_vertex_attribute_read          = 0x00000004,
	graph_access_flags_uniform_read                   = 0x00000008,
	graph_access_flags_input_attachment_read          = 0x00000010,
	graph_access_flags_shader_read                    = 0x00000020,
	graph_access_flags_shader_write                   = 0x00000040,
	graph_access_flags_color_attachment_read          = 0x00000080,
	graph_access_flags_color_attachment_write         = 0x00000100,
	graph_access_flags_depth_stencil_attachment_read  = 0x00000200,
	graph_access_flags_depth_stencil_attachment_write = 0x00000400,
	graph_access_flags_transfer_read                  = 0x00000800,
	graph_access_flags_transfer_write                 = 0x00001000,
	graph_access_flags_host_read                      = 0x00002000,
	graph_access_flags_host_write                     = 0x00004000,
	graph_access_flags_memory_read                    = 0x00008000,
	graph_access_flags_memory_write                   = 0x00010000
} graph_access_flags_t;

#endif
