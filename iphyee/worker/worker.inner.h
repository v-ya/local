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

typedef struct iphyee_worker_device_queue_t iphyee_worker_device_queue_t;
struct iphyee_worker_device_queue_t {
	uint32_t family_index;
	uint32_t queue_count;
};
struct iphyee_worker_device_s {
	VkDevice device;
	iphyee_worker_device_queue_t queue_compute;
	iphyee_worker_device_queue_t queue_transfer;
	const iphyee_worker_instance_s *instance;
	const iphyee_worker_physical_device_s *physical_device;
	PFN_vkCreateShadersEXT vkCreateShadersEXT;
	PFN_vkDestroyShaderEXT vkDestroyShaderEXT;
	PFN_vkCmdBindShadersEXT vkCmdBindShadersEXT;
	PFN_vkGetShaderBinaryDataEXT vkGetShaderBinaryDataEXT;
	VkPhysicalDeviceMemoryProperties memory_properties;
};

// queue

struct iphyee_worker_queue_s {
	VkQueue queue;
	uint32_t family_index;
	uint32_t queue_index;
	iphyee_worker_device_s *device;
};

// fence

struct iphyee_worker_fence_s {
	VkFence fence;
	VkDevice device;
	iphyee_worker_device_s *depend;
};

// semaphore

struct iphyee_worker_semaphore_s {
	VkSemaphore semaphore;
	VkDevice device;
	iphyee_worker_device_s *depend;
	VkPipelineStageFlags wait_stage_mask;
};

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

// setlayout

typedef struct iphyee_worker_setlayout_binding_t iphyee_worker_setlayout_binding_t;
struct iphyee_worker_setlayout_binding_t {
	uint32_t binding;
	VkDescriptorType desc_type;
};
struct iphyee_worker_setlayout_s {
	VkDescriptorSetLayout setlayout;
	VkDevice device;
	iphyee_worker_device_s *depend;
};
iphyee_worker_setlayout_s* iphyee_worker_setlayout_alloc(iphyee_worker_device_s *restrict device, uintptr_t binding_count, const iphyee_worker_setlayout_binding_t *restrict binding_array);

// desc_pool

typedef struct iphyee_worker_desc_set_buffer_t iphyee_worker_desc_set_buffer_t;
struct iphyee_worker_desc_set_buffer_t {
	uint32_t binding;
	VkDescriptorType desc_type;
	iphyee_worker_buffer_s *buffer;
	uint64_t offset;
	uint64_t length;
};
struct iphyee_worker_desc_pool_s {
	VkDescriptorPool desc_pool;
	VkDevice device;
	iphyee_worker_device_s *depend;
	uintptr_t sets_max_count;
	uintptr_t sets_cur_count;
	iphyee_worker_setlayout_s *setlayout_array[];
};

iphyee_worker_desc_pool_s* iphyee_worker_desc_pool_alloc(iphyee_worker_device_s *restrict device, uintptr_t sets_max_count, uintptr_t size_count, const VkDescriptorPoolSize *restrict size_array);
iphyee_worker_desc_pool_s* iphyee_worker_desc_pool_reset(iphyee_worker_desc_pool_s *restrict r);
iphyee_worker_desc_pool_s* iphyee_worker_desc_pool_fetch(iphyee_worker_desc_pool_s *restrict r, uintptr_t number, iphyee_worker_setlayout_s *const *restrict setlayout_array, VkDescriptorSet *restrict desc_sets_array);
void iphyee_worker_desc_set_write_buffer(iphyee_worker_desc_pool_s *restrict r, VkDescriptorSet desc_set, uintptr_t buffer_count, const iphyee_worker_desc_set_buffer_t *restrict buffer_array);

// shader

struct iphyee_worker_shader_s {
	VkShaderEXT shader;
	VkDevice device;
	iphyee_worker_device_s *depend;
	iphyee_worker_setlayout_s *setlayout;
	uint32_t push_constants_offset;
	uint32_t push_constants_length;
};

// pipelayout

struct iphyee_worker_pipelayout_s {
	VkPipelineLayout pipelayout;
	VkDevice device;
	const iphyee_worker_shader_s *shader;
};

// command_pool

struct iphyee_worker_command_pool_s {
	VkCommandPool command_pool;
	VkQueue queue;
	VkDevice device;
	iphyee_worker_queue_s *depend;
};

// command_buffer

struct iphyee_worker_command_buffer_s {
	VkCommandBuffer command_buffer;
	VkCommandPool command_pool;
	VkQueue queue;
	VkDevice device;
	iphyee_worker_command_pool_s *depend;
};

#endif
