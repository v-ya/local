#include "support_pri.h"
#include "device_pri.h"
#include <hashmap.h>
#include <stdlib.h>

static const char* graph_layer_mapping[graph_layer$number] = {
	[graph_layer_null] = NULL,
	[graph_layer_validation] = "VK_LAYER_KHRONOS_validation",
	[graph_layer_api_dump] = "VK_LAYER_LUNARG_api_dump",
};

static const char* graph_extension_mapping[graph_extension$number] = {
	[graph_extension_null] = NULL,
	[graph_extension_ext_debug_utils] = "VK_EXT_debug_utils",
	[graph_extension_khr_swapchain] = "VK_KHR_swapchain",
	[graph_extension_khr_surface] = "VK_KHR_surface",
	[graph_extension_khr_xcb_surface] = "VK_KHR_xcb_surface",
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

static inline const uint32_t* mapping_length(const uint32_t *restrict p, uint32_t *restrict length, uint32_t n)
{
	uint32_t i;
	i = 0;
	while (*p)
	{
		if (*p >= n) goto Err;
		++i;
		++p;
	}
	p = NULL;
	Err:
	*length = i;
	return p;
}

static inline void mapping_tr(const uint32_t *restrict p, const char **restrict d, const char* mapping[])
{
	while (*p) *d++ = mapping[*p++];
}

const graph_layer_t* graph_layer_mapping_length(const graph_layer_t *restrict p, uint32_t *restrict length)
{
	return (const graph_layer_t *) mapping_length((const uint32_t *restrict) p, length, graph_layer$number);
}

const graph_extension_t* graph_extension_mapping_length(const graph_extension_t *restrict p, uint32_t *restrict length)
{
	return (const graph_extension_t *) mapping_length((const uint32_t *restrict) p, length, graph_extension$number);
}

const char** graph_layer_mapping_copy(const graph_layer_t *restrict p, const char* d[])
{
	mapping_tr((const uint32_t *restrict) p, d, graph_layer_mapping);
	return d;
}

const char** graph_extension_mapping_copy(const graph_extension_t *restrict p, const char* d[])
{
	mapping_tr((const uint32_t *restrict) p, d, graph_extension_mapping);
	return d;
}

static void graph_support_free_func(graph_support_s *restrict r)
{
	hashmap_uini(&r->support);
	if (r->pdata) free(r->pdata);
	if (r->type) free(r->type);
}

static graph_support_s* graph_support_alloc(uint32_t number, size_t size)
{
	graph_support_s *restrict r;
	if (number)
	{
		r = (graph_support_s *) refer_alloc(sizeof(graph_support_s));
		if (r)
		{
			r->pdata = NULL;
			r->type = NULL;
			if (!hashmap_init(&r->support))
				goto Err;
			refer_set_free(r, (refer_free_f) graph_support_free_func);
			r->pdata = malloc(size * number);
			r->type = (uint32_t *) calloc(number, size);
			if (r->pdata && r->type)
				return r;
			Err:
			refer_free(r);
		}
	}
	return NULL;
}

static graph_support_s* graph_support_set(graph_support_s *restrict r, uint32_t number, size_t size, uintptr_t offset, uint32_t mn, const char* mapping[])
{
	uint8_t *restrict p;
	uint32_t *restrict t;
	p = (uint8_t *) r->pdata;
	t = r->type;
	do {
		if (!hashmap_set_name(&r->support, (char *) (p + offset), p, NULL))
			goto Err;
		p += size;
	} while (--number);
	number = mn;
	while (--number)
	{
		if ((p = (uint8_t *) hashmap_get_name(&r->support, mapping[number])))
			t[(uintptr_t) (p - (uint8_t *) r->pdata) / size] = number;
	}
	return r;
	Err:
	return NULL;
}

graph_layer_support_s* graph_layer_support_get_instance(void)
{
	graph_support_s *restrict r;
	uint32_t number;
	if (!vkEnumerateInstanceLayerProperties(&number, NULL) && number)
	{
		r = graph_support_alloc(number, sizeof(VkLayerProperties));
		if (r)
		{
			if (!vkEnumerateInstanceLayerProperties(&number, (VkLayerProperties *) r->pdata))
			{
				if (graph_support_set(r, number,
					sizeof(VkLayerProperties),
					offsetof(VkLayerProperties, layerName),
					graph_layer$number,
					graph_layer_mapping
				)) return (graph_layer_support_s *) r;
			}
			refer_free(r);
		}
	}
	return NULL;
}

graph_layer_support_s* graph_layer_support_get_dev(const struct graph_device_t *restrict device)
{
	graph_support_s *restrict r;
	uint32_t number;
	if (!vkEnumerateDeviceLayerProperties(device->phydev, &number, NULL) && number)
	{
		r = graph_support_alloc(number, sizeof(VkLayerProperties));
		if (r)
		{
			if (!vkEnumerateDeviceLayerProperties(device->phydev, &number, (VkLayerProperties *) r->pdata))
			{
				if (graph_support_set(r, number,
					sizeof(VkLayerProperties),
					offsetof(VkLayerProperties, layerName),
					graph_layer$number,
					graph_layer_mapping
				)) return (graph_layer_support_s *) r;
			}
			refer_free(r);
		}
	}
	return NULL;
}

graph_extension_support_s* graph_extension_support_get_instance(graph_layer_t layer)
{
	if ((uint32_t) layer < graph_layer$number)
	{
		graph_support_s *restrict r;
		const char *layer_name;
		uint32_t number;
		layer_name = graph_layer_mapping[layer];
		if (!vkEnumerateInstanceExtensionProperties(layer_name, &number, NULL) && number)
		{
			r = graph_support_alloc(number, sizeof(VkExtensionProperties));
			if (r)
			{
				if (!vkEnumerateInstanceExtensionProperties(layer_name, &number, (VkExtensionProperties *) r->pdata))
				{
					if (graph_support_set(r, number,
						sizeof(VkExtensionProperties),
						offsetof(VkExtensionProperties, extensionName),
						graph_extension$number,
						graph_extension_mapping
					)) return (graph_extension_support_s *) r;
				}
				refer_free(r);
			}
		}
	}
	return NULL;
}

graph_extension_support_s* graph_extension_support_get_dev(const struct graph_device_t *restrict device, graph_layer_t layer)
{
	if ((uint32_t) layer < graph_layer$number)
	{
		graph_support_s *restrict r;
		const char *layer_name;
		uint32_t number;
		layer_name = graph_layer_mapping[layer];
		if (!vkEnumerateDeviceExtensionProperties(device->phydev, layer_name, &number, NULL) && number)
		{
			r = graph_support_alloc(number, sizeof(VkExtensionProperties));
			if (r)
			{
				if (!vkEnumerateDeviceExtensionProperties(device->phydev, layer_name, &number, (VkExtensionProperties *) r->pdata))
				{
					if (graph_support_set(r, number,
						sizeof(VkExtensionProperties),
						offsetof(VkExtensionProperties, extensionName),
						graph_extension$number,
						graph_extension_mapping
					)) return (graph_extension_support_s *) r;
				}
				refer_free(r);
			}
		}
	}
	return NULL;
}


const graph_layer_support_s* graph_layer_support_test(const graph_layer_support_s *restrict layer_support, graph_layer_t layer, const char *restrict *restrict name, const char *restrict *restrict description, uint32_t *restrict spec_version, uint32_t *restrict impl_version)
{
	VkLayerProperties *restrict p;
	if (layer && (uint32_t) layer < graph_layer$number)
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
	VkLayerProperties *p;
	graph_layer_t *t;
	uint32_t n;
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

const graph_extension_support_s* graph_extension_support_test(const graph_extension_support_s *restrict extension_support, graph_extension_t extension, const char *restrict *restrict name, uint32_t *restrict spec_version)
{
	VkExtensionProperties *restrict p;
	if (extension && (uint32_t) extension < graph_layer$number)
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
	VkExtensionProperties *p;
	graph_extension_t *t;
	uint32_t n;
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
