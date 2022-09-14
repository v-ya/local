#include "xwin_pool.h"
#include <vattr.h>
#include <yaw.h>

struct xwin_pool_s {
	vattr_s *pool;
	yaw_lock_s *read;
	yaw_lock_s *write;
};

struct xwin_pool_item_s {
	yaw_s *yaw;
	xwindow_s *xwin;
	xwindow_event_s *event;
	xwin_pool_events_done_f done;
	refer_t data;
	uintptr_t time_gap;
};

static void xwin_pool_free_func(xwin_pool_s *restrict r)
{
	if (r->pool)
	{
		vattr_vlist_t *restrict vl;
		for (vl = r->pool->vattr; vl; vl = vl->vattr_next)
			yaw_stop(((struct xwin_pool_item_s *) vl->value)->yaw);
		refer_free(r->pool);
	}
	if (r->read) refer_free(r->read);
	if (r->write) refer_free(r->write);
}

static void xwin_pool_item_free_func(struct xwin_pool_item_s *restrict r)
{
	if (r->yaw)
	{
		yaw_stop_and_wait(r->yaw);
		refer_free(r->yaw);
	}
	if (r->event)
	{
		xwindow_event_clear_register(r->event);
		refer_free(r->event);
	}
	if (r->data)
		refer_free(r->data);
	if (r->xwin)
	{
		xwindow_unmap(r->xwin);
		refer_free(r->xwin);
	}
}

xwin_pool_s* xwin_pool_alloc(void)
{
	xwin_pool_s *restrict r;
	if ((r = (xwin_pool_s *) refer_alloz(sizeof(xwin_pool_s))))
	{
		refer_set_free(r, (refer_free_f) xwin_pool_free_func);
		if ((r->pool = vattr_alloc()) &&
			!yaw_lock_alloc_rwlock(&r->read, &r->write))
			return r;
		refer_free(r);
	}
	return NULL;
}

static struct xwin_pool_item_s* xwin_pool_create_xwindow(struct xwin_pool_item_s *restrict item, const xwin_pool_param_t *restrict param, refer_t *restrict data)
{
	xwindow_s *restrict r;
	xwindow_event_s *restrict e;
	uintptr_t en;
	xwindow_event_t ea[xwindow_event$number + 1];
	*data = NULL;
	if ((item->xwin = r = xwindow_alloc(param->x, param->y, (uint32_t) param->w, (uint32_t) param->h, param->depth)))
	{
		if (param->title)
			xwindow_set_title(r, param->title);
		if (param->icon_data)
			xwindow_set_icon(r, param->icon_width, param->icon_height, param->icon_data);
		if (param->data_allocer)
		{
			if (!(*data = param->data_allocer(r)))
				goto label_fail;
		}
		if ((item->event = e = xwindow_event_alloc(r, NULL)))
		{
			en = 0;
			#define d_register_event(_id)  if (param->event_##_id##_func) xwindow_event_register_##_id(e, param->event_##_id##_func, *data), ea[en++] = xwindow_event_##_id
			d_register_event(close);
			d_register_event(expose);
			d_register_event(key);
			d_register_event(button);
			d_register_event(pointer);
			d_register_event(area);
			d_register_event(focus);
			d_register_event(config);
			#undef d_register_event
			ea[en] = xwindow_event_null;
			if (!xwindow_event_used(e, ea))
				goto label_fail;
			if (xwindow_map(r))
				return item;
		}
	}
	label_fail:
	return NULL;
}

static void xwin_pool_item_thread(yaw_s *restrict yaw)
{
	struct xwin_pool_item_s *restrict item;
	item = (struct xwin_pool_item_s *) yaw->data;
	while (yaw->running)
	{
		xwindow_event_done(item->event);
		if (item->done && !item->done(item->xwin, item->data))
			break;
		yaw_msleep(item->time_gap);
	}
}

xwin_pool_s* xwin_pool_add_xwin(xwin_pool_s *xp, const char *restrict name, const xwin_pool_param_t *restrict param)
{
	struct xwin_pool_item_s *restrict item;
	item = NULL;
	yaw_lock_lock(xp->write);
	if (!vattr_get_vslot(xp->pool, name) &&
		(item = (struct xwin_pool_item_s *) refer_alloz(sizeof(struct xwin_pool_item_s))))
	{
		refer_set_free(item, (refer_free_f) xwin_pool_item_free_func);
		if (xwin_pool_create_xwindow(item, param, &item->data))
		{
			item->done = param->events_done_func;
			item->time_gap = param->events_time_gap_msec;
			if (!(item->yaw = yaw_alloc_and_start(xwin_pool_item_thread, NULL, item)))
				goto label_fail;
			if (!vattr_insert_tail(xp->pool, name, item))
				goto label_fail;
			refer_free(item);
		}
		else
		{
			label_fail:
			refer_free(item);
			item = NULL;
		}
	}
	yaw_lock_unlock(xp->write);
	if (item) return xp;
	return NULL;
}
