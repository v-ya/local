#include "worker.inner.h"
#include "worker.debug.h"

static void iphyee_worker_instance_free_func(iphyee_worker_instance_s *restrict r)
{
	if (r->physical_device) refer_free(r->physical_device);
	if (r->debug_messenger) refer_free(r->debug_messenger);
	if (r->instance) vkDestroyInstance(r->instance, NULL);
	if (r->application_name) refer_free(r->application_name);
	if (r->engine_name) refer_free(r->engine_name);
}

iphyee_worker_instance_s* iphyee_worker_instance_alloc(const char *restrict application_name, uint32_t app_version, const char *restrict engine_name, uint32_t engine_version, struct mlog_s *restrict mlog, iphyee_worker_debug_t level)
{
	static const char *const debug_layer_array[] = {"VK_LAYER_KHRONOS_validation"};
	static const char *const debug_extension_array[] = {"VK_EXT_debug_utils"};
	iphyee_worker_instance_s *restrict r;
	VkInstanceCreateInfo info;
	VkApplicationInfo app_info;
	VkDebugUtilsMessageSeverityFlagsEXT msf;
	if ((r = (iphyee_worker_instance_s *) refer_alloz(sizeof(iphyee_worker_instance_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_instance_free_func);
		if ((!application_name || (r->application_name = refer_dump_string(application_name))) &&
			(!engine_name || (r->engine_name = refer_dump_string(engine_name))))
		{
			app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			app_info.pNext = NULL;
			app_info.pApplicationName = r->application_name;
			app_info.applicationVersion = app_version;
			app_info.pEngineName = r->engine_name;
			app_info.engineVersion = engine_version;
			app_info.apiVersion = VK_MAKE_VERSION(
				VK_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE),
				VK_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE),
				0);
			info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			info.pNext = 0;
			info.flags = 0;
			info.pApplicationInfo = &app_info;
			if (!mlog) level = iphyee_worker_debug__none;
			if (level == iphyee_worker_debug__none)
			{
				info.enabledLayerCount = 0;
				info.ppEnabledLayerNames = NULL;
				info.enabledExtensionCount = 0;
				info.ppEnabledExtensionNames = NULL;
			}
			else
			{
				info.enabledLayerCount = sizeof(debug_layer_array) / sizeof(const char *);
				info.ppEnabledLayerNames = debug_layer_array;
				info.enabledExtensionCount = sizeof(debug_extension_array) / sizeof(const char *);
				info.ppEnabledExtensionNames = debug_extension_array;
			}
			if (!vkCreateInstance(&info, NULL, &r->instance))
			{
				msf = 0;
				switch (level)
				{
					default:
					case iphyee_worker_debug__version:
						msf |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
						// fall through
					case iphyee_worker_debug__info:
						msf |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
						// fall through
					case iphyee_worker_debug__warning:
						msf |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
						// fall through
					case iphyee_worker_debug__error:
						msf |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
						// fall through
						if (!(r->debug_messenger = iphyee_worker_debug_utils_messenger_alloc(
							r->instance, mlog, msf,
							VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
							VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
							VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
							VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT
						))) goto label_fail;
						break;
					case iphyee_worker_debug__none:
						break;
				}
				return r;
			}
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}

iphyee_worker_instance_s* iphyee_worker_instance_enumerate_device(iphyee_worker_instance_s *restrict r)
{
	VkPhysicalDevice *p;
	uint32_t i, count;
	if (r->physical_device)
	{
		refer_free(r->physical_device);
		r->physical_device = NULL;
	}
	count = 0;
	if (!vkEnumeratePhysicalDevices(r->instance, &count, NULL) &&
		(r->physical_device = iphyee_worker_enumerate_alloc(count, sizeof(VkPhysicalDevice))) &&
		!vkEnumeratePhysicalDevices(r->instance, &count, p = (VkPhysicalDevice *) r->physical_device->edata))
	{
		for (i = 0; i < count; ++i)
			r->physical_device->array[i] = iphyee_worker_physical_device_alloc(p[i]);
		return r;
	}
	return NULL;
}

uintptr_t iphyee_worker_instance_count_device(const iphyee_worker_instance_s *restrict r)
{
	return r->physical_device?r->physical_device->count:0;
}

iphyee_worker_physical_device_s* iphyee_worker_instance_index_device(const iphyee_worker_instance_s *restrict r, uintptr_t index)
{
	if (r->physical_device && index < r->physical_device->count)
		return (iphyee_worker_physical_device_s *) r->physical_device->array[index];
	return NULL;
}

iphyee_worker_device_s* iphyee_worker_instance_create_device(iphyee_worker_instance_s *restrict r, uintptr_t index)
{
	const iphyee_worker_physical_device_s *restrict physical_device;
	if (r->physical_device && index < r->physical_device->count &&
		(physical_device = (const iphyee_worker_physical_device_s *) r->physical_device->array[index]))
		return iphyee_worker_device_alloc(r, physical_device);
	return NULL;
}
