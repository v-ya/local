#include "graph_pri.h"
#include <hashmap.h>
#include <stdlib.h>
#include <alloca.h>

static const char* graph_layer_mapping[graph_layer_number] = {
	[graph_layer_null] = NULL,
	[graph_layer_validation] = "VK_LAYER_KHRONOS_validation",
	[graph_layer_api_dump] = "VK_LAYER_LUNARG_api_dump",
};

static const char* graph_extension_mapping[graph_extension_number] = {
	[graph_extension_null] = NULL,
	[graph_extension_ext_debug_utils] = "VK_EXT_debug_utils",
};

typedef struct graph_support_s {
	hashmap_t support;
	void *pdata;
	uint32_t *type;
} graph_support_s;

struct graph_layer_support_s {
	graph_support_s layer;
};

struct graph_extension_support_s {
	graph_support_s extension;
};

static void graph_support_free_func(register graph_support_s *restrict r)
{
	hashmap_uini(&r->support);
	if (r->pdata) free(r->pdata);
	if (r->type) free(r->type);
}

graph_layer_support_s* graph_layer_support_get(void)
{
	register graph_support_s *restrict r;
	r = (graph_support_s *) refer_alloc(sizeof(graph_support_s));
	if (r)
	{
		VkLayerProperties *p;
		uint32_t *type;
		uint32_t number;
		r->pdata = NULL;
		r->type = NULL;
		if (!hashmap_init(&r->support))
			goto Err;
		refer_set_free(r, (refer_free_f) graph_support_free_func);
		if (vkEnumerateInstanceLayerProperties(&number, NULL))
			goto Err;
		if (number)
		{
			r->pdata = p = (VkLayerProperties *) malloc(sizeof(VkLayerProperties) * number);
			r->type = type = (uint32_t *) calloc(number, sizeof(uint32_t));
			if (!p || !type) goto Err;
			if (vkEnumerateInstanceLayerProperties(&number, p))
				goto Err;
			do {
				if (!hashmap_set_name(&r->support, p->layerName, p, NULL))
					goto Err;
				++p;
			} while (--number);
			number = graph_layer_number;
			while (--number)
			{
				if ((p = (VkLayerProperties *) hashmap_get_name(&r->support, graph_layer_mapping[number])))
					type[(uintptr_t)(p - (VkLayerProperties *) r->pdata)] = number;
			}
		}
		return (graph_layer_support_s *) r;
		Err:
		refer_free(r);
	}
	return NULL;
}

const graph_layer_support_s* graph_layer_support_test(const graph_layer_support_s *restrict layer_support, graph_layer_t layer, const char *restrict *restrict name, const char *restrict *restrict description, uint32_t *restrict spec_version, uint32_t *restrict impl_version)
{
	register VkLayerProperties *restrict p;
	if (layer && (uint32_t) layer < graph_layer_number)
	{
		p = (VkLayerProperties *) hashmap_get_name(&layer_support->layer.support, graph_layer_mapping[layer]);
		if (!p) goto label_miss;
		if (name) *name = p->layerName;
		if (description) *description = p->description;
		if (spec_version) *spec_version = p->specVersion;
		if (impl_version) *impl_version = p->implementationVersion;
		return layer_support;
	}
	else
	{
		label_miss:
		if (name) *name = NULL;
		if (description) *description = NULL;
		if (spec_version) *spec_version = 0;
		if (impl_version) *impl_version = 0;
		return NULL;
	}
}

const graph_layer_t* graph_layer_support_all(const graph_layer_support_s *restrict layer_support, uint32_t *restrict number)
{
	*number = layer_support->layer.support.number;
	return (graph_layer_t *) layer_support->layer.type;
}

void graph_layer_support_dump(const graph_layer_support_s *restrict layer_support, mlog_s *restrict ml)
{
	register VkLayerProperties *p;
	register graph_layer_t *t;
	register uint32_t n;
	n = layer_support->layer.support.number;
	if (n)
	{
		p = (VkLayerProperties *) layer_support->layer.pdata;
		t = (graph_layer_t *) layer_support->layer.type;
		do {
			mlog_printf(ml,
				"[%2d] %s\n"
				"\t" "%s\n"
				"\t" "spec[%u.%u.%u], impl[%u.%u.%u]\n",
				*t++, p->layerName, p->description,
				VK_VERSION_MAJOR(p->specVersion),
				VK_VERSION_MINOR(p->specVersion),
				VK_VERSION_PATCH(p->specVersion),
				VK_VERSION_MAJOR(p->implementationVersion),
				VK_VERSION_MINOR(p->implementationVersion),
				VK_VERSION_PATCH(p->implementationVersion)
			);
			++p;
		} while (--n);
	}
}

graph_extension_support_s* graph_extension_support_get(graph_layer_t layer)
{
	register graph_support_s *restrict r;
	if ((uint32_t) layer < graph_layer_number)
	{
		r = (graph_support_s *) refer_alloc(sizeof(graph_support_s));
		if (r)
		{
			const char *layer_name;
			VkExtensionProperties *p;
			uint32_t *type;
			uint32_t number;
			r->pdata = NULL;
			r->type = NULL;
			if (!hashmap_init(&r->support))
				goto Err;
			refer_set_free(r, (refer_free_f) graph_support_free_func);
			layer_name = graph_layer_mapping[layer];
			if (vkEnumerateInstanceExtensionProperties(layer_name, &number, NULL))
				goto Err;
			if (number)
			{
				r->pdata = p = (VkExtensionProperties *) malloc(sizeof(VkExtensionProperties) * number);
				r->type = type = (uint32_t *) calloc(number, sizeof(uint32_t));
				if (!p || !type) goto Err;
				if (vkEnumerateInstanceExtensionProperties(layer_name, &number, p))
					goto Err;
				do {
					if (!hashmap_set_name(&r->support, p->extensionName, p, NULL))
						goto Err;
					++p;
				} while (--number);
				number = graph_extension_number;
				while (--number)
				{
					if ((p = (VkExtensionProperties *) hashmap_get_name(&r->support, graph_extension_mapping[number])))
						type[(uintptr_t)(p - (VkExtensionProperties *) r->pdata)] = number;
				}
			}
			return (graph_extension_support_s *) r;
			Err:
			refer_free(r);
		}
	}
	return NULL;
}

const graph_extension_support_s* graph_extension_support_test(const graph_extension_support_s *restrict extension_support, graph_extension_t extension, const char *restrict *restrict name, uint32_t *restrict spec_version)
{
	register VkExtensionProperties *restrict p;
	if (extension && (uint32_t) extension < graph_layer_number)
	{
		p = (VkExtensionProperties *) hashmap_get_name(&extension_support->extension.support, graph_extension_mapping[extension]);
		if (!p) goto label_miss;
		if (name) *name = p->extensionName;
		if (spec_version) *spec_version = p->specVersion;
		return extension_support;
	}
	else
	{
		label_miss:
		if (name) *name = NULL;
		if (spec_version) *spec_version = 0;
		return NULL;
	}
}

const graph_extension_t* graph_extension_support_all(const graph_extension_support_s *restrict extension_support, uint32_t *restrict number)
{
	*number = extension_support->extension.support.number;
	return (graph_extension_t *) extension_support->extension.type;
}

void graph_extension_support_dump(const graph_extension_support_s *restrict extension_support, mlog_s *restrict ml)
{
	register VkExtensionProperties *p;
	register graph_extension_t *t;
	register uint32_t n;
	n = extension_support->extension.support.number;
	if (n)
	{
		p = (VkExtensionProperties *) extension_support->extension.pdata;
		t = (graph_extension_t *) extension_support->extension.type;
		do {
			mlog_printf(ml,
				"[%2d] %s, spec[%u.%u.%u]\n",
				*t++, p->extensionName,
				VK_VERSION_MAJOR(p->specVersion),
				VK_VERSION_MINOR(p->specVersion),
				VK_VERSION_PATCH(p->specVersion)
			);
			++p;
		} while (--n);
	}
}

static void graph_free_func(register graph_s *restrict r)
{
	register void *v;
	register VkAllocationCallbacks *ga;
	ga = &r->ga->alloc;
	if ((v = r->mlog)) refer_free(v);
	if ((v = r->debug)) refer_free(v);
	if ((v = r->instance)) vkDestroyInstance((VkInstance) v, ga);
	if ((v = r->ga)) refer_free(v);
}

graph_s* graph_alloc(mlog_s *ml, struct graph_allocator_s *ga)
{
	register graph_s *restrict r;
	r = (graph_s *) refer_alloz(sizeof(graph_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_free_func);
		r->mlog = (mlog_s *) refer_save(ml);
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
	uint32_t i;
	VkResult r;
	if (g->instance) goto label_exist;
	layer_number = extension_number = 0;
	layer_list = extension_list = NULL;
	// build layer
	if (layer)
	{
		i = 0;
		while (layer[i])
		{
			if ((uint32_t) layer[i] >= graph_layer_number)
				goto label_layer_error;
			++i;
		}
		if ((layer_number = i))
		{
			layer_list = (const char **) alloca(sizeof(char *) * layer_number);
			if (!layer_list) goto label_alloca;
			do {
				--i;
				layer_list[i] = graph_layer_mapping[layer[i]];
			} while (i);
		}
	}
	// build extension
	if (extension)
	{
		i = 0;
		while (extension[i])
		{
			if ((uint32_t) extension[i] >= graph_extension_number)
				goto label_extension_error;
			++i;
		}
		if ((extension_number = i))
		{
			extension_list = (const char **) alloca(sizeof(char *) * extension_number);
			if (!extension_list) goto label_alloca;
			do {
				--i;
				extension_list[i] = graph_extension_mapping[extension[i]];
			} while (i);
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
	mlog_printf(g->mlog, "[graph_init_instance] vkCreateInstance = %d\n", r);
	label_return_null:
	return NULL;
	label_exist:
	mlog_printf(g->mlog, "[graph_init_instance] already exist\n");
	goto label_return_null;
	label_alloca:
	mlog_printf(g->mlog, "[graph_init_instance] alloca fail\n");
	goto label_return_null;
	label_layer_error:
	mlog_printf(g->mlog, "[graph_init_instance] layer(%d) not find\n", layer[i]);
	goto label_return_null;
	label_extension_error:
	mlog_printf(g->mlog, "[graph_init_instance] extension(%d) not find\n", extension[i]);
	goto label_return_null;
}

graph_s* graph_instance_enable_debug_message(graph_s *restrict g, graph_debug_message_level_t level, graph_debug_message_type_t type)
{
	if (g->debug) goto label_exist;
	if (!g->instance) goto label_null;
	g->debug = graph_value_debug_utils_messenger_alloc(g->instance, g->mlog, level, type, g->ga);
	if (g->debug) return g;
	mlog_printf(g->mlog, "[graph_instance_enable_debug_message] enable debug message fail\n");
	label_return_null:
	return NULL;
	label_exist:
	mlog_printf(g->mlog, "[graph_instance_enable_debug_message] already exist\n");
	goto label_return_null;
	label_null:
	mlog_printf(g->mlog, "[graph_instance_enable_debug_message] instance = null\n");
	goto label_return_null;
}
