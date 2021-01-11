#define _DEFAULT_SOURCE
#include "iyii.h"
#include "iyii_graph.h"
#include "iyii_swapchain.h"
#include "iyii_pipeline.h"
#include "iyii_source.h"
#include "iyii_render.h"
#include "iyii_desc.h"
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
	iyii_pipeline_s *pipeline;
	iyii_source_s *source;
	iyii_render_s *render;
	iyii_desc_s *desc;
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

void iyii_surface_event_expose_func(iyii_event_s *restrict data, graph_surface_s *surface, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	mlog_printf(data->iyii->mlog, "expose: (%u, %u)+(%u, %u)\n", x, y, width, height);
	iyii_present(data->iyii);
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

static void iyii_surface_event_config_func(iyii_event_s *restrict data, graph_surface_s *surface, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
	mlog_printf(data->iyii->mlog, "config: (%d, %d)+(%u, %u)\n", x, y, width, height);
	iyii_resize(data->iyii, width, height);
	iyii_present(data->iyii);
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

static graph_command_pool_s* iyii_build_draw_command(graph_command_pool_s *restrict cpool_draw, iyii_s *restrict iyii)
{
	iyii_pipeline_s *restrict pipeline = iyii->pipeline;
	iyii_source_s *restrict source = iyii->source;
	iyii_render_s *restrict render = iyii->render;
	iyii_desc_s *restrict desc = iyii->desc;
	if (graph_command_pool_begin(cpool_draw, ~0, graph_command_buffer_usage_simultaneous_use))
	{
		uint32_t i, n;
		n = render->image_number;
		for (i = 0; i < n; ++i)
		{
			graph_command_begin_render(
				cpool_draw, i,
				render->render_pass,
				render->frame_buffer[i],
				0, 0,
				render->width, render->height
			);
		}
		graph_command_bind_pipe(
			cpool_draw, ~0,
			graph_pipeline_bind_point_graphics,
			pipeline->gpipe);
		graph_command_set_viewport(cpool_draw, ~0, pipeline->viewports);
		graph_command_set_scissor(cpool_draw, ~0, pipeline->viewports);
		graph_command_bind_vertex_buffers(
			cpool_draw, ~0,
			0, 1,
			(const graph_buffer_s *[]) {source->buffer_dev},
			(uint64_t []) {source->vertex$offset});
		graph_command_bind_index_buffer(
			cpool_draw, ~0,
			source->buffer_dev,
			source->index$offset,
			graph_index_type_uint16);
		for (i = 0; i < n; ++i)
		{
			graph_command_bind_desc_sets(
				cpool_draw, i,
				graph_pipeline_bind_point_graphics,
				pipeline->layout,
				desc->sets, i, 1,
				0, NULL
			);
		}
		graph_command_draw_index(cpool_draw, ~0, 6, 1, 0, 0, 0);
		graph_command_end_render(cpool_draw, ~0);
		if (graph_command_pool_end(cpool_draw, ~0))
			return cpool_draw;
	}
	return NULL;
}

static iyii_s* iyii_load_source(iyii_s *restrict r)
{
	static float vertex[] = {
		-0.8f, -0.8f, 1.0f, 0.0f, 0.0f,
		 0.8f, -0.8f, 0.0f, 1.0f, 0.0f,
		-0.8f,  0.8f, 0.0f, 0.0f, 1.0f,
		 0.8f,  0.8f, 1.0f, 1.0f, 1.0f,
	};
	static uint16_t index[] = {
		0, 1, 2, 2, 1, 3
	};
	static uint32_t pixel[] = {
		0x00ff0000, 0xff00ff00, 0x00ff0000, 0xff00ff00,
		0xff00ff00, 0x00ff0000, 0xff00ff00, 0x00ff0000,
		0x00ff0000, 0xff00ff00, 0x00ff0000, 0xff00ff00,
		0xff00ff00, 0x00ff0000, 0xff00ff00, 0x00ff0000,
	};
	iyii_source_texture_t texture;
	iyii_source_copy_vertex(r->source, vertex, 0, sizeof(vertex));
	iyii_source_copy_index(r->source, index, 0, sizeof(index));
	if (iyii_source_map_texture(r->source, &texture))
	{
		iyii_source_texture_copy(&texture, pixel, 0, 0, 4, 4);
		iyii_source_unmap_texture(&texture);
	}
	if (iyii_source_submit(r->source, r->queue_transfer, iyii_source_transfer$transfer) &&
		iyii_source_submit(r->source, r->queue_transfer, iyii_source_transfer$texture))
	{
		graph_dev_wait_idle(r->dev);
		return r;
	}
	return NULL;
}

static void iyii_free_func(iyii_s *restrict r)
{
	if (r->dev) graph_dev_wait_idle(r->dev);
	if (r->desc) refer_free(r->desc);
	if (r->render) refer_free(r->render);
	if (r->source) refer_free(r->source);
	if (r->pipeline) refer_free(r->pipeline);
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
		graph_surface_register_event_expose(r->surface, (graph_surface_do_event_expose_f) iyii_surface_event_expose_func);
		graph_surface_register_event_key(r->surface, (graph_surface_do_event_key_f) iyii_surface_event_key_func);
		graph_surface_register_event_button(r->surface, (graph_surface_do_event_button_f) iyii_surface_event_button_func);
		graph_surface_register_event_pointer(r->surface, (graph_surface_do_event_pointer_f) iyii_surface_event_pointer_func);
		graph_surface_register_event_config(r->surface, (graph_surface_do_event_config_f) iyii_surface_event_config_func);
		if (!graph_surface_set_event(r->surface, (const graph_surface_event_t []) {
			graph_surface_event_close,
			graph_surface_event_expose,
			graph_surface_event_key,
			graph_surface_event_button,
			// graph_surface_event_pointer,
			// graph_surface_event_area,
			// graph_surface_event_focus,
			graph_surface_event_config,
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
		// create pipeline
		r->pipeline = iyii_pipeline_alloc(r->dev, r->swapchain->format,
			0, 0, r->swapchain->width, r->swapchain->height);
		if (!r->pipeline) goto label_fail;
		// create source
		r->source = iyii_source_alloc(r->dev, r->device_queue_transfer);
		if (!r->source) goto label_fail;
		if (!iyii_source_ready(
			r->source,
			4, 4,
			1024, 1024,
			1024, r->swapchain->image_number
		)) goto label_fail;
		if (!iyii_source_build_transfer(r->source))
			goto label_fail;
		// create render
		r->render = iyii_render_alloc(r->pipeline->render_pass, r->swapchain,
			r->dev, r->device_queue_graphics);
		if (!r->render) goto label_fail;
		// create desc
		r->desc = iyii_desc_alloc(r->dev, r->source, r->pipeline->desc_set_layout);
		if (!r->desc) goto label_fail;
		// build draw command
		if (!iyii_build_draw_command(r->render->cpool_draw, r))
			goto label_fail;
		// load source
		if (!iyii_load_source(r))
			goto label_fail;
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

iyii_s* iyii_resize(iyii_s *restrict iyii, uint32_t width, uint32_t height)
{
	iyii_swapchain_s *swapchain;
	if ((swapchain = iyii->swapchain) && width && height)
	{
		if (swapchain->width == width || swapchain->height == height)
			return iyii;
		else
		{
			;
		}
	}
	return NULL;
}

void iyii_present(iyii_s *restrict iyii)
{
	uint32_t index, i;
	if (iyii_swapchain_acquire(iyii->swapchain, &index, &i))
	{
		iyii_swapchain_render(iyii->swapchain, iyii->queue_graphics, iyii->render->cpool_draw, index, i);
	}
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
