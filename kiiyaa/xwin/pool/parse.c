#include "xwin_pool.h"
#include <pocket/pocket.h>
#include <dylink.h>
#include <hashmap.h>

static const char parse_name[] = "xwin.pool.parse";

static const pocket_attr_t* xwin_pool_parse_application(xwin_pool_s *restrict xp, pocket_s *restrict pocket, const pocket_attr_t *restrict application, dylink_pool_t *restrict pool)
{
	static const xwin_pool_param_t default_param = {
		.x = 0,
		.y = 0,
		.w = 0,
		.h = 0,
		.depth = 24,
		.enable_shm = 1,
		.shm_size = 0,
		.events_time_gap_msec = 15,
		.title = NULL,
		.icon_data = NULL,
		.icon_width = 0,
		.icon_height = 0,
		.data_allocer = NULL,
		.events_done_func = NULL,
		.event_close_func = NULL,
		.event_expose_func = NULL,
		.event_key_func = NULL,
		.event_button_func = NULL,
		.event_pointer_func = NULL,
		.event_area_func = NULL,
		.event_focus_func = NULL,
		.event_config_func = NULL,
	};
	xwin_pool_param_t param;
	param = default_param;
	const pocket_attr_t *restrict value, *restrict icon;
	icon = NULL;
	// size
	if ((value = pocket_find(pocket, application, "size")))
	{
		if (pocket_preset_tag(pocket, value) != pocket_tag$s32)
			goto label_fail;
		if (value->size != (sizeof(uint32_t) * 4))
			goto label_fail;
		param.x = ((int32_t *) value->data.ptr)[0];
		param.y = ((int32_t *) value->data.ptr)[1];
		param.w = ((int32_t *) value->data.ptr)[2];
		param.h = ((int32_t *) value->data.ptr)[3];
		if (param.w < 0 || param.h < 0)
			goto label_fail;
	}
	// depth
	if ((value = pocket_find(pocket, application, "depth")))
	{
		if (pocket_preset_tag(pocket, value) != pocket_tag$u32)
			goto label_fail;
		if (value->size != sizeof(uint32_t))
			goto label_fail;
		param.depth = *(uint32_t *) value->data.ptr;
		if (param.depth != 24 && param.depth != 32)
			goto label_fail;
	}
	// event.time.gap
	if ((value = pocket_find(pocket, application, "event.time.gap")))
	{
		if (pocket_preset_tag(pocket, value) != pocket_tag$u32)
			goto label_fail;
		if (value->size != sizeof(uint32_t))
			goto label_fail;
		param.events_time_gap_msec = *(uint32_t *) value->data.ptr;
	}
	// title
	if ((value = pocket_find(pocket, application, "title")))
	{
		if (pocket_preset_tag(pocket, value) != pocket_tag$string)
			goto label_fail;
		param.title = (const char *) value->data.ptr;
	}
	// icon
	if ((icon = pocket_find(pocket, application, "icon")))
	{
		// icon size
		if (!(value = pocket_find(pocket, icon, "size")))
			goto label_fail;
		if (pocket_preset_tag(pocket, value) != pocket_tag$u32)
			goto label_fail;
		if (value->size != (sizeof(uint32_t) * 2))
			goto label_fail;
		param.icon_width = ((uint32_t *) value->data.ptr)[0];
		param.icon_height = ((uint32_t *) value->data.ptr)[1];
		if (!param.icon_width || !param.icon_height)
			goto label_fail;
		// icon data
		if (!(value = pocket_find(pocket, icon, "data")))
			goto label_fail;
		if (pocket_preset_tag(pocket, value) != pocket_tag$u32)
			goto label_fail;
		if (value->size != (sizeof(uint32_t) * param.icon_width * param.icon_height))
			goto label_fail;
		param.icon_data = (const uint32_t *) value->data.ptr;
		if (!param.icon_data)
			goto label_fail;
		icon = NULL;
	}
	#define d_get_symbol(_p, _n)  \
		if ((value = pocket_find(pocket, application, _n)))\
		{\
			if (pocket_preset_tag(pocket, value) != pocket_tag$string)\
				goto label_fail;\
			if (!value->data.ptr)\
				goto label_fail;\
			param._p = dylink_pool_get_symbol(pool, (const char *) value->data.ptr, NULL);\
			if (!param._p)\
				goto label_fail;\
		}
	d_get_symbol(data_allocer, "data.allocer")
	d_get_symbol(events_done_func, "events.done")
	d_get_symbol(event_close_func, "event.close")
	d_get_symbol(event_expose_func, "event.expose")
	d_get_symbol(event_key_func, "event.key")
	d_get_symbol(event_button_func, "event.button")
	d_get_symbol(event_pointer_func, "event.pointer")
	d_get_symbol(event_area_func, "event.area")
	d_get_symbol(event_focus_func, "event.focus")
	d_get_symbol(event_config_func, "event.config")
	#undef d_get_symbol
	value = application;
	if (xwin_pool_add_xwin(xp, application->name.string, &param))
		return NULL;
	label_fail:
	return icon?icon:value;
}

static pocket_s* xwin_pool_parse(pocket_s *restrict pocket, const pocket_attr_t *restrict root, const char *restrict name, const char *restrict *restrict error)
{
	extern hashmap_t *$dylink;
	extern xwin_pool_s *xwin_pool;
	dylink_pool_t *restrict pool;
	const pocket_attr_t *restrict e;
	uint64_t n;
	pocket_tag_t tag;
	e = root;
	if ((pool = hashmap_get_name($dylink, name)) && (root = pocket_find(pocket, root, parse_name)))
	{
		tag = pocket_preset_tag(pocket, root);
		if (tag == pocket_tag$index)
		{
			n = root->size;
			root = (const pocket_attr_t *) root->data.ptr;
			while (n)
			{
				--n;
				if (!root->name.string)
					goto label_fail;
				if ((e = xwin_pool_parse_application(xwin_pool, pocket, root, pool)))
					goto label_fail;
				++root;
			}
			return pocket;
		}
	}
	label_fail:
	*error = e->name.string;
	return NULL;
}

hashmap_vlist_t* xwin_pool_parse_initial(void)
{
	extern hashmap_t *$parse;
	hashmap_vlist_t *restrict vl;
	void *value;
	if ((vl = hashmap_put_name($parse, parse_name, value = xwin_pool_parse, NULL)) &&
		vl->value == value)
		return vl;
	return NULL;
}

void xwin_pool_parse_finally(void)
{
	extern hashmap_t *$parse;
	hashmap_delete_name($parse, parse_name);
}
