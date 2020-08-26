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
    graph_image_layout_unknow,
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

typedef enum graph_pipeline_bind_point_e {
    graph_pipeline_bind_point_graphics,
    graph_pipeline_bind_point_compute,
    graph_pipeline_bind_point$number
} graph_pipeline_bind_point_t;

typedef enum graph_pipe_flags_e {
	graph_pipe_flags_disable_optimization         = 0x00000001,
	graph_pipe_flags_allow_derivatives            = 0x00000002,
	graph_pipe_flags_derivative                   = 0x00000004,
	graph_pipe_flags_view_index_from_device_index = 0x00000008,
	graph_pipe_flags_dispatch_base                = 0x00000010
} graph_pipe_flags_t;

#endif
