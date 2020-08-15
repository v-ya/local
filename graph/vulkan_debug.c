#include "vulkan_debug.h"
#include "allocator_pri.h"

struct graph_vulkan_debug_utils_messenger_s {
	mlog_s *ml;
	graph_allocator_s *ga;
	VkInstance instance;
	VkDebugUtilsMessengerEXT message;
	PFN_vkDestroyDebugUtilsMessengerEXT destroy;
};

static const char* vulkan_debug_get_object_type(VkObjectType type)
{
	switch (type)
	{
		#define _case_return_func(t) case VK_OBJECT_TYPE_##t: return #t
		_case_return_func(INSTANCE);
		_case_return_func(PHYSICAL_DEVICE);
		_case_return_func(DEVICE);
		_case_return_func(QUEUE);
		_case_return_func(SEMAPHORE);
		_case_return_func(COMMAND_BUFFER);
		_case_return_func(FENCE);
		_case_return_func(DEVICE_MEMORY);
		_case_return_func(BUFFER);
		_case_return_func(IMAGE);
		_case_return_func(EVENT);
		_case_return_func(QUERY_POOL);
		_case_return_func(BUFFER_VIEW);
		_case_return_func(IMAGE_VIEW);
		_case_return_func(SHADER_MODULE);
		_case_return_func(PIPELINE_CACHE);
		_case_return_func(PIPELINE_LAYOUT);
		_case_return_func(RENDER_PASS);
		_case_return_func(PIPELINE);
		_case_return_func(DESCRIPTOR_SET_LAYOUT);
		_case_return_func(SAMPLER);
		_case_return_func(DESCRIPTOR_POOL);
		_case_return_func(DESCRIPTOR_SET);
		_case_return_func(FRAMEBUFFER);
		_case_return_func(COMMAND_POOL);

		_case_return_func(SAMPLER_YCBCR_CONVERSION);
		_case_return_func(DESCRIPTOR_UPDATE_TEMPLATE);
		_case_return_func(SURFACE_KHR);
		_case_return_func(SWAPCHAIN_KHR);
		_case_return_func(DISPLAY_KHR);
		_case_return_func(DISPLAY_MODE_KHR);
		_case_return_func(DEBUG_REPORT_CALLBACK_EXT);
		_case_return_func(DEBUG_UTILS_MESSENGER_EXT);
		_case_return_func(VALIDATION_CACHE_EXT);
		_case_return_func(ACCELERATION_STRUCTURE_KHR);
		_case_return_func(PERFORMANCE_CONFIGURATION_INTEL);
		_case_return_func(DEFERRED_OPERATION_KHR);
		_case_return_func(INDIRECT_COMMANDS_LAYOUT_NV);
		_case_return_func(PRIVATE_DATA_SLOT_EXT);
		#undef _case_return_func
		default: return "UNKNOWN";
	}
}

static VkBool32 graph_vulkan_instance_debug_callback_func(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT *data,
	graph_vulkan_debug_utils_messenger_s *restrict r)
{
	char *s_severity, *s_type;
	const VkDebugUtilsLabelEXT *l;
	const VkDebugUtilsObjectNameInfoEXT *o;
	uint32_t i, n;
	switch (severity)
	{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			// 诊断信息
			s_severity = "\e[1;34mverbose\e[0m";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			// 资源创建之类的信息
			s_severity = "\e[1;32minfo\e[0m";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			// 警告信息
			s_severity = "\e[1;33mwarning\e[0m";
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			// 不合法和可能造成崩溃的操作信息
			s_severity = "\e[1;31merror\e[0m";
			break;
		default:
			s_severity = "unknow";
			break;
	}
	switch (type)
	{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
			// 规范和性能无关的事件
			s_type = "\e[1;37mgeneral\e[0m";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
			// 违反规范的情况或发生了一个可能的错误
			s_type = "\e[1;36mvalidation\e[0m";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
			// 可能影响 Vulkan 性能的行为
			s_type = "\e[1;35mperformance\e[0m";
			break;
		default:
			s_type = "unknow";
			break;
	}
	mlog_printf(r->ml, "[%s::%s] \e[1m%d:%s\e[0m: %s\n", s_severity, s_type, data->messageIdNumber, data->pMessageIdName, data->pMessage);
	if ((n = data->queueLabelCount))
	{
		mlog_printf(r->ml, "\t" "QueueLabels: %u\n", n);
		l = data->pQueueLabels;
		for (i = 0; i < n; ++i)
		{
			mlog_printf(r->ml,
				"\t\t" "[%u] name: %s, color: [%f, %f, %f, %f]\n",
				i,
				l->pLabelName,
				l->color[0],
				l->color[1],
				l->color[2],
				l->color[3]
			);
			++l;
		}
	}
	if ((n = data->cmdBufLabelCount))
	{
		mlog_printf(r->ml, "\t" "CmdBufLabels: %u\n", n);
		l = data->pCmdBufLabels;
		for (i = 0; i < n; ++i)
		{
			mlog_printf(r->ml,
				"\t\t" "[%u] name: %s, color: [%f, %f, %f, %f]\n",
				i,
				l->pLabelName,
				l->color[0],
				l->color[1],
				l->color[2],
				l->color[3]
			);
			++l;
		}
	}
	if ((n = data->objectCount))
	{
		mlog_printf(r->ml, "\t" "Objects: %u\n", n);
		o = data->pObjects;
		for (i = 0; i < n; ++i)
		{
			mlog_printf(r->ml,
				"\t\t" "[%u] %#lx, type: %x(%s), name: %s\n",
				i,
				o->objectHandle,
				o->objectType,
				vulkan_debug_get_object_type(o->objectType),
				o->pObjectName
			);
			++o;
		}
	}
	return VK_FALSE;
}

static void graph_value_debug_utils_messenger_free_func(register graph_vulkan_debug_utils_messenger_s *restrict r)
{
	if (r->message && r->destroy) r->destroy(r->instance, r->message, &r->ga->alloc);
	if (r->ga) refer_free(r->ga);
	if (r->ml) refer_free(r->ml);
}

graph_vulkan_debug_utils_messenger_s* graph_value_debug_utils_messenger_alloc(VkInstance instance, mlog_s *ml, graph_debug_message_level_t level, graph_debug_message_type_t type, graph_allocator_s *ga)
{
	graph_vulkan_debug_utils_messenger_s *restrict r;
	PFN_vkCreateDebugUtilsMessengerEXT func;
	func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func)
	{
		r = (graph_vulkan_debug_utils_messenger_s *) refer_alloz(sizeof(graph_vulkan_debug_utils_messenger_s));
		if (r)
		{
			VkDebugUtilsMessengerCreateInfoEXT info;
			VkResult ret;
			refer_set_free(r, (refer_free_f) graph_value_debug_utils_messenger_free_func);
			r->ml = (mlog_s *) refer_save(ml);
			r->ga = (graph_allocator_s *) refer_save(ga);
			r->instance = instance;
			info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			info.pNext = NULL;
			info.flags = 0;
			info.messageSeverity = level;
			info.messageType = type;
			info.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT) graph_vulkan_instance_debug_callback_func;
			info.pUserData = r;
			if (!(ret = func(instance, &info, &ga->alloc, &r->message)))
			{
				r->destroy = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
				if (!r->destroy) mlog_printf(ml, "[graph_value_debug_utils_messenger_alloc] vkGetInstanceProcAddr(\"vkDestroyDebugUtilsMessengerEXT\") fail\n");
				return r;
			}
			else mlog_printf(ml, "[graph_value_debug_utils_messenger_alloc] vkCreateDebugUtilsMessengerEXT = %d\n", ret);
			refer_free(r);
		}
	}
	else mlog_printf(ml, "[graph_value_debug_utils_messenger_alloc] vkGetInstanceProcAddr(\"vkCreateDebugUtilsMessengerEXT\") fail\n");
	return NULL;
}
