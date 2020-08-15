#ifndef _graph_device_pri_h_
#define _graph_device_pri_h_

#include "device.h"
#include <vulkan/vulkan.h>

struct graph_device_t {
	VkPhysicalDevice phydev;
	size_t index;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
};

struct graph_devices_s {
	size_t number;
	graph_device_t device_array[];
};

#endif
