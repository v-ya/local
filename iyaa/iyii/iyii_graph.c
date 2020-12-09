#include "iyii_graph.h"

graph_s* iyii_graph_create_graph(mlog_s *restrict mlog, const char *restrict app_name, const char *restrict engine_name, uint32_t enable_validation)
{
	static graph_layer_t g_layer_array[] = {
		graph_layer_validation,
		0
	};
	static graph_extension_t g_extension_array[] = {
		graph_extension_ext_debug_utils,
		graph_extension_khr_surface,
		graph_extension_khr_xcb_surface,
		0
	};
	graph_s *restrict r;
	r = graph_alloc(mlog, NULL);
	if (r)
	{
		enable_validation = !enable_validation;
		if (graph_init_instance(r, app_name, engine_name, g_layer_array + enable_validation, g_extension_array + enable_validation))
		{
			if (enable_validation || graph_instance_enable_debug_message(r,
				// graph_debug_message_level_version |
				graph_debug_message_level_info |
				graph_debug_message_level_warning |
				graph_debug_message_level_error,
				graph_debug_message_type_general |
				graph_debug_message_type_validation |
				graph_debug_message_type_performance
			)) return r;
		}
		refer_free(r);
	}
	return NULL;
}

graph_device_t* iyii_graph_select_device(const graph_devices_s *devices)
{
	uint32_t i, n;
	n = graph_devices_number(devices);
	for (i = 0; i < n; ++i)
	{
		graph_device_dump(graph_devices_index(devices, i));
		// graph_device_queue_surface_support;
	}
	return NULL;
}

