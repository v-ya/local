#include "type_pri.h"
#include "device_pri.h"
#include "graph_pri.h"
#include "support_pri.h"
#include "surface_pri.h"
#include <inttypes.h>
#include <stdlib.h>
#include <memory.h>
#include <alloca.h>

#define graph_df_offset(_n)  [graph_device_feature_##_n] = (uint32_t) offsetof(VkPhysicalDeviceFeatures, _n)
static uint32_t graph_device_feature_offset[graph_device_feature$number] = {
	graph_df_offset(robustBufferAccess),
	graph_df_offset(fullDrawIndexUint32),
	graph_df_offset(imageCubeArray),
	graph_df_offset(independentBlend),
	graph_df_offset(geometryShader),
	graph_df_offset(tessellationShader),
	graph_df_offset(sampleRateShading),
	graph_df_offset(dualSrcBlend),
	graph_df_offset(logicOp),
	graph_df_offset(multiDrawIndirect),
	graph_df_offset(drawIndirectFirstInstance),
	graph_df_offset(depthClamp),
	graph_df_offset(depthBiasClamp),
	graph_df_offset(fillModeNonSolid),
	graph_df_offset(depthBounds),
	graph_df_offset(wideLines),
	graph_df_offset(largePoints),
	graph_df_offset(alphaToOne),
	graph_df_offset(multiViewport),
	graph_df_offset(samplerAnisotropy),
	graph_df_offset(textureCompressionETC2),
	graph_df_offset(textureCompressionASTC_LDR),
	graph_df_offset(textureCompressionBC),
	graph_df_offset(occlusionQueryPrecise),
	graph_df_offset(pipelineStatisticsQuery),
	graph_df_offset(vertexPipelineStoresAndAtomics),
	graph_df_offset(fragmentStoresAndAtomics),
	graph_df_offset(shaderTessellationAndGeometryPointSize),
	graph_df_offset(shaderImageGatherExtended),
	graph_df_offset(shaderStorageImageExtendedFormats),
	graph_df_offset(shaderStorageImageMultisample),
	graph_df_offset(shaderStorageImageReadWithoutFormat),
	graph_df_offset(shaderStorageImageWriteWithoutFormat),
	graph_df_offset(shaderUniformBufferArrayDynamicIndexing),
	graph_df_offset(shaderSampledImageArrayDynamicIndexing),
	graph_df_offset(shaderStorageBufferArrayDynamicIndexing),
	graph_df_offset(shaderStorageImageArrayDynamicIndexing),
	graph_df_offset(shaderClipDistance),
	graph_df_offset(shaderCullDistance),
	graph_df_offset(shaderFloat64),
	graph_df_offset(shaderInt64),
	graph_df_offset(shaderInt16),
	graph_df_offset(shaderResourceResidency),
	graph_df_offset(shaderResourceMinLod),
	graph_df_offset(sparseBinding),
	graph_df_offset(sparseResidencyBuffer),
	graph_df_offset(sparseResidencyImage2D),
	graph_df_offset(sparseResidencyImage3D),
	graph_df_offset(sparseResidency2Samples),
	graph_df_offset(sparseResidency4Samples),
	graph_df_offset(sparseResidency8Samples),
	graph_df_offset(sparseResidency16Samples),
	graph_df_offset(sparseResidencyAliased),
	graph_df_offset(variableMultisampleRate),
	graph_df_offset(inheritedQueries),
};
#undef graph_df_offset

static void graph_p_mlog_free_func(mlog_s **restrict p)
{
	if (*p) refer_free(*p);
}

graph_devices_s* graph_instance_devices_get(const struct graph_s *restrict g)
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
		r->ml = (mlog_s *) refer_save(g->ml);
		r->number = 0;
		refer_set_free(r, (refer_free_f) graph_p_mlog_free_func);
		da = r->device_array;
		do {
			da->ml = g->ml;
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
	mlog_printf(g->ml, "[graph_instance_devices_get] instance = null\n");
	goto label_return_null;
	label_fail:
	mlog_printf(g->ml, "[graph_instance_devices_get] vkEnumeratePhysicalDevices = %d\n", ret);
	goto label_return_null;
	label_alloca:
	mlog_printf(g->ml, "[graph_instance_devices_get] alloca fail\n");
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

graph_device_queues_s* graph_device_queues_get(register const graph_device_t *restrict gd)
{
	graph_device_queues_s *restrict r;
	VkQueueFamilyProperties *p;
	uint32_t i, number;
	vkGetPhysicalDeviceQueueFamilyProperties(gd->phydev, &number, NULL);
	if (number)
	{
		r = (graph_device_queues_s *) refer_alloc(sizeof(graph_device_queues_s) + (sizeof(graph_device_queue_t) + sizeof(VkQueueFamilyProperties)) * number);
		if (r)
		{
			r->ml = (mlog_s *) refer_save(gd->ml);
			r->phydev = gd->phydev;
			refer_set_free(r, (refer_free_f) graph_p_mlog_free_func);
			p = (VkQueueFamilyProperties *) ((uint8_t *) r + (sizeof(graph_device_queues_s) + sizeof(graph_device_queue_t) * number));
			vkGetPhysicalDeviceQueueFamilyProperties(gd->phydev, &number, p);
			r->number = number;
			for (i = 0; i < number; ++i)
			{
				r->queue_array[i].ml = gd->ml;
				r->queue_array[i].index = i;
				r->queue_array[i].properties = p + i;
			}
			return r;
		}
	}
	return NULL;
}

uint32_t graph_device_queues_number(register const graph_device_queues_s *restrict gdqs)
{
	return (uint32_t) gdqs->number;
}

const graph_device_queue_t* graph_device_queues_index(register const graph_device_queues_s *restrict gdqs, register uint32_t index)
{
	if (index < gdqs->number)
		return gdqs->queue_array + index;
	return NULL;
}

static void graph_dev_param_free_func(register graph_dev_param_s *restrict r)
{
	register void *v;
	register uint32_t n;
	if ((v = r->layer_list)) free(v);
	if ((v = r->extension_list)) free(v);
	n = r->qf_number;
	while (n)
	{
		v = (void *) r->queue_info[--n].pQueuePriorities;
		if (v && (uintptr_t) ((uint8_t *) v - (uint8_t *) r->priorities) >= (sizeof(float) * graph_dev_param_priorities_preset_number))
			free(v);
	}
}

graph_dev_param_s* graph_dev_param_alloc(register uint32_t queue_family_number)
{
	register graph_dev_param_s *restrict r;
	r = (graph_dev_param_s *) refer_alloz(sizeof(graph_dev_param_s) + sizeof(VkDeviceQueueCreateInfo) * queue_family_number);
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_dev_param_free_func);
		r->qf_number = queue_family_number;
		r->p_over = graph_dev_param_priorities_preset_number;
		while (queue_family_number)
			r->queue_info[--queue_family_number].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		return r;
	}
	return NULL;
}

graph_dev_param_s* graph_dev_param_set_queue(register graph_dev_param_s *restrict param, uint32_t index, const graph_device_queue_t *restrict queue, uint32_t number, const float *restrict priorities, graph_queue_create_flags_t flags)
{
	if (index < param->qf_number && number && number <= queue->properties->queueCount)
	{
		register VkDeviceQueueCreateInfo *restrict info;
		register float *restrict p;
		info = param->queue_info + index;
		if ((p = (float *) info->pQueuePriorities))
		{
			if ((uintptr_t) ((uint8_t *) p - (uint8_t *) param->priorities) >= (sizeof(float) * graph_dev_param_priorities_preset_number))
				free(p);
			info->pQueuePriorities = NULL;
		}
		if (param->p_over >= number)
			p = param->priorities + (param->p_over -= number);
		else p = (float *) malloc(sizeof(float) * number);
		if (p)
		{
			info->flags = (VkDeviceQueueCreateFlags) flags;
			info->queueFamilyIndex = (uint32_t) queue->index;
			info->queueCount = number;
			info->pQueuePriorities = p;
			if (priorities) memcpy(p, priorities, sizeof(float) * number);
			else do {
				*p++ = 1.0f;
			} while (--number);
			return param;
		}
	}
	return NULL;
}

graph_dev_param_s* graph_dev_param_set_layer(register graph_dev_param_s *restrict param, const graph_layer_t *restrict layer)
{
	const char **s;
	uint32_t n;
	if (param->layer_list) free(param->layer_list);
	param->layer_list = NULL;
	param->layer_number = 0;
	if (layer)
	{
		if (graph_layer_mapping_length(layer, &n))
			goto Err;
		if (n)
		{
			s = (const char **) malloc(sizeof(const char *) * n);
			if (!s) goto Err;
			param->layer_list = graph_layer_mapping_copy(layer, s);
			param->layer_number = n;
		}
	}
	return param;
	Err:
	return NULL;
}

graph_dev_param_s* graph_dev_param_set_extension(register graph_dev_param_s *restrict param, const graph_extension_t *restrict extension)
{
	const char **s;
	uint32_t n;
	if (param->extension_list) free(param->extension_list);
	param->extension_list = NULL;
	param->extension_number = 0;
	if (extension)
	{
		if (graph_extension_mapping_length(extension, &n))
			goto Err;
		if (n)
		{
			s = (const char **) malloc(sizeof(const char *) * n);
			if (!s) goto Err;
			param->extension_list = graph_extension_mapping_copy(extension, s);
			param->extension_number = n;
		}
	}
	return param;
	Err:
	return NULL;
}

void graph_dev_param_feature_enable(register graph_dev_param_s *restrict param, register graph_device_feature_t feature)
{
	if ((uint32_t) feature < graph_device_feature$number)
		*(VkBool32 *) ((uint8_t *) &param->features + graph_device_feature_offset[feature]) = 1;
}

void graph_dev_param_feature_disable(register graph_dev_param_s *restrict param, register graph_device_feature_t feature)
{
	if ((uint32_t) feature < graph_device_feature$number)
		*(VkBool32 *) ((uint8_t *) &param->features + graph_device_feature_offset[feature]) = 0;
}

void graph_dev_param_feature_enable_all(register graph_dev_param_s *restrict param, register const graph_device_t *restrict gd)
{
	memcpy(&param->features, &gd->features, sizeof(param->features));
}

static void graph_dev_free_func(register graph_dev_s *restrict r)
{
	register void *v;
	graph_dev_wait_idle(r);
	if ((v = r->dev)) vkDestroyDevice((VkDevice) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->g)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_dev_s* graph_dev_alloc(register const graph_dev_param_s *restrict param, register const struct graph_s *restrict g, register const graph_device_t *restrict gd)
{
	register graph_dev_s *restrict r;
	VkDeviceCreateInfo info;
	uint32_t i, n;
	VkResult ret;
	info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	info.pNext = NULL;
	info.flags = 0;
	info.queueCreateInfoCount = param->qf_number;
	info.pQueueCreateInfos = param->queue_info;
	info.enabledLayerCount = param->layer_number;
	info.ppEnabledLayerNames = param->layer_list;
	info.enabledExtensionCount = param->extension_number;
	info.ppEnabledExtensionNames = param->extension_list;
	info.pEnabledFeatures = &param->features;
	for (i = n = 0; i < info.queueCreateInfoCount; ++i)
		n += info.pQueueCreateInfos[i].queueCount;
	if (n)
	{
		r = (graph_dev_s *) refer_alloz(sizeof(graph_dev_s) + sizeof(graph_queue_t) * n);
		if (r)
		{
			refer_set_free(r, (refer_free_f) graph_dev_free_func);
			r->ml = (mlog_s *) refer_save(g->ml);
			r->g = (graph_s *) refer_save(g);
			r->ga = (graph_allocator_s *) refer_save(g->ga);
			ret = vkCreateDevice(r->phydev = gd->phydev, &info, &r->ga->alloc, &r->dev);
			if (ret) goto label_fail;
			r->qf_number = info.queueCreateInfoCount;
			r->q_number = n;
			i = 0;
			do {
				for (n = 0; n < info.pQueueCreateInfos->queueCount; ++n)
				{
					vkGetDeviceQueue(
						r->dev,
						r->queue[i].qfi = info.pQueueCreateInfos->queueFamilyIndex,
						r->queue[i].qi = n, &r->queue[i].queue
					);
					++i;
				}
				++info.pQueueCreateInfos;
			} while (--info.queueCreateInfoCount);
			return r;
			label_fail:
			mlog_printf(gd->ml, "[graph_dev_alloc] vkCreateDevice = %d\n", ret);
			refer_free(r);
		}
	}
	return NULL;
}

graph_queue_t* graph_dev_queue(register graph_dev_s *restrict dev, const graph_device_queue_t *restrict queue, uint32_t qi)
{
	register graph_queue_t *restrict r;
	register uint32_t i, n;
	i = (uint32_t) queue->index;
	n = dev->q_number;
	r = dev->queue;
	do {
		if (r->qfi == i && !qi--)
			return r;
		++r;
	} while (--n);
	return NULL;
}

graph_dev_s* graph_dev_wait_idle(register graph_dev_s *restrict dev)
{
	VkResult r;
	r = vkDeviceWaitIdle(dev->dev);
	if (!r) return dev;
	mlog_printf(dev->ml, "[graph_dev_wait_idle] vkDeviceWaitIdle = %d\n", r);
	return NULL;
}

graph_physical_device_type_t graph_device_type(register const graph_device_t *restrict gd)
{
	return (graph_physical_device_type_t) gd->properties.deviceType;
}

const char* graph_device_name(register const graph_device_t *restrict gd)
{
	return gd->properties.deviceName;
}

graph_queue_flags_t graph_device_queue_flags(register const graph_device_queue_t *restrict q)
{
	return (graph_queue_flags_t) q->properties->queueFlags;
}

uint32_t graph_device_queue_count(register const graph_device_queue_t *restrict q)
{
	return q->properties->queueCount;
}

uint32_t graph_device_queue_timestamp_valid_bits(register const graph_device_queue_t *restrict q)
{
	return q->properties->timestampValidBits;
}

void graph_device_queue_min_image_transfer_granularity(register const graph_device_queue_t *restrict q, uint32_t *restrict width, uint32_t *restrict height, uint32_t *restrict depth)
{
	register const VkExtent3D *restrict e = &q->properties->minImageTransferGranularity;
	if (width) *width = e->width;
	if (height) *height = e->height;
	if (depth) *depth = e->depth;
}

graph_bool_t graph_device_features_test(register const graph_device_t *restrict gd, register graph_device_feature_t feature)
{
	if ((uint32_t) feature < graph_device_feature$number)
		return *(VkBool32 *) ((uint8_t *) &gd->features + graph_device_feature_offset[feature]);
	return 0;
}

graph_bool_t graph_device_queue_surface_support(register const graph_device_queue_t *restrict q, register const struct graph_surface_s *restrict sf)
{
	register const graph_device_queues_s *restrict qs;
	VkResult r;
	VkBool32 b;
	qs = (graph_device_queues_s *) ((uint8_t *) (q - q->index) - offsetof(graph_device_queues_s, queue_array));
	r = vkGetPhysicalDeviceSurfaceSupportKHR(qs->phydev, (uint32_t) q->index, sf->surface, &b);
	if (!r) goto label_return;
	mlog_printf(q->ml, "[graph_device_queue_surface_support] vkGetPhysicalDeviceSurfaceSupportKHR = %d\n", r);
	b = 0;
	label_return:
	return b;
}

void graph_device_properties_header_dump(const graph_device_t *restrict gd)
{
	register mlog_s *ml;
	register const VkPhysicalDeviceProperties *restrict p;
	register uint32_t v;
	ml = gd->ml;
	p = &gd->properties;
	v = p->apiVersion;
	mlog_printf(ml, "\t" "api version                                           = %u.%u.%u\n", VK_VERSION_MAJOR(v), VK_VERSION_MINOR(v), VK_VERSION_PATCH(v));
	v = p->driverVersion;
	mlog_printf(ml, "\t" "driver version                                        = %u.%u.%u\n", VK_VERSION_MAJOR(v), VK_VERSION_MINOR(v), VK_VERSION_PATCH(v));
	mlog_printf(ml, "\t" "vendor ID                                             = 0x%x\n", p->vendorID);
	mlog_printf(ml, "\t" "device ID                                             = 0x%x\n", p->deviceID);
	mlog_printf(ml, "\t" "device type                                           = %d (%s)\n",
		p->deviceType, graph_physical_device_type$string((graph_physical_device_type_t) p->deviceType));
	mlog_printf(ml, "\t" "device name                                           = %s\n", p->deviceName);
	mlog_printf(ml, "\t" "pipeline cache UUID                                   = %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
		p->pipelineCacheUUID[0], p->pipelineCacheUUID[1], p->pipelineCacheUUID[2], p->pipelineCacheUUID[3],
		p->pipelineCacheUUID[4], p->pipelineCacheUUID[5], p->pipelineCacheUUID[6], p->pipelineCacheUUID[7],
		p->pipelineCacheUUID[8], p->pipelineCacheUUID[9], p->pipelineCacheUUID[10], p->pipelineCacheUUID[11],
		p->pipelineCacheUUID[12], p->pipelineCacheUUID[13], p->pipelineCacheUUID[14], p->pipelineCacheUUID[15]);
}

void graph_device_properties_limits_dump(const graph_device_t *restrict gd)
{
	register mlog_s *ml;
	register const VkPhysicalDeviceLimits *restrict p;
	ml = gd->ml;
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
	mlog_printf(ml, "\t" "timestamp compute and graphics                        = %s\n", graph_bool$string(p->timestampComputeAndGraphics));
	mlog_printf(ml, "\t" "timestamp period                                      = %f\n", p->timestampPeriod);
	mlog_printf(ml, "\t" "max clip distances                                    = %u\n", p->maxClipDistances);
	mlog_printf(ml, "\t" "max cull distances                                    = %u\n", p->maxCullDistances);
	mlog_printf(ml, "\t" "max combined clip and cull distances                  = %u\n", p->maxCombinedClipAndCullDistances);
	mlog_printf(ml, "\t" "discrete queue priorities                             = %u\n", p->discreteQueuePriorities);
	mlog_printf(ml, "\t" "point size range                                      = [%f, %f]\n", p->pointSizeRange[0], p->pointSizeRange[1]);
	mlog_printf(ml, "\t" "line width range                                      = [%f, %f]\n", p->lineWidthRange[0], p->lineWidthRange[1]);
	mlog_printf(ml, "\t" "point size granularity                                = %f\n", p->pointSizeGranularity);
	mlog_printf(ml, "\t" "line width granularity                                = %f\n", p->lineWidthGranularity);
	mlog_printf(ml, "\t" "strict lines                                          = %s\n", graph_bool$string(p->strictLines));
	mlog_printf(ml, "\t" "standard sample locations                             = %s\n", graph_bool$string(p->standardSampleLocations));
	mlog_printf(ml, "\t" "optimal buffer copy offset alignment                  = %" PRIu64 "\n", p->optimalBufferCopyOffsetAlignment);
	mlog_printf(ml, "\t" "optimal buffer copy row pitch alignment               = %" PRIu64 "\n", p->optimalBufferCopyRowPitchAlignment);
	mlog_printf(ml, "\t" "non coherent atom size                                = %" PRIu64 "\n", p->nonCoherentAtomSize);
}

void graph_device_properties_sparse_dump(const graph_device_t *restrict gd)
{
	register mlog_s *ml;
	register const VkPhysicalDeviceSparseProperties *restrict p;
	ml = gd->ml;
	p = &gd->properties.sparseProperties;
	mlog_printf(ml, "\t" "residency standard 2D block shape                     = %s\n", graph_bool$string(p->residencyStandard2DBlockShape));
	mlog_printf(ml, "\t" "residency standard 2D multisample block shape         = %s\n", graph_bool$string(p->residencyStandard2DMultisampleBlockShape));
	mlog_printf(ml, "\t" "residency standard 3D block shape                     = %s\n", graph_bool$string(p->residencyStandard3DBlockShape));
	mlog_printf(ml, "\t" "residency aligned mip size                            = %s\n", graph_bool$string(p->residencyAlignedMipSize));
	mlog_printf(ml, "\t" "residency non resident strict                         = %s\n", graph_bool$string(p->residencyNonResidentStrict));
}

void graph_device_properties_dump(const graph_device_t *restrict gd)
{
	graph_device_properties_header_dump(gd);
	graph_device_properties_limits_dump(gd);
	graph_device_properties_sparse_dump(gd);
}

void graph_device_features_dump(const graph_device_t *restrict gd)
{
	register mlog_s *ml;
	register const VkPhysicalDeviceFeatures *restrict p;
	ml = gd->ml;
	p = &gd->features;
	mlog_printf(ml, "\t" "robust buffer access                         = %s\n", graph_bool$string(p->robustBufferAccess));
	mlog_printf(ml, "\t" "full draw index uint32                       = %s\n", graph_bool$string(p->fullDrawIndexUint32));
	mlog_printf(ml, "\t" "image cube array                             = %s\n", graph_bool$string(p->imageCubeArray));
	mlog_printf(ml, "\t" "independent blend                            = %s\n", graph_bool$string(p->independentBlend));
	mlog_printf(ml, "\t" "geometry shader                              = %s\n", graph_bool$string(p->geometryShader));
	mlog_printf(ml, "\t" "tessellation shader                          = %s\n", graph_bool$string(p->tessellationShader));
	mlog_printf(ml, "\t" "sample rate shading                          = %s\n", graph_bool$string(p->sampleRateShading));
	mlog_printf(ml, "\t" "dual src blend                               = %s\n", graph_bool$string(p->dualSrcBlend));
	mlog_printf(ml, "\t" "logic op                                     = %s\n", graph_bool$string(p->logicOp));
	mlog_printf(ml, "\t" "multi draw indirect                          = %s\n", graph_bool$string(p->multiDrawIndirect));
	mlog_printf(ml, "\t" "draw indirect first instance                 = %s\n", graph_bool$string(p->drawIndirectFirstInstance));
	mlog_printf(ml, "\t" "depth clamp                                  = %s\n", graph_bool$string(p->depthClamp));
	mlog_printf(ml, "\t" "depth bias clamp                             = %s\n", graph_bool$string(p->depthBiasClamp));
	mlog_printf(ml, "\t" "fill mode non solid                          = %s\n", graph_bool$string(p->fillModeNonSolid));
	mlog_printf(ml, "\t" "depth bounds                                 = %s\n", graph_bool$string(p->depthBounds));
	mlog_printf(ml, "\t" "wide lines                                   = %s\n", graph_bool$string(p->wideLines));
	mlog_printf(ml, "\t" "large points                                 = %s\n", graph_bool$string(p->largePoints));
	mlog_printf(ml, "\t" "alpha to one                                 = %s\n", graph_bool$string(p->alphaToOne));
	mlog_printf(ml, "\t" "multi viewport                               = %s\n", graph_bool$string(p->multiViewport));
	mlog_printf(ml, "\t" "sampler anisotropy                           = %s\n", graph_bool$string(p->samplerAnisotropy));
	mlog_printf(ml, "\t" "texture compression ETC2                     = %s\n", graph_bool$string(p->textureCompressionETC2));
	mlog_printf(ml, "\t" "texture compression ASTC LDR                 = %s\n", graph_bool$string(p->textureCompressionASTC_LDR));
	mlog_printf(ml, "\t" "texture compression BC                       = %s\n", graph_bool$string(p->textureCompressionBC));
	mlog_printf(ml, "\t" "occlusion query precise                      = %s\n", graph_bool$string(p->occlusionQueryPrecise));
	mlog_printf(ml, "\t" "pipeline statistics query                    = %s\n", graph_bool$string(p->pipelineStatisticsQuery));
	mlog_printf(ml, "\t" "vertex pipeline stores and atomics           = %s\n", graph_bool$string(p->vertexPipelineStoresAndAtomics));
	mlog_printf(ml, "\t" "fragment stores and atomics                  = %s\n", graph_bool$string(p->fragmentStoresAndAtomics));
	mlog_printf(ml, "\t" "shader tessellation and geometry point size  = %s\n", graph_bool$string(p->shaderTessellationAndGeometryPointSize));
	mlog_printf(ml, "\t" "shader image gather extended                 = %s\n", graph_bool$string(p->shaderImageGatherExtended));
	mlog_printf(ml, "\t" "shader storage image extended formats        = %s\n", graph_bool$string(p->shaderStorageImageExtendedFormats));
	mlog_printf(ml, "\t" "shader storage image multisample             = %s\n", graph_bool$string(p->shaderStorageImageMultisample));
	mlog_printf(ml, "\t" "shader storage image read without format     = %s\n", graph_bool$string(p->shaderStorageImageReadWithoutFormat));
	mlog_printf(ml, "\t" "shader storage image write without format    = %s\n", graph_bool$string(p->shaderStorageImageWriteWithoutFormat));
	mlog_printf(ml, "\t" "shader uniform buffer array dynamic indexing = %s\n", graph_bool$string(p->shaderUniformBufferArrayDynamicIndexing));
	mlog_printf(ml, "\t" "shader sampled image array dynamic indexing  = %s\n", graph_bool$string(p->shaderSampledImageArrayDynamicIndexing));
	mlog_printf(ml, "\t" "shader storage buffer array dynamic indexing = %s\n", graph_bool$string(p->shaderStorageBufferArrayDynamicIndexing));
	mlog_printf(ml, "\t" "shader storage image array dynamic indexing  = %s\n", graph_bool$string(p->shaderStorageImageArrayDynamicIndexing));
	mlog_printf(ml, "\t" "shader clip distance                         = %s\n", graph_bool$string(p->shaderClipDistance));
	mlog_printf(ml, "\t" "shader cull distance                         = %s\n", graph_bool$string(p->shaderCullDistance));
	mlog_printf(ml, "\t" "shader float64                               = %s\n", graph_bool$string(p->shaderFloat64));
	mlog_printf(ml, "\t" "shader int64                                 = %s\n", graph_bool$string(p->shaderInt64));
	mlog_printf(ml, "\t" "shader int16                                 = %s\n", graph_bool$string(p->shaderInt16));
	mlog_printf(ml, "\t" "shader resource residency                    = %s\n", graph_bool$string(p->shaderResourceResidency));
	mlog_printf(ml, "\t" "shader resource min lod                      = %s\n", graph_bool$string(p->shaderResourceMinLod));
	mlog_printf(ml, "\t" "sparse binding                               = %s\n", graph_bool$string(p->sparseBinding));
	mlog_printf(ml, "\t" "sparse residency buffer                      = %s\n", graph_bool$string(p->sparseResidencyBuffer));
	mlog_printf(ml, "\t" "sparse residency image 2D                    = %s\n", graph_bool$string(p->sparseResidencyImage2D));
	mlog_printf(ml, "\t" "sparse residency image 3D                    = %s\n", graph_bool$string(p->sparseResidencyImage3D));
	mlog_printf(ml, "\t" "sparse residency 2 samples                   = %s\n", graph_bool$string(p->sparseResidency2Samples));
	mlog_printf(ml, "\t" "sparse residency 4 samples                   = %s\n", graph_bool$string(p->sparseResidency4Samples));
	mlog_printf(ml, "\t" "sparse residency 8 samples                   = %s\n", graph_bool$string(p->sparseResidency8Samples));
	mlog_printf(ml, "\t" "sparse residency 16 samples                  = %s\n", graph_bool$string(p->sparseResidency16Samples));
	mlog_printf(ml, "\t" "sparse residency aliased                     = %s\n", graph_bool$string(p->sparseResidencyAliased));
	mlog_printf(ml, "\t" "variable multi sample rate                   = %s\n", graph_bool$string(p->variableMultisampleRate));
	mlog_printf(ml, "\t" "inherited queries                            = %s\n", graph_bool$string(p->inheritedQueries));
}

void graph_device_dump(const graph_device_t *restrict gd)
{
	mlog_printf(gd->ml, "device[%u] (%s):\n", (uint32_t) gd->index, gd->properties.deviceName);
	graph_device_properties_dump(gd);
	graph_device_features_dump(gd);
}

void graph_device_queue_dump(const graph_device_queue_t *restrict gdq)
{
	char buffer[128];
	register mlog_s *ml;
	register VkQueueFamilyProperties *restrict p;
	ml = gdq->ml;
	p = gdq->properties;
	mlog_printf(ml, "queue[%u]:\n", (uint32_t) gdq->index);
	mlog_printf(ml, "\t" "queue flags                    = 0x%02x (%s)\n",
		p->queueFlags, graph_queue_flags$list(buffer, (graph_queue_flags_t) p->queueFlags));
	mlog_printf(ml, "\t" "queue count                    = %u\n", p->queueCount);
	mlog_printf(ml, "\t" "timestamp valid bits           = 0x%08x\n", p->timestampValidBits);
	mlog_printf(ml, "\t" "min image transfer granularity = [w:%u, h:%u, d:%u]\n", p->timestampValidBits);
}

