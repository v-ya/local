#ifndef _graph_device_pri_h_
#define _graph_device_pri_h_

#include "device.h"
#include <vulkan/vulkan.h>
#include "allocator_pri.h"

struct graph_device_t {
	mlog_s *ml;
	VkPhysicalDevice phydev;
	size_t index;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
};

struct graph_devices_s {
	mlog_s *ml;
	size_t number;
	graph_device_t device_array[];
};

struct graph_device_queue_t {
	mlog_s *ml;
	size_t index;
	VkQueueFamilyProperties *properties;
};

struct graph_device_queues_s {
	mlog_s *ml;
	size_t number;
	graph_device_queue_t queue_array[];
};

#define graph_dev_param_priorities_preset_number  16

struct graph_dev_param_s {
	uint32_t qf_number;
	uint32_t p_over;
	uint32_t layer_number;
	uint32_t extension_number;
	const char **layer_list;
	const char **extension_list;
	VkPhysicalDeviceFeatures features;
	float priorities[graph_dev_param_priorities_preset_number];
	VkDeviceQueueCreateInfo queue_info[];
};

struct graph_dev_s {
	mlog_s *ml;
	VkDevice dev;
	graph_allocator_s *ga;
};

#endif
