#include "worker.inner.h"

iphyee_worker_physical_device_s* iphyee_worker_physical_device_alloc(VkPhysicalDevice physical_device)
{
	iphyee_worker_physical_device_s *restrict r;
	uintptr_t size;
	uint32_t count;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, NULL);
	size = sizeof(iphyee_worker_physical_device_s) + sizeof(VkQueueFamilyProperties) * count;
	if (physical_device && (r = (iphyee_worker_physical_device_s *) refer_alloc(size)))
	{
		r->physical_device = physical_device;
		r->queue_famliy_count = count;
		vkGetPhysicalDeviceProperties(physical_device, &r->properties);
		vkGetPhysicalDeviceFeatures(physical_device, &r->features);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, r->queue_famliy_array);
		r->queue_famliy_count = count;
		return r;
	}
	return NULL;
}

const iphyee_worker_physical_device_s* iphyee_worker_physical_device_find_queue(const iphyee_worker_physical_device_s *restrict r, VkQueueFlagBits exist, VkQueueFlagBits discard, uint32_t *restrict queue_family_index, uint32_t *restrict queue_family_count)
{
	const VkQueueFamilyProperties *restrict p;
	uintptr_t i, n;
	p = r->queue_famliy_array;
	n = r->queue_famliy_count;
	for (i = 0; i < n; ++i)
	{
		if ((p[i].queueFlags & exist) == exist && (p[i].queueFlags & discard) == 0)
		{
			if (queue_family_index) *queue_family_index = (uint32_t) i;
			if (queue_family_count) *queue_family_count = p[i].queueCount;
			return r;
		}
	}
	return NULL;
}
