#include "iyii.h"
#include "iyii_graph.h"

#define iyii_graph_enable_validation  1

typedef struct iyii_event_s {
	iyii_s *iyii;
} iyii_event_s;

struct iyii_s {
	mlog_s *mlog;
	graph_s *graph;
	graph_devices_s *devices;
	iyii_event_s *event_data;
	graph_surface_s *surface;
	graph_device_t *physical_device;
	uint32_t closing;
};

static void iyii_free_func(iyii_s *restrict r)
{
	if (r->surface) refer_free(r->surface);
	if (r->event_data) refer_free(r->event_data);
	if (r->devices) refer_free(r->devices);
	if (r->graph) refer_free(r->graph);
	if (r->mlog) refer_free(r->mlog);
}

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

iyii_s* iyii_alloc(mlog_s *restrict mlog)
{
	iyii_s *restrict r;
	r = (iyii_s *) refer_alloz(sizeof(iyii_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) iyii_free_func);
		r->mlog = (mlog_s *) refer_save(mlog);
		r->graph = iyii_graph_create_graph(mlog, "iyaa", "iyii", iyii_graph_enable_validation);
		if (!r->graph) goto label_fail;
		r->devices = graph_instance_devices_get(r->graph);
		if (!r->devices) goto label_fail;
		r->event_data = (iyii_event_s *) refer_alloz(sizeof(iyii_event_s));
		if (!r->event_data) goto label_fail;
		r->event_data->iyii = r;
		r->surface = graph_surface_xcb_create_window(r->graph, NULL, 0, 0, 1920, 1080, 0);
		if (!r->surface) goto label_fail;
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
			graph_surface_event_pointer,
			// graph_surface_event_area,
			// graph_surface_event_focus,
			// graph_surface_event_resize,
			graph_surface_event_null
		})) goto label_fail;
		// r->physical_device = iyii_graph_select_device(r->devices);
		// if (!r->physical_device) goto label_fail;
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

uint32_t iyii_do_events(iyii_s *restrict iyii)
{
	graph_surface_do_events(iyii->surface);
	return iyii->closing;
}
