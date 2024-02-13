#include "worker.inner.h"
#include <memory.h>

static const iphyee_worker_physical_device_s* iphyee_worker_device_find_queue_compute(const iphyee_worker_physical_device_s *restrict physical_device, uint32_t *restrict queue_family_index, uint32_t *restrict queue_family_count)
{
	if (iphyee_worker_physical_device_find_queue(physical_device, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT, queue_family_index, queue_family_count) ||
		iphyee_worker_physical_device_find_queue(physical_device, VK_QUEUE_COMPUTE_BIT, 0, queue_family_index, queue_family_count))
		return physical_device;
	return NULL;
}
static const iphyee_worker_physical_device_s* iphyee_worker_device_find_queue_transfer(const iphyee_worker_physical_device_s *restrict physical_device, uint32_t *restrict queue_family_index, uint32_t *restrict queue_family_count)
{
	if (iphyee_worker_physical_device_find_queue(physical_device, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, queue_family_index, queue_family_count) ||
		iphyee_worker_physical_device_find_queue(physical_device, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT, queue_family_index, queue_family_count) ||
		iphyee_worker_physical_device_find_queue(physical_device, VK_QUEUE_TRANSFER_BIT, 0, queue_family_index, queue_family_count))
		return physical_device;
	return NULL;
}
static const iphyee_worker_physical_device_s* iphyee_worker_device_fetch_queue_info(const iphyee_worker_physical_device_s *restrict physical_device, iphyee_worker_device_queue_t *restrict compute, iphyee_worker_device_queue_t *restrict transfer, uint32_t compute_count, uint32_t transfer_count)
{
	memset(compute, 0, sizeof(*compute));
	memset(transfer, 0, sizeof(*transfer));
	if (compute_count)
	{
		iphyee_worker_device_find_queue_compute(physical_device, &compute->family_index, &compute->queue_number);
		iphyee_worker_device_find_queue_transfer(physical_device, &transfer->family_index, &transfer->queue_number);
		if (compute_count <= compute->queue_number)
		{
			compute->queue_number = compute_count;
			if (transfer->family_index == compute->family_index)
				transfer->queue_offset = compute->queue_number;
			if (transfer_count <= transfer->queue_number &&
				transfer->queue_offset + transfer_count <= transfer->queue_number)
			{
				transfer->queue_number = transfer_count;
				return physical_device;
			}
		}
	}
	return NULL;
}

void iphyee_worker_device_get_queue_limit(const iphyee_worker_physical_device_s *restrict physical_device, uint32_t *restrict compute_max_count, uint32_t *restrict transfer_max_count, uint32_t *restrict sum_count_limit)
{
	uint32_t cf, cn, tf, tn;
	cf = cn = tf = tn = 0;
	iphyee_worker_device_find_queue_compute(physical_device, &cf, &cn);
	iphyee_worker_device_find_queue_transfer(physical_device, &tf, &tn);
	if (compute_max_count) *compute_max_count = cn;
	if (transfer_max_count) *transfer_max_count = tn;
	if (sum_count_limit) *sum_count_limit = (cf != tf)?(cf + cn):((cn <= tn)?cn:tn);
}

static void iphyee_worker_device_free_func(iphyee_worker_device_s *restrict r)
{
	if (r->device) vkDestroyDevice(r->device, NULL);
	if (r->physical_device) refer_free(r->physical_device);
	if (r->instance) refer_free(r->instance);
}

iphyee_worker_device_s* iphyee_worker_device_alloc(const iphyee_worker_instance_s *restrict instance, const iphyee_worker_physical_device_s *restrict physical_device, uint32_t compute_queue_count, uint32_t transfer_queue_count)
{
	static const char *const debug_layer_array[] = {"VK_LAYER_KHRONOS_validation"};
	static const char *const debug_extension_array[] = {VK_EXT_SHADER_OBJECT_EXTENSION_NAME};
	iphyee_worker_device_s *restrict r;
	iphyee_worker_device_queue_t q_compute;
	iphyee_worker_device_queue_t q_transfer;
	VkDeviceCreateInfo info;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceBufferDeviceAddressFeatures buffer_device_address;
	VkPhysicalDeviceShaderObjectFeaturesEXT shader_object_features;
	VkDeviceQueueCreateInfo info_queue[2];
	uintptr_t i, n;
	info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	info.pNext = &buffer_device_address;
	info.flags = 0;
	info.queueCreateInfoCount = 0;
	info.pQueueCreateInfos = info_queue;
	info.enabledLayerCount = 0;
	info.ppEnabledLayerNames = NULL;
	info.enabledExtensionCount = sizeof(debug_extension_array) / sizeof(const char *);
	info.ppEnabledExtensionNames = debug_extension_array;
	info.pEnabledFeatures = &features;
	buffer_device_address.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	buffer_device_address.pNext = &shader_object_features;
	buffer_device_address.bufferDeviceAddress = VK_TRUE;
	buffer_device_address.bufferDeviceAddressCaptureReplay = VK_FALSE;
	buffer_device_address.bufferDeviceAddressMultiDevice = VK_FALSE;
	shader_object_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT;
	shader_object_features.pNext = NULL;
	shader_object_features.shaderObject = VK_TRUE;
	memset(&features, 0, sizeof(features));
	if (iphyee_worker_device_fetch_queue_info(physical_device, &q_compute, &q_transfer, compute_queue_count, transfer_queue_count))
	{
		float queue_priorities[n = q_compute.queue_number + q_transfer.queue_number];
		info_queue[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		info_queue[0].pNext = NULL;
		info_queue[0].flags = 0;
		info_queue[0].queueFamilyIndex = q_compute.family_index;
		info_queue[0].queueCount = q_compute.queue_number;
		info_queue[0].pQueuePriorities = queue_priorities;
		if (q_transfer.queue_number && q_transfer.family_index != q_compute.family_index)
		{
			info_queue[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			info_queue[1].pNext = NULL;
			info_queue[1].flags = 0;
			info_queue[1].queueFamilyIndex = q_transfer.family_index;
			info_queue[1].queueCount = q_transfer.queue_number;
			info_queue[1].pQueuePriorities = queue_priorities + q_compute.queue_number;
			info.queueCreateInfoCount = 2;
		}
		else
		{
			info_queue[0].queueCount += q_transfer.queue_number;
			info.queueCreateInfoCount = 1;
		}
		for (i = 0; i < n; ++i)
			queue_priorities[i] = 1.0f;
		if (instance->debug_messenger)
		{
			info.enabledLayerCount = sizeof(debug_layer_array) / sizeof(const char *);
			info.ppEnabledLayerNames = debug_layer_array;
		}
		if ((r = (iphyee_worker_device_s *) refer_alloz(sizeof(iphyee_worker_device_s))))
		{
			refer_set_free(r, (refer_free_f) iphyee_worker_device_free_func);
			r->instance = (const iphyee_worker_instance_s *) refer_save(instance);
			r->physical_device = (const iphyee_worker_physical_device_s *) refer_save(physical_device);
			if (!vkCreateDevice(physical_device->physical_device, &info, NULL, &r->device))
			{
				memcpy(&r->queue_compute, &q_compute, sizeof(q_compute));
				memcpy(&r->queue_transfer, &q_transfer, sizeof(q_compute));
				vkGetPhysicalDeviceMemoryProperties(physical_device->physical_device, &r->memory_properties);
				#define d_get_proc(_func)  r->_func = (PFN_##_func) vkGetDeviceProcAddr(r->device, #_func)
				d_get_proc(vkCreateShadersEXT);
				d_get_proc(vkDestroyShaderEXT);
				d_get_proc(vkCmdBindShadersEXT);
				d_get_proc(vkGetShaderBinaryDataEXT);
				#undef d_get_proc
				return r;
			}
			refer_free(r);
		}
	}
	return NULL;
}

iphyee_worker_device_s* iphyee_worker_device_wait_idle(iphyee_worker_device_s *restrict r)
{
	if (!vkDeviceWaitIdle(r->device))
		return r;
	return NULL;
}
