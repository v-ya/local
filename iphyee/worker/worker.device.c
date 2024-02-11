#include "worker.inner.h"
#include <memory.h>

static uint32_t* iphyee_worker_device_find_queue_compute(uint32_t *restrict queue_family_index, const iphyee_worker_physical_device_s *restrict physical_device)
{
	if (iphyee_worker_physical_device_find_queue(physical_device, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT, queue_family_index, NULL) ||
		iphyee_worker_physical_device_find_queue(physical_device, VK_QUEUE_COMPUTE_BIT, 0, queue_family_index, NULL))
		return queue_family_index;
	return NULL;
}
static uint32_t* iphyee_worker_device_find_queue_transfer(uint32_t *restrict queue_family_index, const iphyee_worker_physical_device_s *restrict physical_device)
{
	if (iphyee_worker_physical_device_find_queue(physical_device, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, queue_family_index, NULL) ||
		iphyee_worker_physical_device_find_queue(physical_device, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT, queue_family_index, NULL) ||
		iphyee_worker_physical_device_find_queue(physical_device, VK_QUEUE_TRANSFER_BIT, 0, queue_family_index, NULL))
		return queue_family_index;
	return NULL;
}

static void iphyee_worker_device_free_func(iphyee_worker_device_s *restrict r)
{
	if (r->device) vkDestroyDevice(r->device, NULL);
	if (r->physical_device) refer_free(r->physical_device);
	if (r->instance) refer_free(r->instance);
}

iphyee_worker_device_s* iphyee_worker_device_alloc(const iphyee_worker_instance_s *restrict instance, const iphyee_worker_physical_device_s *restrict physical_device)
{
	static const char *const debug_layer_array[] = {"VK_LAYER_KHRONOS_validation"};
	// static const char *const debug_extension_array[] = {"VK_EXT_debug_utils"};
	iphyee_worker_device_s *restrict r;
	VkDeviceCreateInfo info;
	VkPhysicalDeviceFeatures features;
	VkDeviceQueueCreateInfo info_queue[2];
	float queue_priorities[1];
	memset(&features, 0, sizeof(features));
	info_queue[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	info_queue[0].pNext = NULL;
	info_queue[0].flags = 0;
	info_queue[0].queueFamilyIndex = 0;
	info_queue[0].queueCount = 1;
	info_queue[0].pQueuePriorities = queue_priorities;
	info_queue[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	info_queue[1].pNext = NULL;
	info_queue[1].flags = 0;
	info_queue[1].queueFamilyIndex = 0;
	info_queue[1].queueCount = 1;
	info_queue[1].pQueuePriorities = queue_priorities;
	info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	info.pNext = NULL;
	info.flags = 0;
	info.queueCreateInfoCount = sizeof(info_queue) / sizeof(VkDeviceQueueCreateInfo);
	info.pQueueCreateInfos = info_queue;
	info.enabledLayerCount = 0;
	info.ppEnabledLayerNames = NULL;
	info.enabledExtensionCount = 0;
	info.ppEnabledExtensionNames = NULL;
	info.pEnabledFeatures = &features;
	if (instance->debug_messenger)
	{
		info.enabledLayerCount = sizeof(debug_layer_array) / sizeof(const char *);
		info.ppEnabledLayerNames = debug_layer_array;
	}
	if (iphyee_worker_device_find_queue_compute(&info_queue[0].queueFamilyIndex, physical_device) &&
		iphyee_worker_device_find_queue_transfer(&info_queue[1].queueFamilyIndex, physical_device) &&
		(r = (iphyee_worker_device_s *) refer_alloz(sizeof(iphyee_worker_device_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_device_free_func);
		r->instance = (const iphyee_worker_instance_s *) refer_save(instance);
		r->physical_device = (const iphyee_worker_physical_device_s *) refer_save(physical_device);
		if (!vkCreateDevice(physical_device->physical_device, &info, NULL, &r->device))
		{
			vkGetDeviceQueue(r->device, info_queue[0].queueFamilyIndex, 0, &r->queue_compute);
			vkGetDeviceQueue(r->device, info_queue[1].queueFamilyIndex, 0, &r->queue_transfer);
			vkGetPhysicalDeviceMemoryProperties(physical_device->physical_device, &r->memory_properties);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

iphyee_worker_device_s* iphyee_worker_device_wait_idle(iphyee_worker_device_s *restrict r)
{
	if (!vkDeviceWaitIdle(r->device))
		return r;
	return NULL;
}
