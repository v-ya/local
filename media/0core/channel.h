#ifndef _media_core_channel_h_
#define _media_core_channel_h_

#include <refer.h>
#include "cell.h"

struct media_channel_s {
	void *data;
	uintptr_t size;
	refer_t data_source;
	uintptr_t dimension;
	struct media_cell_t cell[];
};

struct media_channel_s* media_channel_alloc(uintptr_t dimension, const struct media_cell_info_t cell[], const uintptr_t *restrict dimension_value);
struct media_channel_s* media_channel_set_dimension(struct media_channel_s *restrict channel, uintptr_t dimension, const struct media_cell_info_t cell[], const uintptr_t *restrict dimension_value);
struct media_channel_s* media_channel_test_dimension(struct media_channel_s *restrict channel, uintptr_t dimension, const uintptr_t *restrict dimension_value);
struct media_channel_s* media_channel_set_data(struct media_channel_s *restrict channel, void *data, uintptr_t size, refer_t data_source);
struct media_channel_s* media_channel_touch_data(struct media_channel_s *restrict channel);
void media_channel_clear_data(struct media_channel_s *restrict channel);

#endif
