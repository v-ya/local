#ifndef _media_core_frame_id_h_
#define _media_core_frame_id_h_

#include <refer.h>
#include "cell.h"
#include "channel.h"

struct media_frame_id_s {
	refer_string_t name;
	refer_string_t compat;
	uintptr_t channel;
	uintptr_t dimension;
	struct media_cell_info_t cell[];
};

struct media_frame_id_s* media_frame_id_alloc(const char *restrict name, uintptr_t channel, uintptr_t dimension);
struct media_frame_id_s* media_frame_id_initial(struct media_frame_id_s *restrict frame_id, uintptr_t channel_index, uintptr_t dimension, const struct media_cell_info_t *restrict params);

const struct media_cell_info_t* media_frame_id_get_cells(const struct media_frame_id_s *restrict frame_id, uintptr_t channel_index, uintptr_t dimension);

#endif
