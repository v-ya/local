#define _DEFAULT_SOURCE
#include "iyii.h"
#include "iyii_graph.h"
#include "iyii_swapchain.h"
#include <unistd.h>

typedef struct iyii_event_s {
	iyii_s *iyii;
} iyii_event_s;

struct iyii_s {
	mlog_s *mlog;
	graph_s *graph;
	iyii_event_s *event_data;
	graph_surface_s *surface;
	graph_devices_s *devices;
	graph_device_queues_s *queues;
	graph_dev_s *dev;
	iyii_swapchain_s *swapchain;
	// not need free
	const graph_device_t *physical_device;
	const graph_device_queue_t *device_queue_graphics;
	const graph_device_queue_t *device_queue_transfer;
	graph_queue_t *queue_graphics;
	graph_queue_t *queue_transfer;
	volatile uint32_t closing;
};

static void iyii_surface_event_close_func(iyii_event_s *restrict data, graph_surface_s *surface)
{
	data->iyii->closing = 1;
}

static void iyii_surface_event_key_func(iyii_event_s *restrict data, graph_surface_s *surface, uint32_t key, uint32_t press, graph_surface_do_event_state_t *restrict state)
{
	mlog_printf(data->iyii->mlog, "%s[%u:0x%04x]: (%d, %d) + (%d, %d), %04x\n",
		(const char *[]){"press", "release"}[!!press], key, key,
		state->x - state->root_x, state->root_y - state->y, state->x, state->y, state->state);
}

static void iyii_surface_event_button_func(iyii_event_s *restrict data, graph_surface_s *surface, uint32_t button, uint32_t press, graph_surface_do_event_state_t *restrict state)
{
	mlog_printf(data->iyii->mlog, "%s[%u:0x%04x]: (%d, %d) + (%d, %d), %04x\n",
		(const char *[]){"press", "release"}[!!press], button, button,
		state->x - state->root_x, state->root_y - state->y, state->x, state->y, state->state);
}

static void iyii_surface_event_pointer_func(iyii_event_s *restrict data, graph_surface_s *surface, graph_surface_do_event_state_t *restrict state)
{
	mlog_printf(data->iyii->mlog, "move: (%d, %d) + (%d, %d), %04x\n",
		state->x - state->root_x, state->root_y - state->y, state->x, state->y, state->state);
}

static iyii_s* iyii_select_device_queue(iyii_s *restrict r)
{
	const graph_device_queue_t *restrict q;
	uint32_t i, n;
	graph_queue_flags_t flags;
	n = graph_device_queues_number(r->queues);
	for (i = 0; i < n; ++i)
	{
		if ((q = graph_device_queues_index(r->queues, i)))
		{
			flags = graph_device_queue_flags(q);
			if (!r->device_queue_graphics && (flags & graph_queue_flags_graphics) &&
				graph_device_queue_surface_support(q, r->surface))
				r->device_queue_graphics = q;
			if (!r->device_queue_transfer && (flags & graph_queue_flags_transfer))
				r->device_queue_transfer = q;
		}
	}
	if (r->device_queue_graphics && r->device_queue_transfer)
		return r;
	return NULL;
}

static void iyii_free_func(iyii_s *restrict r)
{
	if (r->swapchain) refer_free(r->swapchain);
	if (r->dev) refer_free(r->dev);
	if (r->queues) refer_free(r->queues);
	if (r->devices) refer_free(r->devices);
	if (r->surface) refer_free(r->surface);
	if (r->event_data) refer_free(r->event_data);
	if (r->graph) refer_free(r->graph);
	if (r->mlog) refer_free(r->mlog);
}

iyii_s* iyii_alloc(mlog_s *restrict mlog, uint32_t enable_validation)
{
	iyii_s *restrict r;
	r = (iyii_s *) refer_alloz(sizeof(iyii_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) iyii_free_func);
		// save mlog
		r->mlog = (mlog_s *) refer_save(mlog);
		// create graph
		r->graph = iyii_graph_create_graph(mlog, "iyaa", "iyii", enable_validation);
		if (!r->graph) goto label_fail;
		// create window surface
		r->event_data = (iyii_event_s *) refer_alloz(sizeof(iyii_event_s));
		if (!r->event_data) goto label_fail;
		r->event_data->iyii = r;
		r->surface = graph_surface_xcb_create_window(r->graph, NULL, 0, 0, 1920, 1080, 0);
		if (!r->surface) goto label_fail;
		// set window event
		graph_surface_set_event_data(r->surface, r->event_data);
		graph_surface_register_event_close(r->surface, (graph_surface_do_event_close_f) iyii_surface_event_close_func);
		graph_surface_register_event_key(r->surface, (graph_surface_do_event_key_f) iyii_surface_event_key_func);
		graph_surface_register_event_button(r->surface, (graph_surface_do_event_button_f) iyii_surface_event_button_func);
		graph_surface_register_event_pointer(r->surface, (graph_surface_do_event_pointer_f) iyii_surface_event_pointer_func);
		if (!graph_surface_set_event(r->surface, (const graph_surface_event_t []) {
			graph_surface_event_close,
			// graph_surface_event_expose,
			graph_surface_event_key,
			graph_surface_event_button,
			// graph_surface_event_pointer,
			// graph_surface_event_area,
			// graph_surface_event_focus,
			// graph_surface_event_resize,
			graph_surface_event_null
		})) goto label_fail;
		// select phtsical device
		r->devices = graph_instance_devices_get(r->graph);
		if (!r->devices) goto label_fail;
		r->physical_device = iyii_graph_select_device(r->devices, r->surface);
		if (!r->physical_device) goto label_fail;
		mlog_printf(r->mlog, "===== select device =====\n");
		graph_device_dump(r->physical_device);
		// select queue
		r->queues = graph_device_queues_get(r->physical_device);
		if (!r->queues) goto label_fail;
		if (!iyii_select_device_queue(r)) goto label_fail;
		mlog_printf(r->mlog, "===== graphics queue =====\n");
		graph_device_queue_dump(r->device_queue_graphics);
		mlog_printf(r->mlog, "===== transfer queue =====\n");
		graph_device_queue_dump(r->device_queue_transfer);
		// create device
		r->dev = iyii_graph_create_device(
			r->graph, r->physical_device,
			r->device_queue_graphics,
			r->device_queue_transfer,
			NULL);
		if (!r->dev) goto label_fail;
		// get queue
		r->queue_graphics = graph_dev_queue(r->dev, r->device_queue_graphics, 0);
		r->queue_transfer = graph_dev_queue(r->dev, r->device_queue_transfer, 0);
		if (!r->queue_graphics || !r->queue_transfer) goto label_fail;
		// create swapchain
		r->swapchain = iyii_swapchain_alloc(r->dev, r->physical_device, r->surface);
		if (!r->swapchain) goto label_fail;
		mlog_printf(r->mlog, "[swapchain] image: %u, format: %d, size: %ux%u\n",
			r->swapchain->image_number, r->swapchain->format,
			r->swapchain->width, r->swapchain->height);
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

iyii_s* iyii_do_events(iyii_s *restrict iyii)
{
	graph_surface_do_events(iyii->surface);
	return iyii->closing?NULL:iyii;
}

void iyii_wait_exit(iyii_s *restrict iyii)
{
	while (!iyii->closing)
	{
		usleep(10000);
		graph_surface_do_events(iyii->surface);
	}
}
