#include "graph_pri.h"
#include "support_pri.h"
#include <alloca.h>

static void graph_free_func(graph_s *restrict r)
{
	void *v;
	VkAllocationCallbacks *ga;
	ga = &r->ga->alloc;
	if ((v = r->ml)) refer_free(v);
	if ((v = r->debug)) refer_free(v);
	if ((v = r->instance)) vkDestroyInstance((VkInstance) v, ga);
	if ((v = r->ga)) refer_free(v);
}

graph_s* graph_alloc(mlog_s *ml, struct graph_allocator_s *ga)
{
	graph_s *restrict r;
	r = (graph_s *) refer_alloz(sizeof(graph_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_free_func);
		r->ml = (mlog_s *) refer_save(ml);
		r->ga = (graph_allocator_s *) refer_save(ga);
	}
	return r;
}

graph_s* graph_init_instance(graph_s *restrict g, const char *restrict app_name, const char *restrict engine_name, const graph_layer_t *restrict layer, const graph_extension_t *restrict extension)
{
	VkInstanceCreateInfo info;
	VkApplicationInfo app_info;
	uint32_t layer_number;
	uint32_t extension_number;
	const char **layer_list;
	const char **extension_list;
	VkResult r;
	if (g->instance) goto label_exist;
	layer_number = extension_number = 0;
	layer_list = extension_list = NULL;
	// build layer
	if (layer)
	{
		if (graph_layer_mapping_length(layer, &layer_number))
			goto label_layer_error;
		if (layer_number)
		{
			layer_list = (const char **) alloca(sizeof(char *) * layer_number);
			if (!layer_list) goto label_alloca;
			graph_layer_mapping_copy(layer, layer_list);
		}
	}
	// build extension
	if (extension)
	{
		if (graph_extension_mapping_length(extension, &extension_number))
			goto label_extension_error;
		if (extension_number)
		{
			extension_list = (const char **) alloca(sizeof(char *) * extension_number);
			if (!extension_list) goto label_alloca;
			graph_extension_mapping_copy(extension, extension_list);
		}
	}
	// build info
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = NULL;
	app_info.pApplicationName = app_name;
	app_info.applicationVersion = 0;
	app_info.pEngineName = engine_name;
	app_info.engineVersion = 0;
	app_info.apiVersion = VK_MAKE_VERSION(
		VK_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE),
		VK_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE),
		0);
	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.pNext = 0;
	info.flags = 0;
	info.pApplicationInfo = &app_info;
	info.enabledLayerCount = layer_number;
	info.ppEnabledLayerNames = layer_list;
	info.enabledExtensionCount = extension_number;
	info.ppEnabledExtensionNames = extension_list;
	r = vkCreateInstance(&info, &g->ga->alloc, &g->instance);
	if (!r) return g;
	mlog_printf(g->ml, "[graph_init_instance] vkCreateInstance = %d\n", r);
	label_return_null:
	return NULL;
	label_exist:
	mlog_printf(g->ml, "[graph_init_instance] already exist\n");
	goto label_return_null;
	label_alloca:
	mlog_printf(g->ml, "[graph_init_instance] alloca fail\n");
	goto label_return_null;
	label_layer_error:
	mlog_printf(g->ml, "[graph_init_instance] layer[%u](%d) not find\n", layer_number, layer[layer_number]);
	goto label_return_null;
	label_extension_error:
	mlog_printf(g->ml, "[graph_init_instance] extension[%u](%d) not find\n", extension_number, extension[extension_number]);
	goto label_return_null;
}

graph_s* graph_instance_enable_debug_message(graph_s *restrict g, graph_debug_message_level_t level, graph_debug_message_type_t type)
{
	if (g->debug) goto label_exist;
	if (!g->instance) goto label_null;
	g->debug = graph_value_debug_utils_messenger_alloc(g->instance, g->ml, level, type, g->ga);
	if (g->debug) return g;
	mlog_printf(g->ml, "[graph_instance_enable_debug_message] enable debug message fail\n");
	label_return_null:
	return NULL;
	label_exist:
	mlog_printf(g->ml, "[graph_instance_enable_debug_message] already exist\n");
	goto label_return_null;
	label_null:
	mlog_printf(g->ml, "[graph_instance_enable_debug_message] instance = null\n");
	goto label_return_null;
}
