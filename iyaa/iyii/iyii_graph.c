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

const graph_device_t* iyii_graph_select_device(const graph_devices_s *restrict devices, const graph_surface_s *restrict surface)
{
	static graph_queue_flags_t queue_need_flags = graph_queue_flags_graphics | graph_queue_flags_transfer;
	graph_device_queues_s *restrict queues;
	const graph_device_t *restrict device, *save;
	const graph_device_queue_t *restrict queue;
	uint32_t i, n, j, m;
	save = NULL;
	n = graph_devices_number(devices);
	for (i = 0; i < n; ++i)
	{
		if ((device = graph_devices_index(devices, i)) && (queues = graph_device_queues_get(device)))
		{
			m = graph_device_queues_number(queues);
			for (j = 0; j < m; ++j)
			{
				if ((queue = graph_device_queues_index(queues, j)) &&
					(graph_device_queue_flags(queue) & queue_need_flags) == queue_need_flags)
				{
					if (graph_device_queue_surface_support(queue, surface) && !save)
					{
						save = device;
					}
				}
			}
			refer_free(queues);
		}
	}
	return save;
}

