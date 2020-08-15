#include "device_pri.h"
#include "graph_pri.h"
#include <inttypes.h>
#include <alloca.h>

graph_devices_s* graph_instance_devices_get(const graph_s *restrict g)
{
	graph_devices_s *restrict r;
	graph_device_t *restrict da;
	VkPhysicalDevice *pd;
	uint32_t number;
	VkResult ret;
	if (!g->instance) goto label_null;
	ret = vkEnumeratePhysicalDevices(g->instance, &number, NULL);
	if (ret) goto label_fail;
	pd = (VkPhysicalDevice *) alloca(sizeof(VkPhysicalDevice) * number);
	if (!pd) goto label_alloca;
	ret = vkEnumeratePhysicalDevices(g->instance, &number, pd);
	if (ret) goto label_fail;
	r = (graph_devices_s *) refer_alloc(sizeof(graph_devices_s) + sizeof(graph_device_t) * number);
	if (r)
	{
		r->number = 0;
		da = r->device_array;
		do {
			da->phydev = *pd;
			da->index = r->number++;
			vkGetPhysicalDeviceProperties(*pd, &da->properties);
			vkGetPhysicalDeviceFeatures(*pd, &da->features);
			++pd;
		} while (--number);
		return r;
	}
	label_return_null:
	return NULL;
	label_null:
	mlog_printf(g->mlog, "[graph_instance_devices_get] instance = null\n");
	goto label_return_null;
	label_fail:
	mlog_printf(g->mlog, "[graph_instance_devices_get] vkEnumeratePhysicalDevices = %d\n", ret);
	goto label_return_null;
	label_alloca:
	mlog_printf(g->mlog, "[graph_instance_devices_get] alloca fail\n");
	goto label_return_null;
}

uint32_t graph_devices_number(register const graph_devices_s *restrict gds)
{
	return (uint32_t) gds->number;
}

const graph_device_t* graph_devices_index(register const graph_devices_s *restrict gds, register uint32_t index)
{
	if (index < gds->number)
		return gds->device_array + index;
	return NULL;
}

const char* graph_physical_device_type_string(graph_physical_device_type_t type)
{
	static const char* mapping[] = {
		[graph_physical_device_type_other]          = "other",
		[graph_physical_device_type_integrated_gpu] = "integrated gpu",
		[graph_physical_device_type_discrete_gpu]   = "discrete gpu",
		[graph_physical_device_type_virtual_gpu]    = "virtual gpu",
		[graph_physical_device_type_cpu]            = "cpu"
	};
	if ((uint32_t) type < (sizeof(mapping) / sizeof(const char *)))
		return mapping[type];
	return "unknow";
}

static inline const char* graph_device_bool(VkBool32 b)
{
	return (const char * []){"false", "true"}[!!b];
}

void graph_device_properties_limits_dump(const graph_device_t *restrict gd, mlog_s *ml)
{
	register const VkPhysicalDeviceLimits *restrict p;
	p = &gd->properties.limits;
	mlog_printf(ml, "\t" "max image dimension 1D                                = %u\n", p->maxImageDimension1D);
	mlog_printf(ml, "\t" "max image dimension 2D                                = %u\n", p->maxImageDimension2D);
	mlog_printf(ml, "\t" "max image dimension 3D                                = %u\n", p->maxImageDimension3D);
	mlog_printf(ml, "\t" "max image dimension cube                              = %u\n", p->maxImageDimensionCube);
	mlog_printf(ml, "\t" "max image array layers                                = %u\n", p->maxImageArrayLayers);
	mlog_printf(ml, "\t" "max texel buffer elements                             = %u\n", p->maxTexelBufferElements);
	mlog_printf(ml, "\t" "max uniform buffer range                              = %u\n", p->maxUniformBufferRange);
	mlog_printf(ml, "\t" "max storage buffer range                              = %u\n", p->maxStorageBufferRange);
	mlog_printf(ml, "\t" "max push constants size                               = %u\n", p->maxPushConstantsSize);
	mlog_printf(ml, "\t" "max memory allocation count                           = %u\n", p->maxMemoryAllocationCount);
	mlog_printf(ml, "\t" "max sampler allocation count                          = %u\n", p->maxSamplerAllocationCount);
	mlog_printf(ml, "\t" "buffer image granularity                              = %" PRIu64 "\n", p->bufferImageGranularity);
	mlog_printf(ml, "\t" "sparse address space size                             = %" PRIu64 "\n", p->sparseAddressSpaceSize);
	mlog_printf(ml, "\t" "max bound descriptor sets                             = %u\n", p->maxBoundDescriptorSets);
	mlog_printf(ml, "\t" "max per stage descriptor samplers                     = %u\n", p->maxPerStageDescriptorSamplers);
	mlog_printf(ml, "\t" "max per stage descriptor uniform buffers              = %u\n", p->maxPerStageDescriptorUniformBuffers);
	mlog_printf(ml, "\t" "max per stage descriptor storage buffers              = %u\n", p->maxPerStageDescriptorStorageBuffers);
	mlog_printf(ml, "\t" "max per stage descriptor sampled images               = %u\n", p->maxPerStageDescriptorSampledImages);
	mlog_printf(ml, "\t" "max per stage descriptor storage images               = %u\n", p->maxPerStageDescriptorStorageImages);
	mlog_printf(ml, "\t" "max per stage descriptor input attachments            = %u\n", p->maxPerStageDescriptorInputAttachments);
	mlog_printf(ml, "\t" "max per stage resources                               = %u\n", p->maxPerStageResources);
	mlog_printf(ml, "\t" "max descriptor set samplers                           = %u\n", p->maxDescriptorSetSamplers);
	mlog_printf(ml, "\t" "max descriptor set uniform buffers                    = %u\n", p->maxDescriptorSetUniformBuffers);
	mlog_printf(ml, "\t" "max descriptor set uniform buffers dynamic            = %u\n", p->maxDescriptorSetUniformBuffersDynamic);
	mlog_printf(ml, "\t" "max descriptor set storage buffers                    = %u\n", p->maxDescriptorSetStorageBuffers);
	mlog_printf(ml, "\t" "max descriptor set storage buffers dynamic            = %u\n", p->maxDescriptorSetStorageBuffersDynamic);
	mlog_printf(ml, "\t" "max descriptor set sampled images                     = %u\n", p->maxDescriptorSetSampledImages);
	mlog_printf(ml, "\t" "max descriptor set storage images                     = %u\n", p->maxDescriptorSetStorageImages);
	mlog_printf(ml, "\t" "max descriptor set input attachments                  = %u\n", p->maxDescriptorSetInputAttachments);
	mlog_printf(ml, "\t" "max vertex input attributes                           = %u\n", p->maxVertexInputAttributes);
	mlog_printf(ml, "\t" "max vertex input bindings                             = %u\n", p->maxVertexInputBindings);
	mlog_printf(ml, "\t" "max vertex input attributes offset                    = %u\n", p->maxVertexInputAttributeOffset);
	mlog_printf(ml, "\t" "max vertex input binding stride                       = %u\n", p->maxVertexInputBindingStride);
	mlog_printf(ml, "\t" "max vertex output components                          = %u\n", p->maxVertexOutputComponents);
	mlog_printf(ml, "\t" "max tessellation generation level                     = %u\n", p->maxTessellationGenerationLevel);
	mlog_printf(ml, "\t" "max tessellation patch size                           = %u\n", p->maxTessellationPatchSize);
	mlog_printf(ml, "\t" "max tessellation control per vertex input components  = %u\n", p->maxTessellationControlPerVertexInputComponents);
	mlog_printf(ml, "\t" "max tessellation control per vertex output components = %u\n", p->maxTessellationControlPerVertexOutputComponents);
	mlog_printf(ml, "\t" "max tessellation control per patch output components  = %u\n", p->maxTessellationControlPerPatchOutputComponents);
	mlog_printf(ml, "\t" "max tessellation control total output components      = %u\n", p->maxTessellationControlTotalOutputComponents);
	mlog_printf(ml, "\t" "max tessellation evaluation input components          = %u\n", p->maxTessellationEvaluationInputComponents);
	mlog_printf(ml, "\t" "max tessellation evaluation output components         = %u\n", p->maxTessellationEvaluationOutputComponents);
	mlog_printf(ml, "\t" "max geometry shader invocations                       = %u\n", p->maxGeometryShaderInvocations);
	mlog_printf(ml, "\t" "max geometry input components                         = %u\n", p->maxGeometryInputComponents);
	mlog_printf(ml, "\t" "max geometry output components                        = %u\n", p->maxGeometryOutputComponents);
	mlog_printf(ml, "\t" "max geometry output vertices                          = %u\n", p->maxGeometryOutputVertices);
	mlog_printf(ml, "\t" "max geometry total output components                  = %u\n", p->maxGeometryTotalOutputComponents);
	mlog_printf(ml, "\t" "max fragment input components                         = %u\n", p->maxFragmentInputComponents);
	mlog_printf(ml, "\t" "max fragment output attachments                       = %u\n", p->maxFragmentOutputAttachments);
	mlog_printf(ml, "\t" "max fragment dual src attachments                     = %u\n", p->maxFragmentDualSrcAttachments);
	mlog_printf(ml, "\t" "max fragment combined output resources                = %u\n", p->maxFragmentCombinedOutputResources);
	mlog_printf(ml, "\t" "max compute shared memory size                        = %u\n", p->maxComputeSharedMemorySize);
	mlog_printf(ml, "\t" "max compute work group count                          = [%u, %u, %u]\n", p->maxComputeWorkGroupCount[0], p->maxComputeWorkGroupCount[1], p->maxComputeWorkGroupCount[2]);
	mlog_printf(ml, "\t" "max compute work group invocations                    = %u\n", p->maxComputeWorkGroupInvocations);
	mlog_printf(ml, "\t" "max compute work group size                           = [%u, %u, %u]\n", p->maxComputeWorkGroupSize[0], p->maxComputeWorkGroupSize[1], p->maxComputeWorkGroupSize[2]);
	mlog_printf(ml, "\t" "sub pixel precision bits                              = %u\n", p->subPixelPrecisionBits);
	mlog_printf(ml, "\t" "sub texel precision bits                              = %u\n", p->subTexelPrecisionBits);
	mlog_printf(ml, "\t" "mipmap precision bits                                 = %u\n", p->mipmapPrecisionBits);
	mlog_printf(ml, "\t" "max draw indexed index value                          = %u\n", p->maxDrawIndexedIndexValue);
	mlog_printf(ml, "\t" "max draw indirect count                               = %u\n", p->maxDrawIndirectCount);
	mlog_printf(ml, "\t" "max sampler lod bias                                  = %f\n", p->maxSamplerLodBias);
	mlog_printf(ml, "\t" "max sampler anisotropy                                = %f\n", p->maxSamplerAnisotropy);
	mlog_printf(ml, "\t" "max viewports                                         = %u\n", p->maxViewports);
	mlog_printf(ml, "\t" "max viewport dimensions                               = [%u, %u]\n", p->maxViewportDimensions[0], p->maxViewportDimensions[1]);
	mlog_printf(ml, "\t" "viewport bounds range                                 = [%f, %f]\n", p->viewportBoundsRange[0], p->viewportBoundsRange[1]);
	mlog_printf(ml, "\t" "viewport sub pixel bits                               = %u\n", p->viewportSubPixelBits);
	mlog_printf(ml, "\t" "min memory map alignment                              = %" PRIuPTR "\n", p->minMemoryMapAlignment);
	mlog_printf(ml, "\t" "min texel buffer offset alignment                     = %" PRIu64 "\n", p->minTexelBufferOffsetAlignment);
	mlog_printf(ml, "\t" "min uniform buffer offset alignment                   = %" PRIu64 "\n", p->minUniformBufferOffsetAlignment);
	mlog_printf(ml, "\t" "min storage buffer offset alignment                   = %" PRIu64 "\n", p->minStorageBufferOffsetAlignment);
	mlog_printf(ml, "\t" "min texel offset                                      = %d\n", p->minTexelOffset);
	mlog_printf(ml, "\t" "max texel offset                                      = %u\n", p->maxTexelOffset);
	mlog_printf(ml, "\t" "min texel gather offset                               = %d\n", p->minTexelGatherOffset);
	mlog_printf(ml, "\t" "max texel gather offset                               = %u\n", p->maxTexelGatherOffset);
	mlog_printf(ml, "\t" "min interpolation offset                              = %f\n", p->minInterpolationOffset);
	mlog_printf(ml, "\t" "max interpolation offset                              = %f\n", p->maxInterpolationOffset);
	mlog_printf(ml, "\t" "sub pixel interpolation offset bits                   = %u\n", p->subPixelInterpolationOffsetBits);
	mlog_printf(ml, "\t" "max frame buffer width                                = %u\n", p->maxFramebufferWidth);
	mlog_printf(ml, "\t" "max frame buffer height                               = %u\n", p->maxFramebufferHeight);
	mlog_printf(ml, "\t" "max frame buffer layers                               = %u\n", p->maxFramebufferLayers);
	mlog_printf(ml, "\t" "frame buffer color sample counts                      = 0x%02x\n", p->framebufferColorSampleCounts);
	mlog_printf(ml, "\t" "frame buffer depth sample counts                      = 0x%02x\n", p->framebufferDepthSampleCounts);
	mlog_printf(ml, "\t" "frame buffer stencil sample counts                    = 0x%02x\n", p->framebufferStencilSampleCounts);
	mlog_printf(ml, "\t" "frame buffer no attachments sample counts             = 0x%02x\n", p->framebufferNoAttachmentsSampleCounts);
	mlog_printf(ml, "\t" "max color attachments                                 = %u\n", p->maxColorAttachments);
	mlog_printf(ml, "\t" "sampled image color sample counts                     = 0x%02x\n", p->sampledImageColorSampleCounts);
	mlog_printf(ml, "\t" "sampled image integer sample counts                   = 0x%02x\n", p->sampledImageIntegerSampleCounts);
	mlog_printf(ml, "\t" "sampled image depth sample counts                     = 0x%02x\n", p->sampledImageDepthSampleCounts);
	mlog_printf(ml, "\t" "sampled image stencil sample counts                   = 0x%02x\n", p->sampledImageStencilSampleCounts);
	mlog_printf(ml, "\t" "storage image sample counts                           = 0x%02x\n", p->storageImageSampleCounts);
	mlog_printf(ml, "\t" "max sample mask words                                 = %u\n", p->maxSampleMaskWords);
	mlog_printf(ml, "\t" "timestamp compute and graphics                        = %s\n", graph_device_bool(p->timestampComputeAndGraphics));
	mlog_printf(ml, "\t" "timestamp period                                      = %f\n", p->timestampPeriod);
	mlog_printf(ml, "\t" "max clip distances                                    = %u\n", p->maxClipDistances);
	mlog_printf(ml, "\t" "max cull distances                                    = %u\n", p->maxCullDistances);
	mlog_printf(ml, "\t" "max combined clip and cull distances                  = %u\n", p->maxCombinedClipAndCullDistances);
	mlog_printf(ml, "\t" "discrete queue priorities                             = %u\n", p->discreteQueuePriorities);
	mlog_printf(ml, "\t" "point size range                                      = [%f, %f]\n", p->pointSizeRange[0], p->pointSizeRange[1]);
	mlog_printf(ml, "\t" "line width range                                      = [%f, %f]\n", p->lineWidthRange[0], p->lineWidthRange[1]);
	mlog_printf(ml, "\t" "point size granularity                                = %f\n", p->pointSizeGranularity);
	mlog_printf(ml, "\t" "line width granularity                                = %f\n", p->lineWidthGranularity);
	mlog_printf(ml, "\t" "strict lines                                          = %s\n", graph_device_bool(p->strictLines));
	mlog_printf(ml, "\t" "standard sample locations                             = %s\n", graph_device_bool(p->standardSampleLocations));
	mlog_printf(ml, "\t" "optimal buffer copy offset alignment                  = %" PRIu64 "\n", p->optimalBufferCopyOffsetAlignment);
	mlog_printf(ml, "\t" "optimal buffer copy row pitch alignment               = %" PRIu64 "\n", p->optimalBufferCopyRowPitchAlignment);
	mlog_printf(ml, "\t" "non coherent atom size                                = %" PRIu64 "\n", p->nonCoherentAtomSize);
}

void graph_device_properties_sparse_dump(const graph_device_t *restrict gd, mlog_s *ml)
{
	register const VkPhysicalDeviceSparseProperties *restrict p;
	p = &gd->properties.sparseProperties;
	mlog_printf(ml, "\t" "residency standard 2D block shape                     = %s\n", graph_device_bool(p->residencyStandard2DBlockShape));
	mlog_printf(ml, "\t" "residency standard 2D multisample block shape         = %s\n", graph_device_bool(p->residencyStandard2DMultisampleBlockShape));
	mlog_printf(ml, "\t" "residency standard 3D block shape                     = %s\n", graph_device_bool(p->residencyStandard3DBlockShape));
	mlog_printf(ml, "\t" "residency aligned mip size                            = %s\n", graph_device_bool(p->residencyAlignedMipSize));
	mlog_printf(ml, "\t" "residency non resident strict                         = %s\n", graph_device_bool(p->residencyNonResidentStrict));
}

void graph_device_properties_dump(const graph_device_t *restrict gd, mlog_s *ml)
{
	register const VkPhysicalDeviceProperties *restrict p;
	register uint32_t v;
	p = &gd->properties;
	v = p->apiVersion;
	mlog_printf(ml, "\t" "api version                                           = %u.%u.%u\n", VK_VERSION_MAJOR(v), VK_VERSION_MINOR(v), VK_VERSION_PATCH(v));
	v = p->driverVersion;
	mlog_printf(ml, "\t" "driver version                                        = %u.%u.%u\n", VK_VERSION_MAJOR(v), VK_VERSION_MINOR(v), VK_VERSION_PATCH(v));
	mlog_printf(ml, "\t" "vendor ID                                             = 0x%x\n", p->vendorID);
	mlog_printf(ml, "\t" "device ID                                             = 0x%x\n", p->deviceID);
	mlog_printf(ml, "\t" "device type                                           = %d (%s)\n",
		p->deviceType, graph_physical_device_type_string((graph_physical_device_type_t) p->deviceType));
	mlog_printf(ml, "\t" "device name                                           = %s\n", p->deviceName);
	mlog_printf(ml, "\t" "pipeline cache UUID                                   = %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
		p->pipelineCacheUUID[0], p->pipelineCacheUUID[1], p->pipelineCacheUUID[2], p->pipelineCacheUUID[3],
		p->pipelineCacheUUID[4], p->pipelineCacheUUID[5], p->pipelineCacheUUID[6], p->pipelineCacheUUID[7],
		p->pipelineCacheUUID[8], p->pipelineCacheUUID[9], p->pipelineCacheUUID[10], p->pipelineCacheUUID[11],
		p->pipelineCacheUUID[12], p->pipelineCacheUUID[13], p->pipelineCacheUUID[14], p->pipelineCacheUUID[15]);
	graph_device_properties_limits_dump(gd, ml);
	graph_device_properties_sparse_dump(gd, ml);
}

void graph_device_features_dump(const graph_device_t *restrict gd, mlog_s *ml)
{
	register const VkPhysicalDeviceFeatures *restrict p;
	p = &gd->features;
	mlog_printf(ml, "\t" "robust buffer access                         = %s\n", graph_device_bool(p->robustBufferAccess));
	mlog_printf(ml, "\t" "full draw index uint32                       = %s\n", graph_device_bool(p->fullDrawIndexUint32));
	mlog_printf(ml, "\t" "image cube array                             = %s\n", graph_device_bool(p->imageCubeArray));
	mlog_printf(ml, "\t" "independent blend                            = %s\n", graph_device_bool(p->independentBlend));
	mlog_printf(ml, "\t" "geometry shader                              = %s\n", graph_device_bool(p->geometryShader));
	mlog_printf(ml, "\t" "tessellation shader                          = %s\n", graph_device_bool(p->tessellationShader));
	mlog_printf(ml, "\t" "sample rate shading                          = %s\n", graph_device_bool(p->sampleRateShading));
	mlog_printf(ml, "\t" "dual src blend                               = %s\n", graph_device_bool(p->dualSrcBlend));
	mlog_printf(ml, "\t" "logic op                                     = %s\n", graph_device_bool(p->logicOp));
	mlog_printf(ml, "\t" "multi draw indirect                          = %s\n", graph_device_bool(p->multiDrawIndirect));
	mlog_printf(ml, "\t" "draw indirect first instance                 = %s\n", graph_device_bool(p->drawIndirectFirstInstance));
	mlog_printf(ml, "\t" "depth clamp                                  = %s\n", graph_device_bool(p->depthClamp));
	mlog_printf(ml, "\t" "depth bias clamp                             = %s\n", graph_device_bool(p->depthBiasClamp));
	mlog_printf(ml, "\t" "fill mode non solid                          = %s\n", graph_device_bool(p->fillModeNonSolid));
	mlog_printf(ml, "\t" "depth bounds                                 = %s\n", graph_device_bool(p->depthBounds));
	mlog_printf(ml, "\t" "wide lines                                   = %s\n", graph_device_bool(p->wideLines));
	mlog_printf(ml, "\t" "large points                                 = %s\n", graph_device_bool(p->largePoints));
	mlog_printf(ml, "\t" "alpha to one                                 = %s\n", graph_device_bool(p->alphaToOne));
	mlog_printf(ml, "\t" "multi viewport                               = %s\n", graph_device_bool(p->multiViewport));
	mlog_printf(ml, "\t" "sampler anisotropy                           = %s\n", graph_device_bool(p->samplerAnisotropy));
	mlog_printf(ml, "\t" "texture compression ETC2                     = %s\n", graph_device_bool(p->textureCompressionETC2));
	mlog_printf(ml, "\t" "texture compression ASTC LDR                 = %s\n", graph_device_bool(p->textureCompressionASTC_LDR));
	mlog_printf(ml, "\t" "texture compression BC                       = %s\n", graph_device_bool(p->textureCompressionBC));
	mlog_printf(ml, "\t" "occlusion query precise                      = %s\n", graph_device_bool(p->occlusionQueryPrecise));
	mlog_printf(ml, "\t" "pipeline statistics query                    = %s\n", graph_device_bool(p->pipelineStatisticsQuery));
	mlog_printf(ml, "\t" "vertex pipeline stores and atomics           = %s\n", graph_device_bool(p->vertexPipelineStoresAndAtomics));
	mlog_printf(ml, "\t" "fragment stores and atomics                  = %s\n", graph_device_bool(p->fragmentStoresAndAtomics));
	mlog_printf(ml, "\t" "shader tessellation and geometry point size  = %s\n", graph_device_bool(p->shaderTessellationAndGeometryPointSize));
	mlog_printf(ml, "\t" "shader image gather extended                 = %s\n", graph_device_bool(p->shaderImageGatherExtended));
	mlog_printf(ml, "\t" "shader storage image extended formats        = %s\n", graph_device_bool(p->shaderStorageImageExtendedFormats));
	mlog_printf(ml, "\t" "shader storage image multisample             = %s\n", graph_device_bool(p->shaderStorageImageMultisample));
	mlog_printf(ml, "\t" "shader storage image read without format     = %s\n", graph_device_bool(p->shaderStorageImageReadWithoutFormat));
	mlog_printf(ml, "\t" "shader storage image write without format    = %s\n", graph_device_bool(p->shaderStorageImageWriteWithoutFormat));
	mlog_printf(ml, "\t" "shader uniform buffer array dynamic indexing = %s\n", graph_device_bool(p->shaderUniformBufferArrayDynamicIndexing));
	mlog_printf(ml, "\t" "shader sampled image array dynamic indexing  = %s\n", graph_device_bool(p->shaderSampledImageArrayDynamicIndexing));
	mlog_printf(ml, "\t" "shader storage buffer array dynamic indexing = %s\n", graph_device_bool(p->shaderStorageBufferArrayDynamicIndexing));
	mlog_printf(ml, "\t" "shader storage image array dynamic indexing  = %s\n", graph_device_bool(p->shaderStorageImageArrayDynamicIndexing));
	mlog_printf(ml, "\t" "shader clip distance                         = %s\n", graph_device_bool(p->shaderClipDistance));
	mlog_printf(ml, "\t" "shader cull distance                         = %s\n", graph_device_bool(p->shaderCullDistance));
	mlog_printf(ml, "\t" "shader float64                               = %s\n", graph_device_bool(p->shaderFloat64));
	mlog_printf(ml, "\t" "shader int64                                 = %s\n", graph_device_bool(p->shaderInt64));
	mlog_printf(ml, "\t" "shader int16                                 = %s\n", graph_device_bool(p->shaderInt16));
	mlog_printf(ml, "\t" "shader resource residency                    = %s\n", graph_device_bool(p->shaderResourceResidency));
	mlog_printf(ml, "\t" "shader resource min lod                      = %s\n", graph_device_bool(p->shaderResourceMinLod));
	mlog_printf(ml, "\t" "sparse binding                               = %s\n", graph_device_bool(p->sparseBinding));
	mlog_printf(ml, "\t" "sparse residency buffer                      = %s\n", graph_device_bool(p->sparseResidencyBuffer));
	mlog_printf(ml, "\t" "sparse residency image 2D                    = %s\n", graph_device_bool(p->sparseResidencyImage2D));
	mlog_printf(ml, "\t" "sparse residency image 3D                    = %s\n", graph_device_bool(p->sparseResidencyImage3D));
	mlog_printf(ml, "\t" "sparse residency 2 samples                   = %s\n", graph_device_bool(p->sparseResidency2Samples));
	mlog_printf(ml, "\t" "sparse residency 4 samples                   = %s\n", graph_device_bool(p->sparseResidency4Samples));
	mlog_printf(ml, "\t" "sparse residency 8 samples                   = %s\n", graph_device_bool(p->sparseResidency8Samples));
	mlog_printf(ml, "\t" "sparse residency 16 samples                  = %s\n", graph_device_bool(p->sparseResidency16Samples));
	mlog_printf(ml, "\t" "sparse residency aliased                     = %s\n", graph_device_bool(p->sparseResidencyAliased));
	mlog_printf(ml, "\t" "variable multi sample rate                   = %s\n", graph_device_bool(p->variableMultisampleRate));
	mlog_printf(ml, "\t" "inherited queries                            = %s\n", graph_device_bool(p->inheritedQueries));
}

void graph_device_dump(const graph_device_t *restrict gd, mlog_s *ml)
{
	mlog_printf(ml, "[%u] (%s):\n", (uint32_t) gd->index, gd->properties.deviceName);
	graph_device_properties_dump(gd, ml);
	graph_device_features_dump(gd, ml);
}

