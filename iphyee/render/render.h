#ifndef _iphyee_render_pri_h_
#define _iphyee_render_pri_h_

#include "../iphyee.h"
#include "../iphyee.worker.h"

typedef struct iphyee_render_screen_s iphyee_render_screen_s;

struct iphyee_render_screen_s {
	iphyee_worker_buffer_s *screen_host;
	iphyee_worker_buffer_s *buffer_device;
	const void *screen_data;
	uint64_t screen_address;
	uint64_t screen_offset;
	uint64_t screen_size;
	uint64_t depth_address;
	uint64_t depth_offset;
	uint64_t depth_size;
	uint64_t mutex_address;
	uint64_t mutex_offset;
	uint64_t mutex_size;
	uint32_t width;
	uint32_t height;
};

iphyee_render_screen_s* iphyee_render_screen_alloc(iphyee_worker_s *restrict worker, uint32_t width, uint32_t height);

#endif
