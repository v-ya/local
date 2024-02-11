#ifndef _iphyee_worker_inner_h_
#define _iphyee_worker_inner_h_

#include "worker.h"
#include <vulkan/vulkan.h>

// enumerate

struct iphyee_worker_enumerate_s {
	uintptr_t count;
	uintptr_t bsize;
	refer_t *array;
	void *edata;
};
iphyee_worker_enumerate_s* iphyee_worker_enumerate_alloc(uintptr_t count, uintptr_t bsize);

// worker

struct iphyee_worker_s {
	;
};

// instance

struct iphyee_worker_instance_s {
	VkInstance instance;
	refer_string_t application_name;
	refer_string_t engine_name;
	refer_t debug_messenger;
	iphyee_worker_enumerate_s *physical_device;
};

// physical_device

struct iphyee_worker_physical_device_s {
	VkPhysicalDevice physical_device;
	uintptr_t queue_famliy_count;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	VkQueueFamilyProperties queue_famliy_array[];
};
iphyee_worker_physical_device_s* iphyee_worker_physical_device_alloc(VkPhysicalDevice physical_device);
const iphyee_worker_physical_device_s* iphyee_worker_physical_device_find_queue(const iphyee_worker_physical_device_s *restrict r, VkQueueFlagBits exist, VkQueueFlagBits discard, uint32_t *restrict queue_family_index, uint32_t *restrict queue_family_count);

// device

struct iphyee_worker_device_s {
	VkDevice device;
	VkQueue queue_compute;
	VkQueue queue_transfer;
	const iphyee_worker_instance_s *instance;
	const iphyee_worker_physical_device_s *physical_device;
	VkPhysicalDeviceMemoryProperties memory_properties;
};
iphyee_worker_device_s* iphyee_worker_device_alloc(const iphyee_worker_instance_s *restrict instance, const iphyee_worker_physical_device_s *restrict physical_device);

// memory_heap

struct iphyee_worker_memory_heap_s {
	VkDevice device;
	uint32_t memory_type_index;
	uint32_t memory_heap_index;
	iphyee_worker_device_s *depend;
};
iphyee_worker_memory_heap_s* iphyee_worker_memory_heap_alloc(iphyee_worker_device_s *restrict device, VkMemoryPropertyFlags flags);

// buffer

struct iphyee_worker_buffer_s {
	VkBuffer buffer;
	VkDevice device;
	VkDeviceMemory memory;
	uint64_t size;
	iphyee_worker_memory_heap_s *memory_heap;
	VkMemoryRequirements require;
};
iphyee_worker_buffer_s* iphyee_worker_buffer_alloc(iphyee_worker_memory_heap_s *restrict memory_heap, uint64_t size, VkBufferCreateFlags flags, VkBufferUsageFlags usage);

#endif
