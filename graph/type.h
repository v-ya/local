#ifndef _graph_type_h_
#define _graph_type_h_

#include <stdint.h>

typedef uint32_t graph_bool_t;

typedef enum graph_layer_e {
	graph_layer_null,
	graph_layer_validation,
	graph_layer_api_dump,
	graph_layer$number
} graph_layer_t;

typedef enum graph_extension_e {
	graph_extension_null,
	graph_extension_ext_debug_utils,
	graph_extension_khr_surface,
	graph_extension_khr_xcb_surface,
	graph_extension$number
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

#endif
